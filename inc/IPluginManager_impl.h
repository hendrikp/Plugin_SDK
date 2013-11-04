/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#include <IPluginManager.h>
#include <CryLibrary.h>

// Definition can only happen once
#if defined(__GAMESTARTUP_H__) || defined(PLUGIN_SDK_DEFINE_MANAGER)
PluginManager::IPluginManager* gPluginManager = NULL; //!< Global plugin manager pointer inside game dll
#endif

#if defined(__GAMESTARTUP_H__)
SSystemInitParams* gStartupParams = NULL; //!< Global startup params
#elif defined(__GAME_H__)
extern SSystemInitParams* gStartupParams;
#endif

/**
* @brief Provide a macro to realize WinProc injector with minimal modifications
*/
#ifndef PLUGIN_SDK_WINPROC_INJECTOR
#define PLUGIN_SDK_WINPROC_INJECTOR(...) \
    LRESULT CALLBACK WndProc_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); \
    __VA_ARGS__ { \
        if(gPluginManager) { \
            if(gPluginManager->PreWinProcInterceptor(hWnd, msg, wParam, lParam)) { \
                return gPluginManager->PostWinProcInterceptor(hWnd, msg, wParam, lParam, 0); \
            } else { \
                return gPluginManager->PostWinProcInterceptor(hWnd, msg, wParam, lParam, WndProc_(hWnd, msg, wParam, lParam)); \
            } \
        } \
        return WndProc_(hWnd, msg, wParam, lParam); \
    } \
    LRESULT CALLBACK WndProc_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif

namespace PluginManager
{


    /**
    * @brief Initialize the Plugin Manager
    * This code must be called once in the game dll GameStartup
    * @return success
    * @param startupParams CryEngine Startup Params
    * @param sBaseInterfaceVersion Plugin SDK Base Interface Version
    */
    static bool InitPluginManager( SSystemInitParams& startupParams, const char* sBaseInterfaceVersion = NULL, const char* sConcreteInterfaceVersion = NULL )
    {
        string sPluginManagerPath = PLUGIN_FOLDER "\\" PLUGIN_TEXT "_" PLUGIN_MANAGER CrySharedLibrayExtension;
        HINSTANCE hModule = CryLoadLibrary( sPluginManagerPath );

        // Plugin link library found?
        if ( hModule )
        {
            // Check if its a plugin
            void* fptr = CryGetProcAddress( hModule, PLUGIN_ENTRYPOINT );

            if ( fptr )
            {
                // its a plugin create the baseinterface
                IPluginBase* iface = fGetPluginInterface( fptr )( sBaseInterfaceVersion );

                if ( iface )
                {
                    char buildVersion[MAX_PATH];
                    const SFileVersion& buildVer = startupParams.pSystem->GetBuildVersion();
                    sprintf( buildVersion, "%d.%d.%d", buildVer.v[3], buildVer.v[2], buildVer.v[1] );

                    if ( iface->Check( buildVersion ) )
                    {
                        // plugin link library found
                        if ( !iface->IsInitialized() ) // Initialize plugins in order
                        {
#if defined(SYS_ENV_AS_STRUCT)

                            if ( !iface->Init( gEnv, startupParams, NULL, NULL ) )
#else
                            if ( !iface->Init( *gEnv, startupParams, NULL, NULL ) )
#endif
                            {
                                CryLogAlways( "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] Init failed" );
                            }
                        }

                        if ( iface->IsInitialized() && !iface->IsFullyInitialized() )
                        {
                            if ( iface->CheckDependencies() )
                            {
                                if ( !iface->InitDependencies() )
                                {
                                    CryLogAlways( "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] InitDependencies failed" );
                                }

                            }

                            else
                            {
                                CryLogAlways( "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] CheckDependencies failed" );
                            }
                        }

                        if ( iface->IsFullyInitialized() )
                        {
                            if ( iface->GetConcreteInterface( sConcreteInterfaceVersion ) )
                            {
                                gPluginManager = ( IPluginManager* )iface->GetConcreteInterface( sConcreteInterfaceVersion );
                                return true;
                            }

                            else
                            {
                                CryLogAlways(  "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] Concrete Interface not available in the requested version" );
                            }
                        }

                        else
                        {
                            CryLogAlways(  "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] Couldn't be fully initialized" );
                        }
                    }

                    else
                    {
                        CryLogAlways(  "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] Not compatible with this CryEngine SDK version(%s)", buildVersion );
                    }
                }

                else
                {
                    CryLogAlways(  "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] Base interface version(%s) couldn't be retrieved", sBaseInterfaceVersion ? sBaseInterfaceVersion : "" );
                }
            }

            else
            {
                CryLogAlways(  "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] Plugin entry point " PLUGIN_ENTRYPOINT " not found" );
            }
        }

        else
        {
            CryLogAlways(  "[" PLUGIN_TEXT "_" PLUGIN_MANAGER "] CryLoadLibrary Module(%s) couldn't be loaded, check path or dependencies", sPluginManagerPath.c_str() );
        }

        return false; // Failure
    }

    static void RememberStartupParams( SSystemInitParams& startupParams )
    {
        gStartupParams = &startupParams;
    }

    static bool InitPluginManager( const char* sBaseInterfaceVersion = NULL, const char* sConcreteInterfaceVersion = NULL )
    {
        if ( gStartupParams )
        {
            return InitPluginManager( *gStartupParams, sBaseInterfaceVersion, sConcreteInterfaceVersion );
        }

        return false;
    }

    /**
    * @brief Initialize plugins before the framework
    * Only sensible if the plugin has no game objects and flownodes.
    * @attention the global environment is not full initialized.
    * @see InitPluginsLast
    */
    static void InitPluginsBeforeFramework()
    {
        if ( gPluginManager )
        {
            gPluginManager->ReloadAllPlugins();
            gPluginManager->InitializePluginRange( IM_BeforeFramework, IM_BeforeFramework_3 );
            gPluginManager->RegisterTypesPluginRange( IM_BeforeFramework, IM_BeforeFramework_3, FT_None );
            gPluginManager->RegisterTypesPluginRange( IM_BeforeFramework, IM_BeforeFramework_3, FT_CVar );
            gPluginManager->RegisterTypesPluginRange( IM_BeforeFramework, IM_BeforeFramework_3, FT_CVarCommand );
            gPluginManager->RegisterTypesPluginRange( IM_BeforeFramework, IM_BeforeFramework_3, FT_GameObjectExtension );
            gPluginManager->RegisterTypesPluginRange( IM_BeforeFramework, IM_BeforeFramework_3, FT_UIEvent );
        }
    }

    /**
    * @brief Initialize plugins after the game is initialized.
    * Most sensible for all plugins.
    */
    static void InitPluginsLast()
    {
        if ( gPluginManager )
        {
            gPluginManager->InitializePluginRange( IM_Last, IM_Last_3 );
            gPluginManager->RegisterTypesPluginRange( IM_Last, IM_Last_3, FT_None );
            gPluginManager->RegisterTypesPluginRange( IM_Last, IM_Last_3, FT_CVar );
            gPluginManager->RegisterTypesPluginRange( IM_Last, IM_Last_3, FT_CVarCommand );
            gPluginManager->RegisterTypesPluginRange( IM_Last, IM_Last_3, FT_GameObjectExtension );
            // UI Events cant be registered at this point

            if ( gEnv && gEnv->pConsole )
            {
                gEnv->pConsole->ExecuteString( "pm_listsi" );
                gEnv->pConsole->ExecuteString( "pm_list" );
            }
        }
    }

    /**
    * @brief Flownodes need to be registered when Game Flownodes are registered to allow Sandbox Flownode Reset
    */
    static void RegisterPluginFlownodes()
    {
        if ( gPluginManager )
        {
            gPluginManager->RegisterTypesPluginRange( IM_Min, IM_Max, FT_Flownode );
        }
    }
}

/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#include <IPluginManager.h>

PluginManager::IPluginManager* gPluginManager = NULL; //!< Global plugin manager pointer inside game dll

namespace PluginManager
{
    /**
    * @brief Initialize the Plugin Manager
    * This code must be called once in the game dll GameStartup
    * @return success
    * @param startupParams CryEngine Startup Params
    * @param sSDKVersion CryEngine SDK Version
    * @param sBaseInterfaceVersion Plugin SDK Base Interface Version
    */
    bool InitPluginManager( SSystemInitParams& startupParams, const char* sBaseInterfaceVersion = NULL, const char* sConcreteInterfaceVersion = NULL )
    {
        HINSTANCE hModule = CryLoadLibrary( PLUGIN_FOLDER "\\" PLUGIN_TEXT "_"  "Manager" CrySharedLibrayExtension );

        // Plugin link library found?
        if ( hModule )
        {
            // Check if its a plugin
            void* fptr = CryGetProcAddress( hModule, "GetPluginInterface" );

            if ( fptr )
            {
                // its a plugin create the baseinterface
                IPluginBase* iface = fGetPluginInterface( fptr )( sBaseInterfaceVersion );

                if ( iface )
                {
                    char* buildVersion = new char[255];
                    startupParams.pSystem->GetBuildVersion().ToShortString( buildVersion );

                    if ( iface->Check( buildVersion ) )
                    {
                        // plugin link library found
                        if ( !iface->IsInitialized() ) // Initialize plugins in order
                        {
                            if ( !iface->Init( *gEnv, startupParams, NULL ) )
                            {
                                CryLogAlways( "Init failed" );
                            }
                        }

                        if ( iface->IsInitialized() && !iface->IsFullyInitialized() )
                        {
                            if ( iface->CheckDependencies() )
                            {
                                if ( !iface->InitDependencies() )
                                {
                                    CryLogAlways( "InitDependencies failed" );
                                }

                            }

                            else
                            {
                                CryLogAlways( "CheckDependencies failed" );
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
                                CryLogAlways( "Concrete Interface not available in the requested version" );
                            }
                        }
                    }
                }
            }
        }

        return false; // Failure
    }

    /**
    * @brief Initialize plugins before the framework
    * Only sensible if the plugin has no game objects and flownodes.
    * @attention the global environment is not full initialized.
    * @see InitPluginsLast
    */
    void InitPluginsBeforeFramework()
    {
        if ( gPluginManager )
        {
            gPluginManager->ReloadAllPlugins();
            gPluginManager->InitializePluginRange( IM_BeforeFramework, IM_BeforeFramework_3 );
        }
    }

    /**
    * @brief Initialize plugins after the game is initialized.
    * Most sensible for all plugins.
    */
    void InitPluginsLast()
    {
        if ( gPluginManager )
        {
            gPluginManager->InitializePluginRange( IM_Last, IM_Last_3 );
        }
    }
}

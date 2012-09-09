/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginManager.h>

#define SAFESTR(x) ((const char*)(x?x:""))
#define BOOLSTR(x) (x?"true":"false")

namespace PluginManager
{
    void Command_ListAll( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager )
        {
            return;
        }

        gPluginManager->ListAllPlugins();
    };

    void Command_Dump( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager && pArgs && pArgs->GetArgCount() > 1 )
        {
            return;
        }

        gPluginManager->DumpPlugin( pArgs->GetArg( 1 ) );
    };

    void Command_DumpAll( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager )
        {
            return;
        }

        gPluginManager->DumpAllPlugins();
    };

    void Command_Unload( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager && pArgs && pArgs->GetArgCount() > 1 )
        {
            return;
        }

        gPluginManager->UnloadPlugin( pArgs->GetArg( 1 ) );
    };

    void Command_UnloadAll( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager )
        {
            return;
        }

        gPluginManager->UnloadAllPlugins();
    };

    void Command_Reload( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager && pArgs && pArgs->GetArgCount() > 1 )
        {
            return;
        }

        gPluginManager->ReloadPlugin( pArgs->GetArg( 1 ), true );
    };

    void Command_ReloadAll( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager )
        {
            return;
        }

        gPluginManager->UnloadAllPlugins();
        gPluginManager->ReloadAllPlugins();
        gPluginManager->InitializePluginRange();
    };

    CPluginManager::CPluginManager()
    {
        gPluginManager = this;
    }

    CPluginManager::~CPluginManager()
    {
        gPluginManager = NULL;
    }

    void CPluginManager::Release()
    {
        // Should be called while Game is still active otherwise there might be leaks/problems
        CPluginBase::Release();

        // Depending on your plugin you might not want to unregister anything
        // if the System is quitting.
        // if(gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting()) {

        // Unregister CVars
        if ( gEnv && gEnv->pConsole )
        {
            gEnv->pConsole->RemoveCommand( "pm_list" );
            gEnv->pConsole->RemoveCommand( "pm_dump" );
            gEnv->pConsole->RemoveCommand( "pm_dumpall" );
            gEnv->pConsole->RemoveCommand( "pm_unload" );
            gEnv->pConsole->RemoveCommand( "pm_unloadall" );
            gEnv->pConsole->RemoveCommand( "pm_reload" );
            gEnv->pConsole->RemoveCommand( "pm_reloadall" );
        }

        // Unregister game objects
        if ( gEnv && gEnv->pGameFramework )
        {
            // ...
        }

        // Cleanup all plugins (special case only in manager...)
        UnloadAllPlugins();

        // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
        gPluginManager->UnloadPlugin( GetName() );
    };

    bool CPluginManager::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager )
    {
        CPluginBase::Init( env, startupParams, gPluginManager->GetBase() );

        // Save Startup parameters
        gStartupInitParams = startupParams;

        // Register CVars
        if ( gEnv && gEnv->pConsole )
        {
            gEnv->pConsole->AddCommand( "pm_list", Command_ListAll, VF_NULL, "List one info row for all plugins" );
            gEnv->pConsole->AddCommand( "pm_dump", Command_Dump, VF_NULL, "Dump info on a specific plugins" );
            gEnv->pConsole->AddCommand( "pm_dumpall", Command_DumpAll, VF_NULL, "Dump info on all plugins" );
            gEnv->pConsole->AddCommand( "pm_unload", Command_Unload, VF_NULL, "Unload a specific plugin using its name" );
            gEnv->pConsole->AddCommand( "pm_unloadall", Command_UnloadAll, VF_NULL, "Unload all plugins with the exception of the plugin manager" );
            gEnv->pConsole->AddCommand( "pm_reload", Command_Reload, VF_NULL, "Reload a specific plugin using its path" );
            gEnv->pConsole->AddCommand( "pm_reloadall", Command_ReloadAll, VF_NULL, "Reload all plugins" );
        }

        // Register Game Objects
        // ...

        // Register the plugin manager itself (plugin manager only, plugins don't need to do this)
        LogAlways( "Plugin Manager initialized!" );
        m_Plugins[ GetName() ] = std::pair<HINSTANCE, IPluginBase*>( CryLoadLibrary( PLUGIN_PATH ), GetBase() );

        return true;
    }

    bool CPluginManager::Check( const char* sAPIVersion ) const
    {
        if ( !sAPIVersion )
        {
            return false;
        }

        gsSDKInterfaceVersion = sAPIVersion;

        if ( strcmp( sAPIVersion, "3.4" ) == 0 )
        {
            return true;
        }

        return false;
    }

    const char* CPluginManager::GetStatus() const
    {
        return "OK";
    }

    void CPluginManager::UnloadAllPlugins()
    {
        LogAlways( "Unloading all plugins..." );

        // Keep only the plugin manager
        for ( int nMode = IM_Max; nMode >= IM_Min; --nMode )
        {
            // Repeat until nothing is left to do (done this way because iterator is invalidated after an unload)
            bool bEndReached = false;

            while ( !bEndReached )
            {
                bEndReached = true;

                for ( tPluginNameMap::iterator pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
                {
                    // Don't unload the plugin manager and unload in reverse order
                    if ( pluginIter->second.second != GetBase() && pluginIter->second.second->GetInitializationMode() == nMode )
                    {
                        bEndReached = false;
                        UnloadPlugin( pluginIter->second.second->GetName() );
                        break; // Iterators are invalidated get new iterator for next cleanup...
                    }
                }
            }
        }

        LogAlways( "Unloaded all plugins!" );
    };

    void CPluginManager::UnloadPlugin( const char* sPluginName )
    {
        tPluginNameMap::iterator pluginIter = m_Plugins.find( sPluginName );

        LogAlways( "Unloading: Name(%s)", sPluginName );

        if ( pluginIter != m_Plugins.end() )
        {
            if ( !pluginIter->second.second->IsUnloading() )
            {
                // Let the plugin first clean itself up
                pluginIter->second.second->Release();
            }

            else
            {
                // Clean library up if plugin has itself cleaned up
                CryFreeLibrary( pluginIter->second.first );
                m_Plugins.erase( pluginIter );

                LogAlways( "Unloaded: Name(%s)", sPluginName );
            }
        }

        else
        {
            LogWarning( "Nothing to unload." );
        }
    }

    void CPluginManager::ReloadAllPlugins()
    {
        ICryPak* pCryPak = gEnv->pCryPak;
        string sPluginsPath = pCryPak->GetGameFolder();
        sPluginsPath += PLUGIN_FOLDER;

        // create plugin directory if it doesnt exist
        if ( !pCryPak->IsFileExist( sPluginsPath.c_str() ) )
        {
            pCryPak->MakeDir( sPluginsPath.c_str() );
        }

        sPluginsPath += "\\";

        LogAlways( "Loading plugins from '%s'", sPluginsPath.c_str() );

        _finddata_t fileData;

        string sSearchPath = sPluginsPath + "*" CrySharedLibrayExtension;

        intptr_t hFileFind = intptr_t( INVALID_HANDLE_VALUE );
        int hNextFile = int( INVALID_HANDLE_VALUE );

        for ( hNextFile = hFileFind = pCryPak->FindFirst( sSearchPath.c_str(), &fileData, 0, true );
                hNextFile != int( INVALID_HANDLE_VALUE );
                hNextFile = pCryPak->FindNext( hFileFind, &fileData ) )
        {
            ReloadPlugin( sPluginsPath + fileData.name );
        }

        pCryPak->FindClose( hFileFind );

        LogAlways( "Currently %d plugins are loaded!", m_Plugins.size() );
    }

    bool CPluginManager::ReloadPlugin( const char* sPluginPath, bool bInitialize )
    {
        if ( !sPluginPath )
        {
            return false;
        }

        LogAlways( "Loading: File(%s)", sPluginPath );

        HINSTANCE hModule = CryLoadLibrary( sPluginPath );

        // Plugin link library found?
        if ( hModule )
        {
            // Check if its a plugin
            void* fptr = CryGetProcAddress( hModule, "GetPluginInterface" );

            // Don't reload PluginManager...
            if ( fptr == GetBase() )
            {
                return true;
            }

            if ( fptr )
            {
                // its a plugin create the baseinterface
                IPluginBase* iface =  fGetPluginInterface( fptr )( gsBaseInterfaceVersion );

                if ( iface )
                {
                    if ( iface->Check( gsSDKInterfaceVersion ) )
                    {
                        string sPluginName = SAFESTR( iface->GetName() );

                        if ( sPluginName.size() > 0 )
                        {
                            // Check if Plugin is already loaded, if so unload it and Load again
                            tPluginNameMap::iterator pluginIter = m_Plugins.find( sPluginName );

                            if ( pluginIter == m_Plugins.end() )
                            {
                                if ( gsSDKInterfaceVersion )
                                {
                                    LogAlways( "Loaded: Name(%s) Version(%s) Category(%s)", sPluginName.c_str(), SAFESTR( iface->GetVersion() ), SAFESTR( iface->GetCategory() ) );
                                }

                                m_Plugins[sPluginName] = std::pair<HINSTANCE, IPluginBase*>( hModule, iface );

                                if ( bInitialize )
                                {
                                    return InitializePlugin( sPluginName );
                                }

                                else
                                {
                                    return true;
                                }
                            }

                            else
                            {
                                // Plugin was already loaded so reload it
                                UnloadPlugin( sPluginName );
                                return ReloadPlugin( sPluginPath );
                            }
                        }

                        else
                        {
                            LogError( "Plugin has no name." );
                        }
                    }

                    else
                    {
                        LogError( "CryEngine SDK interface version not compatible." );
                    }
                }

                else
                {
                    LogError( "Plugin SDK base interface version not compatible." );
                }
            }

            else
            {
                LogError( "Not a plugin." );
            }

            CryFreeLibrary( hModule );
        }

        else
        {
            LogError( "Path not found." );
        }

        return false;
    }

    bool CPluginManager::InitializePlugin( const char* sPluginName )
    {
        tPluginNameMap::iterator pluginIter = m_Plugins.find( sPluginName );

        if ( pluginIter != m_Plugins.end() )
        {
            IPluginBase* iface = pluginIter->second.second;

            if ( !iface->IsInitialized() ) // Initialize plugins in order
            {
                if ( !iface->Init( *gEnv, gStartupInitParams, GetBase() ) )
                {
                    LogError( "Init failed: Name(%s)", SAFESTR( iface->GetName() ) );
                }
            }

            if ( iface->IsInitialized() && !iface->IsFullyInitialized() )
            {
                if ( iface->CheckDependencies() )
                {
                    if ( !iface->InitDependencies() )
                    {
                        LogError( "InitDependencies failed: Name(%s)", SAFESTR( iface->GetName() ) );
                    }

                }

                else
                {
                    LogError( "CheckDependencies failed: Name(%s)", SAFESTR( iface->GetName() ) );
                }
            }

            if ( iface->IsFullyInitialized() )
            {
                return true;
            }
        }

        return false;
    }

    void CPluginManager::InitializePluginRange( int nBeginAtMode, int nEndAtMode )
    {
        for ( int nMode = nBeginAtMode; nMode <= nEndAtMode; ++nMode )
        {
            // First initialize all in this dependency level
            for ( tPluginNameMap::iterator pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
            {
                IPluginBase* iface = pluginIter->second.second;

                if ( iface->GetInitializationMode() == nMode && !iface->IsInitialized() ) // Initialize plugins in order
                {
                    if ( !iface->Init( *gEnv, gStartupInitParams, GetBase() ) )
                    {
                        LogError( "Init failed: Name(%s)", SAFESTR( iface->GetName() ) );
                    }
                }
            }

            // Now initialize the dependencies
            for ( tPluginNameMap::iterator pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
            {
                IPluginBase* iface = pluginIter->second.second;

                if ( iface->GetInitializationMode() == nMode && iface->IsInitialized() && !iface->IsFullyInitialized() ) // Initialize plugins in order
                {
                    if ( iface->CheckDependencies() )
                    {
                        if ( !iface->InitDependencies() )
                        {
                            LogError( "InitDependencies failed: Name(%s)", SAFESTR( iface->GetName() ) );
                        }

                    }

                    else
                    {
                        LogError( "CheckDependencies failed: Name(%s)", SAFESTR( iface->GetName() ) );
                    }
                }
            }
        }
    }

    IPluginBase* CPluginManager::GetPluginByName( const char* sPluginName )
    {
        tPluginNameMap::iterator pluginIter = m_Plugins.find( sPluginName );

        if ( pluginIter != m_Plugins.end() )
        {
            return pluginIter->second.second;
        }

        return NULL;
    }

    void CPluginManager::DumpPlugin( const char* sPluginName )
    {
        IPluginBase* iface = GetPluginByName(  sPluginName );

        if ( iface )
        {
            LogAlways( "\r\nPlugin: Name(%s) Version(%s) Category(%s) Status(%s)", SAFESTR( iface->GetName() ), SAFESTR( iface->GetVersion() ), SAFESTR( iface->GetCategory() ), SAFESTR( iface->GetStatus() ) );
            LogAlways( "Authors: %s", SAFESTR( iface->ListAuthors() ) );
            LogAlways( "CVars: {%s}", SAFESTR( iface->ListCVars() ) );
            LogAlways( "Nodes: {%s}", SAFESTR( iface->ListNodes() ) );
            LogAlways( "GameObjects: {%s}", SAFESTR( iface->ListGameObjects() ) );
            LogAlways( "Interface: Concrete(%s) Extended(%s)", SAFESTR( iface->GetCurrentConcreteInterfaceVersion() ), SAFESTR( iface->GetCurrentExtendedInterfaceVersion() ) );
            LogAlways( "Flags: Unloading(%s) Initialized(%s) FullyInitialized(%s)", BOOLSTR( iface->IsUnloading() ) , BOOLSTR( iface->IsInitialized() ), BOOLSTR( iface->IsFullyInitialized() ) );
        }
    }

    void CPluginManager::DumpAllPlugins()
    {
        for ( tPluginNameMap::iterator pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
        {
            DumpPlugin( pluginIter->second.second->GetName() );
        }
    }

    void  CPluginManager::ListAllPlugins()
    {
        for ( tPluginNameMap::iterator pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
        {
            IPluginBase* iface = pluginIter->second.second;
            LogAlways( "%s: V(%s) C(%s) S(%s) U(%s) I(%s) FI(%s)", SAFESTR( iface->GetName() ), SAFESTR( iface->GetVersion() ), SAFESTR( iface->GetCategory() ), SAFESTR( iface->GetStatus() ), BOOLSTR( iface->IsUnloading() ) , BOOLSTR( iface->IsInitialized() ), BOOLSTR( iface->IsFullyInitialized() ) );
        }

        LogAlways( "Currently %d plugins are loaded!", m_Plugins.size() );
    }
}
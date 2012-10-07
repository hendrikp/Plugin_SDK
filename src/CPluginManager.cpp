/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginManager.h>

#define SAFESTR(x) (((const char*)x)?((const char*)x):"")
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

    bool CPluginManager::Release( bool bForce )
    {
        // Should be called while Game is still active otherwise there might be leaks/problems
        bool bRet = CPluginBase::Release( bForce );

        if ( bRet )
        {
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

            // Unregister listeners
            if ( gEnv && gEnv->pGameFramework )
            {
                gEnv->pGameFramework->UnregisterListener( this );
            }

            // Cleanup all plugins (special case only in manager...)
            UnloadAllPlugins();

            // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
            gPluginManager->UnloadPlugin( GetName() );
            m_bCanUnload = true;

            // special case only in manager...
            PluginGarbageCollector();
        }

        return bRet;
    };

    bool CPluginManager::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager )
    {
        // Save Startup parameters
        gStartupInitParams = startupParams;

        bool bRet = CPluginBase::Init( env, startupParams, gPluginManager->GetBase() );

        if ( bRet )
        {
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

            // Register Listeners
            if ( gEnv->pGameFramework )
            {
                gEnv->pGameFramework->RegisterListener( this, PLUGIN_NAME, eFLPriority_Default );
            }

            // Register the plugin manager itself (plugin manager only, plugins don't need to do this)
            LogAlways( "Plugin Manager initialized!" );

            m_Plugins[ GetName() ] = std::pair<HINSTANCE, IPluginBase*>( GetModuleHandle( PLUGIN_FILENAME ), GetBase() );
        }

        return bRet;
    }

    const char* CPluginManager::ListCVars() const
    {
        return "pm_list,\n" "pm_dump,\n" "pm_dumpall,\n" "pm_unload,\n" "pm_unloadall,\n" "pm_reload,\n" "pm_reloadall,\n";
    }

    bool CPluginManager::Check( const char* sAPIVersion ) const
    {
        if ( !sAPIVersion )
        {
            return false;
        }

        gsSDKInterfaceVersion = sAPIVersion;

        if ( SFileVersion( sAPIVersion ) == SFileVersion( "3.4.0" ) )
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

        PluginGarbageCollector();

        LogAlways( "Unloaded all plugins!" );
    };

    void CPluginManager::UnloadPlugin( const char* sPluginName )
    {
        if ( !sPluginName )
        {
            return;
        }

        tPluginNameMap::iterator pluginIter = m_Plugins.find( sPluginName );

        if ( pluginIter != m_Plugins.end() )
        {
            if ( !pluginIter->second.second->IsUnloading() )
            {
                LogAlways( "Trying to unload: Name(%s)", sPluginName );

                // Let the plugin first clean itself up
                pluginIter->second.second->Release();
                PluginGarbageCollector();
            }

            else
            {
                // Mark library for cleanup and remove from active plugin list (other plugins might still depend on it)
                m_UnloadingPlugins[pluginIter->first] = pluginIter->second;
                m_Plugins.erase( pluginIter );

                LogAlways( "Marked for unload: Name(%s)", sPluginName );
            }
        }

        else
        {
            LogWarning( "Nothing to unload." );
        }
    }

    // IGameFrameworkListener
    void CPluginManager::OnPostUpdate( float fDeltaTime )
    {
        static int nCounter = 1;

        // Don't collect garbage every frame
        if ( ++nCounter % 50 == 0 )
        {
            nCounter = 1;
            PluginGarbageCollector();
        }
    }

    void CPluginManager::PluginGarbageCollector()
    {
        if ( m_UnloadingPlugins.size() )
        {
            bool bUnloadedSomething = false;

            for ( tPluginNameMap::iterator pluginIter = m_UnloadingPlugins.begin(); pluginIter != m_UnloadingPlugins.end(); ++pluginIter )
            {
                if ( pluginIter->second.second->DllCanUnloadNow() )
                {
                    LogAlways( "Garbage Collector Unloading: Name(%s)", SAFESTR( pluginIter->second.second->GetName() ) );
                    CryFreeLibrary( pluginIter->second.first );
                    pluginIter = m_UnloadingPlugins.erase( pluginIter );
                    bUnloadedSomething = true;

                    if ( pluginIter == m_UnloadingPlugins.end() )
                    {
                        break;
                    }
                }
            }

            // A shame unregistering flownodes types still will produce errors later on in sandbox (when clicked on)
            // it would be much better if UnregisterTypes would automatically unload and set related flownodes to missing status.
            /*
            if ( bUnloadedSomething && gEnv && gEnv->pFlowSystem )
            {
                // Also this will unload all custom flownodes
                gEnv->pFlowSystem->ReloadAllNodeTypes();
            }*/
        }
    }

    void CPluginManager::ReloadAllPlugins()
    {
        ICryPak* pCryPak = gEnv->pCryPak;

        // The CryPak CWD seems to be Game
        string sPluginsPath;
#if defined(WIN64)
        sPluginsPath = ".." PATH_SEPERATOR "Bin64" PATH_SEPERATOR;
#elif defined(WIN32)
        sPluginsPath = ".." PATH_SEPERATOR "Bin32" PATH_SEPERATOR;
#endif

        sPluginsPath += PLUGIN_FOLDER;

        // create plugin directory if it doesnt exist
        if ( !pCryPak->IsFileExist( sPluginsPath.c_str() ) )
        {
            pCryPak->MakeDir( sPluginsPath.c_str() );
        }

        // Load Plugins from the main directory and then also recursively load subdirectories (max depth 1)
        LoadPluginsFromDirectory( sPluginsPath.c_str() );

        LogAlways( "Currently %d plugins are loaded!", m_Plugins.size() );
    }

    void CPluginManager::LoadPluginsFromDirectory( const char* sPath, int nDepth )
    {
        // This limits the search to one subdirectory under Plugins
        if ( nDepth > 1 || !sPath )
        {
            return;
        }

        ICryPak* pCryPak = gEnv->pCryPak;

        // Append Path Seperator if not present
        string sSearchDirectory = sPath;

        if ( sSearchDirectory.Right( 1 ).at( 0 ) != PATH_SEPERATOR[0] )
        {
            sSearchDirectory += PATH_SEPERATOR;
        }

        // Append wildcard search
        string sSearchFilter = sSearchDirectory;
        sSearchFilter += "*";

        string sPluginExtension = CrySharedLibrayExtension;

        LogAlways( "Loading plugins from '%s'", sSearchFilter.c_str() );

        _finddata_t fileData;

        intptr_t hFileFind = intptr_t( INVALID_HANDLE_VALUE );
        int hNextFile = int( INVALID_HANDLE_VALUE );

        // dll hasn't to be in a pak in fact shouldn't be since its not supported by CryLoadLibrary
        // so set AllowFilesystem = true
        for ( hNextFile = hFileFind = pCryPak->FindFirst( sSearchFilter, &fileData, 0, true );
                hNextFile != int( INVALID_HANDLE_VALUE );
                hNextFile = pCryPak->FindNext( hFileFind, &fileData ) )
        {
            // Don't work with . and ..
            if ( !fileData.name[0] || ( '.' == fileData.name[0] && !fileData.name[1] ) || ( '.' == fileData.name[0] && '.' == fileData.name[1] && !fileData.name[2] ) )
            {
                continue;
            }

            if ( fileData.attrib & FILE_ATTRIBUTE_DIRECTORY )
            {
                LoadPluginsFromDirectory( sSearchDirectory + fileData.name, nDepth + 1 );
            }

            else if ( !_stricmp( string( fileData.name ).Right( sPluginExtension.length() ), sPluginExtension ) )
            {
                ReloadPlugin( sSearchDirectory + fileData.name );
            }
        }

        pCryPak->FindClose( hFileFind );
    }

    HMODULE CPluginManager::LoadLibraryWithinOwnDirectory( const char* sPluginPath ) const
    {
        if ( !sPluginPath )
        {
            return NULL;
        }

        HINSTANCE hModule = NULL;

        // Save the current directory so we can reset it after loading plugins
        DWORD nPathLen = GetCurrentDirectory( 0, NULL );
        string sCurrentDirectory;
        {
            char* sTempPath = new char[nPathLen + 1];
            GetCurrentDirectory( nPathLen + 1, sTempPath );
            sCurrentDirectory = sTempPath;
            delete [] sTempPath;
        }

        // CryPak is used because its cross platform not because libraries can actually be in a pak.
        // We have to format the path a bit to work with the same data as crypak.
        string sAbsPluginDirectory = sCurrentDirectory;  // fallback cwd
        string sAbsPluginPath = sPluginPath; // fallback relative
        {
            string sCryPakPluginPath = sPluginPath;

            size_t nStartPos = sCryPakPluginPath.find_first_of( PATH_SEPERATOR );
            size_t nEndPos = sCryPakPluginPath.find_last_of( PATH_SEPERATOR );

            if ( nStartPos != string::npos && nEndPos != string::npos && nStartPos != nEndPos )
            {
                sAbsPluginDirectory += sCryPakPluginPath.Mid( nStartPos, nEndPos - nStartPos );
                sAbsPluginPath = sAbsPluginDirectory + sCryPakPluginPath.Mid( nEndPos );
            }

            else
            {
                LogError( "LoadLibraryWithinOwnDirectory invalid directory(%s)", sCryPakPluginPath.c_str() );
            }
        }

        // Log before modifying working directory, else log will be written into plugin directory
        LogAlways( "Loading: File(%s) CWD(%s)", sAbsPluginPath.c_str(), sAbsPluginDirectory.c_str() );

        // Better not change the current directory (SetCurrentDirectory)
        // (threadsafety: it's used internally for cachefile/logs which would be created sporadically in plugin directory)
        // better use SetDllDirectory so we can still handle plugin dependencies properly.
        nPathLen = GetDllDirectory( 0, NULL );
        string sDllDirectory;
        {
            char* sTempPath = new char[nPathLen + 1];
            GetDllDirectory( nPathLen + 1, sTempPath );
            sDllDirectory = sTempPath;
            delete [] sTempPath;
        }
        SetDllDirectory( sAbsPluginDirectory );

        // Load the library and non lazy linked dependencies
        hModule = CryLoadLibrary( sPluginPath );

        // Reset the dll directory to old value
        SetDllDirectory( sDllDirectory );

        return hModule;
    }

    bool CPluginManager::ReloadPlugin( const char* sPluginPath, bool bInitialize )
    {
        HINSTANCE hModule = LoadLibraryWithinOwnDirectory( sPluginPath );

        // Plugin link library found?
        if ( hModule )
        {
            // Check if its a plugin
            void* fptr = CryGetProcAddress( hModule, "GetPluginInterface" );

            if ( fptr )
            {
                // its a plugin create the baseinterface
                IPluginBase* iface = fGetPluginInterface( fptr )( gsBaseInterfaceVersion );

                // Don't reload PluginManager...
                if ( iface == GetBase() )
                {
                    CryFreeLibrary( hModule );
                    return true;
                }

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
                                LogAlways( "Loaded: Name(%s) Version(%s) Category(%s)", sPluginName.c_str(), SAFESTR( iface->GetVersion() ), SAFESTR( iface->GetCategory() ) );

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
                            LogError( "Plugin has no name" );
                        }
                    }

                    else
                    {
                        LogError( "CryEngine SDK interface version(%s) not compatible", SAFESTR( gsSDKInterfaceVersion.c_str() ) );
                    }
                }

                else
                {
                    LogError( "Plugin SDK base interface version(%s) not compatible", SAFESTR( gsBaseInterfaceVersion.c_str() ) );
                }
            }

            else
            {
                LogError( "Not a plugin, GetPluginInterface signature not present" );
            }

            CryFreeLibrary( hModule );
        }

        else
        {
            LogError( "Path(%s) not found or dependencies not present", SAFESTR( sPluginPath ) );
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
            LogAlways( "\nPlugin: Name(%s) Version(%s) Category(%s) Status(%s)", SAFESTR( iface->GetName() ), SAFESTR( iface->GetVersion() ), SAFESTR( iface->GetCategory() ), SAFESTR( iface->GetStatus() ) );
            LogAlways( "   Authors: %s", SAFESTR( iface->ListAuthors() ) );
            LogAlways( "   CVars: {%s}", SAFESTR( iface->ListCVars() ) );
            LogAlways( "   Nodes: {%s}", SAFESTR( iface->ListNodes() ) );
            LogAlways( "   GameObjects: {%s}", SAFESTR( iface->ListGameObjects() ) );
            LogAlways( "   Interface: Concrete(%s) Extended(%s)", SAFESTR( iface->GetCurrentConcreteInterfaceVersion() ), SAFESTR( iface->GetCurrentExtendedInterfaceVersion() ) );
            LogAlways( "   Flags: Unloading(%s) Initialized(%s) FullyInitialized(%s)", BOOLSTR( iface->IsUnloading() ) , BOOLSTR( iface->IsInitialized() ), BOOLSTR( iface->IsFullyInitialized() ) );
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
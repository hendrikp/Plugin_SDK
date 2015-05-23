/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginManager.h>
#include <PMUtils.hpp>

#include <mhook.h>
#include <HookTool.hpp>

#define COMMAND_LIST        "pm_list"
#define COMMAND_LISTSI      "pm_listsi"
#define COMMAND_DUMP        "pm_dump"
#define COMMAND_DUMPALL     "pm_dumpall"
#define COMMAND_UNLOAD      "pm_unload"
#define COMMAND_UNLOADALL   "pm_unloadall"
#define COMMAND_RELOAD      "pm_reload"
#define COMMAND_RELOADALL   "pm_reloadall"
#define COMMAND_DELAY       "pm_delay"
#define COMMAND_DELAYCANCEL "pm_delaycancel"

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

    void Command_ListStaticInterfaces( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager )
        {
            return;
        }

        gPluginManager->ListStaticInterfaces();
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

        string sPath = pArgs->GetArg( 1 );

        if ( !isAbsolute( sPath ) )
        {
            sPath = pathWithSeperator( gPluginManager->GetDirectoryPlugins() ) + sPath;
        }

        gPluginManager->ReloadPlugin( sPath, true );
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

    void Command_Delay( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager )
        {
            return;
        }


        // Syntax: <filter> <type (1:frames, 2:seconds)> <amount> <commands...>
        //         0        1                             2       3...

        // Get Commands to delay
#undef GetCommandLine
        string sCommandLine( pArgs->GetCommandLine() );
        int nLen = sCommandLine.length();
        int nLenCmd = strlen( COMMAND_DELAY " " );

        // Is text present?
        if ( nLen <= nLenCmd || pArgs->GetArgCount() < 5 )
        {
            return;
        }

        // remove command
        string sText = sCommandLine.Mid( nLenCmd );

        // read over the parameters
        size_t nOffset = sText.find_first_of( ' ' )   + 1;
        nOffset = sText.find_first_of( ' ', nOffset ) + 1;
        nOffset = sText.find_first_of( ' ', nOffset ) + 1;

        // remove paramters
        sText = sText.Mid( nOffset ).Trim();

        // delay the command
        if ( sText.length() > 0 )
        {
            gPluginManager->DelayCommand( sText, pArgs->GetArg( 1 ), ParseString<float>( pArgs->GetArg( 3 ) ), eDelayType( ParseString<int>( pArgs->GetArg( 2 ) ) ) );
        }
    };

    void Command_DelayCancel( IConsoleCmdArgs* pArgs )
    {
        if ( !gPluginManager || pArgs->GetArgCount() < 1 )
        {
            // don't allow console command to cancel all filters
            return;
        }

        gPluginManager->DelayCancel( pArgs->GetArg( 1 ) );
    }

    void CPluginManager::DelayFunction( const char* sFilter, tDelayedCall pFunc, tDelayedCall pFuncCleanup, void* pData, float fDelay, int eType, tDelayedCallTrigger pFuncTrigger, tDelayedCall pFuncTriggerCleanup, void* pDataTrigger )
    {
        GetDelayQueue().DelayFunction( sFilter, pFunc, pFuncCleanup, pData, fDelay, eDelayType( eType ), pFuncTrigger, pFuncTriggerCleanup, pDataTrigger );
    };

    void CPluginManager::DelayCommand( const char* sCommand, const char* sFilter, float fDelay, int eType, tDelayedCallTrigger pFuncTrigger, tDelayedCall pFuncTriggerCleanup, void* pDataTrigger )
    {
        DelayConsoleCommand( GetDelayQueue(), SAFESTR( sCommand ), sFilter, fDelay, eDelayType( eType ), pFuncTrigger, pFuncTriggerCleanup, pDataTrigger );
    };

    void CPluginManager::DelayCancel( const char* sFilter )
    {
        GetDelayQueue().Cancel( sFilter );
    };

    /**
    * @internal
    * @brief Delayed Lua Code
    * @param sCode to execute
    */
    static void _DelayedLuaCode( void* sCode )
    {
        if ( sCode && gPluginManager )
        {
            gPluginManager->RunLua( ( const char* )sCode );
        }
    }

    void CPluginManager::DelayLua( const char* sCode, const char* sFilter, float fDelay, int eType, tDelayedCallTrigger pFuncTrigger, tDelayedCall pFuncTriggerCleanup, void* pDataTrigger )
    {
        string sLua = SAFESTR( sCode );

        if ( sLua.length() > 0 )
        {
            // Prepare data
            size_t nLen = sLua.length() + 1;
            char* sCmd = new char[nLen];
            strncpy( sCmd, sLua.c_str(), nLen );
            sCmd[nLen - 1] = 0;

            const char* sFilterC = ( sFilter && sFilter[0] ) ? sFilter : "luacode";

            GetDelayQueue().DelayFunction( sFilter, &_DelayedLuaCode, &_CleanupDelayedConsoleCommand, sCmd, fDelay, eDelayType( eType ), pFuncTrigger, pFuncTriggerCleanup, pDataTrigger );
        }
    };

    CPluginManager::CPluginManager()
    {
        gPluginManager = this;

        m_sPluginsDirectory = "";
        m_sBinaryDirectory = "";
        m_sRootDirectory = "";
        m_sGameDirectory = "";
        m_sUserDirectory = "";
    }

    CPluginManager::~CPluginManager()
    {
        Release( true );

        gPluginManager = NULL;
    }

    typedef void ( *updateFunc )( IEntitySystem* );
    static updateFunc fpoUpdateFunc = nullptr;
    void __stdcall hookedUpdate( IEntitySystem* THIS )
    {
        if ( gEnv->IsEditor() && gEnv->IsEditing() )
        {
            gPluginManager->OnPostUpdate( gEnv->pTimer->GetFrameTime() );
        }

        fpoUpdateFunc( THIS );
    }

    void hookUpdate( bool bHook )
    {
        int nNo = VTableIndex<IEntitySystem>( &IEntitySystem::Update );

        if ( bHook )
        {
            void** vt = getVT( gEnv->pEntitySystem );
            fpoUpdateFunc = ( updateFunc ) vt[nNo];
            Mhook_SetHook( ( PVOID* )&fpoUpdateFunc, hookedUpdate );
        }

        else if ( fpoUpdateFunc != nullptr )
        {
            Mhook_Unhook( ( PVOID* )&fpoUpdateFunc );
            fpoUpdateFunc = nullptr;
        }
    }

    bool CPluginManager::Release( bool bForce )
    {
        bool bRet = true;
        bool bWasInitialized = m_bIsFullyInitialized; // Will be reset by base class so backup

        if ( !m_bCanUnload )
        {
            // Should be called while Game is still active otherwise there might be leaks/problems
            bRet = CPluginBase::Release( bForce );

            if ( bRet )
            {
                // Remove update hook
                hookUpdate( false );

                // Unregister listeners
                if ( gEnv && !IsBadReadPtr( gEnv, sizeof( void* ) ) && gEnv->pSystem && !IsBadReadPtr( gEnv->pSystem, sizeof( void* ) ) )
                {
                    if ( gEnv->pGame && gEnv->pGame->GetIGameFramework() )
                    {
                        gEnv->pGame->GetIGameFramework()->UnregisterListener( this );
                    }

                    if ( gEnv->pSystem->GetISystemEventDispatcher() )
                    {
                        gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener( this );
                    }
                }

                // Cleanup all plugins (special case only in manager...)
                UnloadAllPlugins();

                // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
                gPluginManager->UnloadPlugin( GetName() );

                // Allow Plugin Manager garbage collector to unload this plugin
                AllowDllUnload();

                // Special case only in manager...
                PluginGarbageCollector();
            }
        }

        return bRet;
    };

    void CPluginManager::RefreshPaths()
    {
        // Save the current directory so we can reset it after loading plugins
        DWORD nPathLen = GetCurrentDirectory( 0, NULL );
        string sCurrentDirectory;
        {
            char* sTempPath = new char[nPathLen + 1];
            GetCurrentDirectory( nPathLen + 1, sTempPath );
            sCurrentDirectory = sTempPath;
            delete [] sTempPath;
        }

        // The root should be the current directory
        m_sRootDirectory = sCurrentDirectory;

        // The binary directory depends on dll type
        m_sBinaryDirectory = sCurrentDirectory;
        m_sBinaryDirectory += PATH_SEPERATOR "Bin";

#if defined(WIN64)
        m_sBinaryDirectory += "64";
#elif defined(WIN32)
        m_sBinaryDirectory += "32";
#else
        // Add platforms
#endif
        // The plugins folders lies within binary directory
        m_sPluginsDirectory = m_sBinaryDirectory + PATH_SEPERATOR + PLUGIN_FOLDER;

        assert( gEnv && gEnv->pCryPak );

        // create plugin directory if it doesn't exist
        if ( !gEnv->pCryPak->IsFileExist( m_sPluginsDirectory ) )
        {
            gEnv->pCryPak->MakeDir( m_sPluginsDirectory );
        }

        // User directory depends on various settings so query current setting from crypak
        m_sUserDirectory = gEnv->pCryPak->GetAlias( "%USER%", false );

        if ( !isAbsolute( m_sUserDirectory ) )
        {
            m_sUserDirectory = m_sRootDirectory + PATH_SEPERATOR + m_sUserDirectory;
        }

        // The game directory also can be influenced so query current setting from crypak
        m_sGameDirectory = gEnv->pCryPak->GetGameFolder();

        if ( !isAbsolute( m_sGameDirectory ) )
        {
            m_sGameDirectory = m_sRootDirectory + PATH_SEPERATOR + m_sGameDirectory;
        }
    };

    bool CPluginManager::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory )
    {
        // Save Startup parameters
        gStartupInitParams = startupParams;

        bool bRet = CPluginBase::Init( env, startupParams, gPluginManager->GetBase(), sPluginDirectory );

        if ( bRet )
        {
            if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
            {
                // Register Listeners
                if ( gEnv && gEnv->pGame && gEnv->pGame->GetIGameFramework() )
                {
                    hookUpdate( true );
                    gEnv->pGame->GetIGameFramework()->RegisterListener( this, PLUGIN_NAME, eFLPriority_Default );
                }

                if ( gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher() )
                {
                    gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener( this );
                }
            }

            else
            {
                LogError( "ISystem == NULL" );
            }

            // Initialize path informations
            RefreshPaths();

            // Register the plugin manager itself (plugin manager only, plugins don't need to do this)
            LogAlways( "Plugin Manager initialized!" );

            m_Plugins[ GetName() ] = SPluginInfo( GetBase(), GetModuleHandle( PLUGIN_FILENAME ), PLUGIN_FILENAME, m_sPluginsDirectory );
        }

        return bRet;
    }

    bool CPluginManager::RegisterTypes( int nFactoryType, bool bUnregister )
    {
        // Note: Autoregister Flownodes will be automatically registered by the Base class
        bool bRet = CPluginBase::RegisterTypes( nFactoryType, bUnregister );

        eFactoryType enFactoryType = eFactoryType( nFactoryType );

        if ( bRet )
        {
            if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
            {
                // CVars Commands
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_CVarCommand ) )
                {
                    if ( !bUnregister )
                    {
                        gEnv->pConsole->AddCommand( COMMAND_LIST, Command_ListAll, VF_NULL, "List one info row for all plugins" );
                        gEnv->pConsole->AddCommand( COMMAND_LISTSI, Command_ListStaticInterfaces, VF_NULL, "List all registered static interfaces" );
                        gEnv->pConsole->AddCommand( COMMAND_DUMP, Command_Dump, VF_NULL, "Dump info on a specific plugins" );
                        gEnv->pConsole->AddCommand( COMMAND_DUMPALL, Command_DumpAll, VF_NULL, "Dump info on all plugins" );
                        gEnv->pConsole->AddCommand( COMMAND_UNLOAD, Command_Unload, VF_NULL, "Unload a specific plugin using its name [DANGEROUS]" );
                        gEnv->pConsole->AddCommand( COMMAND_UNLOADALL, Command_UnloadAll, VF_NULL, "Unload all plugins with the exception of the plugin manager [DANGEROUS]" );
                        gEnv->pConsole->AddCommand( COMMAND_RELOAD, Command_Reload, VF_NULL, "Reload a specific plugin using its path [DANGEROUS if already loaded]" );
                        gEnv->pConsole->AddCommand( COMMAND_RELOADALL, Command_ReloadAll, VF_NULL, "Reload all plugins [DANGEROUS]" );
                        gEnv->pConsole->AddCommand( COMMAND_DELAY, Command_Delay, VF_NULL, "Delays a commands | " COMMAND_DELAY " <filter> <type (1:frames, 2:seconds)> <amount> <commands...>" );
                        gEnv->pConsole->AddCommand( COMMAND_DELAYCANCEL, Command_DelayCancel, VF_NULL, "Delays a commands | " COMMAND_DELAYCANCEL " <filter>" );
                    }

                    else
                    {
                        gEnv->pConsole->RemoveCommand( COMMAND_LIST );
                        gEnv->pConsole->RemoveCommand( COMMAND_LISTSI );
                        gEnv->pConsole->RemoveCommand( COMMAND_DUMP );
                        gEnv->pConsole->RemoveCommand( COMMAND_DUMPALL );
                        gEnv->pConsole->RemoveCommand( COMMAND_UNLOAD );
                        gEnv->pConsole->RemoveCommand( COMMAND_UNLOADALL );
                        gEnv->pConsole->RemoveCommand( COMMAND_RELOAD );
                        gEnv->pConsole->RemoveCommand( COMMAND_RELOADALL );
                        gEnv->pConsole->RemoveCommand( COMMAND_DELAY );
                        gEnv->pConsole->RemoveCommand( COMMAND_DELAYCANCEL );
                    }
                }

                // Lua functionality
                if ( gEnv->pGame && gEnv->pGame->GetIGameFramework() && ( enFactoryType == FT_All || enFactoryType == FT_None ) )
                {
                    if ( !bUnregister )
                    {
                        if ( gEnv->pScriptSystem )
                        {
                            const char* sLuaHandlers = \
                                                       "-- Interprets Lua Logic\n"\
                                                       "function " PM_LUA_TESTLOGIC "(logic)\n"\
                                                       "    return assert(loadstring(logic, \"pm-logic\"))()\n"\
                                                       "end\n"\

                                                       "-- Runs Lua Code\n"\
                                                       "function " PM_LUA_RUN "(command)\n"\
                                                       "    assert(loadstring(command, \"pm-run\"))()\n"\
                                                       "end\n";

                            if ( !gEnv->pScriptSystem->ExecuteBuffer( sLuaHandlers, strlen( sLuaHandlers ), "pmLuaHandlers" ) )
                            {
                                LogError( "ExecuteBuffer failed, couldn't register Lua handlers" );
                            }
                        }

                        else
                        {
                            LogError( "IScriptSystem == NULL" );
                        }
                    }
                }
            }
        }

        return bRet;
    }

    const char* CPluginManager::ListCVars() const
    {
        return COMMAND_LIST",\n" COMMAND_LISTSI",\n" COMMAND_DUMP",\n" COMMAND_DUMPALL",\n" COMMAND_UNLOAD",\n" COMMAND_UNLOADALL",\n" COMMAND_RELOAD",\n" COMMAND_RELOADALL",\n" COMMAND_DELAY",\n" COMMAND_DELAYCANCEL",\n";
    }

    bool CPluginManager::Check( const char* sAPIVersion ) const
    {
        if ( !sAPIVersion )
        {
            return false;
        }

        gsSDKInterfaceVersion = sAPIVersion;

        return CPluginBase::Check( sAPIVersion );
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
                    if ( pluginIter->second.m_pBase && pluginIter->second.m_pBase != GetBase() && pluginIter->second.m_pBase->GetInitializationMode() == nMode )
                    {
                        bEndReached = false;
                        UnloadPlugin( pluginIter->second.m_pBase->GetName() );
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
            if ( pluginIter->second.m_pBase && !pluginIter->second.m_pBase->IsUnloading() )
            {
                LogAlways( "Trying to unload: Name(%s)", sPluginName );

                // Let the plugin first clean itself up
                pluginIter->second.m_pBase->Release();
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

    // ISystemEventListener
    void CPluginManager::OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam )
    {
        static bool bQuitOnce = false;
        bool bQuit = !( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() );

        // Detect quit before plugin dlls get unloaded in undefined order
        if ( event == ESYSTEM_EVENT_SHUTDOWN || event == ESYSTEM_EVENT_LEVEL_POST_UNLOAD && bQuit )
        {
            if ( !bQuitOnce )
            {
                bQuitOnce = true;
                LogAlways( "System Shutdown detected, unloading all plugins!" );
                UnloadAllPlugins();
                OnPostUpdate( 0 ); // call our gc

#if CDK_VERSION >= 350 // In new CDK version gEnv is already invalid in destructor
                Release( true );
                gPluginManager = NULL;
#endif
            }
        }
    }

    // IGameFrameworkListener
    void CPluginManager::OnPostUpdate( float fDeltaTime )
    {
        static int nCounter = 1;
        bool bQuit = !( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() );

        // Advance delayed calls
        if ( !bQuit )
        {
            m_qDelayedCalls.AdvanceAll( fDeltaTime );
        }

        // Don't collect garbage every frame
        if ( bQuit || ++nCounter % 50 == 0 )
        {
            nCounter = 1;
            PluginGarbageCollector();
        }
    }

    void CPluginManager::PluginGarbageCollector()
    {
        bool bQuit = false;

        if ( gEnv && !IsBadReadPtr( gEnv, sizeof( void* ) ) && gEnv->pSystem && !IsBadReadPtr( gEnv->pSystem, sizeof( void* ) ) )
        {
            bQuit = gEnv->pSystem->IsQuitting();
        }

        else
        {
            bQuit = true;
        }

        // Something to do?
        if ( bQuit )
        {
            // Don't unload dlls on quit while a map is still loaded (because flow node types are still in use even if unregistered or unused)
            // instead let the plugins handle their cleanup but keep the plugin dll loaded on quit until they get automatically unloaded.
            m_UnloadingPlugins.clear(); // but clean up this now, else problems with auto destructor map/CryString (maybe already unloaded at this point)

            // this worked very well for all of my plugins in all cases (quit in main menu, quit in level and quit using the menu or X)
        }

        else if ( m_UnloadingPlugins.size() )
        {
            bool bUnloadedSomething = false;

            // Check all plugins marked for unloading if they can now unload
            for ( tPluginNameMap::iterator pluginIter = m_UnloadingPlugins.begin(); pluginIter != m_UnloadingPlugins.end(); ++pluginIter )
            {
                if ( pluginIter->second.m_pBase && pluginIter->second.m_pBase->DllCanUnloadNow() )
                {
                    LogAlways( "Garbage Collector Unloading: Name(%s)", SAFESTR( pluginIter->second.m_pBase->GetName() ) );
                    CryFreeLibrary( pluginIter->second.m_hModule );
                    bUnloadedSomething = true;

                    // Cleanup the plugin from the unloading plugin container
                    pluginIter = m_UnloadingPlugins.erase( pluginIter );

                    if ( pluginIter == m_UnloadingPlugins.end() )
                    {
                        break;
                    }
                }
            }

            // A shame unregistering flownodes types still will produce errors later on in sandbox (when clicked on)
            // it would be much better if UnregisterTypes would automatically unload and set related flownodes to missing status.
            /*
            // Takes a lot of time and unregisters all nodes in part from places where they can't be registered again.
            if ( bUnloadedSomething && gEnv && gEnv->pFlowSystem )
            {
                // Also this will unload all custom flownodes
                gEnv->pFlowSystem->ReloadAllNodeTypes();
            }
            */
        }
    }

    void CPluginManager::ReloadAllPlugins()
    {
        ICryPak* pCryPak = gEnv->pCryPak;

        // Load Plugins from the main directory and then also recursively load subdirectories (max depth 1)
        LoadPluginsFromDirectory( m_sPluginsDirectory );

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

        // Append Path Separator if not present
        string sSearchDirectory = pathWithSeperator( sPath );

        // Append wild card search
        string sSearchFilter = sSearchDirectory;
        sSearchFilter += "*";

        string sPluginExtension = CrySharedLibrayExtension;

        LogAlways( "Loading plugins from '%s'", sSearchFilter.c_str() );

        _finddata_t fileData;

        intptr_t hFileFind = intptr_t( INVALID_HANDLE_VALUE );
        int hNextFile = int( INVALID_HANDLE_VALUE );

        // CryPak is used because its cross platform not because libraries can actually be in a pak.
        // In fact they shouldn't be since its not supported by CryLoadLibrary
        // so set AllowFilesystem = true

        // keep
        //const int nDontMessWithPath = ICryPak::FLAGS_PATH_REAL | ICryPak::FLAGS_NO_LOWCASE | ICryPak::FLAGS_NEVER_IN_PAK | ICryPak::FLAGS_COPY_DEST_ALWAYS;

        // findfirst uses utf-8 (who would have guessed :P)
        for ( hNextFile = hFileFind = pCryPak->FindFirst( ACP2UTF8( sSearchFilter ), &fileData, 0, true );
                hNextFile != int( INVALID_HANDLE_VALUE );
                hNextFile = pCryPak->FindNext( hFileFind, &fileData ) )
        {
            // Don't work with . and ..
            if ( isEmpty( fileData.name ) || isDot( fileData.name ) )
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

        if ( hFileFind != int( INVALID_HANDLE_VALUE ) )
        {
            pCryPak->FindClose( hFileFind );
        }

        else
        {
            LogError( "Plugin path not found, maybe non ASCII characters in path?" );
        }
    }

    HMODULE CPluginManager::LoadLibraryWithinOwnDirectory( const char* sPluginPath ) const
    {
        if ( !sPluginPath )
        {
            return NULL;
        }

        HINSTANCE hModule = NULL;

        string sAbsPluginDirectory = pathBaseName( sPluginPath );

        // Log before modifying working directory, else log will be written into plugin directory
        LogAlways( "Loading: File(%s) CWD(%s)", sPluginPath, sAbsPluginDirectory.c_str() );

        // Better not change the current directory (SetCurrentDirectory)
        // (thread safety: it's used internally for cache file/logs which would be created sporadically in plugin directory)
        // better use SetDllDirectory so we can still handle plugin dependencies properly.
        int nPathLen = GetDllDirectory( 0, NULL );
        string sDllDirectory = "";

        if ( nPathLen > 1 )
        {
            char* sTempPath = new char[nPathLen + 1];
            GetDllDirectory( nPathLen + 1, sTempPath );
            sDllDirectory = sTempPath;
            delete [] sTempPath;
        }

        SetDllDirectory( sAbsPluginDirectory );

        // Load the library and non lazy linked dependencies
        hModule = CryLoadLibrary( sPluginPath );

        if ( !hModule )
        {
            LogError( "CryLoadLibrary(%s) OS reports error: %s", SAFESTR( sPluginPath ), SAFESTR( getOSError() ) );
        }

        // Reset the dll directory to old value
        if ( sDllDirectory.length() )
        {
            SetDllDirectory( sDllDirectory );
        }

        else
        {
            // Its important to set it to NULL if last was empty, else Sandbox skinning wont work (probably windows defaults to another search mode then)
            SetDllDirectory( NULL );
        }

        return hModule;
    }

    bool CPluginManager::ReloadPlugin( const char* sPluginPath, bool bInitialize )
    {
        HINSTANCE hModule = LoadLibraryWithinOwnDirectory( sPluginPath );

        // Plugin link library found?
        if ( hModule )
        {
            // Check if its a plugin
            void* fptr = CryGetProcAddress( hModule, PLUGIN_ENTRYPOINT );

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

                                m_Plugins[sPluginName] = SPluginInfo( iface, hModule, pathFileName( sPluginPath ), pathBaseName( sPluginPath ) );

                                if ( bInitialize )
                                {
                                    return InitializePlugin( sPluginName );
                                }

                                return true;
                            }

                            // Plugin was already loaded so reload it
                            UnloadPlugin( sPluginName );
                            return ReloadPlugin( sPluginPath );
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
                LogError( "Not a plugin, " PLUGIN_ENTRYPOINT " signature not present" );
            }

            CryFreeLibrary( hModule );
        }

        else
        {
            LogError( "Path(%s) not found or dependencies not present or error in DllMain", SAFESTR( sPluginPath ) );
        }

        return false;
    }

    bool CPluginManager::InitializePlugin( const char* sPluginName )
    {
        tPluginNameMap::iterator pluginIter = m_Plugins.find( sPluginName );

        if ( pluginIter != m_Plugins.end() )
        {
            IPluginBase* iface = pluginIter->second.m_pBase;

            if ( iface )
            {
                if ( !iface->IsInitialized() ) // Initialize plugins in order
                {
                    if ( !iface->Init( *gEnv, gStartupInitParams, GetBase(), pluginIter->second.m_sDirectory ) )
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

            else
            {
                LogError( "BaseInterface not present: Name(%s)", SAFESTR( sPluginName ) );
            }
        }

        return false;
    }

    void CPluginManager::InitializePluginRange( int nBeginAtMode, int nEndAtMode )
    {
        for ( int nMode = nBeginAtMode; nMode <= nEndAtMode; ++nMode )
        {
            // First initialize all in this dependency level
            for ( auto pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
            {
                IPluginBase* iface = pluginIter->second.m_pBase;

                if ( iface && iface->GetInitializationMode() == nMode && !iface->IsInitialized() ) // Initialize plugins in order
                {
                    if ( !iface->Init( *gEnv, gStartupInitParams, GetBase(), pluginIter->second.m_sDirectory ) )
                    {
                        LogError( "Init failed: Name(%s)", SAFESTR( iface->GetName() ) );
                    }
                }
            }

            // Now initialize the dependencies
            for ( auto pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
            {
                IPluginBase* iface = pluginIter->second.m_pBase;

                if ( iface && iface->GetInitializationMode() == nMode && iface->IsInitialized() && !iface->IsFullyInitialized() ) // Initialize plugins in order
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

    bool CPluginManager::RegisterTypesPluginRange( int nBeginAtMode, int nEndAtMode, int nFactoryType , bool bUnregister )
    {
        bool bRet = true;

        for ( int nMode = nBeginAtMode; nMode <= nEndAtMode; ++nMode )
        {
            for ( auto pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
            {
                IPluginBase* iface = pluginIter->second.m_pBase;

                if ( iface && iface->GetInitializationMode() == nMode && iface->IsInitialized() && iface->IsFullyInitialized() )
                {
                    // Only Register Types in fully initialized plugins
                    bRet &= iface->RegisterTypes( nFactoryType, bUnregister );
                }
            }
        }

        return bRet;
    }

    IPluginBase* CPluginManager::GetPluginByName( const char* sPluginName ) const
    {
        if ( !sPluginName )
        {
            return NULL;
        }

        auto pluginIter = m_Plugins.find( sPluginName );

        if ( pluginIter != m_Plugins.end() )
        {
            return pluginIter->second.m_pBase;
        }

        return NULL;
    }

    const char* CPluginManager::GetPluginDirectory( const char* sPluginName ) const
    {
        if ( !sPluginName )
        {
            return NULL;
        }

        tPluginNameMap::const_iterator pluginIter = m_Plugins.find( sPluginName );

        if ( pluginIter != m_Plugins.end() )
        {
            return pluginIter->second.m_sDirectory;
        }

        return NULL;
    };

    void CPluginManager::DumpPlugin( const char* sPluginName ) const
    {
        IPluginBase* iface = GetPluginByName( sPluginName );

        auto pluginIter = m_Plugins.find( sPluginName );

        if ( iface && pluginIter != m_Plugins.end() )
        {
            LogAlways( "\nPlugin: Name(%s) Version(%s) Category(%s) Status(%s)", SAFESTR( iface->GetName() ), SAFESTR( iface->GetVersion() ), SAFESTR( iface->GetCategory() ), SAFESTR( iface->GetStatus() ) );
            LogAlways( "   Authors: %s", SAFESTR( iface->ListAuthors() ) );
            LogAlways( "   CVars: {%s}", SAFESTR( iface->ListCVars() ) );
            LogAlways( "   Nodes: {%s}", SAFESTR( iface->ListNodes() ) );
            LogAlways( "   GameObjects: {%s}", SAFESTR( iface->ListGameObjects() ) );
            LogAlways( "   Interface: Concrete(%s) Extended(%s)", SAFESTR( iface->GetCurrentConcreteInterfaceVersion() ), SAFESTR( iface->GetCurrentExtendedInterfaceVersion() ) );
            LogAlways( "   Flags: Unloading(%s) Initialized(%s) FullyInitialized(%s)", BOOLSTR( iface->IsUnloading() ) , BOOLSTR( iface->IsInitialized() ), BOOLSTR( iface->IsFullyInitialized() ) );
            LogAlways( "   Module: Madr(%p) BIadr(%p) File(%s) Directory(%s)", ( void* )pluginIter->second.m_hModule, ( void* )iface, SAFESTR( pluginIter->second.m_sFile.c_str() ), SAFESTR( pluginIter->second.m_sDirectory.c_str() ) );
            LogAlways( "   Dump(%s)", SAFESTR( iface->Dump() ) );
        }
    }

    void CPluginManager::DumpAllPlugins() const
    {
        for ( auto pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
        {
            DumpPlugin( pluginIter->second.m_pBase->GetName() );
        }
    }

    void CPluginManager::ListAllPlugins() const
    {
        for ( auto pluginIter = m_Plugins.begin(); pluginIter != m_Plugins.end(); ++pluginIter )
        {
            IPluginBase* iface = pluginIter->second.m_pBase;

            if ( iface )
            {
                LogAlways( "%s: V(%s) C(%s) S(%s) U(%s) I(%s) FI(%s) F(%s) D(%s) M(%p) B(%p)", SAFESTR( iface->GetName() ), SAFESTR( iface->GetVersion() ), SAFESTR( iface->GetCategory() ), SAFESTR( iface->GetStatus() ), BOOLSTR( iface->IsUnloading() ) , BOOLSTR( iface->IsInitialized() ), BOOLSTR( iface->IsFullyInitialized() ), SAFESTR( pluginIter->second.m_sFile.c_str() ), SAFESTR( pluginIter->second.m_sDirectory.c_str() ), ( void* )pluginIter->second.m_hModule, ( void* )iface );
            }

            else
            {
                LogError( "Plugin found without base interface F(%s) D(%s) M(%p)", SAFESTR( pluginIter->second.m_sFile.c_str() ), SAFESTR( pluginIter->second.m_sDirectory.c_str() ), ( void* )pluginIter->second.m_hModule );
            }
        }

        LogAlways( "Currently %d plugins are loaded!", m_Plugins.size() );
    }

    void CPluginManager::ListStaticInterfaces() const
    {
        for ( auto interfaceIter = m_StaticInterfaces.begin(); interfaceIter != m_StaticInterfaces.end(); ++interfaceIter )
        {
            LogAlways( "%s: V(%s) P(%p)", SAFESTR( interfaceIter->first.first.c_str() ), SAFESTR( interfaceIter->first.second.c_str() ), ( void* )interfaceIter->second );
        }

        LogAlways( "Currently %d static interfaces are registered!", m_StaticInterfaces.size() );
    }

    void* CPluginManager::GetStaticInterface( const char* sName, const char* sVersion ) const
    {
        if ( !sName )
        {
            return NULL;
        }

        auto iterFind = m_StaticInterfaces.find( tStaticInterfaceKey( sName, SAFESTR( sVersion ) ) );

        if ( iterFind != m_StaticInterfaces.end() )
        {
            return ( *iterFind ).second;
        }

        LogWarning( "GetStaticInterface Name(%s) Version(%s) not available", sName, SAFESTR( sVersion ) );
        return NULL;
    }

    void CPluginManager::RegisterStaticInterface( void* pInterface, const char* sName, const char* sVersion )
    {
        if ( !sName || !pInterface )
        {
            return;
        }

        m_StaticInterfaces[ tStaticInterfaceKey( sName, SAFESTR( sVersion ) ) ] = pInterface;

        LogAlways( "RegisterStaticInterface Name(%s) Version(%s) succeeded", sName, SAFESTR( sVersion ) );
    }

    string WrapLuaCode( const string& sCode, bool bCondition )
    {
        string sRet = "do\n";

        if ( bCondition )
        {
            sRet += "return (";
            sRet += sCode;
            sRet += ")";
        }

        else
        {
            sRet += sCode;
            sRet += "\nreturn true";
        }

        sRet += "\nend";

        return sRet;
    }

    bool CPluginManager::TestLuaLogic( const char* sLogic )
    {
        bool bReturn = false;

        string sExpression = SAFESTR( sLogic );
        sExpression = sExpression.Trim();

        if ( sExpression.length() > 0 )
        {
            if ( gEnv->pScriptSystem->BeginCall( PM_LUA_TESTLOGIC ) )
            {
                gEnv->pScriptSystem->PushFuncParam<const char*>( WrapLuaCode( sExpression, true ) );

                if ( !gEnv->pScriptSystem->EndCall<bool>( bReturn ) )
                {
                    LogError( "Lua Logic EndCall failed, maybe Logic or Type error: %s", sExpression.c_str() );
                }
            }

            else
            {
                LogError( "Lua Logic BeginCall failed" );
            }
        }

        else
        {
            LogError( "Lua Logic empty" );
        }

        return bReturn;
    }

    bool CPluginManager::RunLua( const char* sCode )
    {
        bool bReturn = false;

        string sExpression = SAFESTR( sCode );
        sExpression = sExpression.Trim();

        if ( sExpression.length() > 0 )
        {
            if ( gEnv->pScriptSystem->BeginCall( PM_LUA_RUN ) )
            {
                gEnv->pScriptSystem->PushFuncParam<const char*>( WrapLuaCode( sExpression, false ) );

                if ( !gEnv->pScriptSystem->EndCall() )
                {
                    LogError( "Lua Run EndCall failed, maybe Logic or Type error: %s", sExpression.c_str() );
                }
            }

            else
            {
                LogError( "Lua Run BeginCall failed" );
            }
        }

        else
        {
            LogError( "Lua Run empty" );
        }

        return bReturn;
    }

#if defined(WIN_INTERCEPTORS)
    bool CPluginManager::PreWinProcInterceptor( HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam ) const
    {
        bool bRet = false;

        for ( auto iter = m_vecInterceptors.begin(); iter != m_vecInterceptors.end(); iter++ )
        {
            bRet |= ( *iter )->PreWinProc( hWnd, msg, wParam, lParam );
        }

        return bRet;
    }

    LRESULT CPluginManager::PostWinProcInterceptor( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT lResult ) const
    {
        LRESULT lRet = lResult;

        for ( auto iter = m_vecInterceptors.begin(); iter != m_vecInterceptors.end(); iter++ )
        {
            lRet = ( *iter )->PostWinProc( hWnd, msg, wParam, lParam, lRet );
        }

        return lRet;
    }

    void CPluginManager::RegisterWinProcInterceptor( IPluginWinProcInterceptor* pInterceptor )
    {
        if ( !pInterceptor )
        {
            return;
        }

        for ( auto iter = m_vecInterceptors.begin(); iter != m_vecInterceptors.end(); iter++ )
        {
            if ( ( *iter ) == pInterceptor )
            {
                return;
            }
        }

        m_vecInterceptors.push_back( pInterceptor );
    }

    void CPluginManager::UnregisterWinProcInterceptor( IPluginWinProcInterceptor* pInterceptor )
    {
        for ( auto iter = m_vecInterceptors.begin(); iter != m_vecInterceptors.end(); iter++ )
        {
            if ( ( *iter ) == pInterceptor )
            {
                iter = m_vecInterceptors.erase( iter );

                if ( iter == m_vecInterceptors.end() )
                {
                    break;
                }
            }
        }
    }
#endif
}
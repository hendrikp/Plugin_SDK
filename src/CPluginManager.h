/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>
#include <map>

#include <IPluginManager.h>
#include <IPluginBase.h>
#include <CPluginBase.hpp>
#include <CryLibrary.h>

#include <PMUtils.hpp>

#define PLUGIN_NAME PLUGIN_MANAGER
#define PLUGIN_CONSOLE_PREFIX "[" PLUGIN_NAME " " PLUGIN_TEXT "] " //!< Prefix for Logentries by this plugin
#define PLUGIN_FILENAME TEXT(PLUGIN_TEXT "_" PLUGIN_NAME CrySharedLibrayExtension) //!< Filename of the Plugin

#define PLUGIN_PATH PLUGIN_FOLDER PATH_SEPERATOR PLUGIN_FILENAME //!< Full Path of the plugin

#define PM_LUA_TESTLOGIC "pluginLuaLogic"
#define PM_LUA_RUN "pluginLuaRun"

namespace PluginManager
{
    /**
    * @brief Store some information about each plugin in manager
    */
    struct SPluginInfo
    {
        HINSTANCE m_hModule;
        IPluginBase* m_pBase;

        string m_sFile;
        string m_sDirectory;

        SPluginInfo()
        {
            m_hModule = NULL;
            m_pBase = NULL;
            m_sFile = "";
            m_sDirectory = "";
        }

        SPluginInfo( IPluginBase* pBase, HINSTANCE hModule, const char* sFile, const char* sDirectory )
        {
            m_hModule = hModule;
            m_pBase = pBase;
            m_sFile = SAFESTR( sFile );
            m_sDirectory = SAFESTR( sDirectory );
        };
    };

    typedef std::map<string, SPluginInfo> tPluginNameMap; //!< plugin name registry type

    typedef std::pair<string, string> tStaticInterfaceKey; //!< static interface registry key type
    typedef std::map<tStaticInterfaceKey, void*> tStaticInterfaceMap; //!< static interface registry type

#if defined(WIN_INTERCEPTORS)
    typedef std::vector<IPluginWinProcInterceptor*> tInterceptorVec; //!< interceptor registry type
#endif

    /**
    * @brief List All Plugins
    * Console CVar Command
    * Usage: pm_list
    */
    void Command_ListAll( IConsoleCmdArgs* pArgs );

    /**
    * @brief List registered static interfaces
    * Console CVar Command
    * Usage: pm_listsi
    */
    void Command_ListStaticInterfaces( IConsoleCmdArgs* pArgs );

    /**
    * @brief Dump infos about a specific plugin
    * Console CVar Command
    * Usage: dump PLUGINNAME
    */
    void Command_Dump( IConsoleCmdArgs* pArgs );

    /**
    * @brief Dump infos about all loaded plugins
    * Console CVar Command
    * Usage: pm_dumpall
    */
    void Command_DumpAll( IConsoleCmdArgs* pArgs );

    /**
    * @brief Unload a specific plugin.
    * Console CVar Command
    * Usage: pm_unload PLUGINNAME
    * @attention can break dependencies if used incorrectly.
    */
    void Command_Unload( IConsoleCmdArgs* pArgs );

    /**
    * @brief Unload all plugins (in the reverse order)
    * Console CVar Command
    * Usage: pm_unload PLUGINNAME
    */
    void Command_UnloadAll( IConsoleCmdArgs* pArgs );

    /**
    * @brief Reload a specific plugin
    * Console CVar Command
    * Usage: pm_reload PLUGINPATH
    * e.g. "pm_reload Flite/Plugin_Flite.dll"
    */
    void Command_Reload( IConsoleCmdArgs* pArgs );

    /**
    * @brief Reload and initialize all plugins (in the correct order)
    * Console CVar Command
    * Usage: pm_reloadall
    */
    void Command_ReloadAll( IConsoleCmdArgs* pArgs );

    /**
    * @brief Plugin Manager manages the plugin resources
    */
    class CPluginManager :
        public CPluginBase,
        public IPluginManager,
        public IGameFrameworkListener,
        public ISystemEventListener
    {
        private:
            tPluginNameMap m_Plugins; //!< All Plugins
            tPluginNameMap m_UnloadingPlugins; //!< Plugins marked for cleanup

            tStaticInterfaceMap m_StaticInterfaces; //!< All static interfaces

#if defined(WIN_INTERCEPTORS)
            tInterceptorVec m_vecInterceptors; //!< Interceptor registry
#endif

            string m_sPluginsDirectory; //!< Directory containing all plugins (e.g. "C:\cryengine3_3.4.0\Bin32\Plugins")
            string m_sBinaryDirectory; //!< Directory containing all binaries (e.g. "C:\cryengine3_3.4.0\Bin32")
            string m_sRootDirectory; //!< Root engine directory (e.g. "C:\cryengine3_3.4.0")
            string m_sGameDirectory; //!< Game directory (e.g. "C:\cryengine3_3.4.0\Game")
            string m_sUserDirectory; //!< User settings/cache directory (e.g. "C:\cryengine3_3.4.0\USER")

            /**
            * @internal
            * @brief Internal Refresh/Initialize paths
            */
            void CPluginManager::RefreshPaths();

            /**
            * @internal
            * @brief Cleanup of unused plugins.
            */
            void PluginGarbageCollector();

            /**
            * @internal
            * @brief Helper to load link libraries in their own directory
            * @param sPluginPath dll path coming from CryPak
            */
            HMODULE LoadLibraryWithinOwnDirectory( const char* sPluginPath ) const;

            /**
            * @internal
            * @brief Helper to load link libraries from a path
            * @param sPath Path in which to search for plugins
            *  in depth = 0 its a directory with plugins or subdirectories that contain plugins
            *  in depth = 1 its a plugin subdirectory with plugin specific non lazy load dependencies
            * @param nDepth current subdirectory depth (this is currently max 1)
            */
            void LoadPluginsFromDirectory( const char* sPath, int nDepth = 0 );

        public:
            // IGameFrameworkListener
            void OnPostUpdate( float fDeltaTime );
            void OnSaveGame( ISaveGame* pSaveGame ) {};
            void OnLoadGame( ILoadGame* pLoadGame ) {};
            void OnLevelEnd( const char* nextLevel ) {};
            void OnActionEvent( const SActionEvent& event ) {};

            // ISystemEventListener
            void OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam );
        public:
            CPluginManager();
            ~CPluginManager();

            // IPluginBase
            bool Release( bool bForce = false );

            int GetInitializationMode() const
            {
                return int( IM_BeforeFramework );
            };

            bool Check( const char* sAPIVersion ) const;

            bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory );

            bool RegisterTypes( int nFactoryType, bool bUnregister );

            const char* GetVersion() const
            {
                return "1.2.0.0";
            };

            const char* GetName() const
            {
                return "Manager";
            };

            const char* GetCategory() const
            {
                return "General";
            };

            const char* ListAuthors() const
            {
                return "Hendrik Polczynski,\nRaphael \"MrHankey89\" Leiteritz,\nFilip \"i59\" Lundgren";
            };

            const char* ListCVars() const;

            const char* GetStatus() const;

            const char* GetCurrentConcreteInterfaceVersion() const
            {
                return "1.1";
            };

            void* GetConcreteInterface( const char* sInterfaceVersion )
            {
                return static_cast<IPluginManager*>( this );
            };

            // IPluginManager
            IPluginBase* GetBase() const
            {
                return static_cast<IPluginBase*>( const_cast<CPluginManager*>( this ) );
            };

            void UnloadAllPlugins();

            void UnloadPlugin( const char* sPluginName );

            void ReloadAllPlugins();

            bool ReloadPlugin( const char* sPluginPath, bool bInitialize = false );

            bool InitializePlugin(  const char* sPluginName );

            void InitializePluginRange( int nBeginAtMode = IM_Min, int nEndAtMode = IM_Max );

            bool RegisterTypesPluginRange( int nBeginAtMode = IM_Min, int nEndAtMode = IM_Max, int nFactoryType = int( FT_None ), bool bUnregister = false  );

            IPluginBase* GetPluginByName( const char* sPluginName ) const;

            const char* GetPluginDirectory( const char* sPluginName ) const;

            // Directory information functions
            const char* GetDirectoryPlugins() const
            {
                return m_sPluginsDirectory;
            };

            const char* GetDirectoryBinary() const
            {
                return m_sBinaryDirectory;
            };

            const char* GetDirectoryRoot() const
            {
                return m_sRootDirectory;
            };

            const char* GetDirectoryGame() const
            {
                return m_sGameDirectory;
            };

            const char* GetDirectoryUser() const
            {
                return m_sUserDirectory;
            };

            void* GetStaticInterface( const char* sName, const char* sVersion = NULL ) const;

            void RegisterStaticInterface ( void* pInterface, const char* sName, const char* sVersion = NULL );

            // Directory information functions
#if defined(WIN_INTERCEPTORS)
            bool PreWinProcInterceptor( HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam ) const;
            LRESULT PostWinProcInterceptor( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT lResult ) const;
            void RegisterWinProcInterceptor( IPluginWinProcInterceptor* pInterceptor );
            void UnregisterWinProcInterceptor( IPluginWinProcInterceptor* pInterceptor );
#endif

        private:
            CallDelayQueue m_qDelayedCalls; //!< delayed function calls
        public:
            /**
            * @brief GetDelayQueue
            * @return delay queue of videoplayer system
            */
            CallDelayQueue& GetDelayQueue()
            {
                return m_qDelayedCalls;
            };

            void DelayFunction( const char* sFilter = NULL, tDelayedCall pFunc = NULL, tDelayedCall pFuncCleanup = NULL, void* pData = NULL, float fDelay = 1.0f, int eType = int( eDT_Default ), tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL );

            void DelayCommand( const char* sCommand, const char* sFilter = NULL, float fDelay = 1.0f, int eType = int( eDT_Default ), tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL );

            void DelayCancel( const char* sFilter = NULL );

            bool TestLuaLogic( const char* sLogic );

            bool RunLua( const char* sCode );

            void DelayLua( const char* sCode, const char* sFilter = NULL, float fDelay = 1.0f, int eType = int( eDT_Default ), tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL );

            /**
            * @internal
            * @brief Dump infos on specific plugin to console
            */
            void DumpPlugin( const char* sPluginName ) const;

            /**
            * @internal
            * @brief Dump all plugins to console
            */
            void DumpAllPlugins() const;

            /**
            * @internal
            * @brief List all plugins to console
            */
            void ListAllPlugins() const;

            /**
            * @internal
            * @brief List all registered static interfaces to console
            */
            void ListStaticInterfaces() const;
    };
}

static PluginManager::CPluginManager* gPluginManager; //!< Global internal Plugin Manager

static struct SSystemInitParams gStartupInitParams; //!< Global internal CryEngine Startup Init Params (important for Plugin Manager only)
static string gsSDKInterfaceVersion; //!< Global internal CryEngine SDK Version (important for Plugin Manager only, so he can provide this info to the plugins)
static string gsBaseInterfaceVersion; //!< Global internal Plugin SDK Base Interface Version (important for Plugin Manager only, so he can provide this info to the plugins)
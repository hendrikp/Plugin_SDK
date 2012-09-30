/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>
#include <map>

#include <IPluginManager.h>
#include <IPluginBase.h>
#include <CPluginBase.hpp>
#include <CryLibrary.h>

#define PLUGIN_NAME "Manager"
#define PLUGIN_CONSOLE_PREFIX "[" PLUGIN_NAME " " PLUGIN_TEXT "] " //!< Prefix for Logentries by this plugin
#define PLUGIN_FILENAME TEXT(PLUGIN_TEXT "_" PLUGIN_NAME CrySharedLibrayExtension) //!< Filename of the Plugin

#define PLUGIN_PATH PLUGIN_FOLDER "\\" PLUGIN_FILENAME //!< Full Path of the plugin

namespace PluginManager
{
    typedef std::map<string, std::pair<HINSTANCE, IPluginBase*> > tPluginNameMap; //!< plugin name registry type

    /**
    * @brief List All Plugins
    * Console CVar Command
    * Usage: pm_list
    */
    void Command_ListAll( IConsoleCmdArgs* pArgs );

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
    * e.g. pm_reload Plugins/Plugin_Test.dll
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
        public IGameFrameworkListener
    {
        private:
            tPluginNameMap m_Plugins; //!< All Plugins
            tPluginNameMap m_UnloadingPlugins; //!< Plugins marked for cleanup

            /**
            * @brief Internal Cleanup of unused plugins.
            */
            void PluginGarbageCollector();

        public:
            // IGameFrameworkListener
            void OnPostUpdate( float fDeltaTime );
            void OnSaveGame( ISaveGame* pSaveGame ) {};
            void OnLoadGame( ILoadGame* pLoadGame ) {};
            void OnLevelEnd( const char* nextLevel ) {};
            void OnActionEvent( const SActionEvent& event ) {};

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

            bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager );

            const char* GetVersion() const
            {
                return "1.0";
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
                return "Hendrik \"hendrikp\" Polczynski,\nRaphael \"MrHankey89\" Leiteritz";
            };

            const char* ListCVars() const;

            const char* GetStatus() const;

            const char* GetCurrentConcreteInterfaceVersion() const
            {
                return "1.0";
            };

            void* GetConcreteInterface( const char* sInterfaceVersion )
            {
                return static_cast<IPluginManager*>( this );
            };

            // IPluginManager
            IPluginBase* GetBase()
            {
                return static_cast<IPluginBase*>( this );
            };

            void UnloadAllPlugins();

            void UnloadPlugin( const char* sPluginName );

            void ReloadAllPlugins();

            bool ReloadPlugin( const char* sPluginPath, bool bInitialize = false );

            bool InitializePlugin(  const char* sPluginName );

            void InitializePluginRange( int nBeginAtMode = IM_Min, int nEndAtMode = IM_Max );

            IPluginBase* GetPluginByName( const char* sPluginName );

            void DumpPlugin( const char* sPluginName );

            void DumpAllPlugins();

            void ListAllPlugins();
    };
}

static PluginManager::CPluginManager* gPluginManager; //!< Global internal Plugin Manager

static struct SSystemInitParams gStartupInitParams; //!< Global internal CryEngine Startup Init Params (important for Plugin Manager only)
static string gsSDKInterfaceVersion; //!< Global internal CryEngine SDK Version (important for Plugin Manager only, so he can provide this info to the plugins)
static string gsBaseInterfaceVersion; //!< Global internal Plugin SDK Base Interface Version (important for Plugin Manager only, so he can provide this info to the plugins)
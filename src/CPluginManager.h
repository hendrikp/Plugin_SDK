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

    void Command_ListAll( IConsoleCmdArgs* pArgs );
    void Command_Dump( IConsoleCmdArgs* pArgs );
    void Command_DumpAll( IConsoleCmdArgs* pArgs );
    void Command_Unload( IConsoleCmdArgs* pArgs );
    void Command_UnloadAll( IConsoleCmdArgs* pArgs );
    void Command_Reload( IConsoleCmdArgs* pArgs );
    void Command_ReloadAll( IConsoleCmdArgs* pArgs );

    /**
    * @brief Plugin Manager System manages the resources
    */
    class CPluginManager :
        public CPluginBase,
        public IPluginManager
    {
        private:
            tPluginNameMap m_Plugins; // All Plugins

        public:
            CPluginManager();
            ~CPluginManager();

            // IPluginBase
            void Release();

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
                return "Hendrik \"hendrikp\" Polczynski\r\nRaphael \"MrHankey89\" Leiteritz";
            };

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
/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#include <IPluginBase.h>

#pragma once

/**
* @brief Plugin Manager Namespace
*/
namespace PluginManager
{
    /**
    * @brief plugin manager interface
    */
    struct IPluginManager
    {
        /**
        * @brief Get PluginManager base interface
        */
        virtual IPluginBase* GetBase() = 0;

        /**
        * @brief UnloadAllPlugins
        */
        virtual void UnloadAllPlugins() = 0;

        /**
        * @brief Unload Plugin DLL
        * Its better to directly call release on the interface of the plugin.
        * @param sPluginName Name of the plugin to unload.
        */
        virtual void UnloadPlugin(  const char* sPluginName ) = 0;

        /**
        * @brief ReloadAllPlugins
        */
        virtual void ReloadAllPlugins() = 0;

        /**
        * @brief Reload Plugin DLL
        * @param sPluginPath Path of the plugin to reload
        */
        virtual bool ReloadPlugin(  const char* sPluginPath, bool bInitialize = false ) = 0;

        /**
        * @brief Initialize a plugin directly
        * @param sPluginName Name of the plugin to unload.
        * @return success
        */
        virtual bool InitializePlugin(  const char* sPluginName ) = 0;

        /**
        * @brief Initialize a range of plugins
        * @param nBeginAtMode Start of the range to initialize.
        * @param nEndAtMode End of the range to initialize.
        */
        virtual void InitializePluginRange( int nBeginAtMode = IM_Min, int nEndAtMode = IM_Max ) = 0;

        /**
        * @brief Get base interface of a specific plugin if it is loaded
        * @return BaseInterface of the plugin requested or NULL if not found.
        */
        virtual IPluginBase* GetPluginByName(  const char* sPluginName ) = 0;

        /**
        * @brief Dump infos on specific plugin to console
        */
        virtual void DumpPlugin(  const char* sPluginName ) = 0;

        /**
        * @brief Dump all plugins to console
        */
        virtual void DumpAllPlugins() = 0;

        /**
        * @brief List all plugins to console
        */
        virtual void ListAllPlugins() = 0;
    };

    typedef boost::shared_ptr<IPluginManager> IPluginManagerPtr;
};

// Forces you to declare global PluginManager when including IPluginManager
#if !defined(PLUGINMANAGER_EXPORTS)
extern PluginManager::IPluginManager* gPluginManager; //!< Global plugin manager pointer for game link libraries.
#endif

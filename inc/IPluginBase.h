/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#define PLUGIN_TEXT "Plugin"

#define PLUGIN_FOLDER "Plugins"

#pragma once

namespace PluginManager
{
    /**
    * @brief Initialization modes for plugins
    */
    enum eInitMode
    {
        IM_BeforeFramework = 0, //!< Initialize this plugin before the framework
        IM_Min = IM_BeforeFramework,
        IM_BeforeFramework_2 = 1, //!< For additional ordering of dependencies
        IM_BeforeFramework_3 = 2, //!< For additional ordering of dependencies
        IM_Last = 3, //!< Initialize this plugin as last step in the initializiation
        IM_Last_2 = 4, //!< For additional ordering of dependencies
        IM_Last_3 = 5, //!< For additional ordering of dependencies
        IM_Max = IM_Last_3,
        IM_Default = IM_Last, //!< Default is initialize in the end
    };

    /**
    * @brief Base Interface all plugins must implement
    * This interface should be creatable without any initialization happening in the plugin.
    * Initializations must be delayed until Init and InitDependencies are called.
    */
    struct IPluginBase
    {
        /**
        * @brief Unregisters the Plugin and Unload the DLL
        * @attention The Plugin should clean everything up, it registered in the engine so that the plugin can be reloaded.
        */
        virtual void Release() = 0;

        /**
        * @brief Set while Plugin is cleaning up itself
        * @return Plugin is unloading
        */
        virtual bool IsUnloading() = 0;

        /**
        * @brief Set if ready to initialize dependencies
        * @return Ready to initialize dependencies
        */
        virtual bool IsInitialized() = 0;

        /**
        * @brief Set if ready to used and all dependencies were initialized
        * @return Ready to be used
        */
        virtual bool IsFullyInitialized() = 0;

        /**
        * @brief Return eInitMode
        * Plugins in the BeforeFramework mode can not have dependencies in the Last mode.
        * Plugins in the Last mode can however have dependencies in the BeforeFramework mode.
        * Unless Plugins get loaded manually in a later stage... (In which case the depency chain must be handled manually)
        */
        virtual int GetInitializationMode() const = 0;

        /**
        * @brief Check if this plugin is compatible with this CryEngine SDK version
        */
        virtual bool Check( const char* sAPIVersion ) const = 0;

        /**
        * @brief Initialize the plugin so far that dependencies can do their dependency initialization.
        */
        virtual bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager ) = 0;

        /**
        * @brief Check if all dependencies are avaible in compatible versions trough the Plugin Manager
        * Please note the plugin manager itself is also a dependency and must be checked for compatiblity
        * @see Init
        */
        virtual bool CheckDependencies() const = 0;

        /**
        * @brief Do additional a final initialization as now all dependencies are avaible
        * After executing this function the plugin must be in usable state.
        */
        virtual bool InitDependencies() = 0;

        /**
        * @brief Get the plugin version string
        * @return Plugin Version (e.g. "1.0")
        */
        virtual const char* GetVersion() const = 0;

        /**
        * @brief Get the name of the plugin
        * @return Plugin Name (must be something unique)
        */
        virtual const char* GetName() const = 0;

        /**
        * @brief Category/Type of the plugin
        * @return Plugin Category (e.g. General, Utility, Flownode, Media, Camera, ...)
        */
        virtual const char* GetCategory() const = 0;

        /**
        * @brief List authors of the plugin
        * @return Authors of the plugin
        */
        virtual const char* ListAuthors() const = 0;

        /**
        * @brief List flownodes of the plugin
        * @return Flownnodes of the plugin
        */
        virtual const char* ListNodes() const = 0;

        /**
        * @brief List CVars of the plugin
        * @return CVars of the plugin
        */
        virtual const char* ListCVars() const = 0;

        /**
        * @brief List game objects of the plugin
        * @return game objects of the plugin
        */
        virtual const char* ListGameObjects() const = 0;

        /**
        * @brief Current Status of the plugin
        * Can be used to indicate errors.
        */
        virtual const char* GetStatus() const = 0;

        /**
        * @brief Return the current concrete interface version
        * @return current version of the concrete interface
        */
        virtual const char* GetCurrentConcreteInterfaceVersion() const = 0;

        /**
        * @brief Return the requested concrete interface version if implemented by this plugin version
        * @param sInterfaceVersion The requested interface version.
        * @arg NULL for most current.
        * @return NULL or concrete interface in the requested version is avaible.
        */
        virtual void* GetConcreteInterface( const char* sInterfaceVersion ) = 0;

        /**
        * @brief Return the current extended interface version
        * @return current version of the extended interface
        */
        virtual const char* GetCurrentExtendedInterfaceVersion() const = 0;

        /**
        * @brief Reserved for future use to ensure ABI compability
        * @param sInterfaceVersion The requested interface version.
        * @arg NULL for most current.
        * @return NULL or concrete interface in the requested version is avaible.
        */
        virtual void* GetExtendedInterface( const char* sInterfaceVersion ) = 0;
    };

    /**
    * @brief Plugins should use their own logging functionallity so it can easily be recognized and redirected to other Logging facilities then the cryengine console.
    * Its for internal use inside each plugin only.
    */
    struct IPluginLog
    {
        /**
        * @brief Log information (White)
        * @param sFormat printf like format string
        * @param ... Format parameters
        */
        virtual void LogAlways( const char* sFormat, ... ) const = 0;

        /**
        * @brief Log warnings (Yellow)
        * @param sFormat printf like format string
        * @param ... Format parameters
        */
        virtual void LogWarning( const char* sFormat, ... ) const = 0;

        /**
        * @brief Log errors (Red)
        * @param sFormat printf like format string
        * @param ... Format parameters
        */
        virtual void LogError( const char* sFormat, ... ) const = 0;
    };

    /**
    * @brief Function pointer for getting Plugin Base Interface
    */
    typedef IPluginBase* ( *fGetPluginInterface )( const char* );
};

/**
* @brief DLL C-APIs
*/
extern "C"
{
    /**
    * @brief Create the Plugin Base Interface in the requested version
    * @param sInterfaceVersion The requested interface version.
    * @arg NULL for most current.
    * @return NULL or concrete interface in the requested version is avaible.
    */
    GAME_API PluginManager::IPluginBase* GetPluginInterface( const char* sInterfaceVersion );
}
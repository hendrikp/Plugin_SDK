/* Plugin SDK - for licensing and copyright see license.txt */

#define PLUGIN_TEXT "Plugin" //!< for logging and misc
#define PLUGIN_FOLDER "Plugins" //!< directory

#pragma once

#include <CDKVersion.h>

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
        IM_Default = IM_BeforeFramework, //!< Default is initialize in the beginning (previously end)
    };

    /**
    * @brief Registry factory types for plugins
    * Certain object types need to be registered or reregistered at certain points in time or even certain threads.
    */
    enum eFactoryType
    {
        FT_None = 0, //!< No certain type defined / misc types / lua
        FT_Flownode = 1, //!< For Flownodes (important for Sandbox reregistration through: GameDLL CGame::RegisterGameFlowNodes)
        FT_GameObjectExtension = 2, //!< For Game Object Classes
        FT_CVar = 3, //!< For CVars
        FT_CVarCommand = 4, //!< For Console Commands
        FT_UIEvent = 5, //!< For UI Events
        FT_All = 99, //! For all Types affected at once
        // To be extended once needed
    };

    /**
    * @brief Base Interface all plugins must implement
    * This interface should be creatable without any initialization happening in the plugin.
    * Initializations must be delayed until Init and InitDependencies are called.
    */
    struct IPluginBase
    {
        /**
        * @brief Increment reference counter
        * @attention Dependencies should use this to prevent unloading of DLL's required.
        */
        virtual void AddRef() = 0;

        /**
        * @brief Unregisters the Plugin and Unload the DLL if reference counter reaches zero
        * @param bForce Force an unload even if there are additional references
        * @return true if plugin was cleaned up and can unload.
        * @attention The Plugin should clean everything up, it registered in the engine so that the plugin can be reloaded.
        */
        virtual bool Release( bool bForce = false ) = 0;

        /**
        * @brief Returns true if no more references exist and the dll can now unload
        * @return Plugin DLL can be unloaded now
        */
        virtual bool DllCanUnloadNow() = 0;

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
        * @param env CryEngine environment (Engine pointers)
        * @param startupParams CryEngine startup parameters (Command line)
        * @param pPluginManager Plugin SDK plugin manager
        * @param sPluginDirectory Absolute path in which your plugin was loaded (e.g. "C:\cryengine3_3.4.0\Bin32\Plugins\Flite")
        */
        virtual bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory ) = 0;

        /**
        * @brief Check if all dependencies are available in compatible versions trough the Plugin Manager
        * Please note the plugin manager itself is also a dependency and must be checked for compatibility
        * @see Init
        */
        virtual bool CheckDependencies() const = 0;

        /**
        * @brief Do additional a final initialization as now all dependencies are available
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
        * @brief General plugin dump functionality
        * @return all plugin internals for debugging purposes
        */
        virtual const char* Dump() const = 0;

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
        * @return NULL or concrete interface in the requested version is available.
        */
        virtual void* GetConcreteInterface( const char* sInterfaceVersion = NULL ) = 0;

        /**
        * @brief Return the current extended interface version
        * @return current version of the extended interface
        */
        virtual const char* GetCurrentExtendedInterfaceVersion() const = 0;

        /**
        * @brief Reserved for future use to ensure ABI compatibility
        * @param sInterfaceVersion The requested interface version.
        * @arg NULL for most current.
        * @return NULL or concrete interface in the requested version is available.
        */
        virtual void* GetExtendedInterface( const char* sInterfaceVersion = NULL ) = 0;

        /**
        * @brief Register Types independent of Init / InitDependencies
        * Certain object types need to be registered or reregistered at certain points in time or even certain threads.
        * This functionality was added to allow for more flexibility in that regard.
        * @param nFactoryType The factory type objects to be registered.
        * @see eFactoryType
        * @param bUnregister Set to true if types should be unregistered (most types can't be unregistered atm, but maybe it'll be possible in the future)
        * @return success
        */
        virtual bool RegisterTypes( int nFactoryType = int( FT_None ), bool bUnregister = false ) = 0;
    };

    /**
    * @brief Plugins should use their own logging functionality so it can easily be recognized and redirected to other Logging facilities then the cryengine console.
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

#define PLUGIN_ENTRYPOINT "GetPluginInterface"

/**
* @brief DLL C-APIs
*/
extern "C"
{
    /**
    * @brief Create the Plugin Base Interface in the requested version
    * @param sInterfaceVersion The requested interface version.
    * @arg NULL for most current.
    * @return NULL or concrete interface in the requested version is available.
    */
    DLL_EXPORT PluginManager::IPluginBase* GetPluginInterface( const char* sInterfaceVersion = NULL );
}

/** Now some macros required for entity classes */

/* This causes a redefinition error when using uber files

#ifndef REGISTER_GAME_OBJECT_EXTENSION

#define HIDE_FROM_EDITOR(className)                                                                                                                                             \
    { IEntityClass *pItemClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(className);\
        pItemClass->SetFlags(pItemClass->GetFlags() | ECLF_INVISIBLE); }                                                                                \

#define REGISTER_EDITOR_VOLUME_CLASS(frameWork, className)                                          \
    {                                                                                                     \
        IGameVolumes* pGameVolumes = frameWork->GetIGameVolumesManager();                                 \
        IGameVolumesEdit* pGameVolumesEdit = pGameVolumes ? pGameVolumes->GetEditorInterface() : NULL;    \
        if (pGameVolumesEdit != NULL)                                                                     \
        {                                                                                                 \
            pGameVolumesEdit->RegisterEntityClass( className );                                             \
        }                                                                                                 \
    }

#define REGISTER_GAME_OBJECT(framework, name, script)\
    {\
        IEntityClassRegistry::SEntityClassDesc clsDesc;\
        clsDesc.sName = #name;\
        clsDesc.sScriptFile = script;\
        struct C##name##Creator : public IGameObjectExtensionCreatorBase\
        {\
            IGameObjectExtensionPtr Create()\
            {\
                return ComponentCreate_DeleteWithRelease<C##name>();\
            }\
            void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
            {\
                C##name::GetGameObjectExtensionRMIData( ppRMI, nCount );\
            }\
        };\
        static C##name##Creator _creator;\
        framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, &clsDesc);\
    }

#define REGISTER_GAME_OBJECT_WITH_IMPL(framework, name, impl, script)\
    {\
        IEntityClassRegistry::SEntityClassDesc clsDesc;\
        clsDesc.sName = #name;\
        clsDesc.sScriptFile = script;\
        struct C##name##Creator : public IGameObjectExtensionCreatorBase\
        {\
            IGameObjectExtensionPtr Create()\
            {\
                return ComponentCreate_DeleteWithRelease<C##impl>();\
            }\
            void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
            {\
                C##impl::GetGameObjectExtensionRMIData( ppRMI, nCount );\
            }\
        };\
        static C##name##Creator _creator;\
        framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, &clsDesc);\
    }

#define REGISTER_GAME_OBJECT_EXTENSION(framework, name)\
    {\
        struct C##name##Creator : public IGameObjectExtensionCreatorBase\
        {\
            IGameObjectExtensionPtr Create()\
            {\
                return ComponentCreate_DeleteWithRelease<C##name>();\
            }\
            void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
            {\
                C##name::GetGameObjectExtensionRMIData( ppRMI, nCount );\
            }\
        };\
        static C##name##Creator _creator;\
        framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, NULL);\
    }

#endif

*/
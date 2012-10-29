/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#include <IPluginBase.h>

#define PLUGIN_MANAGER "Manager"

// Needed for some Windows types
#include <platform.h>
#if defined(WIN32) || defined(WIN64)
#define WIN_INTERCEPTORS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// get rid of (really) annoying MS defines
#undef min
#undef max
#undef WIN32_LEAN_AND_MEAN
#endif

/**
* @brief Plugin Manager Namespace
*/
namespace PluginManager
{

#if defined(WIN_INTERCEPTORS)
    /**
    * @brief Plugin WinProc interceptor interface
    */
    struct IPluginWinProcInterceptor
    {
        /**
        * @brief Intercept Message before original WinProc got it.
        * Allows you to manipulate the parameters and skip the default implementation (the post listeners will still be called)
        * @return If true doesn't process the original WinProc
        */
        virtual bool PreWinProc( HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam ) = 0;

        /**
        * @brief Intercept Message after the original WinProc got it.
        * Allows you to manipulate the LRESULT.
        * @param lResult original WinProc LRESULT
        */
        virtual LRESULT PostWinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT lResult ) = 0;
    };
#endif

    /**
    * @internal
    * @brief delayed function type
    * @param pData data for the delayed call
    * This is the delayed call
    */
    typedef void ( *tDelayedCall )( void* );

    /**
    * @internal
    * @brief delay call trigger type
    * Delay Trigger type functions can be used to monitor dynamic conditions
    * @param pDataTrigger data for the delay trigger
    * On true this executes the delayed call
    */
    typedef bool ( *tDelayedCallTrigger )( void* );

    /**
    * @brief plugin manager interface
    */
    struct IPluginManager
    {
        /**
        * @brief Get PluginManager base interface
        */
        virtual IPluginBase* GetBase() const = 0;

        /**
        * @brief UnloadAllPlugins
        */
        virtual void UnloadAllPlugins() = 0;

        /**
        * @brief Unload Plugin DLL
        * Its better to directly call release on the interface of the plugin.
        * @param sPluginName Name of the plugin to unload.
        */
        virtual void UnloadPlugin( const char* sPluginName ) = 0;

        /**
        * @brief ReloadAllPlugins
        */
        virtual void ReloadAllPlugins() = 0;

        /**
        * @brief Reload Plugin DLL
        * @param sPluginPath Path of the plugin to reload
        */
        virtual bool ReloadPlugin( const char* sPluginPath, bool bInitialize = false ) = 0;

        /**
        * @brief Initialize a plugin directly
        * @param sPluginName Name of the plugin to unload.
        * @return success
        */
        virtual bool InitializePlugin( const char* sPluginName ) = 0;

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
        virtual IPluginBase* GetPluginByName( const char* sPluginName ) const = 0;

        /**
        * @brief Retrieve directory of a plugin (e.g. "C:\cryengine3_3.4.0\Bin32\Plugins\Flite")
        * @return directory path without slash
        */
        virtual const char* GetPluginDirectory( const char* sPluginName ) const = 0;

        /**
        * @brief Retrieve directory containing all plugins (e.g. "C:\cryengine3_3.4.0\Bin32\Plugins")
        * @return directory path without slash
        */
        virtual const char* GetDirectoryPlugins() const = 0;

        /**
        * @brief Retrieve directory containing all binaries (e.g. "C:\cryengine3_3.4.0\Bin32")
        * @return directory path without slash
        */
        virtual const char* GetDirectoryBinary() const = 0;

        /**
        * @brief Retrieve root engine directory (e.g. "C:\cryengine3_3.4.0")
        * @return directory path without slash
        */
        virtual const char* GetDirectoryRoot() const = 0;

        /**
        * @brief Retrieve Game directory (e.g. "C:\cryengine3_3.4.0\Game")
        * @return directory path without slash
        */
        virtual const char* GetDirectoryGame() const = 0;

        /**
        * @brief Retrieve User settings/cache directory (e.g. "C:\cryengine3_3.4.0\USER")
        * @return directory path without slash
        */
        virtual const char* GetDirectoryUser() const = 0;

        /**
        * @brief Retrieve a static interface
        * @warning This should never be a requirement for a plugin to work it can be an additional improvement, so use it sparingly if you want your plugin to be reusable.
        * @param sName name of the interface to retrieve
        * @param sVersion of the interface to retrieve
        *  @arg NULL current version/doesn't matter
        * @return the interface pointer, or NULL if not found. Use static_cast to get your interface.
        */
        virtual void* GetStaticInterface( const char* sName, const char* sVersion = NULL ) const = 0;

        /**
        * @brief Register a static interface
        * A static interface is an interface that exist in a gamedll through the whole lifetime of the plugin manager.
        * It can be used to optionally provide/retrieve additional information/functionality to/from a gamedll.
        * @warning This should never be a requirement for a plugin to work it can be an additional improvement, so use it sparingly if you want your plugin to be reusable.
        * @param pInterface interface pointer (must exist through the whole plugin manager lifetime)
        * @param sName name of the interface to retrieve
        * @param sVersion of the interface to retrieve
        *  @arg NULL current version/doesn't matter
        */
        virtual void RegisterStaticInterface ( void* pInterface, const char* sName, const char* sVersion = NULL ) = 0;

#if defined(WIN_INTERCEPTORS)
        /**
        * @brief Redirect unprocessed Message to interceptors
        * Can manipulate the parameters and skip the default implementation.
        * @return If true doen't process the original WinProc
        */
        virtual bool PreWinProcInterceptor( HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam ) const = 0;

        /**
        * @brief Redirect processed Message to interceptors
        * Can manipulate the LRESULT.
        * @param lResult original WinProc LRESULT
        */
        virtual LRESULT PostWinProcInterceptor( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT lResult ) const = 0;

        /**
        * @brief Register a WinProc Message Interceptor
        * @param pInterceptor Interceptor to register
        */
        virtual void RegisterWinProcInterceptor( IPluginWinProcInterceptor* pInterceptor ) = 0;

        /**
        * @brief Unregister a WinProc Message Interceptor
        * @param pInterceptor Interceptor to unregister
        */
        virtual void UnregisterWinProcInterceptor( IPluginWinProcInterceptor* pInterceptor ) = 0;
#endif

        /**
        * @brief Delay the call of the function
        * can be used if you don't need full control over delayed execution (the plugin manager will handle it)
        * @param sFilter custom filter for remove control
        * @param pFunc function pointer of delayed function
        * @param pFuncCleanup function pointer that cleans up the data when the delayed call is canceled or finished
        * @param pData Data for delayed function
        * @param fDelay delay amount
        * @param eType delay type
        * @param pFuncTrigger function pointer delay trigger function
        * @param pFuncTriggerCleanup function pointer that cleans up the data when the delayed call is canceled or finished
        * @param pDataTrigger Data for delay trigger function
        */
        virtual void DelayFunction( const char* sFilter = NULL, tDelayedCall pFunc = NULL, tDelayedCall pFuncCleanup = NULL, void* pData = NULL, float fDelay = 1.0f, int eType = 1, tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL ) = 0;

        /**
        * @brief Uses the Plugin Manager Execution delayer
        * can be used if you don't need full control over delayed execution (the plugin manager will handle it)
        * @see PluginManager::DelayConsoleCommand
        * @param sCommand the console command
        * @param fDelay delay amount
        * @param eType delay type
        * @param pFuncTrigger function pointer delay trigger function
        * @param pFuncTriggerCleanup function pointer that cleans up the data when the delayed call is canceled or finished
        * @param pDataTrigger Data for delay trigger function
        */
        virtual void DelayCommand( const char* sCommand, const char* sFilter = NULL, float fDelay = 1.0f, int eType = 1, tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL ) = 0;

        /**
        * @brief Cancels registered delayed calls
        * @param sFilter only delete these delayed calls (if NULL then everything is deleted)
        */
        virtual void DelayCancel( const char* sFilter = NULL ) = 0;
    };
}

// Forces you to declare global PluginManager when including IPluginManager
#if !defined(PLUGINMANAGER_EXPORTS)
extern PluginManager::IPluginManager* gPluginManager; //!< Global plugin manager pointer for game link libraries.
#else
namespace PluginManager
{
    class CPluginManager;
    static CPluginManager* gPluginManager; //!< Global plugin manager pointer for game link libraries.
}
#endif

// Helper functions
namespace PluginManager
{
    /**
    * @brief Fast and safe way to get the concrete interface of a plugin
    * @tparam tCIFace concrete interface pointer datatype
    * @param sPlugin plugin name
    * @param sVersion concrete interface version
    * @return Pointer to concrete interface or NULL
    */
    template<typename tCIFace>
    static tCIFace safeGetPluginConcreteInterface( const char* sPlugin, const char* sVersion = NULL )
    {
        return static_cast<tCIFace>( gPluginManager && gPluginManager->GetPluginByName( sPlugin ) ? gPluginManager->GetPluginByName( sPlugin )->GetConcreteInterface( sVersion ) : NULL );
    };

    /**
    * @brief Fast and safe way to get the concrete interface of a plugin and keep it loaded (by adding a reference)
    * The plugin needs to be released again as this function adds a reference to the plugin
    * @tparam tCIFace concrete interface pointer datatype
    * @param sPlugin plugin name
    * @param sVersion concrete interface version
    * @return Pointer to concrete interface or NULL
    */
    template<typename tCIFace>
    static tCIFace safeUsePluginConcreteInterface( const char* sPlugin, const char* sVersion = NULL )
    {
        IPluginBase* pBase = gPluginManager ? gPluginManager->GetPluginByName( sPlugin ) : NULL;
        tCIFace pPlugin = static_cast<tCIFace>( pBase ? pBase->GetConcreteInterface( sVersion ) : NULL );

        if ( pPlugin )
        {
            pBase->AddRef();
        }

        return pPlugin;
    };

    /**
    * @brief Fast and safe way to release a plugin
    * @tparam tCIFace concrete interface pointer datatype
    * @param sPlugin plugin name
    * @param refpCIFace Reference to pointer of concrete interface or NULL
    */
    template<typename tCIFace>
    static void safeReleasePlugin( const char* sPlugin, tCIFace& refpCIFace )
    {
        IPluginBase* pBase = gPluginManager ? gPluginManager->GetPluginByName( sPlugin ) : NULL;

        if ( pBase )
        {
            pBase->Release();
        }

        refpCIFace = NULL;
    };
};

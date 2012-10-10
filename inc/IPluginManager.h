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
        * @brief Retrieve directory of a plugin (e.g. "C:\cryengine3_3.4.0\Bin32\Plugins\Flite")
        * @return directory path without slash
        */
        virtual const char* GetPluginDirectory( const char* sPluginName ) const = 0;

        /**
        * @brief Dump infos on specific plugin to console
        */
        virtual void DumpPlugin( const char* sPluginName ) = 0;

        /**
        * @brief Dump all plugins to console
        */
        virtual void DumpAllPlugins() = 0;

        /**
        * @brief List all plugins to console
        */
        virtual void ListAllPlugins() = 0;

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

    };
};

// Forces you to declare global PluginManager when including IPluginManager
#if !defined(PLUGINMANAGER_EXPORTS)
extern PluginManager::IPluginManager* gPluginManager; //!< Global plugin manager pointer for game link libraries.
#endif

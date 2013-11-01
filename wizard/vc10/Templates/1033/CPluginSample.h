/* [!output PROJECT_NAME_SAFE]_Plugin - for licensing and copyright see license.txt */

#pragma once

#include <IPluginBase.h>
#include <Game.h>

#include <IPluginManager.h>
#include <CPluginBase.hpp>

#include <IPlugin[!output PROJECT_NAME_SAFE].h>

#define PLUGIN_NAME "[!output PROJECT_NAME_SAFE]"
#define PLUGIN_CONSOLE_PREFIX "[" PLUGIN_NAME " " PLUGIN_TEXT "] " //!< Prefix for Logentries by this plugin

namespace [!output PROJECT_NAME_SAFE]Plugin
{
    /**
    * @brief Provides information and manages the resources of this plugin.
    */
    class CPlugin[!output PROJECT_NAME_SAFE] :
        public PluginManager::CPluginBase,
        public IPlugin[!output PROJECT_NAME_SAFE]
    {
        public:
            CPlugin[!output PROJECT_NAME_SAFE]();
            ~CPlugin[!output PROJECT_NAME_SAFE]();

            // IPluginBase
            bool Release( bool bForce = false );

            int GetInitializationMode() const
            {
                return int( PluginManager::IM_Default );
            };

            bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory );

            bool RegisterTypes( int nFactoryType, bool bUnregister );

            const char* GetVersion() const
            {
                return "[!output PLUGIN_VERSION]";
            };

            const char* GetName() const
            {
                return PLUGIN_NAME;
            };

            const char* GetCategory() const
            {
                return "[!output PLUGIN_CATEGORY]";
            };

            const char* ListAuthors() const
            {
                return "[!output PLUGIN_AUTHOR]";
            };

            const char* ListCVars() const;

            const char* GetStatus() const;

            const char* GetCurrentConcreteInterfaceVersion() const
            {
                return "1.0";
            };

            void* GetConcreteInterface( const char* sInterfaceVersion )
            {
                return static_cast < IPlugin[!output PROJECT_NAME_SAFE]* > ( this );
            };

            // IPlugin[!output PROJECT_NAME_SAFE]
            IPluginBase* GetBase()
            {
                return static_cast<IPluginBase*>( this );
            };

            // TODO: Add your concrete interface implementation
    };

    extern CPlugin[!output PROJECT_NAME_SAFE]* gPlugin;
}

/**
* @brief This function is required to use the Autoregister Flownode without modification.
* Include the file "CPlugin[!output PROJECT_NAME_SAFE].h" in front of flownode.
*/
inline void GameWarning( const char* sFormat, ... ) PRINTF_PARAMS( 1, 2 );
inline void GameWarning( const char* sFormat, ... )
{
    va_list ArgList;
    va_start( ArgList, sFormat );
    [!output PROJECT_NAME_SAFE]Plugin::gPlugin->LogV( ILog::eWarningAlways, sFormat, ArgList );
    va_end( ArgList );
};

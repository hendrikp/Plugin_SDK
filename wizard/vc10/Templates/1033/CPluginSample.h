/* [!output PROJECT_NAME_SAFE]_Plugin - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>

#include <IPluginManager.h>
#include <IPluginBase.h>
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
            void Release();

            int GetInitializationMode() const
            {
                return int( PluginManager::IM_Default );
            };

            bool Check( const char* sAPIVersion ) const;

            bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory );

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
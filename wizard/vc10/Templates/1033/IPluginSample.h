/* [!output PROJECT_NAME_SAFE]_Plugin - for licensing and copyright see license.txt */

#include <IPluginBase.h>

#pragma once

/**
* @brief [!output PROJECT_NAME_SAFE] Plugin Namespace
*/
namespace [!output PROJECT_NAME_SAFE]Plugin
{
    /**
    * @brief plugin manager interface
    */
    struct IPlugin[!output PROJECT_NAME_SAFE]
    {
        /**
        * @brief Get PluginManager base interface
        */
        virtual PluginManager::IPluginBase* GetBase() = 0;

        // TODO: Add your custom abstract functions here
    };
};
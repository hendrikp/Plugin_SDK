/* [!output PROJECT_NAME_SAFE]_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPlugin[!output PROJECT_NAME_SAFE].h>

namespace [!output PROJECT_NAME_SAFE]Plugin
{
    CPlugin[!output PROJECT_NAME_SAFE]* gPlugin = NULL;

    CPlugin[!output PROJECT_NAME_SAFE]::CPlugin[!output PROJECT_NAME_SAFE]()
    {
        gPlugin = this;
    }

    CPlugin[!output PROJECT_NAME_SAFE]::~CPlugin[!output PROJECT_NAME_SAFE]()
    {
        gPlugin = NULL;
    }

    void CPlugin[!output PROJECT_NAME_SAFE]::Release()
    {
        // Should be called while Game is still active otherwise there might be leaks/problems
        CPluginBase::Release();

        // Depending on your plugin you might not want to unregister anything
        // if the System is quitting.
        // if(gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting()) {

        // Unregister CVars
        if ( gEnv && gEnv->pConsole )
        {
            // ...
        }

        // Unregister game objects
        if ( gEnv && gEnv->pGameFramework )
        {
            // ...
        }

        // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
        gPluginManager->UnloadPlugin( GetName() );
    };

    bool CPlugin[!output PROJECT_NAME_SAFE]::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager )
    {
        gPluginManager = ( PluginManager::IPluginManager* )pPluginManager->GetConcreteInterface( NULL );
        CPluginBase::Init( env, startupParams, pPluginManager );

        // Register CVars/Commands
        if ( gEnv && gEnv->pConsole )
        {
            // TODO: Register CVARs/Commands here if you have some
            // ...
        }

        // Register Game Objects
        // TODO: Register Game Objects here if you have some
        // ...

        // Note: Autoregister Flownodes will be automatically registered

        return true;
    }

    const char* CPlugin[!output PROJECT_NAME_SAFE]::ListCVars() const
    {
        return "..."; // TODO: Enter CVARs/Commands here if you have some
    }

    bool CPlugin[!output PROJECT_NAME_SAFE]::Check( const char* sAPIVersion ) const
    {
        if ( !sAPIVersion )
        {
            return false;
        }

        // TODO: You could add a more complex version check here (e.g. if you support multiple versions)
        if ( SFileVersion( sAPIVersion ) == SFileVersion( "[!output PLUGIN_COMPATIBLITY]" ) )
        {
            return true;
        }

        return false;
    }

    const char* CPlugin[!output PROJECT_NAME_SAFE]::GetStatus() const
    {
        return "OK";
    }

    // TODO: Add your plugin concrete interface implementation
}
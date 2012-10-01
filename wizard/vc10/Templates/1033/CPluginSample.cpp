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

        // Register CVars
        if ( gEnv && gEnv->pConsole )
        {
            // ...
        }

        // Register Game Objects
        // ...

        return true;
    }

    const char* CPlugin[!output PROJECT_NAME_SAFE]::ListCVars() const
    {
        return "...";
    }

    bool CPlugin[!output PROJECT_NAME_SAFE]::Check( const char* sAPIVersion ) const
    {
        if ( !sAPIVersion )
        {
            return false;
        }

        if ( SFileVersion( sAPIVersion ) == SFileVersion( "3.4.0" ) )
        {
            return true;
        }

        return false;
    }

    const char* CPlugin[!output PROJECT_NAME_SAFE]::GetStatus() const
    {
        return "OK";
    }

    // TODO: add your plugin concrete interface implementation
}
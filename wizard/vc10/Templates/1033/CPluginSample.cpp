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
        Release( true );

        gPlugin = NULL;
    }

    bool CPlugin[!output PROJECT_NAME_SAFE]::Release( bool bForce )
    {
        bool bRet = true;

        if ( !m_bCanUnload )
        {
            // Should be called while Game is still active otherwise there might be leaks/problems
            bRet = CPluginBase::Release( bForce );

            if ( bRet )
            {
                // Depending on your plugin you might not want to unregister anything
                // if the System is quitting.
                if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
                {
                    // Unregister CVars
                    if ( gEnv && gEnv->pConsole )
                    {
                        // ...
                    }

                    // Unregister game objects
                    if ( gEnv && gEnv->pGameFramework && gEnv->pGame )
                    {
                        // ...
                    }
                }

                // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
                gPluginManager->UnloadPlugin( GetName() );

                // Allow Plugin Manager garbage collector to unload this plugin
                AllowDllUnload();
            }
        }

        return bRet;
    };

    bool CPlugin[!output PROJECT_NAME_SAFE]::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory )
    {
        gPluginManager = ( PluginManager::IPluginManager* )pPluginManager->GetConcreteInterface( NULL );
        CPluginBase::Init( env, startupParams, pPluginManager, sPluginDirectory );

        if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
        {
            // Register CVars/Commands
            if ( gEnv && gEnv->pConsole )
            {
                // TODO: Register CVARs/Commands here if you have some
                // ...
            }

            // Register Game Objects
            if ( gEnv && gEnv->pGameFramework )
            {
                // TODO: Register Game Objects here if you have some
                // ...
            }
        }

        // Note: Autoregister Flownodes will be automatically registered

        return true;
    }

    const char* CPlugin[!output PROJECT_NAME_SAFE]::ListCVars() const
    {
        return "..."; // TODO: Enter CVARs/Commands here if you have some
    }

    const char* CPlugin[!output PROJECT_NAME_SAFE]::GetStatus() const
    {
        return "OK";
    }

    // TODO: Add your plugin concrete interface implementation
}
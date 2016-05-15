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
        bool bWasInitialized = m_bIsFullyInitialized; // Will be reset by base class so backup

        if ( !m_bCanUnload )
        {
            // Note: Type Unregistration will be automatically done by the Base class (Through RegisterTypes)
            // Should be called while Game is still active otherwise there might be leaks/problems
            bRet = CPluginBase::Release( bForce );

            if ( bRet )
            {
                if ( bWasInitialized )
                {
                    // TODO: Cleanup stuff that can only be cleaned up if the plugin was initialized
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

        return true;
    }

    bool CPlugin[!output PROJECT_NAME_SAFE]::RegisterTypes( int nFactoryType, bool bUnregister )
    {
        // Note: Autoregister Flownodes will be automatically registered by the Base class
        bool bRet = CPluginBase::RegisterTypes( nFactoryType, bUnregister );

        using namespace PluginManager;
        eFactoryType enFactoryType = eFactoryType( nFactoryType );

        if ( bRet )
        {
            if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
            {
                // UIEvents
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_UIEvent ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVars here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVars here if you have some
                        // ...
                    }
                }

                // CVars
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_CVar ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVars here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVars here if you have some
                        // ...
                    }
                }

                // CVars Commands
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_CVarCommand ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVar Commands here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVar Commands here if you have some
                        // ...
                    }
                }

                // Game Objects
                if ( gEnv->pGame && gEnv->pGame->GetIGameFramework() && ( enFactoryType == FT_All || enFactoryType == FT_GameObjectExtension ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register Game Object Extensions here if you have some
                        // ...
                    }
                }
            }
        }

        return bRet;
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
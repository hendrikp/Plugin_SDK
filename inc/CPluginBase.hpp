/* Plugin SDK - for licensing and copyright see license.txt */

#pragma once

#include <IPluginBase.h>
#include <IPluginManager.h>

#if defined(PLUGINMANAGER_EXPORTS)
#include <CPluginManager.h>
#endif

#include <Nodes/G2FlowBaseNode.h>
#include <CPluginBaseMinimal.hpp>
#include <PMUtils.hpp>
#include <HookTool.hpp>

namespace PluginManager
{
    /**
    * @brief Plugin Base Class
    */
    class CPluginBase :
        public CPluginBaseMinimal,
        public IPluginLog
    {
        public:
            CPluginBase()
            {
                CPluginBaseMinimal();
            };

            virtual ~CPluginBase()
            {
                Release( true );
            };

            // IPluginBase
            virtual bool Release( bool bForce = false )
            {
                bool bRet = true;
                bool bWasInitialized = m_bIsFullyInitialized; // Will be reset by base class so backup

                if ( !m_bCanUnload )
                {
                    bRet = CPluginBaseMinimal::Release( bForce );

                    if ( bRet )
                    {
                        if ( bWasInitialized )
                        {
                            // Unregister all types
                            RegisterTypes( FT_All, true );
                        }
                    }
                }

                return bRet;
            };

            virtual bool InitDependencies()
            {
                return CPluginBaseMinimal::InitDependencies( );
            }

            virtual bool RegisterTypes( int nFactoryType, bool bUnregister )
            {
                bool bRet = CPluginBaseMinimal::RegisterTypes( nFactoryType, bUnregister );

                if ( bRet )
                {
                    eFactoryType enFactoryType = eFactoryType( nFactoryType );

                    if ( enFactoryType == FT_All || enFactoryType == FT_Flownode )
                    {
                        IFlowSystem* pFlow = NULL;

                        if ( !IsBadReadPtr( gEnv, sizeof( void* ) ) && !IsBadReadPtr( gEnv->pSystem, sizeof( void* ) ) && !gEnv->pSystem->IsQuitting() && gEnv->pGame && gEnv->pGame->GetIGameFramework() && ( pFlow = gEnv->pGame->GetIGameFramework()->GetIFlowSystem() ) )
                        {
                            if ( !bUnregister )
                            {
                                // Register all flownodes of this plugin in the crygame loading this plugin
                                for ( CG2AutoRegFlowNodeBase* pFactory = CG2AutoRegFlowNodeBase::m_pFirst; pFactory; pFactory = pFactory->m_pNext )
                                {
                                    TFlowNodeTypeId nTypeId = pFlow->RegisterType( pFactory->m_sClassName, pFactory );

                                    if ( nTypeId != InvalidFlowNodeTypeId )
                                    {
                                        LogAlways( "Flownode Class(%s) Ptr(%p) TypeId(%d) registered.", SAFESTR( pFactory->m_sClassName ), pFactory, ( int )nTypeId );
                                    }

                                    else
                                    {
                                        LogError( "Flownode Class(%s) Ptr(%p) couldn't register.", SAFESTR( pFactory->m_sClassName ), pFactory );
                                        //bRet = false;
                                    }
                                }
                            }

                            else
                            {
                                // Unregister all flownodes of this plugin
                                for ( CG2AutoRegFlowNodeBase* pFactory = CG2AutoRegFlowNodeBase::m_pFirst; pFactory; pFactory = pFactory->m_pNext )
                                {
                                    // A shame unregistering flownodes types still will produce errors later on in sandbox (then when clicked on)
                                    // it would be much better if UnregisterTypes would automatically unload and set related flownodes to missing status.
                                    bool bUnregistered = pFlow->UnregisterType( pFactory->m_sClassName );

                                    LogAlways( "Flownode Class(%s) Ptr(%p) Ret(%s) unregistered.", SAFESTR( pFactory->m_sClassName ), pFactory );
                                }
                            }
                        }

                        else if ( CG2AutoRegFlowNodeBase::m_pFirst )
                        {
                            LogWarning( "Flownodes couldn't be un/registered" );
                            //bRet = false;
                        }

                        // else there where no flownodes to register
                    }
                }

                return bRet;
            };

            virtual const char* ListNodes() const
            {
                static char sNodes[8192];
                char* sBeginPtr = sNodes;

                // List all flownodes of this plugin
                for ( CG2AutoRegFlowNodeBase* pFactory = CG2AutoRegFlowNodeBase::m_pFirst; pFactory; pFactory = pFactory->m_pNext )
                {
                    if ( pFactory->m_sClassName )
                    {
                        sBeginPtr += _snprintf_c( sBeginPtr, sizeof( sNodes ) - ( sBeginPtr - sNodes ), "%s,\n", pFactory->m_sClassName );
                    }
                }

                return sNodes;
            };

            // IPluginLog
            virtual void CPluginBase::LogAlways( const char* sFormat, ... ) const
            {
                va_list ArgList;
                va_start( ArgList, sFormat );
                LogV( ILog::eAlways, sFormat, ArgList );
                va_end( ArgList );
            }

            virtual void CPluginBase::LogWarning( const char* sFormat, ... ) const
            {
                va_list ArgList;
                va_start( ArgList, sFormat );
                LogV( ILog::eWarningAlways, sFormat, ArgList );
                va_end( ArgList );
            }

            virtual void CPluginBase::LogError( const char* sFormat, ... ) const
            {
                va_list ArgList;
                va_start( ArgList, sFormat );
                LogV( ILog::eErrorAlways, sFormat, ArgList );
                va_end( ArgList );
            }

            virtual void CPluginBase::LogV( ILog::ELogType nType, const char* sFormat, va_list ArgList ) const
            {
                if ( !sFormat )
                {
                    return;
                }

                string strFormat = "[";
                strFormat += GetName();
                strFormat += "_";
                strFormat += PLUGIN_TEXT;
                strFormat += "] ";
                strFormat += sFormat;

                if ( gEnv && !IsBadReadPtr( gEnv, sizeof( void* ) ) && gEnv->pSystem && !IsBadReadPtr( gEnv->pSystem, sizeof( void* ) ) && gEnv->pLog )
                {
                    gEnv->pLog->LogV( nType, strFormat.c_str(), ArgList );
                }
            };
    };
}
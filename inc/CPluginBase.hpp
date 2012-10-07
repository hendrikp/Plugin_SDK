/* Plugin SDK - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>

#include <IPluginBase.h>
#include <IPluginManager.h>

#if defined(PLUGINMANAGER_EXPORTS)
#include <CPluginManager.h>
#endif

#include <Nodes/G2FlowBaseNode.h>

#include <CPluginBaseMinimal.hpp>

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
                CPluginBaseMinimal::CPluginBaseMinimal();
            };

            virtual ~CPluginBase()
            { };

            // IPluginBase
            virtual bool Release( bool bForce = false )
            {
                bool bFlownodesPresent = m_bIsFullyInitialized;
                bool bRet = CPluginBaseMinimal::Release( bForce );

                if ( bRet )
                {
                    if ( bFlownodesPresent )
                    {
                        // Unregister flownodes
                        if ( gEnv && gEnv->pFlowSystem && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
                        {
                            // Flowsystem is required
                            IFlowSystem* pFlow = gEnv->pFlowSystem;

                            // Unregister all flownodes of this plugin
                            for ( CG2AutoRegFlowNodeBase* pFactory = CG2AutoRegFlowNodeBase::m_pFirst; pFactory; pFactory = pFactory->m_pNext )
                            {
                                // A shame unregistering flownodes types still will produce errors later on in sandbox (then when clicked on)
                                // it would be much better if UnregisterTypes would automatically unload and set related flownodes to missing status.
                                pFlow->UnregisterType( pFactory->m_sClassName );
                            }
                        }
                    }
                }

                return bRet;
            };

            virtual bool InitDependencies()
            {
                // Flowsystem is required
                IFlowSystem* pFlow = gEnv->pFlowSystem;

                if ( pFlow )
                {
                    // Register all flownodes of this plugin in the crygame loading this plugin
                    for ( CG2AutoRegFlowNodeBase* pFactory = CG2AutoRegFlowNodeBase::m_pFirst; pFactory; pFactory = pFactory->m_pNext )
                    {
                        pFlow->RegisterType( pFactory->m_sClassName, pFactory );
                    }
                }

                else if ( CG2AutoRegFlowNodeBase::m_pFirst )
                {
                    LogWarning( "Flownodes couldn't be registered" );
                }

                return CPluginBaseMinimal::InitDependencies( );
            }

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
                string strFormat = "[";
                strFormat += GetName();
                strFormat += "_";
                strFormat += PLUGIN_TEXT;
                strFormat += "] ";
                strFormat += sFormat;

                if ( gEnv && gEnv->pSystem && gEnv->pLog )
                {
                    gEnv->pLog->LogV( nType, strFormat.c_str(), ArgList );
                }
            };
    };
}
/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>

#include <IPluginBase.h>
#include <IPluginManager.h>

#if defined(PLUGINMANAGER_EXPORTS)
#include <CPluginManager.h>
#endif

#include <Nodes/G2FlowBaseNode.h>

namespace PluginManager
{
    /**
    * @brief Plugin Base Class
    */
    class CPluginBase :
        public IPluginBase,
        public IPluginLog
    {
        private:
            bool bIsUnloading;
            bool bIsInitialized;
            bool bIsFullyInitialized;

        public:
            CPluginBase()
            {
                bIsUnloading = false;
                bIsInitialized = false;
                bIsFullyInitialized = false;
            };

            ~CPluginBase() {};

            // IPluginBase
            virtual void CPluginBase::Release()
            {
                bIsUnloading = true;
                bIsInitialized = false;
                bIsFullyInitialized = false;

                // Unregister flownodes
                if ( gEnv && gEnv->pFlowSystem && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
                {
                    // Flowsystem is required
                    IFlowSystem* pFlow = gEnv->pFlowSystem;

                    // Unregister all flownodes of this plugin
                    for ( CG2AutoRegFlowNodeBase* pFactory = CG2AutoRegFlowNodeBase::m_pFirst; pFactory; pFactory = pFactory->m_pNext )
                    {
                        pFlow->UnregisterType( pFactory->m_sClassName );
                    }
                }
            };

            virtual bool IsUnloading()
            {
                return bIsUnloading;
            };

            virtual bool IsInitialized()
            {
                return bIsInitialized;
            };

            virtual bool IsFullyInitialized()
            {
                return bIsFullyInitialized;
            };

            virtual int GetInitializationMode() const
            {
                return int( IM_BeforeFramework );
            };

            virtual bool Check( const char* sAPIVersion ) const = 0;

            virtual bool CPluginBase::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager )
            {
                // Initialize Module
                ModuleInitISystem( env.pSystem, GetName() );

                // Flowsystem is required
                IFlowSystem* pFlow = env.pFlowSystem;

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

#if !defined(PLUGINMANAGER_EXPORTS)
                gPluginManager = ( IPluginManager* )pPluginManager->GetConcreteInterface( NULL );
#endif

                bIsInitialized = true;

                return true;
            };

            virtual bool CheckDependencies() const
            {
#if !defined(PLUGINMANAGER_EXPORTS)
                return gPluginManager && gPluginManager->GetBase();
#else
                return true;
#endif
            };

            virtual bool InitDependencies()
            {
                bIsFullyInitialized = true;
                return true;
            }

            virtual const char* GetVersion() const
            {
                return "1.0";
            };

            virtual const char* GetName() const
            {
                return "Undefined";
            };

            virtual const char* GetCategory() const
            {
                return "Undefined";
            };

            virtual const char* ListAuthors() const
            {
                return "";
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

            virtual const char* ListCVars() const
            {
                return "";
            };

            virtual const char* ListGameObjects() const
            {
                return "";
            };

            virtual const char* GetStatus() const
            {
                return "Undefined";
            };

            virtual const char* GetCurrentConcreteInterfaceVersion() const
            {
                return "0";
            };

            virtual void* GetConcreteInterface( const char* sInterfaceVersion )
            {
                return NULL;
            };

            virtual const char* GetCurrentExtendedInterfaceVersion() const
            {
                return "0";
            };

            virtual void* GetExtendedInterface( const char* sInterfaceVersion )
            {
                return NULL;
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
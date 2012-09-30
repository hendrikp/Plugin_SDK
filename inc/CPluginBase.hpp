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
        protected:
            bool m_bIsUnloading; //!< flag that the plugin is currently unloading and can't be used anymore
            bool m_bIsInitialized; //!< flag that everything for dependencies is now initialized
            bool m_bIsFullyInitialized; //!< flag that all dependencies are fully initialized and the plugin can be used now
            bool m_bCanUnload; //!< flag that plugin is fully released and the dll can be unloaded
            int m_nReferences; //!< reference counter

        public:
            CPluginBase()
            {
                m_nReferences = 1;
                m_bIsUnloading = false;
                m_bIsInitialized = false;
                m_bIsFullyInitialized = false;
                m_bCanUnload = false;
            };

            virtual ~CPluginBase()
            {
                Release( true );
            };

            // IPluginBase
            virtual void AddRef()
            {
                if ( ++m_nReferences > 0 )
                {
                    m_bIsUnloading = false;
                    m_bCanUnload = false;
                }
            };

            virtual bool CPluginBase::Release( bool bForce = false )
            {
                if ( --m_nReferences <= 0 || bForce )
                {
                    m_bIsUnloading = true;
                    m_nReferences = 0;

                    if ( m_bIsInitialized )
                    {
#if !defined(PLUGINMANAGER_EXPORTS)
                        gPluginManager->GetBase()->Release();
#endif
                    }

                    if ( m_bIsFullyInitialized )
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

                    m_bIsInitialized = false;
                    m_bIsFullyInitialized = false;
                }

                return m_bIsUnloading && m_nReferences == 0;
            };

            virtual bool DllCanUnloadNow()
            {
                return m_bCanUnload;
            }

            virtual bool IsUnloading()
            {
                return m_bIsUnloading;
            };

            virtual bool IsInitialized()
            {
                return m_bIsInitialized;
            };

            virtual bool IsFullyInitialized()
            {
                return m_bIsFullyInitialized;
            };

            virtual int GetInitializationMode() const
            {
                return int( IM_Default );
            };

            virtual bool Check( const char* sAPIVersion ) const = 0;

            virtual bool CPluginBase::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager )
            {
                // Initialize Module
                ModuleInitISystem( env.pSystem, GetName() );
                m_bIsInitialized = true;

#if !defined(PLUGINMANAGER_EXPORTS)
                gPluginManager = ( IPluginManager* )pPluginManager->GetConcreteInterface( NULL );

                if ( gPluginManager && pPluginManager )
                {
                    pPluginManager->AddRef();
                }

                else
                {
                    m_bIsInitialized = false;
                }

#endif

                return m_bIsInitialized;
            };

            virtual bool CheckDependencies() const
            {
#if !defined(PLUGINMANAGER_EXPORTS)
                return gEnv && gEnv->pSystem && gPluginManager && gPluginManager->GetBase();
#else
                return gEnv && gEnv->pSystem;
#endif
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

                m_bIsFullyInitialized = true;
                return  m_bIsFullyInitialized;
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
/* Plugin SDK - for licensing and copyright see license.txt */

#pragma once

#include <IPluginBase.h>
#include <IPluginManager.h>

#if defined(PLUGINMANAGER_EXPORTS)
#include <CPluginManager.h>
#endif

namespace PluginManager
{
    /**
    * @brief Plugin Minimal Base Class
    * Only the essentials for plugins that don't use autoregister flownodes or logging
    */
    class CPluginBaseMinimal :
        public IPluginBase
    {
        protected:
            bool m_bIsUnloading; //!< flag that the plugin is currently unloading and can't be used anymore
            bool m_bIsInitialized; //!< flag that everything for dependencies is now initialized
            bool m_bIsFullyInitialized; //!< flag that all dependencies are fully initialized and the plugin can be used now
            bool m_bCanUnload; //!< flag that plugin is fully released and the dll can be unloaded
            int m_nReferences; //!< reference counter

        public:
            CPluginBaseMinimal()
            {
                m_nReferences = 1;
                m_bIsUnloading = false;
                m_bIsInitialized = false;
                m_bIsFullyInitialized = false;
                m_bCanUnload = false;
            };

            virtual ~CPluginBaseMinimal()
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

            virtual bool Release( bool bForce = false )
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

                    m_bIsInitialized = false;
                    m_bIsFullyInitialized = false;
                }

                return m_bIsUnloading && m_nReferences == 0;
            };

            virtual bool DllCanUnloadNow()
            {
                return m_bCanUnload;
            }

        protected:
            /**
            * @brief This can be used by a plugin to tell the manager that the dll can be unloaded.
            * Call only after you cleaned up everything in the Release function
            */
            void AllowDllUnload()
            {
                m_bCanUnload = true;
            }

        public:
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

            virtual bool Check( const char* sAPIVersion ) const
            {
                return true;
            };

            virtual bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory )
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
                return "";
            };

            virtual const char* ListCVars() const
            {
                return "";
            };

            virtual const char* ListGameObjects() const
            {
                return "";
            };

            virtual const char* Dump() const
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
    };
}
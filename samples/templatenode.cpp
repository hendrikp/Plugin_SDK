/* TODO_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginTODO.h>

#include <Nodes/G2FlowBaseNode.h>

#include <Game.h>

namespace TODOPlugin
{
    class CFlowTODONode :
        public CFlowBaseNode<eNCT_Instanced>
    {
        private:

            enum EInputPorts
            {
                EIP_ACTIVATE = 0,
            };

            enum EOutputPorts
            {
                EOP_VALUE = 0,
            };
	    
        public:

            CFlowTODONode( SActivationInfo* pActInfo )
            {
            }

            virtual void GetMemoryUsage( ICrySizer* s ) const
            {
                s->Add( *this );
            }

            virtual IFlowNodePtr Clone( SActivationInfo* pActInfo )
            {
                return new CFlowTODONode( pActInfo );
            }

            virtual ~CFlowTODONode()
            {

            }

            virtual void GetConfiguration( SFlowNodeConfig& config )
            {
                static const SInputPortConfig inputs[] =
                {
                    InputPortConfig_Void( "Activate", _HELP( "Activate" ) ),
                    InputPortConfig<float>( "Value", 0, _HELP( "Value" ) ),
                    InputPortConfig_Null(),
                };
		
                static const SOutputPortConfig outputs[] =
                {
                    OutputPortConfig<float>( "Value", _HELP( "Value" ) ),
                    OutputPortConfig_Null(),
                };
		
                config.pInputPorts = inputs;
                config.pOutputPorts = outputs;
                config.sDescription = _HELP( "TODO" );

                config.SetCategory( EFLN_APPROVED );
            }

            virtual void ProcessEvent( EFlowEvent evt, SActivationInfo* pActInfo )
            {
                switch ( evt )
                {
                    case eFE_Suspend:
                        break;

                    case eFE_Resume:
                        break;

                    case eFE_Activate:
                        if ( IsPortActive( pActInfo, EIP_ACTIVATE ) )
                        {
                           
                        }

                        break;

                    case eFE_SetEntityId:
                        //m_pEntity = pActInfo->pEntity;
                        break;

                    case eFE_Update:
                        break;
                }
            }
    };
}

REGISTER_FLOW_NODE_EX( "Various:TODO_", TODOPlugin::CFlowTODONode, CFlowTODONode );
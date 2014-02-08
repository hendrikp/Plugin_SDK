#include "StdAfx.h"
#include <IPluginTODO.h>
#include "CTemplateEntity.h"

CTemplateEntity::CTemplateEntity( void )
{
}

void CTemplateEntity::Release()
{
    delete this;
}

bool CTemplateEntity::Init( IGameObject* pGameObject )
{
    SetGameObject( pGameObject );

    return true;
}


void CTemplateEntity::Update( SEntityUpdateContext& ctx, int updateSlot )
{

}

void CTemplateEntity::ProcessEvent( SEntityEvent& entityEvent )
{

}

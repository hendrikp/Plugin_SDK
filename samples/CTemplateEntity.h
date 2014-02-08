#pragma once
#include <IGameObject.h>

class CTemplateEntity : public CGameObjectExtensionHelper<CTemplateEntity, IGameObjectExtension>
{
    public:
        CTemplateEntity( void );

        // CGameObjectExtensionHelper
        virtual bool Init( IGameObject* pGameObject );
        virtual void ProcessEvent( SEntityEvent& entityEvent );
        virtual void Release();

        virtual void GetMemoryUsage( ICrySizer* pSizer ) const {}
        virtual void PostInit( IGameObject* pGameObject ) {}
        virtual void InitClient( int channelId ) {}
        virtual void PostInitClient( int channelId ) {}
        virtual bool ReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params )
        {
            return true;
        }
        virtual void PostReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params ) {}
        virtual bool GetEntityPoolSignature( TSerialize signature )
        {
            return true;
        }
        virtual void FullSerialize( TSerialize ser ) {}
        virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags )
        {
            return true;
        }
        virtual void PostSerialize() {}
        virtual void SerializeSpawnInfo( TSerialize ser ) {}
        virtual ISerializableInfoPtr GetSpawnInfo()
        {
            return ISerializableInfoPtr();
        }
        virtual void HandleEvent( const SGameObjectEvent& event ) {}
        virtual void SetChannelId( uint16 id ) {}
        virtual void SetAuthority( bool auth ) {}
        virtual const void* GetRMIBase() const
        {
            return 0;
        }
        virtual void PostUpdate( float frameTime ) {}
        virtual void PostRemoteSpawn() {}
        virtual void Update( SEntityUpdateContext& ctx, int updateSlot );
};


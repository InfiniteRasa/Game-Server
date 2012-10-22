#include"Global.h"

sint32 ceid_player = 0;
sint32 ceid_client = 0;
sint32 ceid_object = 0;
sint32 ceid_item = 0;
sint32 ceid_npc = 0;
sint32 ceid_creature = 0;
TMutex csEntityMgr;

#define ENTITYID_BASE	0x1000

unsigned long long entityMgr_getFreeEntityIdForClient()
{
	Thread::LockMutex(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_client*16 + ENTITYTYPE_CLIENT;
	ceid_client++;
	Thread::UnlockMutex(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForPlayer()
{
	Thread::LockMutex(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_player*16 + ENTITYTYPE_PLAYER;
	ceid_player++;
	Thread::UnlockMutex(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForItem()
{
	Thread::LockMutex(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_item*16 + ENTITYTYPE_ITEM;
	ceid_item++;
	Thread::UnlockMutex(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForObject()
{
	Thread::LockMutex(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_object*16 + ENTITYTYPE_OBJECT;
	ceid_object++;
	Thread::UnlockMutex(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForCreature()
{
	Thread::LockMutex(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_creature*16 + ENTITYTYPE_CREATURE;
	ceid_creature++;
	Thread::UnlockMutex(&csEntityMgr);
	return eid;
}



uint8 entityMgr_getEntityType(unsigned long long entityId)
{
	return entityId & 0xF;
}


hashTable_t ht_entityTable;

void entityMgr_registerEntity(unsigned long long entityId, void *entity)
{
	Thread::LockMutex(&csEntityMgr);
	hashTable_set(&ht_entityTable, entityId, entity);
	Thread::UnlockMutex(&csEntityMgr);
}

void entityMgr_unregisterEntity(unsigned long long entityId)
{
	Thread::LockMutex(&csEntityMgr);
	hashTable_set(&ht_entityTable, entityId, NULL);
	Thread::UnlockMutex(&csEntityMgr);
}

void *entityMgr_get(unsigned long long entityId)
{
	void *v = NULL;
	Thread::LockMutex(&csEntityMgr);
	v = hashTable_get(&ht_entityTable, entityId);
	Thread::UnlockMutex(&csEntityMgr);
	return v;
}

void entityMgr_init()
{
	Thread::InitMutex(&csEntityMgr);	
	hashTable_init(&ht_entityTable, 512);

	// get current npc
	unsigned long long npceid = DataInterface_NPC_getLastNPCEntityID();
	if( npceid < ENTITYID_BASE )
		npceid = ENTITYID_BASE;
	// ceid_npc
	npceid = (npceid - ENTITYID_BASE) / 16;
	npceid++;
	//npceid += 16;
	//npceid &= ~0xF;
	//npceid += ENTITYTYPE_NPC;
	ceid_npc = npceid;
}
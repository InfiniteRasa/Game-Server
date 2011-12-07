#include"Global.h"

int ceid_player = 0;
int ceid_client = 0;
int ceid_object = 0;
int ceid_item = 0;
int ceid_npc = 0;
int ceid_creature = 0;
CRITICAL_SECTION csEntityMgr;

#define ENTITYID_BASE	0x1000

unsigned long long entityMgr_getFreeEntityIdForClient()
{
	EnterCriticalSection(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_client*16 + ENTITYTYPE_CLIENT;
	ceid_client++;
	LeaveCriticalSection(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForPlayer()
{
	EnterCriticalSection(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_player*16 + ENTITYTYPE_PLAYER;
	ceid_player++;
	LeaveCriticalSection(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForItem()
{
	EnterCriticalSection(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_item*16 + ENTITYTYPE_ITEM;
	ceid_item++;
	LeaveCriticalSection(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForObject()
{
	EnterCriticalSection(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_object*16 + ENTITYTYPE_OBJECT;
	ceid_object++;
	LeaveCriticalSection(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForNPC()
{
	EnterCriticalSection(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_npc*16 + ENTITYTYPE_NPC;
	ceid_npc++;
	LeaveCriticalSection(&csEntityMgr);
	return eid;
}

unsigned long long entityMgr_getFreeEntityIdForCreature()
{
	EnterCriticalSection(&csEntityMgr);
	unsigned long long eid = ENTITYID_BASE + ceid_creature*16 + ENTITYTYPE_CREATURE;
	ceid_creature++;
	LeaveCriticalSection(&csEntityMgr);
	return eid;
}



unsigned char entityMgr_getEntityType(unsigned long long entityId)
{
	return entityId & 0xF;
}


HashTable_uint32_t ht_entityTable;

void entityMgr_registerEntity(unsigned long long entityId, void *entity)
{
	EnterCriticalSection(&csEntityMgr);
	hashTable_set(&ht_entityTable, entityId, entity);
	LeaveCriticalSection(&csEntityMgr);
}

void entityMgr_unregisterEntity(unsigned long long entityId)
{
	EnterCriticalSection(&csEntityMgr);
	hashTable_set(&ht_entityTable, entityId, NULL);
	LeaveCriticalSection(&csEntityMgr);
}

void *entityMgr_get(unsigned long long entityId)
{
	void *v = NULL;
	EnterCriticalSection(&csEntityMgr);
	v = hashTable_get(&ht_entityTable, entityId);
	LeaveCriticalSection(&csEntityMgr);
	return v;
}

void entityMgr_init()
{
	InitializeCriticalSection(&csEntityMgr);	
	hashTable_init(&ht_entityTable, 512);

	// get current npc
	unsigned long long npceid = dataInterface_NPC_getLastNPCEntityID();
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
#include"Global.h"

typedef struct  
{
	HashTable_stringSynced_t ht_creatureType;
}creatureEnv_t;

creatureEnv_t creatureEnv;

void creature_updateAppearance(clientGamemain_t* cgm, unsigned int entityId, int weaponId);
/* creature type */

creatureType_t* creatureType_createCreatureType(int entityClassId, int nameId)
{
	creatureType_t *creatureType = (creatureType_t*)malloc(sizeof(creatureType_t));
	memset(creatureType, 0, sizeof(creatureType_t));
	creatureType->entityClassId = entityClassId;
	creatureType->nameId = nameId;
	// set some default settings
	creatureType->maxHealth = 100;
	return creatureType;
}

creatureType_t* creatureType_findType(char *typeName)
{
	creatureType_t *creatureType = (creatureType_t*)hashTable_get(&creatureEnv.ht_creatureType, typeName);
	if( creatureType == NULL )
		return NULL; // type not found
	return creatureType;
}
/*
 * Registers the creature layout for use by the creature_createCreature() function
 */
void creatureType_registerCreatureType(creatureType_t *creatureType, char *name)
{
	hashTable_set(&creatureEnv.ht_creatureType, name, (void*)creatureType);
}

/*
 * Sets the maximum health of the creature type. This is also the value that is used when spawning creatures
 */
void creatureType_setMaxHealth(creatureType_t *creatureType, int maxHealth)
{
	creatureType->maxHealth = maxHealth;
}

/* creature */

creature_t* creature_createCreature(mapChannel_t *mapChannel, creatureType_t *creatureType, spawnPool_t *spawnPool, int faction)
{
	// allocate and init creature
	creature_t *creature = (creature_t*)malloc(sizeof(creature_t));
	memset(creature, 0, sizeof(creature_t));
	creature->type = creatureType; // direct pointer for fast access to type info
	creature->actor.entityClassId = creatureType->entityClassId;
	creature->actor.entityId = entityMgr_getFreeEntityIdForCreature();
	creature->spawnPool = spawnPool;
	// other settings
	creature->velocity = 5.0f;
	creature->attackspeed = 2000;
	creature->rotspeed = 1.8f;
	creature->attack_habbit = 2; //2=range fighter (test)
	creature->range = 22.40f;
	//if(faction <= 0) creature->faction = 0;
	creature->faction = faction; //hostile
	creature->currentHealth = creatureType->maxHealth;
	creature->controller.currentAction = 3;
	creature->wanderstate = 0; //wanderstate: calc new position
	//set wander boundaries 
	creature->wander_dist = 13.12f;
	// update spawnpool
	if( creature->spawnPool )
	{
		// TODO: check for dead-spawned creatures (if that makes any sense)
		spawnPool_increaseAliveCreatureCount(mapChannel, creature->spawnPool);
	}
	return creature;
}

creature_t* creature_createCreature(mapChannel_t *mapChannel, char *typeName, spawnPool_t *spawnPool)
{
	creatureType_t *creatureType = (creatureType_t*)hashTable_get(&creatureEnv.ht_creatureType, typeName);
	if( creatureType == NULL )
		return NULL; // type not found
	return creature_createCreature(mapChannel, creatureType, spawnPool,0);
}

void creature_setLocation(creature_t *creature, float x, float y, float z, float rX, float rY)
{
	//set spawnlocation
	creature->actor.posX = x;
	creature->actor.posY = y;
	creature->actor.posZ = z;
	//set homelocation
	creature->homePos.x = x;
	creature->homePos.y = y;
	creature->homePos.z = z;
	//allocate pathnodes
	creature->pathnodes = (baseBehavior_baseNode*) malloc(sizeof(baseBehavior_baseNode));
	memset(creature->pathnodes, 0x00, sizeof(baseBehavior_baseNode));
	creature->lastattack = GetTickCount();
	creature->lastresttime = GetTickCount();
}

/* update function*/
void creature_createCreatureOnClient(mapChannelClient_t *client, creature_t *creature)
{
	if( !creature )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->actor.entityId); // entityID
	pym_addInt(&pms, creature->actor.entityClassId); // classID
	pym_addNoneStruct(&pms); // entityData (dunno)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	// set attributes - Recv_AttributeInfo (29)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	// body
	pym_addInt(&pms, 1);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 15); // current
	pym_addInt(&pms, 15); // currentMax
	pym_addInt(&pms, 15); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// mind
	pym_addInt(&pms, 2);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10); // current
	pym_addInt(&pms, 0); // currentMax
	pym_addInt(&pms, 0); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// spirit
	pym_addInt(&pms, 3);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10); // current
	pym_addInt(&pms, 0); // currentMax
	pym_addInt(&pms, 0); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// health
	pym_addInt(&pms, 4);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->type->maxHealth); // current (current Max, base)
	pym_addInt(&pms, creature->type->maxHealth); // currentMax (modfierTarget?)
	pym_addInt(&pms, creature->currentHealth); // normalMax (current Value)
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 3); // refreshPeriod (seconds, float?)
	pym_tuple_end(&pms);
	// chi
	pym_addInt(&pms, 5);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10); // current
	pym_addInt(&pms, 50); // currentMax
	pym_addInt(&pms, 100); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// shit
	for(int i=6; i<=10; i++)
	{
		pym_addInt(&pms, i);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 10); // current
		pym_addInt(&pms, 50); // currentMax
		pym_addInt(&pms, 100); // normalMax
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod
		pym_tuple_end(&pms);
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 29, pym_getData(&pms), pym_getLen(&pms));
	// set level
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1); // level
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 103, pym_getData(&pms), pym_getLen(&pms));
	// set actor name
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addUnicode(&pms, creature->actor.name); // name
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 16, pym_getData(&pms), pym_getLen(&pms));
	// set creature info (439)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addNoneStruct(&pms); // pym_addInt(&pms, creature->type->nameId); // creatureNameId
	pym_addBool(&pms, false); // isFlyer
	pym_addNoneStruct(&pms); // leaderId
	pym_tuple_begin(&pms);	// creatureFlags 
	pym_tuple_end(&pms);	// creatureflag.pyo
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 439, pym_getData(&pms), pym_getLen(&pms));
	// set running
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 96, pym_getData(&pms), pym_getLen(&pms));
	// Recv_WorldLocationDescriptor (243)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_tuple_begin(&pms); // position
	pym_addInt(&pms, creature->actor.posX); // x 
	pym_addInt(&pms, creature->actor.posY); // y 
	pym_addInt(&pms, creature->actor.posZ); // z 
	pym_tuple_end(&pms); 
	pym_tuple_begin(&pms); // rotation quaterninion
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 1.0f);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 243, pym_getData(&pms), pym_getLen(&pms));
	// target category
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->faction); // 'HOSTILE'
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 211, pym_getData(&pms), pym_getLen(&pms));
	// is targetable
	// Recv_IsTargetable
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, true);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 594, pym_getData(&pms), pym_getLen(&pms));
	// set default state
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_list_begin(&pms);
	//pym_addInt(&pms, 11); // normal
	//pym_list_end(&pms);
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 206, pym_getData(&pms), pym_getLen(&pms));
	if (creature->actor.entityClassId == 25580) // pistol
	{
		 creature_updateAppearance(client->cgm, creature->actor.entityId, 3782);
	}
	if (creature->actor.entityClassId == 25581) // rifle
	{
		creature_updateAppearance(client->cgm, creature->actor.entityId, 3878);
	}
	if (creature->actor.entityClassId == 6163) // staff
	{
		 creature_updateAppearance(client->cgm, creature->actor.entityId, 6164);
	}
	if (creature->actor.entityClassId == 6043) // spear
	{
		creature_updateAppearance(client->cgm, creature->actor.entityId, 6042);
	}

	// Recv_WorldLocationDescriptor (243)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_tuple_begin(&pms); // position
	pym_addInt(&pms, creature->actor.posX); // x 
	pym_addInt(&pms, creature->actor.posY); // y 
	pym_addInt(&pms, creature->actor.posZ); // z 
	pym_tuple_end(&pms); 
	pym_tuple_begin(&pms); // rotation quaterninion
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 1.0f);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->mapChannel, creature->actor.entityId, 243, pym_getData(&pms), pym_getLen(&pms));
	
	netCompressedMovement_t netMovement = {0};
	netMovement.entityId = creature->actor.entityId;
	netMovement.posX24b = creature->actor.posX * 256.0f;
	netMovement.posY24b = creature->actor.posY * 256.0f;
	netMovement.posZ24b = creature->actor.posZ * 256.0f;
	netMovement.flag = 0x08;
	netMgr_cellDomain_sendEntityMovement(client->mapChannel, &creature->actor, &netMovement);
	//rangeattack (every 2 sec)
	/*srand(GetTickCount());
    int rnd1 = rand() % 200;
	unsigned int time = GetTickCount();
	if( (time-creature->lastattack) > creature->attackspeed )
	{
		//__debugbreak();
		creature->lastattack = time;
		//if( creature->controller.targetEntityId )
		

	}*/
}

void creature_updateAppearance(clientGamemain_t* cgm, unsigned int entityId, int weaponId)
{
	pyMarshalString_t pms;
	// Recv_AppearanceData (27)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
		pym_addInt(&pms, 13); // index(equipmentSlotId)
		pym_tuple_begin(&pms);
			pym_addInt(&pms, weaponId); // classId
			pym_tuple_begin(&pms);
				// hue
				pym_addInt(&pms, (int)(0xFF808080&0xFF));
				pym_addInt(&pms, (int)((0xFF808080>>8)&0xFF));
				pym_addInt(&pms, (int)((0xFF808080>>16)&0xFF));
				pym_addInt(&pms, (int)((0xFF808080>>24)&0xFF));
			pym_tuple_end(&pms);
		pym_tuple_end(&pms);
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityId, 27, pym_getData(&pms), pym_getLen(&pms));
}

// 1:n
void creature_cellIntroduceCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, int playerCount)
{
	for(int i=0; i<playerCount; i++)
	{
		creature_createCreatureOnClient(playerList[i], creature);
	}
}

// n:1
void creature_cellIntroduceCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, int creatureCount)
{
	for(int i=0; i<creatureCount; i++)
	{
		creature_createCreatureOnClient(client, creatureList[i]);
	}
}

void creature_cellDiscardCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, int playerCount)
{
	if( !creature )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->actor.entityId); // entityID
	pym_tuple_end(&pms);
	for(int i=0; i<playerCount; i++)
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
}

void creature_cellDiscardCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, int creatureCount)
{
	if( !client )
		return;
	pyMarshalString_t pms;
	for(int i=0; i<creatureCount; i++)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, creatureList[i]->actor.entityId); // entityID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
	}
}

void creature_init()
{
	hashTable_init(&creatureEnv.ht_creatureType, 65536); // have to set limit high, see below..
	/*
		note on internal string hashtable handling - the table can become 'full' and does NOT auto extend the size during runtime
	    todo: find a way to auto-extend the string hashtable without having to store all strings.
		idea: base the hashtable on uint32 table (which does auto-extend) and just generate a string seed as the uint32
	*/


	// init test creature
	/* 7078, "Testcreature" */

	//20110728 - thuvvik complete "creature dictionary"
	int i=0;
	for (i=1; i<65535; i++)
	{
		char buffer [50];
		int n, a=5, b=3;
		n=sprintf (buffer, "TEST%d", i);

		

		creatureType_t* ct = creatureType_createCreatureType(i, 1337);
		creatureType_setMaxHealth(ct, 150);

		creatureType_registerCreatureType(ct, buffer);
	}
}
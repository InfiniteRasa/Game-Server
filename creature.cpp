#include"Global.h"

typedef struct  
{
	hashTableSynced_t ht_creatureMissile;
	hashTableSynced_t ht_creatureType;
}creatureEnv_t;

creatureEnv_t creatureEnv;

void creature_updateAppearance(clientGamemain_t* cgm, uint32 entityId, sint32 weaponId);
/* creature type */

creatureType_t* creatureType_createCreatureType(sint32 entityClassId, sint32 nameId)
{
	creatureType_t *creatureType = (creatureType_t*)malloc(sizeof(creatureType_t));
	memset(creatureType, 0, sizeof(creatureType_t));
	creatureType->entityClassId = entityClassId;
	creatureType->nameId = nameId;
	// set some default settings
	creatureType->maxHealth = 100;
	return creatureType;
}

creatureType_t* creatureType_findType(sint32 typeId)
{
	creatureType_t *creatureType = (creatureType_t*)hashTable_get(&creatureEnv.ht_creatureType, typeId);
	if( creatureType == NULL )
		return NULL; // type not found
	return creatureType;
}


/*
 * Registers the creature layout for use by the creature_createCreature() function
 */
void creatureType_registerCreatureType(creatureType_t *creatureType, sint32 typeId)
{
	hashTable_set(&creatureEnv.ht_creatureType, typeId, (void*)creatureType);
}

/*
 * Sets the maximum health of the creature type. This is also the value that is used when spawning creatures
 */
void creatureType_setMaxHealth(creatureType_t *creatureType, sint32 maxHealth)
{
	creatureType->maxHealth = maxHealth;
}

/* creature */

creature_t* creature_createCreature(mapChannel_t *mapChannel, creatureType_t *creatureType, spawnPool_t *spawnPool)
{
	// allocate and init creature
	creature_t *creature = (creature_t*)malloc(sizeof(creature_t));
	memset(creature, 0, sizeof(creature_t));
	creature->type = creatureType; // direct pointer for fast access to type info
	creature->actor.entityClassId = creatureType->entityClassId;
	creature->actor.entityId = entityMgr_getFreeEntityIdForCreature();
	creature->actor.stats.level = 1; // test
	creature->updatePositionCounter = CREATURE_LOCATION_UPDATE_TIME;
	creature->type = creatureType;
	creature->spawnPool = spawnPool;
	//if(spawnPool == NULL)
	//{
 //        creature->actor.attackstyle = 1;  
	//	 creature->actor.actionid = 174; //melee
	//	 creature->velocity = 1.1f;
	//     creature->attackspeed = 10000;
	//	 creature->attack_habbit = 1;
	//	 creature->agression = 5000; // hunt enemy for 5 seconds if out of melee range
	//	 creature->lastagression = 0;
	//	 creature->rottime = 350;
	//	 creature->range = 26.40f;
	//	 creature->actor.stats.healthCurrent = creatureType->maxHealth;
	//	 creature->controller.currentAction = 3;
	//	 creature->wanderstate = 0; //wanderstate: calc new position
	//	 creature->movestate = 0; // check if other entity is too close(to avoid overlapping)
	//	 //set wander boundaries 
	//	 creature->wander_dist = 11.12f;
	//	 creature->aggrocount = 0;
	//	 return creature;
	//}
	//creature->actor.attackstyle = spawnPool->attackanim;
	//creature->actor.actionid = spawnPool->actionid;
	//creature->spawnPool = spawnPool;
	//// other settings
	//creature->velocity = spawnPool->velocity;
	//creature->attackspeed = spawnPool->attackspeed;
	////creature->attackanim = spawnPool->attackanim;
	//creature->agression = 5000; // hunt enemy for 5 seconds if out of melee range
	//creature->lastagression = 0;
	//creature->rottime = 350;
	//creature->attack_habbit = spawnPool->attackstyle; //0=range fighter (test)
	//creature->range = 26.40f;
	////if(faction <= 0) creature->faction = 0;
	//creature->faction = faction; //hostile
	creature->actor.stats.healthCurrent = creatureType->maxHealth;
	creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
	creature->controller.actionWander.state = 0; //wanderstate: calc new position
	//set wander boundaries 
	//creature->wander_dist = 11.12f;
	// update spawnpool
	if( creature->spawnPool )
	{
		// TODO: check for dead-spawned creatures (if that makes any sense)
		spawnPool_increaseAliveCreatureCount(mapChannel, creature->spawnPool);
	}
	return creature;
}

void creature_setLocation(creature_t *creature, float x, float y, float z, float rX, float rY)
{
	// set spawnlocation
	creature->actor.posX = x;
	creature->actor.posY = y;
	creature->actor.posZ = z;
	// set homelocation
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
	pym_addInt(&pms, creature->actor.stats.healthCurrent); // current (current Max, base)
	pym_addInt(&pms, creature->type->maxHealth); // currentMax (modfierTarget?)
	pym_addInt(&pms, creature->type->maxHealth); // normalMax (current Value)
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
	for(sint32 i=6; i<=10; i++)
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
	pym_addInt(&pms, creature->actor.stats.level); // level
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 103, pym_getData(&pms), pym_getLen(&pms));
	// set creature info (439)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	if( creature->type->nameId == 0 )
		pym_addNoneStruct(&pms); // creatureNameId (none, server defines name)
	else
		pym_addInt(&pms, creature->type->nameId); // use creaturename table to lookup translated name
	pym_addBool(&pms, false); // isFlyer
	pym_addNoneStruct(&pms); // leaderId
	pym_tuple_begin(&pms);	// creatureFlags 
	pym_tuple_end(&pms);	// creatureflag.pyo
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, METHODID_CREATUREINFO, pym_getData(&pms), pym_getLen(&pms));
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
	pym_tuple_begin(&pms); // rotation quaternion
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 1.0f);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->mapChannel, creature->actor.entityId, 243, pym_getData(&pms), pym_getLen(&pms));
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->type->faction); // 'HOSTILE'
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 211, pym_getData(&pms), pym_getLen(&pms));
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

	if (creature->actor.entityClassId == 29765) // pistol
	{
		creature_updateAppearance(client->cgm, creature->actor.entityId, 6443);
		// weapon ready
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 575, pym_getData(&pms), pym_getLen(&pms));
	}
	if (creature->actor.entityClassId == 29423) 
	{
		creature_updateAppearance(client->cgm, creature->actor.entityId, 6443);
		// weapon ready
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 575, pym_getData(&pms), pym_getLen(&pms));
	}

	if( creature->actor.stats.healthCurrent <= 0 )
	{
        creature->actor.state = ACTOR_STATE_DEAD;
		// dead!
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_list_begin(&pms);
		pym_addInt(&pms, 5); // dead
		pym_list_end(&pms);
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, &creature->actor, creature->actor.entityId, 206, pym_getData(&pms), pym_getLen(&pms));
		// fix health
		creature->actor.stats.healthCurrent = 0;
	}
	if( creature->type->npcData )
	{
		npc_creature_updateConversationStatus(client, creature);
		// send Recv_NPCInfo (only npcPackageId)
		// the only reason to send this is because the language lookup for mission objectives needs it...
		if( creature->type->npcData->npcPackageId != 0 )
		{
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, creature->type->npcData->npcPackageId); // the glorious npcPackageId
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, &creature->actor, creature->actor.entityId, 490, pym_getData(&pms), pym_getLen(&pms));
		}
 	}
}

void creature_updateAppearance(clientGamemain_t* cgm, uint32 entityId, sint32 weaponId)
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
				pym_addInt(&pms, (sint32)( 0xFF808080			& 0xFF));
				pym_addInt(&pms, (sint32)((0xFF808080 >> 8)	& 0xFF));
				pym_addInt(&pms, (sint32)((0xFF808080 >> 16)	& 0xFF));
				pym_addInt(&pms, (sint32)((0xFF808080 >> 24)	& 0xFF));
			pym_tuple_end(&pms);
			// test for .16
			pym_tuple_begin(&pms);
				// hue
				pym_addInt(&pms, (sint32)( 0xFF808080			& 0xFF));
				pym_addInt(&pms, (sint32)((0xFF808080 >> 8)	& 0xFF));
				pym_addInt(&pms, (sint32)((0xFF808080 >> 16)	& 0xFF));
				pym_addInt(&pms, (sint32)((0xFF808080 >> 24)	& 0xFF));
			pym_tuple_end(&pms);
			// end test for .16
		pym_tuple_end(&pms);
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityId, 27, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Called whenever a creature is killed
 */
void creature_handleCreatureKill(mapChannel_t* mapChannel, creature_t *creature, actor_t* killedBy)
{
	pyMarshalString_t pms;
	creature->actor.state = ACTOR_STATE_DEAD;
	// dead
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms);
	pym_addInt(&pms, 5); // dead
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, METHODID_STATECHANGE, pym_getData(&pms), pym_getLen(&pms));
	// tell spawnpool if set
	if( creature->spawnPool )
	{
		spawnPool_decreaseAliveCreatureCount(mapChannel, creature->spawnPool);
		spawnPool_increaseDeadCreatureCount(mapChannel, creature->spawnPool);
	}
	// todo: How were credits and experience calculated when multiple players attacked the same creature? Did only the player with the first strike get experience?
	mapChannelClient_t* cm = killedBy->owner;
	if( cm )
	{
		// give credits
		sint32 creditsToAdd = creature->actor.stats.level * 5;
		sint32 creditAddRange = creature->actor.stats.level * 2;
		creditsToAdd += ((rand()%(creditAddRange*2+1))-creditAddRange);
		manifestation_GainCredits(cm, creditsToAdd);
		// give experience
		sint32 experience = creature->actor.stats.level * 100; // base experience
		sint32 experienceRange = creature->actor.stats.level * 10;
		experience += ((rand()%(experienceRange*2+1))-experienceRange);
		// todo: Depending on level difference reduce experience
		manifestation_GainExperience(cm, experience);
	}
}

// 1:n
void creature_cellintroduceCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, sint32 playerCount)
{
	for(sint32 i=0; i<playerCount; i++)
	{
		creature_createCreatureOnClient(playerList[i], creature);
	}
}

// n:1
void creature_cellIntroduceCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, sint32 creatureCount)
{
	for(sint32 i=0; i<creatureCount; i++)
	{
		creature_createCreatureOnClient(client, creatureList[i]);
	}
}

void creature_cellDiscardCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, sint32 playerCount)
{
	if( !creature )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->actor.entityId); // entityID
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
}

void creature_cellDiscardCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, sint32 creatureCount)
{
	if( !client )
		return;
	pyMarshalString_t pms;
	for(sint32 i=0; i<creatureCount; i++)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, creatureList[i]->actor.entityId); // entityID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
	}
}

/*
 * Updates the cell location of the creature
 * Sends create and destroy packets to all affected clients
 */
void creature_cellUpdateLocation(mapChannel_t *mapChannel, creature_t* creature, sint32 newLocX, sint32 newLocZ)
{
	sint32 oldLocX = creature->actor.cellLocation.x;
	sint32 oldLocZ = creature->actor.cellLocation.z;
	// get old and new cell
	mapCell_t *oldMapCell = cellMgr_getCell(mapChannel, oldLocX, oldLocZ);
	mapCell_t *newMapCell = cellMgr_getCell(mapChannel, newLocX, newLocZ);
	if( newMapCell == NULL || oldMapCell == NULL )
		return;
	// get both notifier lists
	sint32 oldNotifierCount = oldMapCell->ht_playerNotifyList.size();
	mapChannelClient_t** oldNotifiers = NULL;
	if( oldNotifierCount )
		oldNotifiers = &oldMapCell->ht_playerNotifyList[0];
	sint32 newNotifierCount = newMapCell->ht_playerNotifyList.size();
	mapChannelClient_t** newNotifiers = NULL;
	if( newNotifierCount )
		newNotifiers = &newMapCell->ht_playerNotifyList[0];
	// create for new players
	for(sint32 n=0; n<newNotifierCount; n++)
	{
		bool isNew = true;
		for(sint32 m=0; m<oldNotifierCount; m++)
		{
			if( newNotifiers[n] == oldNotifiers[m] )
			{
				isNew = false;
				break;
			}
		}
		if( isNew )
			creature_createCreatureOnClient(newNotifiers[n], creature);
	}
	// destroy for old players
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->actor.entityId); // entityID
	pym_tuple_end(&pms);
	for(sint32 m=0; m<oldNotifierCount; m++)
	{
		bool isGone = true;
		for(sint32 n=0; n<newNotifierCount; n++)
		{
			if( oldNotifiers[m] == newNotifiers[n] )
			{
				isGone = false;
				break;
			}
		}
		if( isGone )
			netMgr_pythonAddMethodCallRaw(&oldNotifiers[m], 1, 5, METHODID_DESTROYPHYSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	}
	// update location
	creature->actor.cellLocation.x = newLocX;
	creature->actor.cellLocation.z = newLocZ;
	// move creature entry
	// we cannot delete the creature right now from the list because this might invalidate current iterators
	// therefore we use a little hack, see updateIterator variable in behaviorController.cpp
	//std::vector<creature_t*>::iterator itr = oldMapCell->ht_creatureList.begin();
	//while (itr != oldMapCell->ht_creatureList.end())
	//{
	//	if ((*itr) == creature)
	//	{
	//		oldMapCell->ht_creatureList.erase(itr);
	//		break;
	//	}
	//	itr++;
	//}
	newMapCell->ht_creatureList.push_back(creature);
}

/*
 * Called for every row of missile data the db returns
 */
void _creature_dbCreatureMissile_cb(void *param, diJob_creatureAction_t *jobData)
{
	creatureMissile_t* creatureMissile = (creatureMissile_t*)malloc(sizeof(creatureMissile_t));
	memset(creatureMissile, 0x00, sizeof(creatureMissile_t));
	// copy values
	creatureMissile->id = jobData->id;
	creatureMissile->actionId = jobData->actionId;
	creatureMissile->actionArgId = jobData->actionArgId;
	creatureMissile->rangeMin = jobData->rangeMin;
	creatureMissile->rangeMax = jobData->rangeMax;
	creatureMissile->recoverTime = jobData->recoverTime;
	creatureMissile->recoverTimeGlobal = jobData->recoverTimeGlobal;
	creatureMissile->windupTime = jobData->windupTime;
	creatureMissile->minDamage = jobData->minDamage;
	creatureMissile->maxDamage = jobData->maxDamage;
	// register
	hashTable_set(&creatureEnv.ht_creatureMissile, jobData->id, creatureMissile);
}

void _creature_dbCreatureType_cb(void *param, diJob_creatureType_t *jobData)
{
	printf("Load creature type '%s'\n", jobData->name);
	creatureType_t *creatureType = (creatureType_t*)malloc(sizeof(creatureType_t));
	memset(creatureType, 0, sizeof(creatureType_t));
	creatureType->typeId = jobData->id;
	creatureType->entityClassId = jobData->classId;
	creatureType->nameId = jobData->nameId;
	strcpy(creatureType->name, jobData->name);
	// faction and flags
	creatureType->faction = jobData->faction;
	// attributes and stats
	creatureType->maxHealth = jobData->hitpoints;
	creatureType->walkspeed = jobData->walkspeed;
	creatureType->runspeed = jobData->runspeed;
	creatureType->wander_dist = jobData->wanderDistance;
	// missile data (available attack actions for creature)
	sint32 missileCount = 0;
	for(sint32 i=0; i<8; i++)
	{
		if( jobData->missile[i] == 0 )
			continue;
		creatureMissile_t* creatureMissile = (creatureMissile_t*)hashTable_get(&creatureEnv.ht_creatureMissile, jobData->missile[i]);
		if( creatureMissile == NULL )
		{
			printf("Could not load missile/action %d for creature type %d\n", jobData->missile[i], jobData->id);
			continue;
		}
		creatureType->actions[missileCount] = creatureMissile;
		missileCount++;
	}
	creatureType->aggressionTime = 5000;
	creatureType_registerCreatureType(creatureType, jobData->id);
}

void creature_init()
{
	// load creature missiles (melee and range attacks, abilities)
	hashTable_init(&creatureEnv.ht_creatureMissile, 128);
	DataInterface_Creature_getCreatureActionList(_creature_dbCreatureMissile_cb, NULL);
	// load 
	hashTable_init(&creatureEnv.ht_creatureType, 128);
	DataInterface_Creature_getCreatureTypeList(_creature_dbCreatureType_cb, NULL);


	// init test creature
	/* 7078, "Testcreature" */

	//20110728 - thuvvik complete "creature dictionary"
	//sint32 i=0;
	//for (i=1; i<65535; i++)
	//{
	//	sint8 buffer [50];
	//	sint32 n, a=5, b=3;
	//	n=sprintf (buffer, "TEST%d", i);

	//	

	//	creatureType_t* ct = creatureType_createCreatureType(i, 1337);
	//	creatureType_setMaxHealth(ct, 150);

	//	ct->meleeAttack.missile = MISSILE_PISTOL;
	//	ct->meleeAttack.damageMin = 7;
	//	ct->meleeAttack.damageMax = 11;
	//	ct->rangeAttack.missile = MISSILE_PISTOL;
	//	ct->rangeAttack.damageMin = 10;
	//	ct->rangeAttack.damageMax = 15;
	//	//---files: actiondata,

	//	if (ct->entityClassId == 6043) // forean spearman
	//	{
	//		ct->rangeAttack.missile = MISSILE_PISTOL;
	//		// no range attack
	//		ct->rangeAttack.damageMin = 0;
	//		ct->rangeAttack.damageMax = 0;
	//	}
	//	else if (ct->entityClassId == 25580) // bane pistol
	//	{
	//		ct->rangeAttack.missile = MISSILE_THRAX_PISTOL;
	//		ct->meleeAttack.missile = MELEE_THRAX;
 // 		}
 // 		else if (ct->entityClassId == 6031) // boargar
	//	{
 // 			ct->rangeAttack.missile = MELEE_BOARGAR;
	//		ct->meleeAttack.missile = MELEE_BOARGAR;
	//	}
	//	else if (ct->entityClassId == 29765 || ct->entityClassId == 29765) // human soldiers
	//	{
	//		ct->rangeAttack.missile = MISSILE_PISTOL;
	//		ct->meleeAttack.missile = MELEE_PISTOL;
	//	}
	//	else if(ct->entityClassId == 10166) //bane hunter
	//	{
 //           ct->rangeAttack.missile = MISSILE_HUNTER_PULSEGUN;
	//		ct->meleeAttack.missile = MELEE_HUNTER;
	//	}
	//	else if(ct->entityClassId == 6032) //amoeboid
	//	{
 //           ct->rangeAttack.missile = MELEE_AMOEBOID;
	//		ct->meleeAttack.missile = MELEE_AMOEBOID;
	//	}
	//	else if(ct->entityClassId == 10442) //AFS Mech
	//	{
 //           ct->rangeAttack.missile = MISSILE_AFSMECH_MG;
	//		ct->meleeAttack.missile = MISSILE_AFSMECH_MG;
	//	}
	//	else if(ct->entityClassId == 3781) //Bane Stalker
	//	{
 //           ct->rangeAttack.missile = MISSILE_STALKER;
	//		ct->meleeAttack.missile = MISSILE_STALKER;
	//	}

	//	creatureType_registerCreatureType(ct, buffer);
	//}
}
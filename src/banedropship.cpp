#include"global.h"

extern dynObjectType_t dynObjectType_banedropship;

typedef struct  
{
	sint32 phaseTimeleft;
	sint8 phase;
	spawnPool_t *spawnPool; // spawnPool the dropship belongs to
	sint8 spawnCount;
	creatureType_t *spawnTypeList[24]; // a maximum of 24 creatures can be spawned at once
}dynObject_baneDropship_t;


void banedropship_create(mapChannel_t *mapChannel, float x, float y, float z, sint32 spawnCount, creatureType_t **spawnTypeList, spawnPool_t *spawnPool)
{
	dynObject_t *dynObject = _dynamicObject_create(9268, &dynObjectType_banedropship);
	if( !dynObject )
		return;
	dynObject_baneDropship_t *banedropShipData = (dynObject_baneDropship_t*)malloc(sizeof(dynObject_baneDropship_t));
	memset(banedropShipData, 0x00, sizeof(dynObject_baneDropship_t));
	dynObject->objectData = banedropShipData;
	dynObject->stateId = 188;
	banedropShipData->phaseTimeleft = 5800;
	banedropShipData->phase = 0;
	banedropShipData->spawnPool = spawnPool;
	banedropShipData->spawnCount = spawnCount;
	for(sint32 i=0; i<spawnCount; i++)
		banedropShipData->spawnTypeList[i] = spawnTypeList[i];

	float randomX = 2.0f - (float)(rand()%100) * 0.04f;
	float randomY = 0.5f - (float)(rand()%100) * 0.01f;
	float randomZ = 2.0f - (float)(rand()%100) * 0.04f;

	dynamicObject_setPosition(dynObject, x + randomX, y + randomY, z + randomZ);

	// randomize position
	float randomRotY = (float)(rand()%640) * 0.01f;
	float randomRotX = 0.0f;//(float)(rand()%80) * 0.01f - 0.4f;
	float randomRotZ = 0.0f;//(float)(rand()%80) * 0.01f - 0.4f;
	dynamicObject_setRotation(dynObject, randomRotY, randomRotX, randomRotZ);
	cellMgr_addToWorld(mapChannel, dynObject);
	// if spawnpool set, increase counters
	if( spawnPool )
	{
		spawnPool_increaseQueueCount(mapChannel, spawnPool);
		spawnPool_increaseQueuedCreatureCount(mapChannel, spawnPool, banedropShipData->spawnCount);
	}
	// init timer
	dynamicObject_setPeriodicUpdate(mapChannel, dynObject, 0, 250);
}

void banedropship_create(mapChannel_t *mapChannel, float x, float y, float z, sint32 spawnCount, creatureType_t **spawnTypeList)
{
	banedropship_create(mapChannel, x, y, z, spawnCount, spawnTypeList, NULL);
}

void banedropship_init(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// init periodic timer

}

void banedropship_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{

}

void banedropship_appearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	/*
		Animation states for bane dropship:
			188 --> enter
			189 --> spawn-effect
			190 --> leave
			97 --> "USE_STATE_NULL"
	*/
	// useable info
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1);	// enabled
	pym_addInt(&pms, dynObject->stateId);	// curState
	pym_addInt(&pms, 0);	// nameOverrideId
	pym_addInt(&pms, 0);	// windupTime
	pym_addInt(&pms, 0);	// missionActivated
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(playerList, playerCount, dynObject->entityId, 229, pym_getData(&pms), pym_getLen(&pms));		
}

void banedropship_disappearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// not used
}

bool banedropship_periodicCallback(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	dynObject_baneDropship_t *baneDropshipData = (dynObject_baneDropship_t*)dynObject->objectData;
	if( baneDropshipData->phase == 0 )
	{
		baneDropshipData->phaseTimeleft -= timePassed;
		if( baneDropshipData->phaseTimeleft > 0 )
			return true;
		baneDropshipData->phaseTimeleft = 5000;
		baneDropshipData->phase = 1;
		dynObject->stateId = 189;
		
	}
	else if( baneDropshipData->phase == 1 )
	{
		baneDropshipData->phaseTimeleft -= timePassed;
		if( baneDropshipData->phaseTimeleft > 0 )
			return true;
		baneDropshipData->phaseTimeleft = 4000;
		baneDropshipData->phase = 2;
		// spawn creatures
		for(sint32 i=0; i<baneDropshipData->spawnCount;i++)
		{
			creature_t *creature = creature_createCreature(mapChannel, 
				baneDropshipData->spawnTypeList[i], 
				baneDropshipData->spawnPool);
			if( creature == NULL )
				continue;
			// set random spawn location
			sint32 srnd = rand() % 5;
			creature_setLocation(creature, dynObject->x+(float)srnd, dynObject->y, dynObject->z+(float)srnd, 0.0f, 0.0f);
			// set ai path if spawnpool has any
			if( baneDropshipData->spawnPool->pathCount > 0 )
				creature->controller.aiPathFollowing.generalPath = baneDropshipData->spawnPool->pathList[rand()%baneDropshipData->spawnPool->pathCount]; // select random path
			// add to world
			cellMgr_addToWorld(mapChannel, creature);
		}
		if( baneDropshipData->spawnPool )
			spawnPool_decreaseQueuedCreatureCount(mapChannel, baneDropshipData->spawnPool, baneDropshipData->spawnCount);
		return true;
	}
	else if( baneDropshipData->phase == 2 )
	{
		baneDropshipData->phaseTimeleft -= timePassed;
		if( baneDropshipData->phaseTimeleft > 0 )
			return true;
		baneDropshipData->phaseTimeleft = 4000;
		baneDropshipData->phase = 3;
		dynObject->stateId = 190;
	}
	else if( baneDropshipData->phase == 3 )
	{
		baneDropshipData->phaseTimeleft -= timePassed;
		if( baneDropshipData->phaseTimeleft > 0 )
			return true;
		baneDropshipData->phaseTimeleft = 3800;
		baneDropshipData->phase = 4;
		// not real state update, just sub-phase to spawn creatures
		return true;
	}
	else if( baneDropshipData->phase == 4 )
	{
		// if spawnpool set, decrease counter
		if( baneDropshipData->spawnPool )
			spawnPool_decreaseQueueCount(mapChannel, baneDropshipData->spawnPool);
		// remove object
		dynamicObject_destroy(mapChannel, dynObject);
		return false;
	}
	// send update
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addNoneStruct(&pms);	// actorId
	pym_addInt(&pms, dynObject->stateId);	// curStateId
	pym_addInt(&pms, 0);	// windupTimeMs
	pym_tuple_begin(&pms);	// args
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, dynObject, dynObject->entityId, 230, pym_getData(&pms), pym_getLen(&pms));

	//netMgr_se
	//dynObject->x += 2.5f;
	//dynObject->velocity = 5.0f;
	//netMgr_cellDomain_sendObjectMovement(mapChannel, dynObject);
	return true;
}

void banedropship_useObject(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	// not used
}


dynObjectType_t dynObjectType_banedropship = 
{
	banedropship_destroy, // destroy
	banedropship_appearForPlayers, // appearForPlayers
	banedropship_disappearForPlayers, // disappearForPlayer
	banedropship_periodicCallback, // periodicCallback
	banedropship_useObject, // useObject
};
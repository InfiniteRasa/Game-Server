#include"global.h"

extern dynObjectType_t dynObjectType_humandropship;

typedef struct  
{
	sint32 phaseTimeleft;
	sint8 phase;
	spawnPool_t *spawnPool; // spawnPool the dropship belongs to
	sint8 spawnCount;
	creatureType_t *spawnTypeList[24]; // a maximum of 24 creatures can be spawned at once
}dynObject_humanDropship_t;


void humandropship_create(mapChannel_t *mapChannel, float x, float y, float z, sint32 spawnCount, creatureType_t **spawnTypeList, spawnPool_t *spawnPool)
{
	dynObject_t *dynObject = _dynamicObject_create(9269, &dynObjectType_humandropship);
	if( !dynObject )
		return;
	dynObject_humanDropship_t *humandropShipData = (dynObject_humanDropship_t*)malloc(sizeof(dynObject_humanDropship_t));
	memset(humandropShipData, 0x00, sizeof(dynObject_humanDropship_t));
	dynObject->objectData = humandropShipData;
	dynObject->stateId = 188;
	humandropShipData->phaseTimeleft = 5400;
	humandropShipData->phase = 0;
	humandropShipData->spawnPool = spawnPool;
	humandropShipData->spawnCount = spawnCount;
	for(sint32 i=0; i<spawnCount; i++)
		humandropShipData->spawnTypeList[i] = spawnTypeList[i];

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
		spawnPool_increaseQueuedCreatureCount(mapChannel, spawnPool, humandropShipData->spawnCount);
	}
	// init timer
	dynamicObject_setPeriodicUpdate(mapChannel, dynObject, 0, 250);
}

void humandropship_create(mapChannel_t *mapChannel, float x, float y, float z, sint32 spawnCount, creatureType_t **spawnTypeList)
{
	humandropship_create(mapChannel, x, y, z, spawnCount, spawnTypeList, NULL);
}

void humandropship_init(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// init periodic timer
}

void humandropship_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// destroy dropship, need to be added
}

void humandropship_appearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	/*
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

void humandropship_disappearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// not used
}

bool humandropship_periodicCallback(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	dynObject_humanDropship_t *humanDropshipData = (dynObject_humanDropship_t*)dynObject->objectData;
	if( humanDropshipData->phase == 0 )
	{
		humanDropshipData->phaseTimeleft -= timePassed;
		if( humanDropshipData->phaseTimeleft > 0 )
			return true;
		humanDropshipData->phaseTimeleft = 500;
		humanDropshipData->phase = 1;
		dynObject->stateId = 189;
	}
	else if( humanDropshipData->phase == 1 )
	{
		humanDropshipData->phaseTimeleft -= timePassed;
		if( humanDropshipData->phaseTimeleft > 0 )
			return true;
		humanDropshipData->phaseTimeleft = 4200;
		humanDropshipData->phase = 2;
		// spawn creatures
		for(sint32 i=0; i<humanDropshipData->spawnCount;i++)
		{
			creature_t *creature = creature_createCreature(mapChannel, 
				humanDropshipData->spawnTypeList[i], 
				humanDropshipData->spawnPool);
			if( creature == NULL )
				continue;
			// set random spawn location
			sint32 srnd = rand() % 5;
			creature_setLocation(creature, dynObject->x+(float)srnd, dynObject->y, dynObject->z+(float)srnd, 0.0f, 0.0f);
			// set ai path if spawnpool has any
			if( humanDropshipData->spawnPool->pathCount > 0 )
				creature->controller.aiPathFollowing.generalPath = humanDropshipData->spawnPool->pathList[rand()%humanDropshipData->spawnPool->pathCount]; // select random path
			// add to world
			cellMgr_addToWorld(mapChannel, creature);
		}
		if( humanDropshipData->spawnPool )
			spawnPool_decreaseQueuedCreatureCount(mapChannel, humanDropshipData->spawnPool, humanDropshipData->spawnCount);
		return true;
	}
	else if( humanDropshipData->phase == 2 )
	{
		humanDropshipData->phaseTimeleft -= timePassed;
		if( humanDropshipData->phaseTimeleft > 0 )
			return true;
		humanDropshipData->phaseTimeleft = 4000;
		humanDropshipData->phase = 3;
		dynObject->stateId = 190;
	}
	else if( humanDropshipData->phase == 3 )
	{
		humanDropshipData->phaseTimeleft -= timePassed;
		if( humanDropshipData->phaseTimeleft > 0 )
			return true;
		humanDropshipData->phaseTimeleft = 5000;
		humanDropshipData->phase = 4;
		// not real state update, just sub-phase to spawn creatures
		return true;
	}
	else if( humanDropshipData->phase == 4 )
	{
		// if spawnpool set, decrease counter
		if( humanDropshipData->spawnPool )
			spawnPool_decreaseQueueCount(mapChannel, humanDropshipData->spawnPool);
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

void humandropship_useObject(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	// not used
}


dynObjectType_t dynObjectType_humandropship = 
{
	humandropship_destroy, // destroy
	humandropship_appearForPlayers, // appearForPlayers
	humandropship_disappearForPlayers, // disappearForPlayer
	humandropship_periodicCallback, // periodicCallback
	humandropship_useObject, // useObject
};
#include "global.h"

#define OBJECTTYPE_FOOTLOCKER		1
#define OBJECTTYPE_UNDEFINED		2
#define OBJECTTYPE_BANE_DROPSHIP	3
#define OBJECTTYPE_CONTROL_POINT	4
#define OBJECTTYPE_LOGOS			5
#define OBJECTTYPE_AFS_DROPSHIP		6

#define ACTION_USEOBJECT	80


#include<math.h>
// Convert from Euler Angles
void _quaternion_fromEuler(float pitch, float yaw, float roll, float quatOut[4])
{
	// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
	// and multiply those together.
	// the calculation below does the same, just shorter

	float p = pitch * 1.57079f / 2.0f;
	float y = yaw * 1.57079f / 2.0f;
	float r = roll * 1.57079f / 2.0f;

	float sinp = sin(p);
	float siny = sin(y);
	float sinr = sin(r);
	float cosp = cos(p);
	float cosy = cos(y);
	float cosr = cos(r);

	quatOut[0] = sinr * cosp * cosy - cosr * sinp * siny; // x
	quatOut[1] = cosr * sinp * cosy + sinr * cosp * siny; // y
	quatOut[2] = cosr * cosp * siny - sinr * sinp * cosy; // z
	quatOut[3] = cosr * cosp * cosy + sinr * sinp * siny; // w
	//normalise();
	// Don't normalize if we don't have to
	float mag2 = quatOut[3] * quatOut[3] + quatOut[0] * quatOut[0] + quatOut[1] * quatOut[1] + quatOut[2] * quatOut[2];
	if (fabs(mag2 - 1.0f) > 0.00001f) {
		float mag = sqrt(mag2);
		quatOut[3] /= mag;
		quatOut[0] /= mag;
		quatOut[1] /= mag;
		quatOut[2] /= mag;
	}
}

dynObject_t *_dynamicObject_create(unsigned int classId, unsigned short objectType)
{
	dynObject_t *dynObject = (dynObject_t*)malloc(sizeof(dynObject_t));
	dynObject->entityId = entityMgr_getFreeEntityIdForObject();
	dynObject->objectType = objectType;
	dynObject->objectData = NULL;
	dynObject->entityClassId = classId;
	dynObject->x = 0.0f;
	dynObject->y = 0.0f;
	dynObject->z = 0.0f;
	dynObject->rotX = 0.0f;
	dynObject->rotY = 0.0f;
	dynObject->rotZ = 0.0f;
	entityMgr_registerEntity(dynObject->entityId, dynObject);
	return dynObject;
}

/* Destroys an object on client and serverside
 * Frees the memory and informs clients about removal
 */
void dynamicObject_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// TODO, check timers
	// remove from world
	cellMgr_removeFromWorld(mapChannel, dynObject);
	// free memory
	if( dynObject->objectData )
		free(dynObject->objectData);
	free(dynObject);
}

void dynamicObject_setPosition(dynObject_t *dynamicObject, float x, float y, float z)
{
	dynamicObject->x = x;
	dynamicObject->y = y;
	dynamicObject->z = z;
}

void dynamicObject_setRotation(dynObject_t *dynamicObject, float pitch, float yaw, float roll)
{
	dynamicObject->rotX = pitch;
	dynamicObject->rotY = yaw;
	dynamicObject->rotZ = roll;
}

void dynamicObject_setPeriodicUpdate(mapChannel_t *mapChannel, dynObject_t *dynamicObject, int periodInMS)
{
	// create work entry
	dynObject_workEntry_t *workEntry = (dynObject_workEntry_t*)malloc(sizeof(dynObject_workEntry_t));
	workEntry->object = dynamicObject;
	workEntry->period = periodInMS;
	workEntry->timeLeft = periodInMS;
	workEntry->entityId = dynamicObject->entityId;
	// register (TODO: check if a previous entry has already been registered)
	hashTable_set(&mapChannel->ht_updateObjectList, workEntry->entityId, workEntry);
	printf("ADD : %08X\n", workEntry);
}

dynObject_t *dynamicObject_createFootlocker(float x, float y, float z, float rotX, float rotY, float rotZ)
{
	dynObject_t *dynObject = _dynamicObject_create(21030, OBJECTTYPE_FOOTLOCKER);
	if( !dynObject )
		return NULL;
	dynamicObject_setPosition(dynObject, x, y, z);
	// set footlocker data
	// todo
	return dynObject;
}

dynObject_t *dynamicObject_createCustom(int classId, float x, float y, float z, float rotX, float rotY, float rotZ)
{
	dynObject_t *dynObject = _dynamicObject_create(classId, OBJECTTYPE_UNDEFINED);
	if( !dynObject )
		return NULL;
	dynamicObject_setPosition(dynObject, x, y, z);
	return dynObject;
}


void dynamicObject_createObjectOnClient(mapChannelClient_t *client, dynObject_t *dynObject)
{
	if( !dynObject )
		return;
	pyMarshalString_t pms;
	// create object entity
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, dynObject->entityId); // entityID
	pym_addInt(&pms, dynObject->entityClassId); // classID
	pym_addNoneStruct(&pms); // entityData (dunno)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	// set position
	pym_init(&pms);
	pym_tuple_begin(&pms);
	// position
	pym_tuple_begin(&pms);
	pym_addFloat(&pms, dynObject->x);	// x
	pym_addFloat(&pms, dynObject->y);	// y
	pym_addFloat(&pms, dynObject->z);	// z
	pym_tuple_end(&pms); 
	// rotation quaterninion

	float qOut[4];
	_quaternion_fromEuler(dynObject->rotX, dynObject->rotY, dynObject->rotZ, qOut);

	pym_tuple_begin(&pms);
	pym_addFloat(&pms, qOut[0]);
	pym_addFloat(&pms, qOut[1]);
	pym_addFloat(&pms, qOut[2]);
	pym_addFloat(&pms, qOut[3]);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 243, pym_getData(&pms), pym_getLen(&pms));	
	// additional info depending on type
	if( dynObject->objectType == OBJECTTYPE_BANE_DROPSHIP )
	{
		/*
			Animation states for bane dropship:
				188 --> enter
				189 --> spawn-effect
				190 --> leave
				97 --> "USE_STATE_NULL"
		*/
		// useable info
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 1);	// enabled
		pym_addInt(&pms, dynObject->stateId);	// curState
		pym_addInt(&pms, 0);	// nameOverrideId
		pym_addInt(&pms, 0);	// windupTime
		pym_addInt(&pms, 0);	// missionActivated
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 229, pym_getData(&pms), pym_getLen(&pms));		
	}
	if (dynObject->objectType == OBJECTTYPE_CONTROL_POINT)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 1);	// enabled
		pym_addInt(&pms, 179);	// curState
		pym_addNoneStruct(&pms); // nameOverrideId
		pym_addInt(&pms, 10000);	// windupTime
		pym_addInt(&pms, 0);	// missionActivated
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 229, pym_getData(&pms), pym_getLen(&pms));

		/* Struct('ControlPointStatus', 
		(Field('controlPointId', types.IntType, None, False),
		Field('ownerId', types.LongType, None, True),
		Field('stateId', types.IntType, None, False),
		Field('endTime', types.IntType, None, False))) */
/*
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addString(&pms, "ControlPointStatus");
		pym_tuple_begin(&pms);
		pym_addString(&pms, "controlPointId");
		pym_addInt(&pms, 1);
		pym_tuple_end(&pms);
		pym_tuple_begin(&pms);
		pym_addString(&pms, "ownerId");
		pym_addInt(&pms, 1);
		pym_tuple_end(&pms);
		pym_tuple_begin(&pms);
		pym_addString(&pms, "stateId");
		pym_addInt(&pms, 1);
		pym_tuple_end(&pms);
		pym_tuple_begin(&pms);
		pym_addString(&pms, "endTime");
		pym_addInt(&pms, 0);
		pym_tuple_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 814, pym_getData(&pms), pym_getLen(&pms));
		*/
		/*
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 6); // ownerTypeId FACTION_OWNED = 6
		pym_addBool(&pms, false); // ownerid true / false
		pym_addNoneStruct(&pms); // nothing
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 666, pym_getData(&pms), pym_getLen(&pms));
		*/
		/*
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 2);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 884, pym_getData(&pms), pym_getLen(&pms));
		*/
	}
	if (dynObject->objectType == OBJECTTYPE_LOGOS)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 1);	// enabled
		pym_addInt(&pms, 81);	// curState
		pym_addNoneStruct(&pms); // nameOverrideId
		pym_addInt(&pms, 10000);	// windupTime
		pym_addInt(&pms, 0);	// missionActivated
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 229, pym_getData(&pms), pym_getLen(&pms));
	}
}

bool dynamicObject_process_BaneDropship(mapChannel_t *mapChannel, dynObject_t *dynObject, int timePassed)
{
	dynObject_baneDropship_t *baneDropshipData = (dynObject_baneDropship_t*)dynObject->objectData;
	if( baneDropshipData->phase == 0 )
	{
		baneDropshipData->phaseTimeleft -= timePassed;
		if( baneDropshipData->phaseTimeleft > 0 )
			return true;
		baneDropshipData->phaseTimeleft = 9000;
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
		dynObject->stateId = 190;
		// spawn creatures ( for some odd reason there is a delay until they appear on the client?)
		for(int i=0; i<baneDropshipData->spawnCount;i++)
		{
			creature_t *creature = creature_createCreature(mapChannel, 
				                                           baneDropshipData->spawnTypeList[i], 
														   baneDropshipData->spawnPool,
														   baneDropshipData->spawnPool->faction);
			if( creature == NULL )
				continue;
			srand(GetTickCount());
			int srnd = rand() % 5;
			creature_setLocation(creature, dynObject->x+(float)srnd, dynObject->y, dynObject->z+(float)srnd, 0.0f, 0.0f);
			cellMgr_addToWorld(mapChannel, creature);
		}
		if( baneDropshipData->spawnPool )
		spawnPool_decreaseQueuedCreatureCount(mapChannel, baneDropshipData->spawnPool, baneDropshipData->spawnCount);
	}
	else if( baneDropshipData->phase == 2 )
	{
		baneDropshipData->phaseTimeleft -= timePassed;
		if( baneDropshipData->phaseTimeleft > 0 )
			return true;
		baneDropshipData->phaseTimeleft = 3800;
		baneDropshipData->phase = 3;
		// not real state update, just sub-phase to spawn creatures
		return true;
	}
	else if( baneDropshipData->phase == 3 )
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

// return false if the object work entry should be removed
bool dynamicObject_process(mapChannel_t *mapChannel, dynObject_t *dynObject, int timePassed)
{
	// ht_updateObjectList
	if( dynObject->objectType == OBJECTTYPE_BANE_DROPSHIP )
	{
		return dynamicObject_process_BaneDropship(mapChannel, dynObject, timePassed);
	}
	return true;
}

void dynamicObject_check(mapChannel_t *mapChannel, int timePassed)
{
	// remove queue
	int removeQueueCount = 0;
	dynObject_workEntry_t *removeQueue[256];
	// parse through all objects
	int count = hashTable_getCount(&mapChannel->ht_updateObjectList);
	dynObject_workEntry_t **objectWorkList = (dynObject_workEntry_t**)hashTable_getValueArray(&mapChannel->ht_updateObjectList);
	for(int i=0; i<count; i++)
	{
		dynObject_workEntry_t *dynObjectWorkEntry = objectWorkList[i];
		dynObjectWorkEntry->timeLeft -= timePassed;
		if( dynObjectWorkEntry->timeLeft <= 0 )
		{
			int objTimePassed = dynObjectWorkEntry->period - dynObjectWorkEntry->timeLeft;
			dynObjectWorkEntry->timeLeft += dynObjectWorkEntry->period;
			// trigger object
			bool remove = dynamicObject_process(mapChannel, dynObjectWorkEntry->object, objTimePassed);
			if( remove == false )
			{
				removeQueue[removeQueueCount] = dynObjectWorkEntry;
				removeQueueCount++;
				if( removeQueueCount >= 256 )
					break; // queue full, we need to force cancle updates and remove all entrys on queue
			}
		}
	}
	// remove queued entrys
	for(int i=0; i<removeQueueCount; i++)
	{
		dynObject_workEntry_t *dynObjectWorkEntry = removeQueue[i];
		printf("FREE: %08X (eid %d)\n", dynObjectWorkEntry, dynObjectWorkEntry->entityId);
		//hashTable_set(&mapChannel->ht_updateObjectList, dynObjectWorkEntry->entityId, NULL);
		bool test = hashTable_set(&mapChannel->ht_updateObjectList, dynObjectWorkEntry->entityId, NULL);
		if( test == false )
		{
			__debugbreak();
			hashTable_get(&mapChannel->ht_updateObjectList, dynObjectWorkEntry->entityId);
		}
		free(dynObjectWorkEntry);
	}
}
//---init all dynamic objects in mapchannel
void dynamicObject_init(mapChannel_t *mapChannel)
{
	hashTable_init(&mapChannel->ht_updateObjectList, 32);
	dynObject_t *footLocker = dynamicObject_createFootlocker(-231.800781f, 101.050781f, -69.894531f, 0.0f, 0.0f, 0.0f);
	cellMgr_addToWorld(mapChannel, footLocker);
	//dynamicObject_createHumanDropship(mapChannel, -231.800781f, 105.0f, -69.894531f);
}

void dynamicObject_developer_createFootlocker(mapChannel_t *mapChannel, float x, float y, float z)
{
	dynObject_t *footLocker = dynamicObject_createFootlocker(x, y, z, 0.0f, 0.0f, 0.0f);
	cellMgr_addToWorld(mapChannel, footLocker);
}

void dynamicObject_developer_createControlPoint(mapChannel_t *mapChannel, float x, float y, float z)
{
	dynObject_t *dynObject = _dynamicObject_create(3814, OBJECTTYPE_CONTROL_POINT); // 3814
	if( !dynObject )
		return;
	dynamicObject_setPosition(dynObject, x, y, z);
	// set footlocker data
	// todo
	cellMgr_addToWorld(mapChannel, dynObject);
}

void dynamicObject_developer_createCustom(mapChannel_t *mapChannel, int classId, float x, float y, float z)
{
	dynObject_t *customObj = dynamicObject_createCustom(classId, x, y, z, 0.0f, 0.0f, 0.0f);
	cellMgr_addToWorld(mapChannel, customObj);
}

void dynamicObject_createBaneDropship(mapChannel_t *mapChannel, float x, float y, float z, int spawnCount, creatureType_t **spawnTypeList, spawnPool_t *spawnPool)
{
	dynObject_t *dynObject = _dynamicObject_create(9268, OBJECTTYPE_BANE_DROPSHIP);// 9269
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
	for(int i=0; i<spawnCount; i++)
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
	dynamicObject_setPeriodicUpdate(mapChannel, dynObject, 250);
}

void dynamicObject_createBaneDropship(mapChannel_t *mapChannel, float x, float y, float z, int spawnCount, creatureType_t **spawnTypeList)
{
	dynamicObject_createBaneDropship(mapChannel, x, y, z, spawnCount, spawnTypeList, NULL);
}

void dynamicObject_footlocker_useObject(mapChannelClient_t *client, int actionId, int actionArgId, dynObject_t *footlocker)
{
	if( actionId == ACTION_USEOBJECT )
	{
		// use lockbox (230)
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->entityId); // actorId
		pym_addInt(&pms, 1); // curState
		pym_addInt(&pms, 500); // windupTimeMs
		pym_addNoneStruct(&pms); // args
		pym_addNoneStruct(&pms); // entityData (dunno)
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, footlocker->entityId, 230, pym_getData(&pms), pym_getLen(&pms));
		// todo: send inventory
		printf("TODO: footlocker Should send inventory");
	}
}

void dynamicObject_recv_RequestUseObject(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	printf("Request use object\n");
	// actionId
	// actionArgId
	// entityId
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int actionId = pym_unpackInt(&pums);
	int actionArgId = pym_unpackInt(&pums);
	unsigned long long entityId = pym_unpackLongLong(&pums);
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_OBJECT )
		return;
	dynObject_t *dynObject = (dynObject_t*)entityMgr_get(entityId);
	if( dynObject == NULL )
		return;
	pyMarshalString_t pms;
	switch( dynObject->objectType )
	{
	case OBJECTTYPE_FOOTLOCKER:
		dynamicObject_footlocker_useObject(client, actionId, actionArgId, dynObject);
		break;
	case OBJECTTYPE_CONTROL_POINT:
		/*
			(176) (USE_CPOINT_STATE_FACTION_A_OWNED)
			(177) (USE_CPOINT_STATE_FACTION_B_CLAIMING)
			(179) (USE_CPOINT_STATE_FACTION_B_OWNED)
			(180) (USE_CPOINT_STATE_FACTION_A_CLAIMING)
			(181) (USE_CPOINT_STATE_UNCLAIMED)
		*/
		//printf("ActionID: %i - ActionArgID: %i\n", actionId, actionArgId);
		// send interruptible use
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->entityId); // actorID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 691, pym_getData(&pms), pym_getLen(&pms));
		client->player->actionEntityId = dynObject->entityId;
		// change CP state - ForceState
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 174); // 180
		pym_addInt(&pms, 10000);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 454, pym_getData(&pms), pym_getLen(&pms));
		client->mapChannel->cp_trigger.cb = _dynamicObject_controlpoint_callback;
		client->mapChannel->cp_trigger.param = client;
		client->mapChannel->cp_trigger.timeLeft = 9500;
		client->mapChannel->cp_trigger.period = 0;
		break;
	case OBJECTTYPE_LOGOS:
		printf("using logos\n");
		// Recv_Use(self, actorId, curStateId, windupTimeMs, *args):
		// send interruptible use
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->entityId); // actorID
		pym_addInt(&pms, 81);
		pym_addInt(&pms, 10000);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 230, pym_getData(&pms), pym_getLen(&pms));
		client->player->actionEntityId = dynObject->entityId;
		break;
	}
}

bool _dynamicObject_controlpoint_callback(mapChannel_t *mapChannel, void *param, int timePassed)
{
	printf("callback control point activated!\n");
	mapChannelClient_t* client = (mapChannelClient_t*)param;
	if (client->player->actionEntityId == NULL)
		return true;
	printf("action entity is not null\n");
	// change CP state - ForceState
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 176); // 180
	pym_addInt(&pms, 10000);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 454, pym_getData(&pms), pym_getLen(&pms));
	// set usable
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, false);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 203, pym_getData(&pms), pym_getLen(&pms));
	client->player->actionEntityId = NULL;
	return true;
}

void dynamicObject_recv_RequestActionInterrupt(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	printf("Request action interrupt\n");
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int actionId = pym_unpackInt(&pums);
	int actionArgId = pym_unpackInt(&pums);
	if (actionId == 80 && actionArgId == 7) //ControlPoint data?
	{
		printf("Interrupting control point claiming\n");
		// Use Interrupted
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->entityId); // actorID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 609, pym_getData(&pms), pym_getLen(&pms));
		// change CP state - ForceState
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 179);
		pym_addInt(&pms, 10000);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 454, pym_getData(&pms), pym_getLen(&pms));
		client->player->actionEntityId = NULL;
		client->mapChannel->cp_trigger.cb = NULL;
	}
}

// 1:n
void dynamicObject_cellIntroduceObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, int playerCount)
{
	for(int i=0; i<playerCount; i++)
	{
		dynamicObject_createObjectOnClient(playerList[i], dynObj);
	}
}

// n:1
void dynamicObject_cellIntroduceObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, int objectCount)
{
	for(int i=0; i<objectCount; i++)
	{
		dynamicObject_createObjectOnClient(client, objectList[i]);
	}
}


void dynamicObject_cellDiscardObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, int playerCount)
{
	if( !dynObj )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, dynObj->entityId); // entityID
	pym_tuple_end(&pms);
	for(int i=0; i<playerCount; i++)
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
}

void dynamicObject_cellDiscardObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, int objectCount)
{
	if( !client )
		return;
	pyMarshalString_t pms;
	for(int i=0; i<objectCount; i++)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, objectList[i]->entityId); // entityID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
	}
}


// TEEEEEEST

void dynamicObject_createLogosObject(mapChannel_t *mapChannel, float x, float y, float z)
{
	dynObject_t *dynObject = _dynamicObject_create(7302, OBJECTTYPE_LOGOS); // 3814
	if( !dynObject )
		return;
	dynamicObject_setPosition(dynObject, x, y, z);
	cellMgr_addToWorld(mapChannel, dynObject);
}

void dynamicObject_createHumanDropship(mapChannel_t *mapChannel, float x, float y, float z)
{
	dynObject_t *dynObject = _dynamicObject_create(9269, OBJECTTYPE_BANE_DROPSHIP); // 3814
	if( !dynObject )
		return;
	printf("Human Dropship Entity ID: %u\n", dynObject->entityId);
	dynamicObject_setPosition(dynObject, x, y, z);
	//dynamicObject_setRotation(dynObject, 0.0f, 0.0f, 0.0f);
	dynObject->stateId = 1;
	cellMgr_addToWorld(mapChannel, dynObject);
}

void dynamicObject_forceState(clientGamemain_t* cgm, unsigned int entityId, int state)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, state);
	pym_addInt(&pms, 10000);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityId, 454, pym_getData(&pms), pym_getLen(&pms));
}
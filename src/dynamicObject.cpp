#include "global.h"


#define ACTION_USEOBJECT	80

extern sint32 gridL1;
extern sint32 gridL2;
extern sint32 gridCount;
extern sint32** entityPosGrid;
extern sint32** forcefieldMap;

extern dynObjectType_t dynObjectType_humandropship;
extern dynObjectType_t dynObjectType_banedropship;

#include<math.h>
// Convert from Euler Angles
void _quaternion_fromEuler(float pitch, float yaw, float roll, float quatOut[4])
{
	// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
	// and multiply those together.
	// the calculation below does the same, just sint16er

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

dynObject_t *_dynamicObject_create(uint32 classId, dynObjectType_t* objectType)
{
	if( objectType == NULL )
	{
		printf("Error: Cannot create object with no type\n", objectType);
		return NULL;
	}
	dynObject_t *dynObject = (dynObject_t*)malloc(sizeof(dynObject_t));
	dynObject->entityId = entityMgr_getFreeEntityIdForObject();
	dynObject->type = objectType;	

	//if( objectType == OBJECTTYPE_FOOTLOCKER )
	//	__debugbreak(); // todo
	//else if( objectType == OBJECTTYPE_BANE_DROPSHIP )
	//	dynObject->type = &dynObjectType_banedropship;
	//else
	//{
	//	dynObject->type = &dynObjectType_defaultObject;
	//	printf("Error: No dynamic object type found for typeID %d\n", objectType);
	//}
	//

	//dynObject->objectType = objectType;


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
	entityMgr_unregisterEntity(dynObject->entityId);
	cellMgr_removeFromWorld(mapChannel, dynObject);
	// destroy callback
	if( dynObject->type->destroy )
		dynObject->type->destroy(mapChannel, dynObject);
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

void dynamicObject_setPeriodicUpdate(mapChannel_t *mapChannel, dynObject_t *dynamicObject, uint8 timerID, sint32 periodInMS)
{
	// create work entry
	dynObject_workEntry_t *workEntry = (dynObject_workEntry_t*)malloc(sizeof(dynObject_workEntry_t));
	workEntry->object = dynamicObject;
	workEntry->period = periodInMS;
	workEntry->timeLeft = periodInMS;
	workEntry->entityId = dynamicObject->entityId;
	workEntry->timerID = timerID;
	// register
	mapChannel->updateObjectList.push_back(workEntry);
}

void dynamicObject_stopPeriodicUpdate(mapChannel_t *mapChannel, dynObject_t *dynamicObject, uint8 timerID)
{
	// use iterator to find entry and delete it
	if( mapChannel->updateObjectList.empty() )
		return;
	std::vector<dynObject_workEntry_t*>::iterator itr = mapChannel->updateObjectList.begin();
	while (itr != mapChannel->updateObjectList.end())
	{
		dynObject_workEntry_t *workEntry = *itr;
		if( workEntry->object == dynamicObject && workEntry->timerID == timerID )
		{
			mapChannel->updateObjectList.erase(itr);
			free(workEntry);
			break;
		}
		itr++;
	}
}

//dynObject_t *dynamicObject_createFootlocker(float x, float y, float z, float rotX, float rotY, float rotZ)
//{
//	return NULL;
//	//dynObject_t *dynObject = _dynamicObject_create(21030, OBJECTTYPE_FOOTLOCKER);
//	//if( !dynObject )
//	//	return NULL;
//	//dynamicObject_setPosition(dynObject, x, y, z);
//	//// set footlocker data
//	//// todo
//	//return dynObject;
//}
//
//dynObject_t *dynamicObject_createCustom(sint32 classId, float x, float y, float z, float rotX, float rotY, float rotZ)
//{
//	return NULL;
//	//dynObject_t *dynObject = _dynamicObject_create(classId, OBJECTTYPE_UNDEFINED);
//	//if( !dynObject )
//	//	return NULL;
//	//dynamicObject_setPosition(dynObject, x, y, z);
//	//return dynObject;
//}


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
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, CreatePhysicalEntity, pym_getData(&pms), pym_getLen(&pms));
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
//	if (dynObject->objectType == OBJECTTYPE_AFS_TURRET)
//	{
//	   
//		
//
//		pym_init(&pms);
//		pym_tuple_begin(&pms); // Packet Start
//		pym_addInt(&pms, 242); // Action ID // 
//		pym_addInt(&pms, 1); // Arg ID // 
//		pym_list_begin(&pms); // Hits Start
//		pym_addLong(&pms, dynObject->entityId); // Each hit creature
//		pym_list_end(&pms); // Hits End
//		pym_list_begin(&pms); // Misses Start
//		pym_list_end(&pms); // Misses End
//		pym_list_begin(&pms); // Misses Data Start
//		pym_list_end(&pms); // Misses Data End
//		pym_list_begin(&pms); // Hits Data Start
//		pym_tuple_begin(&pms); // Each Hit tuple start
//		pym_addInt(&pms, dynObject->entityId); // Creature entity ID
//		pym_tuple_begin(&pms); // rawInfo start
//		pym_addInt(&pms, 1); //self.damageType = normal
//		pym_addInt(&pms, 0); //self.reflected = 0
//		pym_addInt(&pms, 0); //self.filtered = 0
//		pym_addInt(&pms, 0); //self.absorbed = 0
//		pym_addInt(&pms, 0); //self.resisted = 0
//		pym_addInt(&pms, 1); //self.finalAmt = missile->damageA
//		pym_addInt(&pms, 0); //self.isCrit = 0
//		pym_addInt(&pms, 0); //self.deathBlow = 0
//		pym_addInt(&pms, 0); //self.coverModifier = 0
//		pym_addInt(&pms, 0); //self.wasImmune = 0
//		  //targetEffectIds // 131
//		pym_list_begin(&pms);
//		pym_list_end(&pms);
//		//sourceEffectIds
//		pym_list_begin(&pms);
//		pym_list_end(&pms);
//		pym_tuple_end(&pms); // rawInfo end
//		//pym_addNoneStruct(&pms); // OnHitData
//		//pym_addInt(&pms, 12);
//
//		pym_tuple_end(&pms); // Each Hit tuple start
//		pym_list_end(&pms); // Hits Data End
//		pym_tuple_end(&pms); // Packet End
//		// 311
//		netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, dynObject, dynObject->entityId, 125, pym_getData(&pms), pym_getLen(&pms));
//		
//	}
//	if (dynObject->objectType == OBJECTTYPE_TORIODCANNON)
//	{
//		pyMarshalString_t pms;
//		pym_init(&pms);
//		pym_tuple_begin(&pms);
//		pym_addInt(&pms, 56);
//		pym_addInt(&pms, 0);
//		pym_tuple_end(&pms);
//		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, ForceState, pym_getData(&pms), pym_getLen(&pms));
//	}
//	if (dynObject->objectType == OBJECTTYPE_BASEWORMHOLE)
//	{
//		pyMarshalString_t pms;
//		pym_init(&pms);
//		pym_tuple_begin(&pms);
//		pym_addInt(&pms, 56);
//		pym_addInt(&pms, 0);
//		pym_tuple_end(&pms);
//		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, ForceState, pym_getData(&pms), pym_getLen(&pms));
//	}
//	if (dynObject->objectType == OBJECTTYPE_TELEPORTER)
//	{
//		pym_init(&pms);
//		pym_tuple_begin(&pms);
//		pym_addInt(&pms, 1);	// enabled
//		pym_addInt(&pms, 80);	// curState
//		pym_addNoneStruct(&pms); // nameOverrideId
//		pym_addInt(&pms, 1000);	// windupTime
//		pym_addInt(&pms, 0);	// missionActivated
//		pym_tuple_end(&pms);
//		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 229, pym_getData(&pms), pym_getLen(&pms));
//	}

//	if (dynObject->objectType == OBJECTTYPE_DOOR)
//	{
//	
//		pyMarshalString_t pms;
//		pym_init(&pms);
//		pym_tuple_begin(&pms);
//		pym_addInt(&pms, 91);
//		pym_addInt(&pms, 25000);
//		pym_tuple_end(&pms);
//		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 454, pym_getData(&pms), pym_getLen(&pms));
//	}
//	if (dynObject->objectType == OBJECTTYPE_FORCEFIELD)
//	{		
//		pyMarshalString_t pms;
//		pym_init(&pms);
//		pym_tuple_begin(&pms);
//		pym_addInt(&pms, 191);
//		pym_addInt(&pms, 10000);
//		pym_tuple_end(&pms);
//		netMgr_pythonAddMethodCallRaw(client->mapChannel, dynObject->entityId, 454, pym_getData(&pms), pym_getLen(&pms));
//								
//		pym_init(&pms);
//		pym_tuple_begin(&pms);
//		pym_addInt(&pms, client->player->actor->entityId);
//		pym_addInt(&pms, 0);
//		pym_tuple_end(&pms);
//		netMgr_pythonAddMethodCallRaw(client->mapChannel, dynObject->entityId, 624, pym_getData(&pms), pym_getLen(&pms));
//
//	}

//	if (dynObject->objectType == OBJECTTYPE_LOGOS)
//	{
//		pym_init(&pms);
//		pym_tuple_begin(&pms);
//		pym_addInt(&pms, 1);	// enabled
//		pym_addInt(&pms, 81);	// curState
//		pym_addNoneStruct(&pms); // nameOverrideId
//		pym_addInt(&pms, 10000);	// windupTime
//		pym_addInt(&pms, 0);	// missionActivated
//		pym_tuple_end(&pms);
//		netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 229, pym_getData(&pms), pym_getLen(&pms));
//	}
}


// return false if the object timer entry should be removed
bool dynamicObject_process(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	return dynObject->type->periodicCallback(mapChannel, dynObject, timerID, timePassed);
}

void dynamicObject_check(mapChannel_t *mapChannel, sint32 timePassed)
{
	// parse through all objects
	std::vector<dynObject_workEntry_t*>::iterator dynObjectWorkEntryItr = mapChannel->updateObjectList.begin();
	while( dynObjectWorkEntryItr != mapChannel->updateObjectList.end() )
	{
		dynObject_workEntry_t *dynObjectWorkEntry = *dynObjectWorkEntryItr;
		dynObjectWorkEntry->timeLeft -= timePassed;
		if( dynObjectWorkEntry->timeLeft <= 0 )
		{
			sint32 objTimePassed = dynObjectWorkEntry->period - dynObjectWorkEntry->timeLeft;
			dynObjectWorkEntry->timeLeft += dynObjectWorkEntry->period;
			// trigger object
			bool remove = dynamicObject_process(mapChannel, dynObjectWorkEntry->object, dynObjectWorkEntry->timerID, objTimePassed);
			if( remove == false )
			{
				dynObjectWorkEntryItr = mapChannel->updateObjectList.erase(dynObjectWorkEntryItr);
				continue;
			}
		}
		dynObjectWorkEntryItr++;
	}
}

//---init all dynamic objects in mapchannel
void dynamicObject_init(mapChannel_t *mapChannel)
{
	//dynObject_t *footLocker = dynamicObject_createFootlocker(-231.800781f, 101.050781f, -69.894531f, 0.0f, 0.0f, 0.0f);
	//cellMgr_addToWorld(mapChannel, footLocker);
	//dynamicObject_createHumanDropship(mapChannel, -231.800781f, 105.0f, -69.894531f);
}

//
//void dynamicObject_developer_createFootlocker(mapChannel_t *mapChannel, float x, float y, float z)
//{
//	dynObject_t *footLocker = dynamicObject_createFootlocker(x, y, z, 0.0f, 0.0f, 0.0f);
//	cellMgr_addToWorld(mapChannel, footLocker);
//}
//
//
//void dynamicObject_developer_createCustom(mapChannel_t *mapChannel, sint32 classId, float x, float y, float z)
//{
//	dynObject_t *customObj = dynamicObject_createCustom(classId, x, y, z, 0.0f, 0.0f, 0.0f);
//	cellMgr_addToWorld(mapChannel, customObj);
//}


void dynamicObject_teleporter_useObject(mapChannelClient_t *client, sint32 actionId, sint32 actionArgId, dynObject_t *teleporter)
{
	if( actionId == ACTION_USEOBJECT )
	{

		struct tloc
		{
			sint32 x;
			sint32 y;
			sint32 z;
			float rotation; //not necessary, could add
			sint32 mapContextId;
		};

		tloc telepos = {0};
		client->player->actor->posX = 200.1f; 
		client->player->actor->posY = 200.1f;
		client->player->actor->posZ = 200.1f;
		cellMgr_addToWorld(client); // will introduce the player to all clients, including the current owner
		// todo: send inventory
		//printf("TODO: teleporter");
	}
}


void dynamicObject_footlocker_useObject(mapChannelClient_t *client, sint32 actionId, sint32 actionArgId, dynObject_t *footlocker)
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

void dynamicObject_recv_RequestUseObject(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	printf("Request use object\n");
	// actionId
	// actionArgId
	// entityId
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 actionId = pym_unpackInt(&pums);
	sint32 actionArgId = pym_unpackInt(&pums);
	unsigned long long entityId = pym_unpackLongLong(&pums);
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_OBJECT )
		return;
	dynObject_t *dynObject = (dynObject_t*)entityMgr_get(entityId);
	if( dynObject == NULL )
		return;
	// todo: Check if action already in progress and if yes, send error message
	client->usedObject.actionId = actionId;
	client->usedObject.actionArg = actionArgId;
	client->usedObject.entityId = entityId;
	if( dynObject->type->useObject )
		dynObject->type->useObject(client->mapChannel, dynObject, client, actionId, actionArgId);
	//pyMarshalString_t pms;
	

	//switch( dynObject->objectType )
	//{
	//case OBJECTTYPE_TELEPORTER:
	//	printf("use teleporter\n");
	//	 dynamicObject_teleporter_useObject(client, actionId, actionArgId, dynObject);
	//	break;
	//
	//case OBJECTTYPE_FOOTLOCKER:
	//	dynamicObject_footlocker_useObject(client, actionId, actionArgId, dynObject);
	//	break;
	//case OBJECTTYPE_CONTROL_POINT:
	//	/*
	//		(176) (USE_CPOsint32_STATE_FACTION_A_OWNED)
	//		(177) (USE_CPOsint32_STATE_FACTION_B_CLAIMING)
	//		(179) (USE_CPOsint32_STATE_FACTION_B_OWNED)
	//		(180) (USE_CPOsint32_STATE_FACTION_A_CLAIMING)
	//		(181) (USE_CPOsint32_STATE_UNCLAIMED)
	//	*/
	//	//printf("ActionID: %i - ActionArgID: %i\n", actionId, actionArgId);
	//	// send interruptible use
	//	pym_init(&pms);
	//	pym_tuple_begin(&pms);
	//	pym_addInt(&pms, client->player->actor->entityId); // actorID
	//	pym_tuple_end(&pms);
	//	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 691, pym_getData(&pms), pym_getLen(&pms));
	//	client->player->actionEntityId = dynObject->entityId;
	//	// change CP state - ForceState
	//	pym_init(&pms);
	//	pym_tuple_begin(&pms);
	//	pym_addInt(&pms, 174); // 180
	//	pym_addInt(&pms, 10000);
	//	pym_tuple_end(&pms);
	//	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 454, pym_getData(&pms), pym_getLen(&pms));
	//	client->mapChannel->cp_trigger.cb = _dynamicObject_controlpoint_callback;
	//	client->mapChannel->cp_trigger.param = client;
	//	client->mapChannel->cp_trigger.timeLeft = 9500;
	//	client->mapChannel->cp_trigger.period = 0;
	//	break;
	//case OBJECTTYPE_LOGOS:
	//	printf("using logos\n");
	//	// Recv_Use(self, actorId, curStateId, windupTimeMs, *args):
	//	// send interruptible use
	//	pym_init(&pms);
	//	pym_tuple_begin(&pms);
	//	pym_addInt(&pms, client->player->actor->entityId); // actorID
	//	pym_addInt(&pms, 81);
	//	pym_addInt(&pms, 10000);
	//	pym_tuple_end(&pms);
	//	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 230, pym_getData(&pms), pym_getLen(&pms));
	//	client->player->actionEntityId = dynObject->entityId;
	//	break;
	//}
}

//bool _dynamicObject_controlpoint_callback(mapChannel_t *mapChannel, void *param, sint32 timePassed)
//{
//	printf("callback control point activated!\n");
//	mapChannelClient_t* client = (mapChannelClient_t*)param;
//	if (client->player->actionEntityId == NULL)
//		return true;
//	printf("action entity is not null\n");
//	// change CP state - ForceState
//	pyMarshalString_t pms;
//	pym_init(&pms);
//	pym_tuple_begin(&pms);
//	pym_addInt(&pms, 176); // 180
//	pym_addInt(&pms, 10000);
//	pym_tuple_end(&pms);
//	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 454, pym_getData(&pms), pym_getLen(&pms));
//	// set usable
//	pym_init(&pms);
//	pym_tuple_begin(&pms);
//	pym_addBool(&pms, false);
//	pym_tuple_end(&pms);
//	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 203, pym_getData(&pms), pym_getLen(&pms));
//	client->player->actionEntityId = NULL;
//	// complete action (perform recovery)
//	pym_init(&pms);
//	pym_tuple_begin(&pms);  		// Packet Start
//	pym_addInt(&pms, 80);			// Action ID
//	pym_addInt(&pms, 7);			// Arg ID
//	pym_tuple_end(&pms); 			// Packet End
//	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, PerformRecovery, pym_getData(&pms), pym_getLen(&pms));
//	return true;
//}

void dynamicObject_recv_RequestActionInterrupt(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	printf("Request action interrupt\n");
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 actionId = pym_unpackInt(&pums);
	sint32 actionArgId = pym_unpackInt(&pums);
	if( client->usedObject.actionId != actionId || client->usedObject.actionArg != actionArgId || entityMgr_getEntityType(client->usedObject.entityId) != ENTITYTYPE_OBJECT )
	{
		// not the same action as the one the player currently executes
		printf("RequestActionInterrupt: Wrong action or object disappeared\n");
		return;
	}
	
	dynObject_t* dynObject = (dynObject_t*)entityMgr_get(client->usedObject.entityId);
	if( dynObject )
	{
		if( dynObject->type->interruptUse )
			dynObject->type->interruptUse(client->mapChannel, dynObject, client, actionId, actionArgId);
	}
	
	// complete action
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);  				// Packet Start
	pym_addInt(&pms, actionId);				// Action ID // 1 Weapon attack
	pym_addInt(&pms, actionArgId);			// Arg ID // 133 pistol physical not crouched
	pym_tuple_end(&pms); 							// Packet End
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, PerformRecovery, pym_getData(&pms), pym_getLen(&pms));
	// reset object use
	client->usedObject.actionId = 0;
	client->usedObject.actionArg = 0;
	client->usedObject.entityId = 0;
}

// 1:n
void dynamicObject_cellIntroduceObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, sint32 playerCount)
{
	for(sint32 i=0; i<playerCount; i++)
	{
		dynamicObject_createObjectOnClient(playerList[i], dynObj);
		if( dynObj->type->appearForPlayers )
			dynObj->type->appearForPlayers(mapChannel, dynObj, playerList, playerCount);
	}
}

// n:1
void dynamicObject_cellIntroduceObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, sint32 objectCount)
{
	mapChannelClient_t* tempPlayerList[1];
	tempPlayerList[0] = client;
	for(sint32 i=0; i<objectCount; i++)
	{
		dynamicObject_createObjectOnClient(client, objectList[i]);
		if( objectList[i]->type->appearForPlayers )
			objectList[i]->type->appearForPlayers(mapChannel, objectList[i], tempPlayerList, 1);
	}
}


void dynamicObject_cellDiscardObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, sint32 playerCount)
{
	if( !dynObj )
		return;
	if( dynObj->type->disappearForPlayers )
		dynObj->type->disappearForPlayers(mapChannel, dynObj, playerList, playerCount);
	// send entity destroy
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, dynObj->entityId); // entityID
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
}

void dynamicObject_cellDiscardObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, sint32 objectCount)
{
	if( !client )
		return;
	pyMarshalString_t pms;
	mapChannelClient_t* tempPlayerList[1];
	tempPlayerList[0] = client;
	for(sint32 i=0; i<objectCount; i++)
	{
		if( objectList[i]->type->disappearForPlayers )
			objectList[i]->type->disappearForPlayers(mapChannel, objectList[i], tempPlayerList, 1);
		// send entity destroy
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
	//dynObject_t *dynObject = _dynamicObject_create(7302, OBJECTTYPE_LOGOS); // 3814
	//if( !dynObject )
	//	return;
	//dynamicObject_setPosition(dynObject, x, y, z);
	//cellMgr_addToWorld(mapChannel, dynObject);
}

void dynamicObject_forceState(clientGamemain_t* cgm, uint32 entityId, sint32 state)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, state);
	pym_addInt(&pms, 10000);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityId, 454, pym_getData(&pms), pym_getLen(&pms));
}
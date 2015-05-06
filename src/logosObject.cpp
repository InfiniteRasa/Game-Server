#include"global.h"

extern dynObjectType_t dynObjectType_logos;

typedef struct  
{
	uint32 logosClassId; // logos entity classId
	uint8 state;
	bool alreadyInUse;
	sint64 userEntityId;
	//std::vector<logosTriggerCheck_t> triggeredPlayers;
	//uint32 updateCounter; // used to detect if players leave the logos
}logos_t;

// logos animations from the specialFx table:
// 81 --> 81


dynObject_t* logos_create(mapChannel_t *mapChannel, float x, float y, float z, sint32 logosClassId)
{
	dynObject_t *dynObject = _dynamicObject_create(logosClassId, &dynObjectType_logos);
	if( !dynObject )
		return NULL;
	dynObject->stateId = 0;
	dynamicObject_setPosition(dynObject, x, y, z);
	// setup logos specific data
	logos_t* objData = (logos_t*)malloc(sizeof(logos_t));
	memset(objData, 0x00, sizeof(logos_t));
	new(objData) logos_t();
	objData->logosClassId = logosClassId;
	dynObject->objectData = objData;
	// set rotation
	float randomRotY = 0.0f;
	float randomRotX = 0.0f;
	float randomRotZ = 0.0f;
	dynamicObject_setRotation(dynObject, randomRotY, randomRotX, randomRotZ);
	cellMgr_addToWorld(mapChannel, dynObject);
	// return object
	return dynObject;
}

void logos_initForMapChannel(mapChannel_t *mapChannel)
{
	if( mapChannel->mapInfo->contextId == 1220 )
	{
		// concordia wilderness
		logos_create(mapChannel, 493.1054f, 288.2813f+1.0f, 319.7031f, 7302);
	}
}

void logos_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// more or less unused, logos never go away?
}

void logos_appearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, USE_ID_STATE_ACTIVE);
	pym_addInt(&pms, 10000); // windupTime should not be zero to avoid freezing animations?
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(playerList, playerCount, dynObject->entityId, METHODID_FORCESTATE, pym_getData(&pms), pym_getLen(&pms));
}

void logos_disappearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// called before the object is removed from player sight
}


void logos_useObject(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	pyMarshalString_t pms;
	printf("logos use callback\n");
	// called whenever any client sends a UseRequest
	logos_t* objData = (logos_t*)dynObject->objectData;
	if( objData->alreadyInUse )
	{
		// tell client that use action failed
		// todo
		return;
	}
	// 1) send force state to make sure every client has the same state
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, USE_ID_STATE_ACTIVE);
	pym_addInt(&pms, 10000);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 454, pym_getData(&pms), pym_getLen(&pms));
	// 2) send use
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->entityId); // actorID
	//pym_addInt(&pms, (!objData->belongsToBane)?179:176); // next state id (animation transition)
	pym_addInt(&pms, USE_ID_STATE_ACTIVE); // next state id (animation transition) 
	pym_addInt(&pms, 10000); // windupTime
	// possible additional args
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, METHODID_USE, pym_getData(&pms), pym_getLen(&pms));
	//// change CP state - ForceState
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, 174); // 180
	//pym_addInt(&pms, 10000);
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 454, pym_getData(&pms), pym_getLen(&pms));
	// update object
	objData->alreadyInUse = true;
	objData->userEntityId = client->player->actor->entityId;
	dynamicObject_setPeriodicUpdate(mapChannel, dynObject, 1, 10000);
}

void logos_interruptUse(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	logos_t* objData = (logos_t*)dynObject->objectData;
	if( objData->alreadyInUse == false )
		return; // not in use, wat?
	// mark as not used
	objData->alreadyInUse = false;
	objData->userEntityId = 0;
	// force state
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, USE_ID_STATE_ACTIVE);
	pym_addInt(&pms, 10000);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 454, pym_getData(&pms), pym_getLen(&pms));
}

bool logos_periodicCallback(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	pyMarshalString_t pms;
	if( timerID == 1 )
	{
		// timerID 1 indicates that the object was successfully used
		// inform clients and update state
		logos_t* objData = (logos_t*)dynObject->objectData;
		if( objData->alreadyInUse )
		{
			objData->alreadyInUse = false;
			// send performRecovery (complete action)
			pym_init(&pms);
			pym_tuple_begin(&pms);  		// Packet Start
			pym_addInt(&pms, 80);			// Action ID
			pym_addInt(&pms, 6);			// Arg ID
			pym_tuple_end(&pms); 			// Packet End
			netMgr_pythonAddMethodCallRaw(mapChannel, objData->userEntityId, METHODID_PERFORMRECOVERY, pym_getData(&pms), pym_getLen(&pms));
			// update logos point state
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addNoneStruct(&pms);	// actorId
			pym_addInt(&pms, USE_ID_STATE_ACTIVE);	// curStateId
			pym_addInt(&pms, 10000);	// windupTime (the time required to convert the control point)
			pym_tuple_begin(&pms);	// args
			pym_tuple_end(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, dynObject, dynObject->entityId, METHODID_USE, pym_getData(&pms), pym_getLen(&pms));
			// remove user
			objData->userEntityId = 0;
		}
		return false; // immediately unregister timer (one-shot)
	}

	return true;
}

dynObjectType_t dynObjectType_logos = 
{
	logos_destroy, // destroy
	logos_appearForPlayers, // appearForPlayers
	logos_disappearForPlayers, // disappearForPlayer
	logos_periodicCallback, // periodicCallback
	logos_useObject, // useObject
	logos_interruptUse
};
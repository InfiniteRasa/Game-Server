#include"global.h"

extern dynObjectType_t dynObjectType_controlpoint;

typedef struct  
{
	bool alreadyInUse;
	long long userEntityId; // entityId of actor using this object
	bool belongsToBane; 
}controlpoint_t;

// control points have four states:
// 97	-->	off
// 176	-->	owned by AFS
// 179	-->	owned by Bane
// 181	-->	off

// states we know from the db:
//augmentation	stateId		name
//57			176			USE_CPOINT_STATE_FACTION_A_OWNED
//57			179			USE_CPOINT_STATE_FACTION_B_OWNED
//57			181			USE_CPOINT_STATE_UNCLAIMED


// Known transitions from the specialFx table (there may be more)
//state1	state2
//176		97
//176		176
//179		97
//179		179
//181		181



// todo1: Should also send Recv_ControlPointStatus to all mapChannel clients
// todo2: Send Recv_SetUsable when in control of bane to disallow humans to convert it again (assuming this is the way they original server did it)
// todo3: Use Recv_SetOwnerId?

dynObject_t* controlpoint_create(mapChannel_t *mapChannel, float x, float y, float z, float orientation)
{
	dynObject_t *dynObject = _dynamicObject_create(3814, &dynObjectType_controlpoint);
	if( !dynObject )
		return NULL;
	dynObject->stateId = 0;
	dynamicObject_setPosition(dynObject, x, y, z);
	// allocate and init object type specific data
	controlpoint_t* data = (controlpoint_t*)malloc(sizeof(controlpoint_t));
	memset(data, 0, sizeof(controlpoint_t));
	dynObject->objectData = data;
	// randomize rotation
	float randomRotY = orientation;
	float randomRotX = 0.0f;
	float randomRotZ = 0.0f;
	dynamicObject_setRotation(dynObject, randomRotY, randomRotX, randomRotZ);
	cellMgr_addToWorld(mapChannel, dynObject);
	return dynObject;
}

void controlpoint_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// called shortly before free()
	free(dynObject->objectData);
}

void controlpoint_appearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	controlpoint_t* objData = (controlpoint_t*)dynObject->objectData;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1);	// enabled
	pym_addInt(&pms, objData->belongsToBane?179:176);
	pym_addNoneStruct(&pms); // nameOverrideId
	pym_addInt(&pms, 10000);	// windupTime (the time required to convert the control point)
	pym_addInt(&pms, 0);	// missionActivated
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(playerList, playerCount, dynObject->entityId, 229, pym_getData(&pms), pym_getLen(&pms)); // Recv_UseableInfo
	printf("appear to player: Belongs to bane %s\n", objData->belongsToBane?"YES":"NO");
	/* Struct('ControlPointStatus', 
	(Field('controlPointId', types.intType, None, False),
	Field('ownerId', types.LongType, None, True),
	Field('stateId', types.intType, None, False),
	Field('endTime', types.intType, None, False))) */
	
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

void controlpoint_disappearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// called before the object is removed from player sight
}

bool controlpoint_periodicCallback(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	pyMarshalString_t pms;
	if( timerID == 1 )
	{
		// timerID 1 indicates that the object was successfully used
		// inform clients and update state
		controlpoint_t* objData = (controlpoint_t*)dynObject->objectData;
		if( objData->alreadyInUse )
		{
			objData->alreadyInUse = false;
			objData->belongsToBane = !objData->belongsToBane;
			// send performRecovery (complete action)
			pym_init(&pms);
			pym_tuple_begin(&pms);  		// Packet Start
			pym_addInt(&pms, 80);			// Action ID
			pym_addInt(&pms, 7);			// Arg ID
			pym_tuple_end(&pms); 			// Packet End
			netMgr_pythonAddMethodCallRaw(mapChannel, objData->userEntityId, PerformRecovery, pym_getData(&pms), pym_getLen(&pms));
			// update control point state
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addNoneStruct(&pms);	// actorId
			pym_addInt(&pms, objData->belongsToBane?179:176);	// curStateId
			pym_addInt(&pms, 10000);	// windupTime (the time required to convert the control point)
			pym_tuple_begin(&pms);	// args
			pym_tuple_end(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, dynObject, dynObject->entityId, Use, pym_getData(&pms), pym_getLen(&pms));

		}
		return false; // immediately unregister timer (one-shot)
	}

	return true;
}

void controlpoint_useObject(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	pyMarshalString_t pms;
	printf("controlPoint use callback\n");
	// called whenever any client sends a UseRequest
	controlpoint_t* objData = (controlpoint_t*)dynObject->objectData;
	if( objData->alreadyInUse )
	{
		// tell client that use action failed
		// todo
		return;
	}
	// inform clients
	// send interruptible use
	//pyMarshalString_t pms;
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, client->player->actor->entityId); // actorID
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, 691, pym_getData(&pms), pym_getLen(&pms));
	//client->player->actionEntityId = dynObject->entityId; // superfluous? 
	// 1) send force state to make sure every client has the same state
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, objData->belongsToBane?179:176);
	pym_addInt(&pms, 10000);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 454, pym_getData(&pms), pym_getLen(&pms));
	// 2) send use
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->entityId); // actorID
	//pym_addInt(&pms, (!objData->belongsToBane)?179:176); // next state id (animation transition)
	pym_addInt(&pms, 97); // next state id (animation transition) 
	pym_addInt(&pms, 10000); // windupTime
	// possible additional args
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, Use, pym_getData(&pms), pym_getLen(&pms));
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

void controlpoint_interruptUse(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	if (actionID == 80 && actionArg == 7) //ControlPoint data?
	{
		printf("interrupting control point claiming\n");
		// Use interrupted
		controlpoint_t* objData = (controlpoint_t*)dynObject->objectData;
		if( objData->alreadyInUse )
		{	
			pyMarshalString_t pms;
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, client->player->actor->entityId); // actorID
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 609, pym_getData(&pms), pym_getLen(&pms));
			// change CP state - ForceState
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, objData->belongsToBane?179:176);
			pym_addInt(&pms, 10000);
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actionEntityId, 454, pym_getData(&pms), pym_getLen(&pms));
			client->player->actionEntityId = NULL; // superfluous?
			objData->alreadyInUse = false;
			dynamicObject_stopPeriodicUpdate(mapChannel, dynObject, 1);
		}
	}
}

dynObjectType_t dynObjectType_controlpoint = 
{
	controlpoint_destroy, // destroy
	controlpoint_appearForPlayers, // appearForPlayers
	controlpoint_disappearForPlayers, // disappearForPlayer
	controlpoint_periodicCallback, // periodicCallback
	controlpoint_useObject, // useObject
	controlpoint_interruptUse,

};
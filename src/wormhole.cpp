#include"global.h"

extern mapChannelList_t *global_channelList; //20110827 @dennton
extern dynObjectType_t dynObjectType_wormhole;

// this is copy of waypoint file, probobly we need to add/remove something
// 
// todo2: Wormhole discovery not supported yet, all wormholess are always available
// todo3: There is bug, even though there is code to avoid sending Recv_EnteredWormhole after using another wormhole, it is still sent sometimes.

typedef struct  
{
	// used to keep info about players that already received the EnteredWormhole message
	uint64 entityId; // entityID of player
	uint32 lastUpdate; // used to detect if the player left the wormhole
	bool teleportedAway; // used so we dont send client Recv_ExitedWormhole
}wormholeTriggerCheck_t;

typedef struct  
{
	uint32 wormholeID; // id unique for all maps (from teleporter table)
	//uint64 entityId;
	uint32 nameId; // index for wormholelanguage lookup
	uint8 state;
	std::vector<wormholeTriggerCheck_t> triggeredPlayers;
	uint32 updateCounter; // used to detect if players leave the wormhole
	// note: We cannot link players->wormholes in the DB by using entityIds because they are dynamically created, so we use the plain DB row IDs instead
}wormhole_t;

dynObject_t* wormhole_create(mapChannel_t *mapChannel, float x, float y, float z, float orientation, uint32 wormholeID, uint32 nameId, uint32 contextId)
{
	dynObject_t *dynObject = _dynamicObject_create(25408, &dynObjectType_wormhole);
	if( !dynObject )
		return NULL;
	dynObject->stateId = 0;
	dynamicObject_setPosition(dynObject, x, y, z);
	// setup wormhole specific data
	wormhole_t* objData = (wormhole_t*)malloc(sizeof(wormhole_t));
	memset(objData, 0x00, sizeof(wormhole_t));
	new(objData) wormhole_t();
	objData->wormholeID = wormholeID;
	objData->nameId = nameId;
	dynObject->objectData = objData;
	dynObject->contextId = contextId;
	// randomize rotation
	float randomRotY = orientation;
	float randomRotX = 0.0f;
	float randomRotZ = 0.0f;
	dynamicObject_setRotation(dynObject, randomRotY, randomRotX, randomRotZ);
	cellMgr_addToWorld(mapChannel, dynObject);
	// init periodic timer
	dynamicObject_setPeriodicUpdate(mapChannel, dynObject, 0, 400); // call about twice a second
	// add wormhole to map
	mapChannel->wormholes.push_back(dynObject);
	// return object
	return dynObject;
}

void wormhole_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// called shortly before free()
	// todo: remove wormhole from global wormhole list
	printf("wormhole_destroy not implemented\n");
}

void wormhole_appearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 56);
	pym_addInt(&pms, 100); // windupTime should not be zero to avoid freezing animations?
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(playerList, playerCount, dynObject->entityId, ForceState, pym_getData(&pms), pym_getLen(&pms));
}

void wormhole_disappearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// called before the object is removed from player sight
}

void wormhole_playerInAreaOfEffect(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client)
{
	wormhole_t* objData = (wormhole_t*)dynObject->objectData;
	// objData->updateCounter
	std::vector<wormholeTriggerCheck_t>::iterator itr = objData->triggeredPlayers.begin();
	while (itr != objData->triggeredPlayers.end())
	{
		if (itr->entityId == client->clientEntityId)
		{
			itr->lastUpdate = objData->updateCounter; // update counter
			return;
		}
		++itr;
	}
	// player not found, create new entry
	wormholeTriggerCheck_t newTriggerCheck = {0};
	newTriggerCheck.entityId = client->clientEntityId;
	newTriggerCheck.lastUpdate = objData->updateCounter;
	objData->triggeredPlayers.push_back(newTriggerCheck);
	// send wormhole list (Recv_EnteredWormhole)
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, mapChannel->mapInfo->contextId); // currentMapId
	pym_addInt(&pms, mapChannel->mapInfo->contextId); // gameContextId (current)
	pym_list_begin(&pms); // mapWormholeInfoList
	pym_tuple_begin(&pms);
	pym_addInt(&pms, mapChannel->mapInfo->contextId); // gameContextId for wormhole
	pym_list_begin(&pms); // mapInstanceList
	pym_list_end(&pms);
	pym_list_begin(&pms); // wormholes

	// parse through all wormholes and append data
		// todo: add wormholes from all maps to the list
	std::vector<dynObject_t*>::iterator wmItr = mapChannel->wormholes.begin();
	while (wmItr != mapChannel->wormholes.end())
		{
		dynObject_t* wormholeObject = *wmItr;
		wormhole_t* wormholeObjectData = (wormhole_t*)wormholeObject->objectData;
		pym_tuple_begin(&pms); // wormhole
		pym_addInt(&pms, wormholeObjectData->nameId); // wormholeID (not to be confused with our DB wormholeID)
			pym_tuple_begin(&pms); // pos
			pym_addInt(&pms, (sint32)wormholeObject->x);
			pym_addInt(&pms, (sint32)wormholeObject->y);
			pym_addInt(&pms, (sint32)wormholeObject->z);
			pym_tuple_end(&pms);
			pym_addInt(&pms, 0); // wormholeObject->contested 
			pym_tuple_end(&pms);
			++wmItr;
		}
	// idk how was in game transport done befor,
	// atm i'll add all waypoints to wormhole on same map
	// parse through all waypoints on this map and append data

	std::vector<dynObject_t*>::iterator wpItr = mapChannel->waypoints.begin();
	while (wpItr != mapChannel->waypoints.end())
	{
		dynObject_t* wormholeObject = *wpItr;
		wormhole_t* waypointObjectData = (wormhole_t*)wormholeObject->objectData;
		pym_tuple_begin(&pms); // waypoint
		pym_addInt(&pms, waypointObjectData->nameId); // waypointID (not to be confused with our DB waypointID)
		pym_tuple_begin(&pms); // pos
		pym_addInt(&pms, (sint32)wormholeObject->x);
		pym_addInt(&pms, (sint32)wormholeObject->y);
		pym_addInt(&pms, (sint32)wormholeObject->z);
		pym_tuple_end(&pms);
		pym_addInt(&pms, 0); // wormholeObject->contested
		pym_tuple_end(&pms);
		++wpItr;
	}

	pym_list_end(&pms);
	pym_tuple_end(&pms);
	pym_list_end(&pms);
	pym_addNoneStruct(&pms); // tempWormholes
	pym_addInt(&pms, 3); // wormholeTypeId (1 --> 'LOCALWAYPOINT', 2 --> 'MAPWAYPOINT', 3 --> 'WORMHOLE')
	pym_addInt(&pms, objData->wormholeID); // currentWormholeId
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(&client, 1, 5, EnteredWaypoint, pym_getData(&pms), pym_getLen(&pms));
}


bool wormhole_periodicCallback(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	wormhole_t* objData = (wormhole_t*)dynObject->objectData;
	objData->updateCounter++;
	// check for players in range
	// calculate rect of affected cells
	sint32 minX = (sint32)(((dynObject->x-8.0f) / CELL_SIZE) + CELL_BIAS);
	sint32 minZ = (sint32)(((dynObject->z-8.0f) / CELL_SIZE) + CELL_BIAS);
	sint32 maxX = (sint32)(((dynObject->x+8.0f+(CELL_SIZE-0.0001f)) / CELL_SIZE) + CELL_BIAS);
	sint32 maxZ = (sint32)(((dynObject->z+8.0f+(CELL_SIZE-0.0001f)) / CELL_SIZE) + CELL_BIAS);
	// check all cells for players
	for(sint32 ix=minX; ix<=maxX; ix++)
	{
		for(sint32 iz=minZ; iz<=maxZ; iz++)
		{
			mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
			if( nMapCell )
			{
				if( nMapCell->ht_playerList.empty() )
					continue;
				std::vector<mapChannelClient_t*>::iterator itr = nMapCell->ht_playerList.begin();
				while (itr != nMapCell->ht_playerList.end())
				{
					mapChannelClient_t* player = itr[0];
					++itr;
					// check distance to wormhole along xz plane
					float dX = dynObject->x - player->player->actor->posX;
					float dZ = dynObject->z - player->player->actor->posZ;
					float distance = dX*dX+dZ*dZ; // pythagoras but we optimized the sqrt() away
					if( distance >= (2.2f*2.2f) )
						continue;
					// check Y distance (rough)
					float dY = dynObject->y - player->player->actor->posY;
					if( dY < 0.0f ) dY = -dY;
					if( dY >= 10.0f )
						continue;
					wormhole_playerInAreaOfEffect(mapChannel, dynObject, player);
				}			
			}
		}
	}
	// check for not updated triggerCheck entries
	std::vector<wormholeTriggerCheck_t>::iterator itr = objData->triggeredPlayers.begin();
	while (itr != objData->triggeredPlayers.end())
	{
		if (itr->lastUpdate != objData->updateCounter)
		{
			// send exit-wormhole packet
			mapChannelClient_t* client = (mapChannelClient_t*)entityMgr_get(itr->entityId);
			if( client && itr->teleportedAway == false )
			{
				// send ExitedWormhole
				pyMarshalString_t pms;
				pym_init(&pms);
				pym_tuple_begin(&pms);
				pym_tuple_end(&pms);
				netMgr_pythonAddMethodCallRaw(&client, 1, 5, ExitedWaypoint, pym_getData(&pms), pym_getLen(&pms));
			}
			// player gone, remove entry
			itr = objData->triggeredPlayers.erase(itr);
			continue;
		}
		++itr;
	}
	return true;
}

void wormhole_useObject(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	// not used
}

void wormhole_interruptUse(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	// not used
}


void wormhole_recv_SelectWormhole(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 mapInstanceId;
	if( pym_unpack_isNoneStruct(&pums) )
		mapInstanceId = pym_unpackNoneStruct(&pums); // should not happen, seems to be a bug due to wrong Wormhole data we send
	else
		mapInstanceId = pym_unpackInt(&pums);
	sint32 wormholeId = pym_unpackInt(&pums);

	// todo1: Check if player is actually standing on a wormhole
	// todo2: Check if the player has discovered the wormhole

	// find the wormhole
	mapChannel_t* mapChannel = client->mapChannel;
	dynObject_t* wormholeObject = NULL;
	std::vector<dynObject_t*>::iterator itr = mapChannel->wormholes.begin();
	while (itr != mapChannel->wormholes.end())
	{
		dynObject_t* dynObject = *itr;
		wormhole_t* dynObjectData = (wormhole_t*)dynObject->objectData; // dont need to check type
		if (dynObjectData->nameId == wormholeId)
		{
			wormholeObject = dynObject;
			break;
		}
		++itr;
	}	
	if( wormholeObject == NULL )
	{
		printf("Recv_SelectWormhole: Waypoint or Wormhole not found, cannot teleport player.\n");
		return;
	}
	// mark player as teleportedAway to avoid sending ExitedWormhole
	// todo3: add the player to the destination wormhole triggerPlayer list (so we don't send EnteredWormhole again)
	// todo4: check if destination is on same map or not
	if (wormholeObject->contextId == mapChannel->mapInfo->contextId)
	{
		printf("map chanel id %u, wormhole pos id %u \n", wormholeObject->contextId, mapChannel->mapInfo->contextId);
		wormhole_t* wormholeObjectData = (wormhole_t*)wormholeObject->objectData;
		wormholeTriggerCheck_t newTriggerCheck = { 0 };
		newTriggerCheck.entityId = client->clientEntityId;
		newTriggerCheck.lastUpdate = wormholeObjectData->updateCounter;
		wormholeObjectData->triggeredPlayers.push_back(newTriggerCheck);
		// teleport the player (on the same map)
		// the packet is only sent to the teleporting player
		// this could cause problems when the destination is near enough
		// and the player does not leave the sight range of other players
		netCompressedMovement_t netMovement = { 0 };
		client->player->actor->posX = wormholeObject->x;
		client->player->actor->posY = wormholeObject->y + 0.5f;
		client->player->actor->posZ = wormholeObject->z;
		netMovement.entityId = client->player->actor->entityId;
		netMovement.posX24b = client->player->actor->posX * 256.0f;
		netMovement.posY24b = client->player->actor->posY * 256.0f;
		netMovement.posZ24b = client->player->actor->posZ * 256.0f;
		netMgr_sendEntityMovement(client->cgm, &netMovement);
	}
	// teleport player To other Map
	// TO DO:
	// this part of code is from communicator.cpp, ".tele" function,
	// it dosent work as it should
	// need future work
	else if (wormholeObject->contextId != mapChannel->mapInfo->contextId)
	{
		communicator_playerExitMap(client);
		Thread::LockMutex(&client->cgm->cs_general);
		cellMgr_removeFromWorld(client);
		// remove from list
		for (sint32 i = 0; i<client->mapChannel->playerCount; i++)
		{
			if (client == client->mapChannel->playerList[i])
			{
				if (i == client->mapChannel->playerCount - 1)
				{
					client->mapChannel->playerCount--;
				}
				else
				{
					client->mapChannel->playerList[i] = client->mapChannel->playerList[client->mapChannel->playerCount - 1];
					client->mapChannel->playerCount--;
				}
				break;
			}
		}
		Thread::UnlockMutex(&client->cgm->cs_general);
		
		//############## map loading stuff ##############
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 0); // wonkType - actually not used by the game
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, 134, pym_getData(&pms), pym_getLen(&pms));
		// send Wonkavate (inputstateRouter.242)
		client->cgm->mapLoadContextId = wormholeObject->contextId;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, wormholeObject->contextId);	// gameContextId (alias mapId)
		pym_addInt(&pms, 0);	// instanceId ( not important for now )
		// find map version
		sint32 mapVersion = 0; // default = 0;
		for (sint32 i = 0; i<mapInfoCount; i++)
		{
			if (mapInfoArray[i].contextId == wormholeObject->contextId)
			{
				mapVersion = mapInfoArray[i].version;
				break;
			}
		}
		pym_addInt(&pms, mapVersion);	// templateVersion ( from the map file? )
		pym_tuple_begin(&pms);  // startPosition
		pym_addFloat(&pms, wormholeObject->x); // x (todo: send as float)
		pym_addFloat(&pms, wormholeObject->y + 0.5f); // y (todo: send as float)
		pym_addFloat(&pms, wormholeObject->z); // z (todo: send as float)
		pym_tuple_end(&pms);
		pym_addInt(&pms, 0);	// startRotation (todo, read from db and send as float)
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 6, Wonkavate, pym_getData(&pms), pym_getLen(&pms));

		//################## player assigning ###############
		communicator_loginOk(client->mapChannel, client);
		communicator_playerEnterMap(client);
		//---search new mapchannel
		for (sint32 chan = 0; chan < global_channelList->mapChannelCount; chan++)
		{
			mapChannel_t *mapChannel = global_channelList->mapChannelArray + chan;
			if (mapChannel->mapInfo->contextId == wormholeObject->contextId)
			{
				client->mapChannel = mapChannel;
				break;
			}
		}

		mapChannel_t *mapChannel = client->mapChannel;
		Thread::LockMutex(&client->mapChannel->criticalSection);
		mapChannel->playerList[mapChannel->playerCount] = client;
		mapChannel->playerCount++;
		hashTable_set(&mapChannel->ht_socketToClient, (uint32)client->cgm->socket, client); 
		Thread::UnlockMutex(&mapChannel->criticalSection);

		cellMgr_addToWorld(client); //cellsint32roducing to player /from players
		client->player->actor->posX = wormholeObject->x;
		client->player->actor->posY = wormholeObject->y + 0.5f;
		client->player->actor->posZ = wormholeObject->z;
		client->player->actor->contextId = wormholeObject->contextId;
		client->player->controllerUser->inventory = client->inventory;
		client->player->controllerUser->mission = client->mission;
		client->tempCharacterData = client->player->controllerUser->tempCharacterData;
		
		// setCurrentContextId (clientMethod.362)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, wormholeObject->contextId);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, SetCurrentContextId, pym_getData(&pms), pym_getLen(&pms));
	}
}

dynObjectType_t dynObjectType_wormhole = 
{
	wormhole_destroy, // destroy
	wormhole_appearForPlayers, // appearForPlayers
	wormhole_disappearForPlayers, // disappearForPlayer
	wormhole_periodicCallback, // periodicCallback
	wormhole_useObject, // useObject
	wormhole_interruptUse
};

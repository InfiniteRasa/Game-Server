#include "global.h"

// cell index:
// x/z --> 0 to 0xFFFFFF

bool cellMgr_initForMapChannel(mapChannel_t *mapChannel)
{
	hashTable_init(&mapChannel->mapCellInfo.ht_cells, 128);
	mapChannel->mapCellInfo.loadedCellCount = 0;
	mapChannel->mapCellInfo.loadedCellLimit = 128;
	mapChannel->mapCellInfo.loadedCellList = (mapCell_t**)malloc(sizeof(mapCell_t*)*mapChannel->mapCellInfo.loadedCellLimit);
	mapChannel->mapCellInfo.time_updateVisibility = GetTickCount()+1000;
	return true;
}

// will always return a valid cell if within valid range conditions
mapCell_t* cellMgr_getCell(mapChannel_t *mapChannel, int x, int z)
{
	unsigned cellSeed = (x&0xFFFF) | (z<<16);
	mapCell_t *mapCell = (mapCell_t*)hashTable_get(&mapChannel->mapCellInfo.ht_cells, cellSeed);
	if( mapCell == NULL )
	{
		// create cell
		mapCell = (mapCell_t*)malloc(sizeof(mapCell_t));
		// init cell
		hashTable_init(&mapCell->ht_playerList, 8);
		hashTable_init(&mapCell->ht_playerNotifyList, 8);
		hashTable_init(&mapCell->ht_objectList, 8);
		hashTable_init(&mapCell->ht_npcList, 8);
		hashTable_init(&mapCell->ht_creatureList, 8);
		if( mapChannel->mapCellInfo.loadedCellCount == mapChannel->mapCellInfo.loadedCellLimit )
		{
			// enlarge buffer
			int newLimit = mapChannel->mapCellInfo.loadedCellLimit * 2;
			mapCell_t **newLoadedCellList = (mapCell_t**)malloc(sizeof(mapCell_t*) * newLimit);
			for(int i=0; i<mapChannel->mapCellInfo.loadedCellLimit; i++)
				newLoadedCellList[i] = mapChannel->mapCellInfo.loadedCellList[i];
			free(mapChannel->mapCellInfo.loadedCellList);
			mapChannel->mapCellInfo.loadedCellList = newLoadedCellList;
			mapChannel->mapCellInfo.loadedCellLimit = newLimit;
			if( newLoadedCellList == NULL )
			{
				printf("ERROR in 'cellMgr_getCell'\n");
				Sleep(1000*30);
				ExitThread(-1);
			}
		}
		// save cell
		mapChannel->mapCellInfo.loadedCellList[mapChannel->mapCellInfo.loadedCellCount] = mapCell;
		mapChannel->mapCellInfo.loadedCellCount++;
		// register cell
		hashTable_set(&mapChannel->mapCellInfo.ht_cells, cellSeed, mapCell);
		return mapCell;
	}
	return mapCell;
}

// will return the cell only if it exists
mapCell_t* cellMgr_tryGetCell(mapChannel_t *mapChannel, int x, int z)
{
	unsigned cellSeed = (x&0xFFFF) | (z<<16);
	mapCell_t *mapCell = (mapCell_t*)hashTable_get(&mapChannel->mapCellInfo.ht_cells, cellSeed);
	return mapCell;
}

void cellMgr_addToWorld( mapChannelClient_t *client )
{
	if( !client->player )
		return;
	mapChannel_t *mapChannel = client->mapChannel;
	unsigned int x = (unsigned int)((client->player->actor->posX / CELL_SIZE) + CELL_BIAS);
	unsigned int z = (unsigned int)((client->player->actor->posZ / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	client->player->actor->cellLocation.x = x;
	client->player->actor->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// register player in cell
		hashTable_set(&mapCell->ht_playerList, client->clientEntityId, client);
		// register notifications in visible area
		for(int ix=x-CELL_VIEWRANGE; ix<=x+CELL_VIEWRANGE; ix++)
		{
			for(int iz=z-CELL_VIEWRANGE; iz<=z+CELL_VIEWRANGE; iz++)
			{
				mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
				if( nMapCell )
				{
					hashTable_set(&nMapCell->ht_playerNotifyList, client->clientEntityId, client);
					// notify me about all objects that are visible to the cell
					dynamicObject_cellIntroduceObjectsToClient(mapChannel, client, (dynObject_t**)hashTable_getValueArray(&nMapCell->ht_objectList), hashTable_getCount(&nMapCell->ht_objectList));
					// notify me about all npcs that are visible to the cell
					npc_cellIntroduceNPCsToClient(mapChannel, client, (npc_t**)hashTable_getValueArray(&nMapCell->ht_npcList), hashTable_getCount(&nMapCell->ht_npcList));
					// notify me about all creatures that are visible to the cell
					creature_cellIntroduceCreaturesToClient(mapChannel, client, (creature_t**)hashTable_getValueArray(&nMapCell->ht_creatureList), hashTable_getCount(&nMapCell->ht_creatureList));				
				}
			}
		}
		// notify all players of me
		manifestation_cellIntroduceClientToPlayers(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList), hashTable_getCount(&mapCell->ht_playerNotifyList));
		// notify me about all players that are visible here
		manifestation_cellIntroducePlayersToClient(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList), hashTable_getCount(&mapCell->ht_playerNotifyList));
	}
}

void cellMgr_removeFromWorld( mapChannelClient_t *client )
{
	if( client->player == NULL )
		return;
	mapChannel_t *mapChannel = client->mapChannel;
	int oldX1 = client->player->actor->cellLocation.x-CELL_VIEWRANGE;
	int oldX2 = client->player->actor->cellLocation.x+CELL_VIEWRANGE;
	int oldZ1 = client->player->actor->cellLocation.z-CELL_VIEWRANGE;
	int oldZ2 = client->player->actor->cellLocation.z+CELL_VIEWRANGE;
	for(int ix=oldX1; ix<=oldX2; ix++)
	{
		for(int iz=oldZ1; iz<=oldZ2; iz++)
		{
			mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
			if( nMapCell )
			{
				// remove notify entry
				hashTable_set(&nMapCell->ht_playerNotifyList, client->clientEntityId, NULL);
				// remove player visibility client-side
				manifestation_cellDiscardClientToPlayers(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&nMapCell->ht_playerList), hashTable_getCount(&nMapCell->ht_playerList));					
				manifestation_cellDiscardPlayersToClient(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&nMapCell->ht_playerList), hashTable_getCount(&nMapCell->ht_playerList));					
			}
		}
	}
	mapCell_t *nMapCell = cellMgr_getCell(mapChannel, client->player->actor->cellLocation.x, client->player->actor->cellLocation.z);
	if( nMapCell )
	{
		hashTable_set(&nMapCell->ht_playerList, client->clientEntityId, NULL);
	}
}

void cellMgr_addToWorld(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	if( !dynObject )
		return;
	unsigned int x = (unsigned int)((dynObject->x / CELL_SIZE) + CELL_BIAS);
	unsigned int z = (unsigned int)((dynObject->z / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	dynObject->cellLocation.x = x;
	dynObject->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// register object
		hashTable_set(&mapCell->ht_objectList, dynObject->entityId, dynObject);
		// notify all players of object
		dynamicObject_cellIntroduceObjectToClients(mapChannel, dynObject, (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList), hashTable_getCount(&mapCell->ht_playerNotifyList));
	}
}


void cellMgr_removeFromWorld(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	if( !dynObject )
		return;
	unsigned int x = (unsigned int)((dynObject->x / CELL_SIZE) + CELL_BIAS);
	unsigned int z = (unsigned int)((dynObject->z / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	dynObject->cellLocation.x = x;
	dynObject->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// notify all players of object removing
		dynamicObject_cellDiscardObjectToClients(mapChannel, dynObject, (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList), hashTable_getCount(&mapCell->ht_playerNotifyList));
		// unregister object
		hashTable_set(&mapCell->ht_objectList, dynObject->entityId, NULL);
	}
}

// todo: removeFromWorld for dynObjects

void cellMgr_addToWorld(mapChannel_t *mapChannel, npc_t *npc)
{
	if( !npc )
		return;
	// register npc entity
	entityMgr_registerEntity(npc->entityId, npc);
	// get initial cell
	unsigned int x = (unsigned int)((npc->actor.posX / CELL_SIZE) + CELL_BIAS);
	unsigned int z = (unsigned int)((npc->actor.posZ / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	npc->actor.cellLocation.x = x;
	npc->actor.cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// register object
		hashTable_set(&mapCell->ht_npcList, npc->actor.entityId, npc);
		// notify all players of object
		npc_cellIntroduceNPCToClients(mapChannel, npc, (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList), hashTable_getCount(&mapCell->ht_playerNotifyList));
	}
}

// todo: removeFromWorld for npcs

void cellMgr_addToWorld(mapChannel_t *mapChannel, creature_t *creature)
{
	if( !creature )
		return;
	// register creature entity
	entityMgr_registerEntity(creature->actor.entityId, creature);
	// get initial cell
	unsigned int x = (unsigned int)((creature->actor.posX / CELL_SIZE) + CELL_BIAS);
	unsigned int z = (unsigned int)((creature->actor.posZ / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	creature->actor.cellLocation.x = x;
	creature->actor.cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// register object
		hashTable_set(&mapCell->ht_creatureList, creature->actor.entityId, creature);
		// notify all players of object
		creature_cellIntroduceCreatureToClients(mapChannel, creature, (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList), hashTable_getCount(&mapCell->ht_playerNotifyList));
	}
}

// todo: removeFromWorld for creatures


mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannelClient_t *aggregator, int *oCount)
{
	if( !aggregator->player )
		return NULL;
	mapChannel_t *mapChannel = aggregator->mapChannel;
	//unsigned int x = (unsigned int)((aggregator->player->actor->posX / CELL_SIZE) + CELL_BIAS);
	//unsigned int z = (unsigned int)((aggregator->player->actor->posZ / CELL_SIZE) + CELL_BIAS);
	//// calculate initial cell
	//aggregator->cellLocation.x = x;
	//aggregator->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, aggregator->player->actor->cellLocation.x, aggregator->player->actor->cellLocation.z);
	// get players
	*oCount =  hashTable_getCount(&mapCell->ht_playerNotifyList);
	return (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList);
}

mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, actor_t *aggregator, int *oCount)
{
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, aggregator->cellLocation.x, aggregator->cellLocation.z);
	// get players
	*oCount =  hashTable_getCount(&mapCell->ht_playerNotifyList);
	return (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList);
}

mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, dynObject_t *aggregator, int *oCount)
{
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, aggregator->cellLocation.x, aggregator->cellLocation.z);
	// get players
	*oCount =  hashTable_getCount(&mapCell->ht_playerNotifyList);
	return (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList);
}

npc_t *cellMgr_findNPCinViewOf(mapChannelClient_t *aggregator, unsigned long long npcEntityId)
{
	__debugbreak();
	// todo
	return NULL;
}

void cellMgr_updateVisibility( mapChannel_t *mapChannel )
{
	for(int i=0; i<mapChannel->playerCount; i++)
	{
		mapChannelClient_t *client = mapChannel->playerList[i];
		if( client->disconnected || client->player == NULL )
			continue;
		unsigned int x = (unsigned int)((client->player->actor->posX / CELL_SIZE) + CELL_BIAS);
		unsigned int z = (unsigned int)((client->player->actor->posZ / CELL_SIZE) + CELL_BIAS);
		if( client->player->actor->cellLocation.x != x || client->player->actor->cellLocation.z != z )
		{
			// find players that leave visibility range
			int oldX1 = client->player->actor->cellLocation.x-CELL_VIEWRANGE;
			int oldX2 = client->player->actor->cellLocation.x+CELL_VIEWRANGE;
			int oldZ1 = client->player->actor->cellLocation.z-CELL_VIEWRANGE;
			int oldZ2 = client->player->actor->cellLocation.z+CELL_VIEWRANGE;

			for(int ix=oldX1; ix<=oldX2; ix++)
			{
				for(int iz=oldZ1; iz<=oldZ2; iz++)
				{
					if( (ix>=(x-CELL_VIEWRANGE) && ix<=(x+CELL_VIEWRANGE)) && (iz>=(z-CELL_VIEWRANGE) && iz<=(z+CELL_VIEWRANGE)) )
						continue;
					mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
					if( nMapCell )
					{
						// remove notify entry
						hashTable_set(&nMapCell->ht_playerNotifyList, client->clientEntityId, NULL);
						// remove player visibility client-side
						manifestation_cellDiscardClientToPlayers(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&nMapCell->ht_playerList), hashTable_getCount(&nMapCell->ht_playerList));					
						manifestation_cellDiscardPlayersToClient(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&nMapCell->ht_playerList), hashTable_getCount(&nMapCell->ht_playerList));					
						// remove object visibility
						dynamicObject_cellDiscardObjectsToClient(mapChannel, client, (dynObject_t**)hashTable_getValueArray(&nMapCell->ht_objectList), hashTable_getCount(&nMapCell->ht_objectList));	
						// remove npc visibility
						npc_cellDiscardNPCsToClient(mapChannel, client, (npc_t**)hashTable_getValueArray(&nMapCell->ht_npcList), hashTable_getCount(&nMapCell->ht_npcList));	
						// remove creature visibility
						creature_cellDiscardCreaturesToClient(mapChannel, client, (creature_t**)hashTable_getValueArray(&nMapCell->ht_creatureList), hashTable_getCount(&nMapCell->ht_creatureList));				
					}
				}
			}
			// find players that enter visibility range
			for(int ix=x-CELL_VIEWRANGE; ix<=x+CELL_VIEWRANGE; ix++)
			{
				for(int iz=z-CELL_VIEWRANGE; iz<=z+CELL_VIEWRANGE; iz++)
				{
					if( (ix>=oldX1 && ix<=oldX2) && (iz>=oldZ1 && iz<=oldZ2) )
						continue;
					mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
					if( nMapCell )
					{
						// add player visibility client-side
						manifestation_cellIntroduceClientToPlayers(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&nMapCell->ht_playerList), hashTable_getCount(&nMapCell->ht_playerList));					
						manifestation_cellIntroducePlayersToClient(mapChannel, client, (mapChannelClient_t**)hashTable_getValueArray(&nMapCell->ht_playerList), hashTable_getCount(&nMapCell->ht_playerList));					
						// add notify entry
						hashTable_set(&nMapCell->ht_playerNotifyList, client->clientEntityId, client);
						// add object visibility client-side
						dynamicObject_cellIntroduceObjectsToClient(mapChannel, client, (dynObject_t**)hashTable_getValueArray(&nMapCell->ht_objectList), hashTable_getCount(&nMapCell->ht_objectList));				
						// add npc visibility client-side
						npc_cellIntroduceNPCsToClient(mapChannel, client, (npc_t**)hashTable_getValueArray(&nMapCell->ht_npcList), hashTable_getCount(&nMapCell->ht_npcList));				
						// add creature visibility client-side
						creature_cellIntroduceCreaturesToClient(mapChannel, client, (creature_t**)hashTable_getValueArray(&nMapCell->ht_creatureList), hashTable_getCount(&nMapCell->ht_creatureList));				
					}
				}
			}
			// move the player entry
			mapCell_t *nMapCell = cellMgr_getCell(mapChannel, client->player->actor->cellLocation.x, client->player->actor->cellLocation.z);
			if( nMapCell )
			{
				hashTable_set(&nMapCell->ht_playerList, client->clientEntityId, NULL);
			}
			nMapCell = cellMgr_getCell(mapChannel, x, z);
			if( nMapCell )
			{
				hashTable_set(&nMapCell->ht_playerList, client->clientEntityId, client);
			}
			// update location
			client->player->actor->cellLocation.x = x;
			client->player->actor->cellLocation.z = z;
		}
	}
}

void cellMgr_doWork( mapChannel_t *mapChannel )
{
	unsigned int currentTime = GetTickCount();
	if( mapChannel->mapCellInfo.time_updateVisibility < currentTime )
	{
		cellMgr_updateVisibility(mapChannel);
		// update three times a second
		mapChannel->mapCellInfo.time_updateVisibility = currentTime + 300;
	}
	// mob work

	// events etc...

	// clean up empty cells
}
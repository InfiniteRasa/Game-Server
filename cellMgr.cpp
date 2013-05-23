#include "global.h"

// cell index:
// x/z --> 0 to 0xFFFFFF

bool cellMgr_initForMapChannel(mapChannel_t *mapChannel)
{
	hashTable_init(&mapChannel->mapCellInfo.ht_cells, 2048);
	mapChannel->mapCellInfo.loadedCellCount = 0;
	mapChannel->mapCellInfo.loadedCellLimit = 2048;
	mapChannel->mapCellInfo.loadedCellList = (mapCell_t**)malloc(sizeof(mapCell_t*)*mapChannel->mapCellInfo.loadedCellLimit);
	mapChannel->mapCellInfo.time_updateVisibility = GetTickCount()+1000;
	return true;
}

// will always return a valid cell if within valid range conditions
mapCell_t* cellMgr_getCell(mapChannel_t *mapChannel, sint32 x, sint32 z)
{
	unsigned cellSeed = (x&0xFFFF) | (z<<16);
	mapCell_t *mapCell = (mapCell_t*)hashTable_get(&mapChannel->mapCellInfo.ht_cells, cellSeed);
	if( mapCell == NULL )
	{
		// create cell
		mapCell = new mapCell_t;
		if( mapChannel->mapCellInfo.loadedCellCount == mapChannel->mapCellInfo.loadedCellLimit )
		{
			// enlarge buffer
			sint32 newLimit = mapChannel->mapCellInfo.loadedCellLimit * 2;
			mapCell_t **newLoadedCellList = (mapCell_t**)malloc(sizeof(mapCell_t*) * newLimit);
			for(sint32 i=0; i<mapChannel->mapCellInfo.loadedCellLimit; i++)
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
mapCell_t* cellMgr_tryGetCell(mapChannel_t *mapChannel, sint32 x, sint32 z)
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
	uint32 x = (uint32)((client->player->actor->posX / CELL_SIZE) + CELL_BIAS);
	uint32 z = (uint32)((client->player->actor->posZ / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	client->player->actor->cellLocation.x = x;
	client->player->actor->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// register player in cell
		mapCell->ht_playerList.push_back(client);
		// register notifications in visible area
		for(sint32 ix=x-CELL_VIEWRANGE; ix<=x+CELL_VIEWRANGE; ix++)
		{
			for(sint32 iz=z-CELL_VIEWRANGE; iz<=z+CELL_VIEWRANGE; iz++)
			{
				mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
				if( nMapCell )
				{
					nMapCell->ht_playerNotifyList.push_back(client);
					// notify me about all objects that are visible to the cell
					if( nMapCell->ht_objectList.empty() == false )
						dynamicObject_cellIntroduceObjectsToClient(mapChannel, client, &nMapCell->ht_objectList[0], nMapCell->ht_objectList.size());
					// notify me about all creatures that are visible to the cell
					if( nMapCell->ht_creatureList.empty() == false )
						creature_cellIntroduceCreaturesToClient(mapChannel, client, &nMapCell->ht_creatureList[0], nMapCell->ht_creatureList.size());				
				}
			}
		}
		// notify all players of me
		if( mapCell->ht_playerNotifyList.empty() == false )
			manifestation_cellIntroduceClientToPlayers(mapChannel, client, &mapCell->ht_playerNotifyList[0], mapCell->ht_playerNotifyList.size());
		// notify me about all players that are visible here
		if( mapCell->ht_playerNotifyList.empty() == false )
			manifestation_cellIntroducePlayersToClient(mapChannel, client, &mapCell->ht_playerNotifyList[0], mapCell->ht_playerNotifyList.size());
	}
}

void cellMgr_removeFromWorld( mapChannelClient_t *client )
{
	if( client->player == NULL )
		return;
	mapChannel_t *mapChannel = client->mapChannel;
	sint32 oldX1 = client->player->actor->cellLocation.x-CELL_VIEWRANGE;
	sint32 oldX2 = client->player->actor->cellLocation.x+CELL_VIEWRANGE;
	sint32 oldZ1 = client->player->actor->cellLocation.z-CELL_VIEWRANGE;
	sint32 oldZ2 = client->player->actor->cellLocation.z+CELL_VIEWRANGE;
	for(sint32 ix=oldX1; ix<=oldX2; ix++)
	{
		for(sint32 iz=oldZ1; iz<=oldZ2; iz++)
		{
			mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
			if( nMapCell )
			{
				// remove notify entry
				std::vector<mapChannelClient_t*>::iterator itr = nMapCell->ht_playerNotifyList.begin();
				while (itr != nMapCell->ht_playerNotifyList.end())
				{
					if ((*itr) == client)
					{
						nMapCell->ht_playerNotifyList.erase(itr);
						break;
					}
					++itr;
				}
				// remove player visibility client-side
				if( nMapCell->ht_playerList.empty() == false )
				{
					manifestation_cellDiscardClientToPlayers(mapChannel, client, &nMapCell->ht_playerList[0], nMapCell->ht_playerList.size());					
					manifestation_cellDiscardPlayersToClient(mapChannel, client, &nMapCell->ht_playerList[0], nMapCell->ht_playerList.size());					
				}
			}
		}
	}
	mapCell_t *nMapCell = cellMgr_getCell(mapChannel, client->player->actor->cellLocation.x, client->player->actor->cellLocation.z);
	if( nMapCell )
	{
		//hashTable_set(&nMapCell->ht_playerList, client->clientEntityId, NULL);
		std::vector<mapChannelClient_t*>::iterator itr = nMapCell->ht_playerList.begin();
		while (itr != nMapCell->ht_playerList.end())
		{
			if ((*itr) == client)
			{
				nMapCell->ht_playerList.erase(itr);
				break;
			}
			++itr;
		}
	}
}

void cellMgr_addToWorld(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	if( !dynObject )
		return;
	uint32 x = (uint32)((dynObject->x / CELL_SIZE) + CELL_BIAS);
	uint32 z = (uint32)((dynObject->z / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	dynObject->cellLocation.x = x;
	dynObject->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// register object
		//hashTable_set(&mapCell->ht_objectList, dynObject->entityId, dynObject);
		mapCell->ht_objectList.push_back(dynObject);
		// notify all players of object
		if( mapCell->ht_playerNotifyList.empty() == false )
			dynamicObject_cellIntroduceObjectToClients(mapChannel, dynObject, &mapCell->ht_playerNotifyList[0], mapCell->ht_playerNotifyList.size());
	}
}


void cellMgr_removeFromWorld(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	if( !dynObject )
		return;
	uint32 x = (uint32)((dynObject->x / CELL_SIZE) + CELL_BIAS);
	uint32 z = (uint32)((dynObject->z / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	dynObject->cellLocation.x = x;
	dynObject->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// notify all players of object removing
		if( mapCell->ht_playerNotifyList.empty() == false )
			dynamicObject_cellDiscardObjectToClients(mapChannel, dynObject, &mapCell->ht_playerNotifyList[0], mapCell->ht_playerNotifyList.size());
		// unregister object
		std::vector<dynObject_t*>::iterator itr = mapCell->ht_objectList.begin();
		while (itr != mapCell->ht_objectList.end())
		{
			if ((*itr) == dynObject)
			{
				mapCell->ht_objectList.erase(itr);
				break;
			}
			++itr;
		}
	}
}


void cellMgr_addToWorld(mapChannel_t *mapChannel, creature_t *creature)
{
	if( !creature )
		return;
	// register creature entity
	entityMgr_registerEntity(creature->actor.entityId, creature);
	// get initial cell
	uint32 x = (uint32)((creature->actor.posX / CELL_SIZE) + CELL_BIAS);
	uint32 z = (uint32)((creature->actor.posZ / CELL_SIZE) + CELL_BIAS);
	// calculate initial cell
	creature->actor.cellLocation.x = x;
	creature->actor.cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// register object
		//hashTable_set(&mapCell->ht_creatureList, creature->actor.entityId, creature);
		mapCell->ht_creatureList.push_back(creature);
		// notify all players of object
		if( mapCell->ht_playerNotifyList.empty() == false )
			creature_cellintroduceCreatureToClients(mapChannel, creature, &mapCell->ht_playerNotifyList[0], mapCell->ht_playerNotifyList.size());
	}
}

void cellMgr_removeCreatureFromWorld( mapChannel_t *mapChannel, creature_t *creature )
{
	if( !creature )
		return;
	// get current cell
	uint32 x = creature->actor.cellLocation.x;
	uint32 z = creature->actor.cellLocation.z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, x, z);
	if( mapCell )
	{
		// notify all players of creature removing
		if( mapCell->ht_playerNotifyList.empty() == false )
			creature_cellDiscardCreatureToClients(mapChannel, creature, &mapCell->ht_playerNotifyList[0], mapCell->ht_playerNotifyList.size());
		// unregister creature
		//hashTable_set(&mapCell->ht_creatureList, creature->actor.entityId, NULL);
		std::vector<creature_t*>::iterator itr = mapCell->ht_creatureList.begin();
		while (itr != mapCell->ht_creatureList.end())
		{
			if ((*itr) == creature)
			{
				mapCell->ht_creatureList.erase(itr);
				break;
			}
			++itr;
		}
	}

}


mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannelClient_t *aggregator, sint32 *oCount)
{
	if( !aggregator->player )
		return NULL;
	mapChannel_t *mapChannel = aggregator->mapChannel;
	//uint32 x = (uint32)((aggregator->player->actor->posX / CELL_SIZE) + CELL_BIAS);
	//uint32 z = (uint32)((aggregator->player->actor->posZ / CELL_SIZE) + CELL_BIAS);
	//// calculate initial cell
	//aggregator->cellLocation.x = x;
	//aggregator->cellLocation.z = z;
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, aggregator->player->actor->cellLocation.x, aggregator->player->actor->cellLocation.z);
	// get players
	if( mapCell->ht_playerNotifyList.empty() )
		return NULL;
	*oCount =  mapCell->ht_playerNotifyList.size();
	return &mapCell->ht_playerNotifyList[0];
}

mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, actor_t *aggregator, sint32 *oCount)
{
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, aggregator->cellLocation.x, aggregator->cellLocation.z);
	// get players
	if( mapCell->ht_playerNotifyList.empty() )
		return NULL;
	*oCount =  mapCell->ht_playerNotifyList.size();
	return &mapCell->ht_playerNotifyList[0];
}

mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, dynObject_t *aggregator, sint32 *oCount)
{
	// get cell
	mapCell_t *mapCell = cellMgr_getCell(mapChannel, aggregator->cellLocation.x, aggregator->cellLocation.z);
	// get players
	if( mapCell->ht_playerNotifyList.empty() )
		return NULL;
	*oCount =  mapCell->ht_playerNotifyList.size();
	return &mapCell->ht_playerNotifyList[0];
}


void cellMgr_updateVisibility( mapChannel_t *mapChannel )
{
	for(sint32 i=0; i<mapChannel->playerCount; i++)
	{
		mapChannelClient_t *client = mapChannel->playerList[i];
		if( client->disconnected || client->player == NULL )
			continue;
		uint32 x = (uint32)((client->player->actor->posX / CELL_SIZE) + CELL_BIAS);
		uint32 z = (uint32)((client->player->actor->posZ / CELL_SIZE) + CELL_BIAS);
		if( client->player->actor->cellLocation.x != x || client->player->actor->cellLocation.z != z )
		{
			// find players that leave visibility range
			sint32 oldX1 = client->player->actor->cellLocation.x-CELL_VIEWRANGE;
			sint32 oldX2 = client->player->actor->cellLocation.x+CELL_VIEWRANGE;
			sint32 oldZ1 = client->player->actor->cellLocation.z-CELL_VIEWRANGE;
			sint32 oldZ2 = client->player->actor->cellLocation.z+CELL_VIEWRANGE;

			for(sint32 ix=oldX1; ix<=oldX2; ix++)
			{
				for(sint32 iz=oldZ1; iz<=oldZ2; iz++)
				{
					if( (ix>=(x-CELL_VIEWRANGE) && ix<=(x+CELL_VIEWRANGE)) && (iz>=(z-CELL_VIEWRANGE) && iz<=(z+CELL_VIEWRANGE)) )
						continue;
					mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
					if( nMapCell )
					{
						// remove notify entry
						//hashTable_set(&nMapCell->ht_playerNotifyList, client->clientEntityId, NULL);
						std::vector<mapChannelClient_t*>::iterator itr = nMapCell->ht_playerNotifyList.begin();
						while (itr != nMapCell->ht_playerNotifyList.end())
						{
							if ((*itr) == client)
							{
								nMapCell->ht_playerNotifyList.erase(itr);
								break;
							}
							++itr;
						}
						// remove player visibility client-side
						if( nMapCell->ht_playerList.empty() == false )
						{
							manifestation_cellDiscardClientToPlayers(mapChannel, client, &nMapCell->ht_playerList[0], nMapCell->ht_playerList.size());					
							manifestation_cellDiscardPlayersToClient(mapChannel, client, &nMapCell->ht_playerList[0], nMapCell->ht_playerList.size());
						}
						// remove object visibility
						if( nMapCell->ht_objectList.empty() == false )
							dynamicObject_cellDiscardObjectsToClient(mapChannel, client, &nMapCell->ht_objectList[0], nMapCell->ht_objectList.size());	
						// remove creature visibility
						if( nMapCell->ht_creatureList.empty() == false )
							creature_cellDiscardCreaturesToClient(mapChannel, client, &nMapCell->ht_creatureList[0], nMapCell->ht_creatureList.size());				
					}
				}
			}
			// find players that enter visibility range
			for(sint32 ix=x-CELL_VIEWRANGE; ix<=x+CELL_VIEWRANGE; ix++)
			{
				for(sint32 iz=z-CELL_VIEWRANGE; iz<=z+CELL_VIEWRANGE; iz++)
				{
					if( (ix>=oldX1 && ix<=oldX2) && (iz>=oldZ1 && iz<=oldZ2) )
						continue;
					mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
					if( nMapCell )
					{
						// add player visibility client-side
						if( nMapCell->ht_playerList.empty() == false )
						{
							manifestation_cellIntroduceClientToPlayers(mapChannel, client, &nMapCell->ht_playerList[0], nMapCell->ht_playerList.size());					
							manifestation_cellIntroducePlayersToClient(mapChannel, client, &nMapCell->ht_playerList[0], nMapCell->ht_playerList.size());					
						}
						// add notify entry
						//hashTable_set(&nMapCell->ht_playerNotifyList, client->clientEntityId, client);
						nMapCell->ht_playerNotifyList.push_back(client);
						// add object visibility client-side
						if( nMapCell->ht_objectList.empty() == false )
							dynamicObject_cellIntroduceObjectsToClient(mapChannel, client, &nMapCell->ht_objectList[0], nMapCell->ht_objectList.size());				
						// add creature visibility client-side
						if( nMapCell->ht_creatureList.empty() == false )
							creature_cellIntroduceCreaturesToClient(mapChannel, client, &nMapCell->ht_creatureList[0], nMapCell->ht_creatureList.size());				
					}
				}
			}
			// move the player entry
			mapCell_t *nMapCell = cellMgr_getCell(mapChannel, client->player->actor->cellLocation.x, client->player->actor->cellLocation.z);
			if( nMapCell )
			{
				//hashTable_set(&nMapCell->ht_playerList, client->clientEntityId, NULL);
				std::vector<mapChannelClient_t*>::iterator itr = nMapCell->ht_playerList.begin();
				while (itr != nMapCell->ht_playerList.end())
				{
					if ((*itr) == client)
					{
						nMapCell->ht_playerList.erase(itr);
						break;
					}
					itr++;
				}
			}
			nMapCell = cellMgr_getCell(mapChannel, x, z);
			if( nMapCell )
			{
				//hashTable_set(&nMapCell->ht_playerList, client->clientEntityId, client);
				nMapCell->ht_playerList.push_back(client);

			}
			// update location
			client->player->actor->cellLocation.x = x;
			client->player->actor->cellLocation.z = z;
		}
	}
}

void cellMgr_doWork( mapChannel_t *mapChannel )
{
	uint32 currentTime = GetTickCount();
	if( mapChannel->mapCellInfo.time_updateVisibility < currentTime )
	{
		cellMgr_updateVisibility(mapChannel);
		// update three times a second
		mapChannel->mapCellInfo.time_updateVisibility = currentTime + 300;
	}
	// mob work

	// events etc...

}

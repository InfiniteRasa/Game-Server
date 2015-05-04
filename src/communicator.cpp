#include"global.h"
extern mapChannelList_t *global_channelList; //20110827 @dennton

typedef struct _chatChannel_playerLink_t
{
	unsigned long long entityId;
	_chatChannel_playerLink_t *previous;
	_chatChannel_playerLink_t *next;
}chatChannel_playerLink_t;

typedef struct
{
	sint8 name[40];
	bool isDefaultChannel;
	uint32 mapContextId;
	uint32 sint32anceId;
	sint32 channelId; // 1 - general, ...
	chatChannel_playerLink_t *firstPlayer; // if null --> no player in chat
}chatChannel_t;

struct
{
	TMutex cs;
	hashTable_t ht_playersByName;
	hashTable_t ht_playersByEntityId; // todo, add support for uint64 hashtable
	hashTable_t ht_channelsBySeed;
}communicator;


void communicator_registerPlayer(mapChannelClient_t *client)
{
	Thread::LockMutex(&communicator.cs);
	Thread::LockMutex(&client->cgm->cs_general);
	sint8 upperCase[256];
	sint8 *from = client->player->actor->name;
	for(sint32 i=0; i<255; i++)
	{
		sint8 c = from[i];
		if( c == '\0' )
		{
			upperCase[i] = '\0';
			break;
		}
		if( c >= 'a' && c <= 'z' )
			c += ('A'-'a');
		upperCase[i] = c;
	}
	upperCase[255] = '\0';
	hashTable_set(&communicator.ht_playersByName, upperCase, (void*)client);
	hashTable_set(&communicator.ht_playersByEntityId, client->clientEntityId, (void*)client);
	Thread::UnlockMutex(&client->cgm->cs_general);
	Thread::UnlockMutex(&communicator.cs);
}

void communicator_unregisterPlayer(mapChannelClient_t *client)
{
	Thread::LockMutex(&communicator.cs);
	Thread::LockMutex(&client->cgm->cs_general);
	sint8 upperCase[256];
	if( client->player )
	{
		sint8 *from = client->player->actor->name;
		for(sint32 i=0; i<255; i++)
		{
			sint8 c = from[i];
			if( c == '\0' )
			{
				upperCase[i] = '\0';
				break;
			}
			if( c >= 'a' && c <= 'z' )
				c += ('A'-'a');
			upperCase[i] = c;
		}
		upperCase[255] = '\0';
		hashTable_set(&communicator.ht_playersByName, upperCase, NULL);
		hashTable_set(&communicator.ht_playersByEntityId, client->clientEntityId, (void*)NULL);
	}
	Thread::UnlockMutex(&client->cgm->cs_general);
	Thread::UnlockMutex(&communicator.cs);
}

mapChannelClient_t *communicator_findPlayerByName(sint8 *name)
{
	Thread::LockMutex(&communicator.cs);
	sint8 upperCase[256];
	for(sint32 i=0; i<255; i++)
	{
		sint8 c = name[i];
		if( c == '\0' )
		{
			upperCase[i] = '\0';
			break;
		}
		if( c >= 'a' && c <= 'z' )
			c += ('A'-'a');
		upperCase[i] = c;
	}
	upperCase[255] = '\0';
	mapChannelClient_t *client = (mapChannelClient_t*)hashTable_get(&communicator.ht_playersByName, upperCase);
	Thread::UnlockMutex(&communicator.cs);
	return client;
}

// called after the player entered the world
void communicator_playerEnterMap(mapChannelClient_t *client)
{
	//pyMarshalString_t pms;

	communicator_joinDefaultLocalChannel(client, 1); // join general
	//// send ChatChannelJoined ( MAP_General )
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, 4); // channelId
	//pym_addInt(&pms, client->mapChannel->mapInfo->contextId); // mapId
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, 8, 10000038, pym_getData(&pms), pym_getLen(&pms));
	//// send ChatChannelJoined ( LOOKING_FOR_GROUP )
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, 3); // channelId
	//pym_addInt(&pms, client->mapChannel->mapInfo->contextId); // mapId
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, 8, 10000038, pym_getData(&pms), pym_getLen(&pms));
	//// send ChatChannelJoined ( MAP_TRADE )
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, 6); // channelId
	//pym_addInt(&pms, client->mapChannel->mapInfo->contextId); // mapId
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, 8, 10000038, pym_getData(&pms), pym_getLen(&pms));
	//// send ChatChannelJoined ( MAP_DEFENSE )
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, 7); // channelId
	//pym_addInt(&pms, client->mapChannel->mapInfo->contextId); // mapId
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, 8, 10000038, pym_getData(&pms), pym_getLen(&pms));
}

// called after a player has been ported to a other map
void communicator_playerChangeMap(mapChannelClient_t *client)
{

}

// called on disconnect or logout/quit (socket most likely already closed)
void communicator_playerExitMap(mapChannelClient_t *client)
{
	// remove client from all channels
	Thread::LockMutex(&communicator.cs);
	for(sint32 i=0; i<client->joinedChannels; i++)
	{
		chatChannel_t *chatChannel = (chatChannel_t*)hashTable_get(&communicator.ht_channelsBySeed, client->channelHashes[i]);
		if( chatChannel )
		{
			// remove client link from channel
			chatChannel_playerLink_t *currentLink = chatChannel->firstPlayer;
			while( currentLink )
			{
				if( currentLink->entityId == client->clientEntityId )
				{
					// do removing
					if( currentLink->previous == NULL )
					{
						chatChannel->firstPlayer = currentLink->next;
						if( currentLink->next )
							currentLink->next->previous = NULL;
						free(currentLink);
					}
					else
					{
						currentLink->previous->next = currentLink->next;
						if( currentLink->next )
							currentLink->next->previous = currentLink->previous;
						free(currentLink);
					}
					break;
				}
				// next
				currentLink = currentLink->next;
			}
		}
	}
	client->joinedChannels = 0;
	Thread::UnlockMutex(&communicator.cs);
}

uint32 _communicator_generateDefaultChannelHash(uint32 channelId, uint32 mapContextId, uint32 instanceId)
{
	uint32 v = 0;
	v = (channelId ^ (channelId<<7)) ^ mapContextId ^ (mapContextId*121) ^ ((instanceId + instanceId*13)<<3);
	return v;
}

void _communicator_addClientToChannel(mapChannelClient_t *client, uint32 cHash)
{
	Thread::LockMutex(&communicator.cs);
	chatChannel_t *chatChannel = (chatChannel_t*)hashTable_get(&communicator.ht_channelsBySeed, cHash);
	if( chatChannel )
	{
		chatChannel_playerLink_t *newLink = (chatChannel_playerLink_t*)malloc(sizeof(chatChannel_playerLink_t));
		newLink->next = NULL;
		newLink->entityId = client->clientEntityId;
		newLink->previous = NULL;
		if( chatChannel->firstPlayer )
		{
			// append
			chatChannel_playerLink_t *currentLink = chatChannel->firstPlayer;
			while( currentLink->next ) currentLink = currentLink->next;
			newLink->previous = currentLink;
			currentLink->next = newLink;
		}
		else
		{
			// set as first
			chatChannel->firstPlayer = newLink;
		}
	}
	Thread::UnlockMutex(&communicator.cs);
}

void communicator_joinDefaultLocalChannel(mapChannelClient_t *client, sint32 channelId)
{
	pyMarshalString_t pms;
	Thread::LockMutex(&communicator.cs);
	// check if we can join channel
	if( client->joinedChannels >= CHANNEL_LIMIT )
	{
		Thread::UnlockMutex(&communicator.cs);
		return; // todo, send error to client
	}
	// generate channel hash
	uint32 cHash = _communicator_generateDefaultChannelHash(channelId, client->mapChannel->mapInfo->contextId, 0);
	// find channel
	chatChannel_t *chatChannel;
	chatChannel = (chatChannel_t*)hashTable_get(&communicator.ht_channelsBySeed, cHash);
	if( chatChannel == 0 )
	{
		// channel does not exist, create it
		chatChannel = (chatChannel_t*)malloc(sizeof(chatChannel_t));
		chatChannel->name[0] = '\0';
		chatChannel->sint32anceId = 0;
		chatChannel->channelId = channelId;
		chatChannel->mapContextId = client->mapChannel->mapInfo->contextId;
		chatChannel->isDefaultChannel = true;
		chatChannel->firstPlayer = NULL;
		// register it
		hashTable_set(&communicator.ht_channelsBySeed, cHash, chatChannel);
	}
	// add channel entry to player
	client->channelHashes[client->joinedChannels] = cHash;
	client->joinedChannels++;
	// add client to channel
#ifdef USE_PTHREADS
	Thread::UnlockMutex(&communicator.cs); // pthread specific
#endif
	_communicator_addClientToChannel(client, cHash);
	Thread::UnlockMutex(&communicator.cs);
	// send ChatChannelJoined
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, channelId); // channelId
	pym_addInt(&pms, client->mapChannel->mapInfo->contextId); // mapId
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 8, 10000038, pym_getData(&pms), pym_getLen(&pms));
}

void communicator_leaveChannel(mapChannelClient_t *client, sint32 channelId)
{
	Thread::LockMutex(&communicator.cs);
	for(sint32 i=0; i<client->joinedChannels; i++)
	{
		chatChannel_t *chatChannel = (chatChannel_t*)hashTable_get(&communicator.ht_channelsBySeed, client->channelHashes[i]);
		if( chatChannel )
		{
			if( chatChannel->channelId == channelId )
			{
				// remove client link from channel
				chatChannel_playerLink_t *currentLink = chatChannel->firstPlayer;
				while( currentLink )
				{
					if( currentLink->entityId == client->clientEntityId )
					{
						// do removing
						if( currentLink->previous == NULL )
						{
							chatChannel->firstPlayer = currentLink->next;
							currentLink->next->previous = NULL;
							free(currentLink);
						}
						else
						{
							currentLink->previous->next = currentLink->next;
							currentLink->next->previous = currentLink->previous;
							free(currentLink);
						}
						break;
					}
					// next
					currentLink = currentLink->next;
				}
				// remove channel link from client
				if( i != (client->joinedChannels-1) )
					client->channelHashes[i] = client->channelHashes[(client->joinedChannels-1)];
				client->joinedChannels--;
				break;
			}
		}
	}
	Thread::UnlockMutex(&communicator.cs);
	__debugbreak(); // todo - send ChannelLeft notify
}

void communicator_loginOk(mapChannel_t *mapChannel, mapChannelClient_t *client)
{
	pyMarshalString_t pms;
	// send LoginOk (despite the original description in the python files, this will only show 'You have arrived at ....' msg in chat)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	sint8 playerName[1024];
	wsprintf(playerName, "%s %s", client->player->actor->name, client->player->actor->family);
	pym_addUnicode(&pms, playerName);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 8, 105, pym_getData(&pms), pym_getLen(&pms));

	// send MOTD ( Recv_SendMOTD - receives MOTDDict {languageId: text} )
	// SendMOTD = 770		// Displayed only if different
	// PreviewMOTD = 769	// Displayed always
	pym_init(&pms);
	pym_tuple_begin(&pms);
	//pym_dict_begin(&pms);
	//pym_addInt(&pms, 1);
	char* greeting = "Welcome to the Salsa Crew server.\nBuild date: ";
	char buffer[100];
	sprintf(buffer, "%s %s", greeting, __DATE__);
	pym_addUnicode(&pms, buffer); 
	//pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 8, 769, pym_getData(&pms), pym_getLen(&pms));
}

void communicator_systemMessage(mapChannelClient_t *client, sint8 *message)
{
	pyMarshalString_t pms;
	// send SystemMessage
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, message);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 8, 553, pym_getData(&pms), pym_getLen(&pms));
}

bool communicator_parseCommand(mapChannelClient_t *cm, sint8 *textMsg)
{
	pyMarshalString_t pms;
	

	if( textMsg[0] == '.' )
	{
		if( gm_parseGmCommands(cm, textMsg) == true )
			return true;
	}

	//if( memcmp(textMsg,".pathnode ",10) == 0)
	//{
 //        
	//	//__debugbreak();
	//	//---saves pathnode(s) for a  certain spawntype
	//	/*sint8 cmd[11];
	//	sint32 spawnid,pathindex;
	//	sscanf(textMsg,"%s %d %d",cmd,&spawnid,&pathindex);
	//		
	//	di_pathNodeDataW2_t pNode = {0};
	//	pNode.currentContextId = cm->mapChannel->mapInfo->contextId;
	//	pNode.spawntype = spawnid;
	//	pNode.pathindex = pathindex;
	//	pNode.posX = cm->player->actor->posX;
	//	pNode.posY = (cm->player->actor->posY)+0.1f;
	//	pNode.posZ = cm->player->actor->posZ;
	//	DataInterface_PathNode_setPathnode(&pNode,NULL,NULL);
	//	*/
	//	communicator_systemMessage(cm, "pathnodes deactivated");
	//	return true;
	//}
	if( memcmp(textMsg,".hurtme",7) == 0)
	{
       cm->player->actor->stats.healthCurrent /= 2;
	   pyMarshalString_t pms;
	   pym_init(&pms);
	   pym_tuple_begin(&pms);
	   pym_addInt(&pms, cm->player->actor->stats.healthCurrent); // current
	   pym_addInt(&pms, cm->player->actor->stats.healthCurrentMax); // currentMax
	   pym_addFloat(&pms, cm->player->actor->stats.regenHealthPerSecond); // refreshAmount
	   pym_addInt(&pms, 0); // whoId
	   pym_tuple_end(&pms);
	   netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel,
		   cm->player->actor, 
		   cm->player->actor->entityId, METHODID_UPDATEHEALTH, 
		   pym_getData(&pms), pym_getLen(&pms));
		return true;
	}
    /*if( memcmp(textMsg,".gameobject ",12) == 0 )
	{
		 sint8 cmd[12];
		 uint32 type;
		 uint32 entityClassId;
		 sscanf(textMsg,"%s %d %d",cmd,&entityClassId,&type);

		 di_teleporterData worldObject = {0};
		 worldObject.bx = 2.1f;
		 worldObject.bz = 2.1f;
		 worldObject.sx = cm->player->actor->posX;
		 worldObject.sy = cm->player->actor->posY;
		 worldObject.sz = cm->player->actor->posZ;
		 worldObject.modelid = entityClassId;
		 worldObject.type = type;
		
		 DataInterface_teleporter_updateList(&worldObject,NULL,NULL);
         
		 sprintf(textMsg,"Gameobject placed: %f %f %f",worldObject.sx,worldObject.sy,worldObject.sz);
		 communicator_systemMessage(cm, textMsg);
		 return true;
	}*/
	if( memcmp(textMsg,".waypoint ",10) == 0 )
	{
		sint8 cmd[16];
		uint32 nameId;
		if( sscanf(textMsg,"%s %d",cmd,&nameId) == 2 )
		{
			di_teleporterData worldObject = {0};
			worldObject.bx = 2.1f;
			worldObject.bz = 2.1f;
			worldObject.sx = cm->player->actor->posX;
			worldObject.sy = cm->player->actor->posY;
			worldObject.sz = cm->player->actor->posZ;
			worldObject.nameId = nameId;
			worldObject.type = OBJECTTYPE_WAYPOINT;
			DataInterface_teleporter_updateList(&worldObject,NULL,NULL);
			// todo: Get dataset ID from DB and spawn a new waypoint right here
			sprintf(textMsg,"Waypoint placed: %f %f %f",worldObject.sx,worldObject.sy,worldObject.sz);
			communicator_systemMessage(cm, textMsg);
			communicator_systemMessage(cm, "Restart server for this to take effect");
		}
		return true;
	}
	if( memcmp(textMsg, ".addspawn ", 10) == 0 )
	{
		// parse input
		// mode, animType, respawnTime, creatureType1, creatureMin1, creatureMax1, ...
		diData_spawnEntry_t spawnEntry = {0};
		sint32 parse_mode = 0;
		sint32 parse_animType = 0;
		sint32 parse_respawnTimer = 0;
		//float parse_posX = 0.0f;
		//float parse_posY = 0.0f;
		//float parse_posZ = 0.0f;
		sint32 creatureType[6] = {0};
		sint32 creatureMin[6] = {0};
		sint32 creatureMax[6] = {0};

		sint32 numberOfScannedVars = sscanf(textMsg+10, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &parse_mode, &parse_animType, &parse_respawnTimer, &creatureType[0], &creatureMin[0], &creatureMax[0], &creatureType[1], &creatureMin[1], &creatureMax[1], &creatureType[2], &creatureMin[2], &creatureMax[2], &creatureType[3], &creatureMin[3], &creatureMax[3], &creatureType[4], &creatureMin[4], &creatureMax[4], &creatureType[5], &creatureMin[5], &creatureMax[5]);
		if( numberOfScannedVars < 6 )
		{
			communicator_systemMessage(cm, "Invalid parameters");
			communicator_systemMessage(cm, "Format: mode animType respawnTime creatureType1 creatureMin1 creatureMax1 ...");
			return true;
		}
		spawnEntry.mode = parse_mode;
		spawnEntry.animType = parse_animType;
		spawnEntry.respawnTime = parse_respawnTimer;
		spawnEntry.posX = cm->player->actor->posX;
		spawnEntry.posY = cm->player->actor->posY + 0.2f; // slightly increase height to not spawn creatures in the ground
		spawnEntry.posZ = cm->player->actor->posZ;
		spawnEntry.contextId = cm->mapChannel->mapInfo->contextId;
		for(sint32 c=0; c<6; c++)
		{
			spawnEntry.spawnSlot[c].creatureType = creatureType[c];
			spawnEntry.spawnSlot[c].countMin = creatureMin[c];
			spawnEntry.spawnSlot[c].countMax = creatureMax[c];
			if( spawnEntry.spawnSlot[c].countMin > spawnEntry.spawnSlot[c].countMax )
			{
				communicator_systemMessage(cm, "Spawn count min must not be greater than max");
				return true;
			}
		}
		// create spawn pool
		spawnPool_t *spawnPool = (spawnPool_t*)malloc(sizeof(spawnPool_t));
		memset(spawnPool, 0x00, sizeof(spawnPool_t));
		spawnPool->locationCount = 1; // eventually add support for multiple spawn locations
		spawnPool->locationList = (spawnLocation_t*)malloc(sizeof(spawnLocation_t) * spawnPool->locationCount);
		spawnPool->locationList[0].x = spawnEntry.posX;
		spawnPool->locationList[0].y = spawnEntry.posY;
		spawnPool->locationList[0].z = spawnEntry.posZ;
		spawnPool->animType = spawnEntry.animType;
		spawnPool->mode = spawnEntry.mode;
		for(sint32 spawnSlot=0; spawnSlot<SPAWNPOOL_SPAWNSLOTS; spawnSlot++)
		{
			spawnPool->spawnSlot[spawnSlot].creatureType = spawnEntry.spawnSlot[spawnSlot].creatureType;
			spawnPool->spawnSlot[spawnSlot].countMin = spawnEntry.spawnSlot[spawnSlot].countMin;
			spawnPool->spawnSlot[spawnSlot].countMax = spawnEntry.spawnSlot[spawnSlot].countMax;
		}
		spawnPool_activate(cm->mapChannel, spawnPool);
		sint32 spawnpoolId = DataInterface_SpawnSystem_addSpawnPoint(&spawnEntry);
		char textMsg[128];
		wsprintf(textMsg, "Added spawnpool with id %d", spawnpoolId);
		communicator_systemMessage(cm, textMsg);
		if( cm->player->gmData )
			cm->player->gmData->lastCreatedSpawnpool = spawnpoolId;
		return true;
	}
	if( memcmp(textMsg,".alwaysfriendly",15) == 0 )
	{
		cm->gmFlagAlwaysFriendly = !cm->gmFlagAlwaysFriendly;
		if( cm->gmFlagAlwaysFriendly )
			communicator_systemMessage(cm, "Enemy creatures will now NOT attack you on sight");
		else
			communicator_systemMessage(cm, "Enemy creatures will now attack you on sight");
		return true;
	}


	//if( memcmp(textMsg,".spawner ",9) == 0 )
	//{
	//	//communicator_systemMessage(cm, "spawner deactivated");
	//	sint8 cmd[9];
	//	sint32 spawntype; //---which type of spawn to place( look db for id)
	//	sint32 pwd;
	//	sscanf(textMsg,"%s %d %d",cmd,&spawntype,&pwd);	
	//	
	//	if(pwd != 2104 )
	//	{
	//		sprintf(textMsg, "unvalid password");
	//		communicator_systemMessage(cm, textMsg);
	//		return true;
	//	}

	//	//---todo: make a db check
	//	if(spawntype <= 0 || spawntype >= 200 )
	//	{
	//		sprintf(textMsg, "unvalid spawntype id");
	//		communicator_systemMessage(cm, textMsg);
	//		return true;
	//	}

	//	di_spawnDataW2_t spawndata = {0};
	//	spawndata.currentContextId = cm->mapChannel->mapInfo->contextId;
	//	spawndata.posX = cm->player->actor->posX;
	//	spawndata.posY = cm->player->actor->posY+0.5f;
	//	spawndata.posZ = cm->player->actor->posZ;
	//	spawndata.spawntype = spawntype;	
	//	DataInterface_Spawnpool_updateSpawnW2(&spawndata,NULL,NULL);

	//	sprintf(textMsg, "added spawnlocation: %f %f %f of spawntype-id: %d", 
	//					cm->player->actor->posX, 
	//		            cm->player->actor->posY, 
	//					cm->player->actor->posZ,spawntype);
	//	communicator_systemMessage(cm, textMsg);
	//	return true;
	//}
	if( memcmp(textMsg,".catchme",8) == 0 )
	{
		// make all creatures in range run to the player
		sint32 minX = (sint32)(((cm->player->actor->posX-256.0f) / CELL_SIZE) + CELL_BIAS);
		sint32 minZ = (sint32)(((cm->player->actor->posZ-256.0f) / CELL_SIZE) + CELL_BIAS);
		sint32 maxX = (sint32)(((cm->player->actor->posX+256.0f+(CELL_SIZE-0.0001f)) / CELL_SIZE) + CELL_BIAS);
		sint32 maxZ = (sint32)(((cm->player->actor->posZ+256.0f+(CELL_SIZE-0.0001f)) / CELL_SIZE) + CELL_BIAS);
		// check all cells for creatures
		for(sint32 ix=minX; ix<=maxX; ix++)
		{
			for(sint32 iz=minZ; iz<=maxZ; iz++)
			{
				mapCell_t *nMapCell = cellMgr_getCell(cm->mapChannel, ix, iz);
				if( nMapCell )
				{
					if( nMapCell->ht_creatureList.empty() )
						continue;
					std::vector<creature_t*>::iterator itr = nMapCell->ht_creatureList.begin();
					while (itr != nMapCell->ht_creatureList.end())
					{
						creature_t *creature = itr[0];
						++itr;
						// update combat/wanter state and movement
						creature->controller.pathLength	= 0;
						creature->controller.pathIndex = 0;
						creature->controller.actionWander.wanderDestination[0] = cm->player->actor->posX;
						creature->controller.actionWander.wanderDestination[1] = cm->player->actor->posY;
						creature->controller.actionWander.wanderDestination[2] = cm->player->actor->posZ;
						creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
						creature->controller.actionWander.state = 0;
					}			
				}
			}
		}
	}
	if( memcmp(textMsg, ".givexp ", 8) == 0 )
	{
		sint8 *pch = textMsg + 8;
		sint32 xp = atoi(pch);
		if( xp > 0 )
			manifestation_GainExperience(cm, xp);
		return true;
	}
	if( memcmp(textMsg,".animtest ",10) == 0 )
	{
		    //__debugbreak();
		    creature_t *npc = (creature_t*)entityMgr_get(cm->player->targetEntityId);
			if( !npc )
				return true; 
			// various actions/animations
			sint32 action;
			sint8 cmd[40];
			sscanf(textMsg,"%s %d",cmd,&action);
			if(action <= 0 || action == NULL )
			{
				sprintf(textMsg, "invalid action");
				return true;
			}
			/*pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_list_begin(&pms);
			pym_addInt(&pms, action); // dead
			pym_list_end(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel, &npc->actor, npc->actor.entityId, 206, pym_getData(&pms), pym_getLen(&pms));
			*/

			//pyMarshalString_t pms;
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, action);
			pym_addInt(&pms, 10000);
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(cm->cgm, npc->actor.entityId, 454, pym_getData(&pms), pym_getLen(&pms));
		
		    return true;
	}
	if( strcmp(textMsg, ".savedb") == 0 )
	{ 
		/*if( cm->player->targetEntityId == 0 )
		{
			communicator_systemMessage(cm, "No entity selected");
			return true;
		}
		
		    creature_t *npc = (creature_t*)entityMgr_get(cm->player->targetEntityId);
			if( !npc )
				return true;
						

			di_entityDataW_t entityData = {0};
			entityData.npcID = npc->actor.entityId;
			entityData.posX = npc->actor.posX;
			entityData.posY = npc->actor.posY;
			entityData.posZ = cm->player->actor->posZ; //npc actor havent z value
			entityData.rotation = npc->actor.rotation;
			entityData.entityClassID = npc->type->entityClassId;
			entityData.currentContextId = cm->mapChannel->mapInfo->contextId;
			strcpy(entityData.unicodeName, npc->actor.name);
			
			
			for(sint32 i=0; i<SWAPSET_SIZE; i++)
			{
				entityData.appearanceData[i].classId = npc->actor.appearanceData[i].classId;
				entityData.appearanceData[i].hue = npc->actor.appearanceData[i].hue;
			}
			
			DataInterface_Entity_updateEntityW(&entityData,NULL,NULL);

			sprintf(textMsg, "Player Location(x y z): %f %f %f \n Entity Location(x y z): %f %f %f \n Entityid: %i Classid: %i Nameid: %i \n", 
													cm->player->actor->posX, 
			                                        cm->player->actor->posY, 
													cm->player->actor->posZ,
													npc->actor.posX,
													npc->actor.posY,
													cm->player->actor->posZ,
													npc->actor.entityId,
													npc->type->entityClassId,
													npc->type->nameId);
           
		    communicator_systemMessage(cm, textMsg);
			
			*/
		return true;
	}
	if( strcmp(textMsg, "_test1") == 0 )
	{
		netMgr_entityMovementTest(cm->cgm, 0, 0);
		return true;
	}
	if( strcmp(textMsg, ".wipe") == 0 )
	{
		// 20110803
		// get current target of player
		if( cm->player->targetEntityId == 0 )
		{
			communicator_systemMessage(cm, "No entity selected");
			return true;
		}
		if( entityMgr_getEntityType(cm->player->targetEntityId) == ENTITYTYPE_CREATURE )
		{
			creature_t *creature = (creature_t*)entityMgr_get(cm->player->targetEntityId);
			if( !creature )
				return true;

			// destroy it through 
			//#define METHODID_DESTROYPHYSICALENTITY 56
			creature_cellDiscardCreatureToClients(cm->mapChannel, creature, &cm->player->controllerUser,1);
		}
		return true;
	}
	if( strcmp(textMsg, ".rqs") == 0 )
	{
		// DevRQSWindow
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, 5, 831, pym_getData(&pms), pym_getLen(&pms));	
		return true;
	}
	if( strcmp(textMsg, ".makemedie") == 0 )
	{
		cm->player->actor->stats.healthCurrent = 0;
		cm->player->actor->state = ACTOR_STATE_DEAD;
		// Recv_MadeDead
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel, 
				                                cm->player->actor, 
												cm->player->actor->entityId, 
												629, 
												pym_getData(&pms), pym_getLen(&pms));
		communicator_systemMessage(cm, "Killed!");
		return true;
	}
	if( strcmp(textMsg, ".reviveme") == 0 )
	{
		cm->player->actor->stats.healthCurrent = cm->player->actor->stats.healthCurrentMax;
		cm->player->actor->state = ACTOR_STATE_ALIVE;
		//Recv_Revived(sourceId) -> None
		//Notification that we are being revived, and that 'sourceId' helped us out.
		// This must be called before graveyard respawn
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 0);
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel, 
				                                cm->player->actor, 
												cm->player->actor->entityId, 
												METHODID_REVIVED, 
												pym_getData(&pms), pym_getLen(&pms));
		communicator_systemMessage(cm, "Revived!");
		return true;
	}
	if( memcmp(textMsg, ".creature ", 10) == 0 )
	{
		//20110728 - thuvvik complete "creature dictionary" invocation ... cf creature.cpp line 289
		sint8 *pch = textMsg + 10;

		sint32 creatureClass;
		sint8 cmd[30];
		sscanf(textMsg,"%s %d",cmd,&creatureClass);			
		creatureType_t* ctype = (creatureType_t*)malloc(sizeof(creatureType_t));
		memset(ctype, 0x00, sizeof(creatureType_t));
		ctype->maxHealth = 150;
		ctype->nameId = 0;
		ctype->entityClassId = creatureClass;
		strcpy(ctype->name, "test npc");
		// since this is just a test we create an new creature type (which should normally only be done on init)
		//creatureType_createCreatureType(creatureClass, 0);

		creature_t *creature = creature_createCreature(cm->mapChannel, ctype, NULL);
		creature_setLocation(creature, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ, 0.0f, 0.0f);
		cellMgr_addToWorld(cm->mapChannel, creature);
		//communicator_systemMessage(cm, "creature spawner deactivated");
		return true;
	}
	//if( memcmp(textMsg, ".effect ", 8) == 0 )
	//{
	//	//20110728 - thuvvik complete "creature dictionary" invocation ... cf creature.cpp line 289
	//	sint8 *pch = textMsg + 8;
	//	gameEffect_attach(cm->mapChannel, cm->player->actor, atoi(pch), 1);
	//	return true;
	//}
	//if( strcmp(textMsg, ".bane") == 0 )
	//{
	//	dynamicObject_createBaneDropship(cm->mapChannel, cm->player->actor->posX, cm->player->actor->posY+6.0f, cm->player->actor->posZ);
	//	return true;
	//}
	if( strcmp(textMsg, ".ccp") == 0 )
	{
		controlpoint_create(cm->mapChannel, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ, 0.0f);
		communicator_systemMessage(cm, "ControlPoint spawned!");
		return true;
	}
	if( memcmp(textMsg, ".state ", 7) == 0 )
	{
		uint32 object;
		sint32 state;
		sscanf(textMsg,"%*s %u %i", &object, &state);
		dynamicObject_forceState(cm->cgm, object, state);
		return true;
	}
	if (strcmp(textMsg, ".logosmarker") == 0)
	{
		dynamicObject_createLogosObject(cm->mapChannel, cm->player->actor->posX+2, cm->player->actor->posY+1, cm->player->actor->posZ+2);
		communicator_systemMessage(cm, "Power Logos spawned!");
		return true;
	}
	/*if( strcmp(textMsg, ".logos") == 0 )
	{
		// LogosStoneAdded = 475
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 23); // power
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 475, pym_getData(&pms), pym_getLen(&pms));
		communicator_systemMessage(cm, "Power Logos added");
		return true;
	}*/
	if( strcmp(textMsg, ".gm") == 0 )
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, 5, 366, pym_getData(&pms), pym_getLen(&pms));
		communicator_systemMessage(cm, "GM Mode enabled!");
		return true;
	}
	if( strcmp(textMsg, ".save") == 0 )
	{
		if( cm->player->targetEntityId == 0 )
		{
			communicator_systemMessage(cm, "No entity selected");
			return true;
		}
		//if( entityMgr_getEntityType(cm->player->targetEntityId) == ENTITYTYPE_NPC )
		//{
		//	npc_t *npc = (npc_t*)entityMgr_get(cm->player->targetEntityId);
		//	if( !npc )
		//		return true;
		//	communicator_systemMessage(cm, "Saving npc...");
		//	di_npcData_t npcData = {0};
		//	npcData.npcID = npc->entityId;
		//	npcData.posX = npc->actor.posX;
		//	npcData.posY = npc->actor.posY;
		//	npcData.posZ = npc->actor.posZ;
		//	npcData.entityClassID = npc->actor.entityClassId;
		//	npcData.currentContextId = cm->mapChannel->mapInfo->contextId;
		//	strcpy(npcData.unicodeName, npc->actor.name);
		//	for(sint32 i=0; i<SWAPSET_SIZE; i++)
		//	{
		//		npcData.appearanceData[i].classId = npc->actor.appearanceData[i].classId;
		//		npcData.appearanceData[i].hue = npc->actor.appearanceData[i].hue;
		//	}
		//	DataInterface_NPC_updateNPC(&npcData, NULL, NULL);
		//	return true;
		//}
		//else
		communicator_systemMessage(cm, "Cannot save selected entity");
		return true;
	}
	if( strcmp(textMsg, ".traffic") == 0 )
	{
		wsprintf(textMsg, "S>C %d bytes (%dKB / %dMB)\n", cm->cgm->state_bytesSend, cm->cgm->state_bytesSend/1024, cm->cgm->state_bytesSend/1024/1024);
		communicator_systemMessage(cm, textMsg);
		return true;
	}
	if( strcmp(textMsg, ".where") == 0 )
	{
		sprintf(textMsg, "Location: %f %f %f\n", cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ);
		communicator_systemMessage(cm, textMsg);
		return true;
	}
	//if( strcmp(textMsg, ".footlocker") == 0 )
	//{
	//	dynamicObject_developer_createFootlocker(cm->mapChannel, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ);
	//	communicator_systemMessage(cm, "Spawned a footlocker!");
	//	return true;
	//}
	if( memcmp(textMsg, ".setregion ", 11) == 0 )
	{
		sint32 regionId;
		sscanf(textMsg,"%*s %i", &regionId);
		// Recv_UpdateRegions (568)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_list_begin(&pms);
		pym_addInt(&pms, regionId);
		pym_list_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 568, pym_getData(&pms), pym_getLen(&pms));
	}
	//if( memcmp(textMsg, ".name ", 6) == 0 )
	//{
	//	sint8 *pch = textMsg + 6;
	//	//pch = strtok (textMsg+6, " ");
	//	//if( pch )
	//	if( *pch )
	//	{
	//		if( entityMgr_getEntityType(cm->player->targetEntityId) == ENTITYTYPE_NPC )
	//		{
	//			npc_t *npc = (npc_t*)entityMgr_get(cm->player->targetEntityId);
	//			if( !npc )
	//				return true;
	//			npc_updateName(cm->mapChannel, npc, pch);
	//		}
	//		else
	//			communicator_systemMessage(cm, "Cannot set name of selection");
	//	}
	//	else
	//	{
	//		communicator_systemMessage(cm, "Wrong syntax");
	//	}
	//	return true;
	//}
	if( memcmp(textMsg, ".look ", 6) == 0 )
	{
		sint8 *pch = textMsg + 6;
		pch = strtok (textMsg+6, " ");
		if( pch )
		{
			sint32 itemClassId = 0;
			uint32 hue = 0x80808080;
			sscanf(pch, "%d", &itemClassId);
			pch = strtok (NULL, " ");
			if( pch )
			{
				if( *pch == '0' && *(pch+1) == 'x' )
					pch += 2;
				sscanf(pch, "%X", &hue);
			}
			/*if( entityMgr_getEntityType(cm->player->targetEntityId) == ENTITYTYPE_NPC )
			{
				npc_t *npc = (npc_t*)entityMgr_get(cm->player->targetEntityId);
				if( !npc )
					return true;
				npc_updateAppearanceItem(cm->mapChannel, npc, itemClassId, hue);
			}
			else
				communicator_systemMessage(cm, "Cannot set appearance of selection");*/
			communicator_systemMessage(cm, ".look needs to be updated :(");
		}
		else
		{
			communicator_systemMessage(cm, "Wrong syntax");
		}
		return true;
	}
	if( memcmp(textMsg, ".speed", 6) == 0 )
	{
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addFloat(&pms, 7.0f); // 7x run speed!
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel, cm->player->actor, cm->player->actor->entityId, 580, pym_getData(&pms), pym_getLen(&pms));	
	}
	if( memcmp(textMsg, ".goto ", 6) == 0 )
	{
		sint8 *pch = textMsg + 6;
		pch = strtok (textMsg+6, " ");
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		if( pch )
		{
			sscanf(pch, "%f", &posX);
			pch = strtok (NULL, " ");
			if( pch )
			{
				sscanf(pch, "%f", &posY);
				pch = strtok(NULL, " ");
				if( pch )
				{
					sscanf(pch, "%f", &posZ);
					// do teleport
					netCompressedMovement_t netMovement = {0};
					cm->player->actor->posX = posX;
					cm->player->actor->posY = posY+0.5f;
					cm->player->actor->posZ = posZ;
					netMovement.entityId = cm->player->actor->entityId;
					netMovement.posX24b = cm->player->actor->posX * 256.0f;
					netMovement.posY24b = cm->player->actor->posY * 256.0f;
					netMovement.posZ24b = cm->player->actor->posZ * 256.0f;
					netMgr_sendEntityMovement(cm->cgm, &netMovement);
				}
			}
		}
		return true;
	}
	if( memcmp(textMsg, ".obj ", 5) == 0 )
	{
		sint8 *pch = textMsg + 5;
		if( *pch )
		{
			sint32 classId = 0;
			sscanf(pch, "%d", &classId);
			pyMarshalString_t pms;
			// create object entity
			pym_init(&pms);
			pym_tuple_begin(&pms);
			sint64 randomEntityId = 100000 + rand()*6500 + rand();
			pym_addInt(&pms, randomEntityId); // entityID
			pym_addInt(&pms, classId); // classID
			pym_addNoneStruct(&pms); // entityData (dunno)
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(cm->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
			// set position
			pym_init(&pms);
			pym_tuple_begin(&pms);
			// position
			pym_tuple_begin(&pms);
			pym_addFloat(&pms, cm->player->actor->posX);	// x
			pym_addFloat(&pms, cm->player->actor->posY+1.0f);	// y
			pym_addFloat(&pms, cm->player->actor->posZ);	// z
			pym_tuple_end(&pms); 
			// rotation quaterninion
			//float qOut[4];

			pym_tuple_begin(&pms);
			pym_addFloat(&pms, 0.0f);
			pym_addFloat(&pms, 0.0f);
			pym_addFloat(&pms, 0.0f);
			pym_addFloat(&pms, 1.0f);
			pym_tuple_end(&pms);
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(cm->cgm, randomEntityId, 243, pym_getData(&pms), pym_getLen(&pms));	
			communicator_systemMessage(cm, "Created object");
		}
		else
		{
			communicator_systemMessage(cm, "Wrong syntax");
		}
		return true;
	}
	if(  memcmp(textMsg,".teleport ",10) == 0 )
	{
		sint8 cmd[10];
	    //sint8 *cmdpos = textMsg + 7;

		struct tloc
		{
			sint32 x;
			sint32 y;
			sint32 z;
			float rotation; //not necessary, could add
			sint32 mapContextId;
		};

		tloc telepos = {0};
	    communicator_systemMessage(cm, "Teleporting...");
		sscanf(textMsg,"%s %d %d %d %d",cmd,&telepos.x,&telepos.y,&telepos.z,&telepos.mapContextId);	


		cm->player->actor->posX = telepos.x; 
		cm->player->actor->posY = telepos.y;
		cm->player->actor->posZ = telepos.z;
		cellMgr_addToWorld(cm); // will introduce the player to all clients, including the current owner

		// Recv_WorldLocationDescriptor (243)
		/*pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_tuple_begin(&pms); // position
		pym_addInt(&pms, telepos.x); // x 
		pym_addInt(&pms, telepos.y); // y 
		pym_addInt(&pms, telepos.z); // z 
		pym_tuple_end(&pms); 
		pym_tuple_begin(&pms); // rotation quaterninion
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 1.0f);
		pym_tuple_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 243, pym_getData(&pms), pym_getLen(&pms));
		*/
		return true;
	}
	// 20110827 @dennton

	if(  memcmp(textMsg,".tport ",7) == 0 )
	{
		
		sint8 cmd[7];
	    //sint8 *cmdpos = textMsg + 7;

		struct tloc
		{
			sint32 x;
			sint32 y;
			sint32 z;
			float rotation; //not necessary, could add
			sint32 mapContextId;
		};

		tloc telepos = {0};
	    communicator_systemMessage(cm, "Teleporting...");
		sscanf(textMsg,"%s %d %d %d %d",cmd,&telepos.x,&telepos.y,&telepos.z,&telepos.mapContextId);			
		
		// #################### notify telport ##################
		//---teleport within same map
		if(cm->mapChannel->mapInfo->contextId == telepos.mapContextId)
		{	
		

		// Recv_WorldLocationDescriptor (243)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_tuple_begin(&pms); // position
		pym_addInt(&pms, telepos.x); // x 
		pym_addInt(&pms, telepos.y); // y 
		pym_addInt(&pms, telepos.z); // z 
		pym_tuple_end(&pms); 
		pym_tuple_begin(&pms); // rotation quaterninion
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 1.0f);
		pym_tuple_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 244, pym_getData(&pms), pym_getLen(&pms));

		//cm->player->actor->posX = telepos.x; 
		//cm->player->actor->posY = telepos.y;
		//cm->player->actor->posZ = telepos.z;
	    communicator_systemMessage(cm, "arrived same map");
	    return true;
		}
        //---teleport to other map
		if(cm->mapChannel->mapInfo->contextId != telepos.mapContextId)
		{
            
			//remove entity from old map
			//cm->removeFromMap = true;
	        //remove client from all channels
			communicator_playerExitMap(cm);
			//unregister player
			//communicator_unregisterPlayer(cm);
			//remove visible entity
			Thread::LockMutex(&cm->cgm->cs_general);
			cellMgr_removeFromWorld(cm);
			// remove from list
			for(sint32 i=0; i<cm->mapChannel->playerCount; i++)
			{
				if( cm == cm->mapChannel->playerList[i] )
				{
					if( i == cm->mapChannel->playerCount-1 )
					{
						cm->mapChannel->playerCount--;
					}
					else
					{
						cm->mapChannel->playerList[i] = cm->mapChannel->playerList[cm->mapChannel->playerCount-1];
						cm->mapChannel->playerCount--;
					}
					break;
				}
			}
			Thread::UnlockMutex(&cm->cgm->cs_general);
			//entityMgr_unregisterEntity(cm->player->actor->entityId);
			
			//cm->cgm->mapLoadSlotId = cm->tempCharacterData->slotIndex;
			//############## map loading stuff ##############
			// send PreWonkavate (clientMethod.134)
			pyMarshalString_t pms;
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, 0); // wonkType - actually not used by the game
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(cm->cgm, 5, 134, pym_getData(&pms), pym_getLen(&pms));
			// send Wonkavate (inputstateRouter.242)
			cm->cgm->mapLoadContextId = telepos.mapContextId;
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, telepos.mapContextId);	// gameContextId (alias mapId)
			pym_addInt(&pms, 0);	// instanceId ( not important for now )
			// find map version
			sint32 mapVersion = 0; // default = 0;
			for(sint32 i=0; i<mapInfoCount; i++)
			{
				if( mapInfoArray[i].contextId == telepos.mapContextId )
				{
					mapVersion = mapInfoArray[i].version;
					break;
				}
			}
			pym_addInt(&pms, mapVersion);	// templateVersion ( from the map file? )
			pym_tuple_begin(&pms);  // startPosition
			pym_addFloat(&pms, telepos.x); // x (todo: send as float)
			pym_addFloat(&pms, telepos.y); // y (todo: send as float)
			pym_addFloat(&pms, telepos.z); // z (todo: send as float)
			pym_tuple_end(&pms);
			pym_addInt(&pms, 0);	// startRotation (todo, read from db and send as float)
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(cm->cgm, 6, METHODID_WONKAVATE, pym_getData(&pms), pym_getLen(&pms));
			//wonkavate = buggy?
			//cm->cgm->State = GAMEMAIN_STATE_RELIEVED;
			//################## player assigning ###############
			//communicator_registerPlayer(cm);
			communicator_loginOk(cm->mapChannel, cm);
			communicator_playerEnterMap(cm);
			//add entity to new map
			cm->player->actor->posX = telepos.x; 
			cm->player->actor->posY = telepos.y;
			cm->player->actor->posZ = telepos.z;
			//cm->mapChannel->mapInfo->contextId = telepos.mapContextId;
		  
			
			cm->player->controllerUser->inventory = cm->inventory;
			cm->player->controllerUser->mission = cm->mission;
			cm->tempCharacterData = cm->player->controllerUser->tempCharacterData;

				
			//---search new mapchannel
			for(sint32 chan=0; chan < global_channelList->mapChannelCount; chan++)
			{
               mapChannel_t *mapChannel = global_channelList->mapChannelArray+chan;
			   if(mapChannel->mapInfo->contextId == telepos.mapContextId)
			   {
				   cm->mapChannel = mapChannel;
				   break;
			   }
			}

			mapChannel_t *mapChannel = cm->mapChannel;
			Thread::LockMutex(&cm->mapChannel->criticalSection);
			mapChannel->playerList[mapChannel->playerCount] = cm;
			mapChannel->playerCount++;
			hashTable_set(&mapChannel->ht_socketToClient, (uint32)cm->cgm->socket, cm);
			Thread::UnlockMutex(&mapChannel->criticalSection);
			
			cellMgr_addToWorld(cm); //cellsint32roducing to player /from players
			// setCurrentContextId (clientMethod.362)
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, cm->mapChannel->mapInfo->contextId);
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(cm->cgm, 5, 362, pym_getData(&pms), pym_getLen(&pms));
			
            
		}

		communicator_systemMessage(cm, "arrived");

		return true;
	}

	// void npc_updateAppearanceItem(mapChannel_t *mapChannel, npc_t *npc, uint32 itemClassId, uint32 hue)

	return false;
}


void communicator_recv_radialChat(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint8 textMsg[256];
	pym_unpackUnicode(&pums, textMsg, 256);
	if( pums.unpackErrorEncountered )
		return;
	//__debugbreak();
	// chat commands
	if( communicator_parseCommand(cm, textMsg) )
		return;






	if( cm->player == NULL )
		return;
	// build radial chat msg packet
	pyMarshalString_t pms;
	//sint8 playerName[256];
	//wsprintf(playerName, "%s %s", cm->player->actor->name, cm->cgm->Accountname);
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, cm->player->actor->family); // like in real game, only family name is displayed
	pym_addUnicode(&pms, textMsg);
	pym_addInt(&pms, cm->player->actor->entityId);
	pym_tuple_end(&pms);

	/*
	STR: 'sender' (2E 01 00 00),
	00006DA5                         STR: 'msg' (49 00 00 00),
	00006DAA                         STR: 'senderEntityId' (2C 01 00 00),
	*/

	// go through all players and send chat message ( can ignore sync because playerList will not change )
	double senderX = cm->player->actor->posX;
	double senderY = cm->player->actor->posY;
	double senderZ = cm->player->actor->posZ;
	mapChannel_t *mapChannel = cm->mapChannel;
	for(sint32 i=0; i<mapChannel->playerCount; i++)
	{
		mapChannelClient_t *client = mapChannel->playerList[i];
		if( client->player )
		{
			// calulate distance
			double distX = client->player->actor->posX - senderX;
			double distY = client->player->actor->posY - senderY;
			double distZ = client->player->actor->posZ - senderZ;
			double distance = sqrt(distX*distX+distY*distY+distZ*distZ);
			if( distance <= 70.0 ) // 70 is about the range the client is visible
				netMgr_pythonAddMethodCallRaw(client->cgm, 8, 136, pym_getData(&pms), pym_getLen(&pms));
		}
	}
}

void communicator_recv_shout(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint8 textMsg[256];
	pym_unpackUnicode(&pums, textMsg, 256);
	if( pums.unpackErrorEncountered )
		return;
	if( cm->player == NULL )
		return;
	// build shout chat msg packet
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, cm->player->actor->name);
	pym_addUnicode(&pms, textMsg);
	pym_tuple_end(&pms);

	netMgr_pythonAddMethodCallRaw(cm->mapChannel, 8, 204, pym_getData(&pms), pym_getLen(&pms));
}


void communicator_recv_channelChat(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyMarshalString_t pms;
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	// 11 11 00
	unsigned long long target = pym_unpackLongLong(&pums);
	unsigned long long mapEntityId = pym_unpackLongLong(&pums);
	unsigned long long gameContextId = 0;
	if( pym_unpack_isNoneStruct(&pums) )
		pym_unpackNoneStruct(&pums);
	else
		gameContextId = pym_unpackLongLong(&pums);
	sint8 textMsg[512];
	pym_unpackUnicode(&pums, textMsg, 512);
	if( pums.unpackErrorEncountered )
		return;
	// get chat channel
	uint32 cHash = _communicator_generateDefaultChannelHash(target, cm->mapChannel->mapInfo->contextId, 0);
	Thread::LockMutex(&communicator.cs);
	// find channel
	chatChannel_t *chatChannel;
	chatChannel = (chatChannel_t*)hashTable_get(&communicator.ht_channelsBySeed, cHash);
	if( chatChannel )
	{
		// found, send a message to all clients in the channel
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addUnicode(&pms, cm->player->actor->name);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, target);
		pym_addInt(&pms, mapEntityId);
		pym_addInt(&pms, cm->mapChannel->mapInfo->contextId);
		pym_tuple_end(&pms);
		pym_addUnicode(&pms, textMsg);
		pym_tuple_end(&pms);
		// go through all clients
		chatChannel_playerLink_t *currentLink = chatChannel->firstPlayer;
		while( currentLink )
		{
			mapChannelClient_t *chatClient = (mapChannelClient_t*)hashTable_get(&communicator.ht_playersByEntityId, currentLink->entityId);
			if( chatClient )
			{
				netMgr_pythonAddMethodCallRaw(chatClient->cgm, 8, 10000034, pym_getData(&pms), pym_getLen(&pms));
			}
			// next
			currentLink = currentLink->next;
		}

		//00000006     7D - STORE_FAST          'channelId'
		//	00000009     7D - STORE_FAST          'mapEntityId'
		//	0000000C     7D - STORE_FAST          'gameContextId'

	}
	Thread::UnlockMutex(&communicator.cs);
}

void communicator_recv_whisper(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyMarshalString_t pms;
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint8 target[128];
	sint8 msg[512];
	pym_unpackUnicode(&pums, target, sizeof(target));
	pym_unpackUnicode(&pums, msg, sizeof(msg));
	if( pums.unpackErrorEncountered )
		return;
	mapChannelClient_t *dest = communicator_findPlayerByName(target);
	if( dest )
	{
		if( dest != cm )
		{
			// whisper player
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addUnicode(&pms, dest->player->actor->name); // sender
			pym_addUnicode(&pms, msg); // message
			pym_addInt(&pms, dest->player->actor->entityId); // senderEntityId
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(dest->cgm, 8, 2, pym_getData(&pms), pym_getLen(&pms));
		}
		else
		{
			// whispher self
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addUnicode(&pms, msg); // message
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(dest->cgm, 8, 241, pym_getData(&pms), pym_getLen(&pms));
		}
	}
	else
	{
		// player not found
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addUnicode(&pms, target); // target
		pym_addInt(&pms, 7); // errorCode ( 7 = not found )
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, 8, 240, pym_getData(&pms), pym_getLen(&pms));
	}
}

void communicator_init()
{
	Thread::InitMutex(&communicator.cs);
	hashTable_init(&communicator.ht_playersByName, 2048); // 2048 players
	hashTable_init(&communicator.ht_channelsBySeed, 16); // 16 entries per default, more available (uint32 hashtable automatically expands)
	hashTable_init(&communicator.ht_playersByEntityId, 16);
}


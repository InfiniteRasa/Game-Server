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
	char name[40];
	bool isDefaultChannel;
	unsigned int mapContextId;
	unsigned int intanceId;
	int channelId; // 1 - general, ...
	chatChannel_playerLink_t *firstPlayer; // if null --> no player in chat
}chatChannel_t;

struct
{
	CRITICAL_SECTION cs;
	HashTable_string_t ht_playersByName;
	HashTable_uint32_t ht_playersByEntityId; // todo, convert to uint64 hashtable
	HashTable_uint32_t ht_channelsBySeed;
}communicator;


void communicator_registerPlayer(mapChannelClient_t *client)
{
	EnterCriticalSection(&communicator.cs);
	EnterCriticalSection(&client->cgm->cs_general);
	char upperCase[256];
	char *from = client->player->actor->name;
	for(int i=0; i<255; i++)
	{
		char c = from[i];
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
	LeaveCriticalSection(&client->cgm->cs_general);
	LeaveCriticalSection(&communicator.cs);
}

void communicator_unregisterPlayer(mapChannelClient_t *client)
{
	EnterCriticalSection(&communicator.cs);
	EnterCriticalSection(&client->cgm->cs_general);
	char upperCase[256];
	if( client->player )
	{
		char *from = client->player->actor->name;
		for(int i=0; i<255; i++)
		{
			char c = from[i];
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
	LeaveCriticalSection(&client->cgm->cs_general);
	LeaveCriticalSection(&communicator.cs);
}

mapChannelClient_t *communicator_findPlayerByName(char *name)
{
	EnterCriticalSection(&communicator.cs);
	char upperCase[256];
	for(int i=0; i<255; i++)
	{
		char c = name[i];
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
	LeaveCriticalSection(&communicator.cs);
	return client;
}

// called after the player entered the world
void communicator_playerEnterMap(mapChannelClient_t *client)
{
	pyMarshalString_t pms;

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
	EnterCriticalSection(&communicator.cs);
	for(int i=0; i<client->joinedChannels; i++)
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
	LeaveCriticalSection(&communicator.cs);
}

unsigned int _communicator_generateDefaultChannelHash(unsigned int channelId, unsigned int mapContextId, unsigned int instanceId)
{
	unsigned int v = 0;
	v = (channelId ^ (channelId<<7)) ^ mapContextId ^ (mapContextId*121) ^ ((instanceId + instanceId*13)<<3);
	return v;
}

void _communicator_addClientToChannel(mapChannelClient_t *client, unsigned int cHash)
{
	EnterCriticalSection(&communicator.cs);
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
	LeaveCriticalSection(&communicator.cs);
}

void communicator_joinDefaultLocalChannel(mapChannelClient_t *client, int channelId)
{
	pyMarshalString_t pms;
	EnterCriticalSection(&communicator.cs);
	// check if we can join channel
	if( client->joinedChannels >= CHANNEL_LIMIT )
	{
		LeaveCriticalSection(&communicator.cs);
		return; // todo, send error to client
	}
	// generate channel hash
	unsigned int cHash = _communicator_generateDefaultChannelHash(channelId, client->mapChannel->mapInfo->contextId, 0);
	// find channel
	chatChannel_t *chatChannel;
	chatChannel = (chatChannel_t*)hashTable_get(&communicator.ht_channelsBySeed, cHash);
	if( chatChannel == 0 )
	{
		// channel does not exist, create it
		chatChannel = (chatChannel_t*)malloc(sizeof(chatChannel_t));
		chatChannel->name[0] = '\0';
		chatChannel->intanceId = 0;
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
	_communicator_addClientToChannel(client, cHash);
	LeaveCriticalSection(&communicator.cs);
	// send ChatChannelJoined
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, channelId); // channelId
	pym_addInt(&pms, client->mapChannel->mapInfo->contextId); // mapId
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 8, 10000038, pym_getData(&pms), pym_getLen(&pms));
}

void communicator_leaveChannel(mapChannelClient_t *client, int channelId)
{
	EnterCriticalSection(&communicator.cs);
	for(int i=0; i<client->joinedChannels; i++)
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
	LeaveCriticalSection(&communicator.cs);
	__debugbreak(); // todo - send ChannelLeft notify
}

void communicator_loginOk(mapChannel_t *mapChannel, mapChannelClient_t *client)
{
	pyMarshalString_t pms;
	// send LoginOk (despite the original description in the python files, this will only show 'You have arrived at ....' msg in chat)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	char playerName[1024];
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
	pym_addUnicode(&pms, "Welcome to the InfiniteRasa Testing Server"); 
	//pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 8, 769, pym_getData(&pms), pym_getLen(&pms));
}

void communicator_systemMessage(mapChannelClient_t *client, char *message)
{
	pyMarshalString_t pms;
	// send SystemMessage
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, message);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 8, 553, pym_getData(&pms), pym_getLen(&pms));
}

bool communicator_parseCommand(mapChannelClient_t *cm, char *textMsg)
{
	pyMarshalString_t pms;
	
	if( memcmp(textMsg,".pathnode ",10) == 0)
	{
         
		//__debugbreak();
		//---saves pathnode(s) for a  certain spawntype
		/*char cmd[11];
		int spawnid,pathindex;
		sscanf(textMsg,"%s %d %d",cmd,&spawnid,&pathindex);
			
		di_pathNodeDataW2_t pNode = {0};
		pNode.currentContextId = cm->mapChannel->mapInfo->contextId;
		pNode.spawntype = spawnid;
		pNode.pathindex = pathindex;
		pNode.posX = cm->player->actor->posX;
		pNode.posY = (cm->player->actor->posY)+0.1f;
		pNode.posZ = cm->player->actor->posZ;
		dataInterface_PathNode_setPathnode(&pNode,NULL,NULL);
		*/
		communicator_systemMessage(cm, "pathnodes deactivated");
		return true;
	}
    if( memcmp(textMsg,".gameobject ",12) == 0 )
	{
		 char cmd[12];
		 unsigned int type;
		 unsigned int entityClassId;
		 sscanf(textMsg,"%s %d %d",cmd,&entityClassId,&type);

		 di_teleporterData worldObject = {0};
		 worldObject.bx = 2.1f;
		 worldObject.bz = 2.1f;
		 worldObject.sx = cm->player->actor->posX;
		 worldObject.sy = cm->player->actor->posY;
		 worldObject.sz = cm->player->actor->posZ;
		 worldObject.modelid = entityClassId;
		 worldObject.type = type;

		 dataInterface_teleporter_updateList(&worldObject,NULL,NULL);
         
		 sprintf(textMsg,"Gameobject placed: %f %f %f",worldObject.sx,worldObject.sy,worldObject.sz);
		 communicator_systemMessage(cm, textMsg);
		 return true;
	}
	if( memcmp(textMsg,".spawner ",9) == 0 )
	{
		//communicator_systemMessage(cm, "spawner deactivated");
		char cmd[9];
		int spawntype; //---which type of spawn to place( look db for id)
		int pwd;
		sscanf(textMsg,"%s %d %d",cmd,&spawntype,&pwd);	
		
		if(pwd != 2104 )
		{
			sprintf(textMsg, "unvalid password");
			communicator_systemMessage(cm, textMsg);
			return true;
		}

		//---todo: make a db check
		if(spawntype <= 0 || spawntype >= 200 )
		{
			sprintf(textMsg, "unvalid spawntype id");
			communicator_systemMessage(cm, textMsg);
			return true;
		}

		di_spawnDataW2_t spawndata = {0};
		spawndata.currentContextId = cm->mapChannel->mapInfo->contextId;
		spawndata.posX = cm->player->actor->posX;
		spawndata.posY = cm->player->actor->posY+0.5f;
		spawndata.posZ = cm->player->actor->posZ;
		spawndata.spawntype = spawntype;	
		dataInterface_Spawnpool_updateSpawnW2(&spawndata,NULL,NULL);

		sprintf(textMsg, "added spawnlocation: %f %f %f of spawntype-id: %d", 
						cm->player->actor->posX, 
			            cm->player->actor->posY, 
						cm->player->actor->posZ,spawntype);
		communicator_systemMessage(cm, textMsg);
		return true;
	}
	
	if( memcmp(textMsg,".animtest ",10) == 0 )
	{
		    //__debugbreak();
		    creature_t *npc = (creature_t*)entityMgr_get(cm->player->targetEntityId);
			if( !npc )
				return true; 
			// various actions/animations
			int action;
			char cmd[40];
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
			
			
			for(int i=0; i<SWAPSET_SIZE; i++)
			{
				entityData.appearanceData[i].classId = npc->actor.appearanceData[i].classId;
				entityData.appearanceData[i].hue = npc->actor.appearanceData[i].hue;
			}
			
			dataInterface_Entity_updateEntityW(&entityData,NULL,NULL);

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
	if( strcmp(textMsg, ".npc") == 0 )
	{
		npc_test(cm);
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
	
	if( memcmp(textMsg, ".creature ", 10) == 0 )
	{
		//20110728 - thuvvik complete "creature dictionary" invocation ... cf creature.cpp line 289
		/*char *pch = textMsg + 10;

		int creatureClass;
		char cmd[30];
		sscanf(textMsg,"%s %d",cmd,&creatureClass);			
		creatureType_t ctype = {0};
		ctype.maxHealth = 150;
		ctype.nameId = 0;
		ctype.entityClassId = creatureClass;

		creature_t *creature = creature_createCreature(cm->mapChannel, &ctype, NULL,3);
		creature_setLocation(creature, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ, 0.0f, 0.0f);
		cellMgr_addToWorld(cm->mapChannel, creature);
		//communicator_systemMessage(cm, "creature spawner deactivated");*/
		return true;
	}
	if( memcmp(textMsg, ".effect ", 8) == 0 )
	{
		//20110728 - thuvvik complete "creature dictionary" invocation ... cf creature.cpp line 289
		char *pch = textMsg + 8;
		

		gameEffect_attach(cm->mapChannel, cm->player->actor, atoi(pch), 1);
		return true;
	}
	//if( strcmp(textMsg, ".bane") == 0 )
	//{
	//	dynamicObject_createBaneDropship(cm->mapChannel, cm->player->actor->posX, cm->player->actor->posY+6.0f, cm->player->actor->posZ);
	//	return true;
	//}
	if( strcmp(textMsg, ".ccp") == 0 )
	{
		dynamicObject_developer_createControlPoint(cm->mapChannel, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ);
		communicator_systemMessage(cm, "ControlPoint spawned!");
		return true;
	}
	if( memcmp(textMsg, ".state ", 7) == 0 )
	{
		unsigned int object;
		int state;
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
		if( entityMgr_getEntityType(cm->player->targetEntityId) == ENTITYTYPE_NPC )
		{
			npc_t *npc = (npc_t*)entityMgr_get(cm->player->targetEntityId);
			if( !npc )
				return true;
			communicator_systemMessage(cm, "Saving npc...");
			di_npcData_t npcData = {0};
			npcData.npcID = npc->entityId;
			npcData.posX = npc->actor.posX;
			npcData.posY = npc->actor.posY;
			npcData.posZ = npc->actor.posZ;
			npcData.entityClassID = npc->actor.entityClassId;
			npcData.currentContextId = cm->mapChannel->mapInfo->contextId;
			strcpy(npcData.unicodeName, npc->actor.name);
			for(int i=0; i<SWAPSET_SIZE; i++)
			{
				npcData.appearanceData[i].classId = npc->actor.appearanceData[i].classId;
				npcData.appearanceData[i].hue = npc->actor.appearanceData[i].hue;
			}
			dataInterface_NPC_updateNPC(&npcData, NULL, NULL);
			return true;
		}
		else
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
	if( strcmp(textMsg, ".footlocker") == 0 )
	{
		dynamicObject_developer_createFootlocker(cm->mapChannel, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ);
		communicator_systemMessage(cm, "Spawned a footlocker!");
		return true;
	}
	if( memcmp(textMsg, ".setregion ", 11) == 0 )
	{
		int regionId;
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
	if( memcmp(textMsg, ".name ", 6) == 0 )
	{
		char *pch = textMsg + 6;
		//pch = strtok (textMsg+6, " ");
		//if( pch )
		if( *pch )
		{
			if( entityMgr_getEntityType(cm->player->targetEntityId) == ENTITYTYPE_NPC )
			{
				npc_t *npc = (npc_t*)entityMgr_get(cm->player->targetEntityId);
				if( !npc )
					return true;
				npc_updateName(cm->mapChannel, npc, pch);
			}
			else
				communicator_systemMessage(cm, "Cannot set name of selection");
		}
		else
		{
			communicator_systemMessage(cm, "Wrong syntax");
		}
		return true;
	}
	if( memcmp(textMsg, ".look ", 6) == 0 )
	{
		char *pch = textMsg + 6;
		pch = strtok (textMsg+6, " ");
		if( pch )
		{
			int itemClassId = 0;
			unsigned int hue = 0x80808080;
			sscanf(pch, "%d", &itemClassId);
			pch = strtok (NULL, " ");
			if( pch )
			{
				if( *pch == '0' && *(pch+1) == 'x' )
					pch += 2;
				sscanf(pch, "%X", &hue);
			}
			if( entityMgr_getEntityType(cm->player->targetEntityId) == ENTITYTYPE_NPC )
			{
				npc_t *npc = (npc_t*)entityMgr_get(cm->player->targetEntityId);
				if( !npc )
					return true;
				npc_updateAppearanceItem(cm->mapChannel, npc, itemClassId, hue);
			}
			else
				communicator_systemMessage(cm, "Cannot set appearance of selection");
		}
		else
		{
			communicator_systemMessage(cm, "Wrong syntax");
		}
		return true;
	}
	if( memcmp(textMsg, ".obj ", 5) == 0 )
	{
		char *pch = textMsg + 5;
		if( *pch )
		{
			int classId = 0;
			sscanf(pch, "%d", &classId);
			dynamicObject_developer_createCustom(cm->mapChannel, classId, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ);
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
		char cmd[10];
	    //char *cmdpos = textMsg + 7;

		typedef struct tloc
		{
			int x;
			int y;
			int z;
			float rotation; //not necessary, could add
			int mapContextId;
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
		
		char cmd[7];
	    //char *cmdpos = textMsg + 7;

		typedef struct tloc
		{
			int x;
			int y;
			int z;
			float rotation; //not necessary, could add
			int mapContextId;
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
			EnterCriticalSection(&cm->cgm->cs_general);
			cellMgr_removeFromWorld(cm);
			// remove from list
			for(int i=0; i<cm->mapChannel->playerCount; i++)
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
			LeaveCriticalSection(&cm->cgm->cs_general);
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
			int mapVersion = 0; // default = 0;
			for(int i=0; i<mapInfoCount; i++)
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
			for(int chan=0; chan < global_channelList->mapChannelCount; chan++)
			{
               mapChannel_t *mapChannel = global_channelList->mapChannelArray+chan;
			   if(mapChannel->mapInfo->contextId == telepos.mapContextId)
			   {
				   cm->mapChannel = mapChannel;
				   break;
			   }
			}

			mapChannel_t *mapChannel = cm->mapChannel;
			EnterCriticalSection(&cm->mapChannel->criticalSection);
			mapChannel->playerList[mapChannel->playerCount] = cm;
			mapChannel->playerCount++;
			hashTable_set(&mapChannel->ht_socketToClient, (unsigned int)cm->cgm->socket, cm);
			LeaveCriticalSection(&mapChannel->criticalSection);
			
			cellMgr_addToWorld(cm); //cellintroducing to player /from players
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

	// void npc_updateAppearanceItem(mapChannel_t *mapChannel, npc_t *npc, unsigned int itemClassId, unsigned int hue)

	return false;
}


void communicator_recv_radialChat(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	char textMsg[256];
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
	//char playerName[256];
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
	for(int i=0; i<mapChannel->playerCount; i++)
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

void communicator_recv_shout(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	char textMsg[256];
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


void communicator_recv_channelChat(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
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
	char textMsg[512];
	pym_unpackUnicode(&pums, textMsg, 512);
	if( pums.unpackErrorEncountered )
		return;
	// get chat channel
	unsigned int cHash = _communicator_generateDefaultChannelHash(target, cm->mapChannel->mapInfo->contextId, 0);
	EnterCriticalSection(&communicator.cs);
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
	LeaveCriticalSection(&communicator.cs);
}

void communicator_recv_whisper(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	pyMarshalString_t pms;
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	char target[128];
	char msg[512];
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
	InitializeCriticalSection(&communicator.cs);
	hashTable_init(&communicator.ht_playersByName, 2048); // 2048 players at max
	hashTable_init(&communicator.ht_channelsBySeed, 16); // 16 entries per default, more available (uint32 hashtable automatically expands)
	hashTable_init(&communicator.ht_playersByEntityId, 16);
}

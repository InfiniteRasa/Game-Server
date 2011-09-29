#include"Global.h"

/*
	A MapChannel in general is a isolated thread of the server that is 100% responsible for one or multiple maps
	Whenever a client selects a character the clientinstance is passed to the MapChannel and is no more processed by the main thread.
	If a client changes map his instance is passed to the target MapChannel
*/

HashTable_uint32_t ht_mapChannelsByContextId;
mapChannelList_t *global_channelList; //20110827 @dennton


void _cb_mapChannel_addNewPlayer(void *param, diJob_characterData_t *jobData)
{
	if( jobData->outCharacterData == NULL )
	{
		// todo: add error handling
		return;
	}
	mapChannelClient_t *mc = (mapChannelClient_t*)param;
	mapChannel_t *mapChannel = mc->mapChannel;
	// save character data
	mc->tempCharacterData = (di_characterData_t*)malloc(sizeof(di_characterData_t));
	memcpy(mc->tempCharacterData, jobData->outCharacterData, sizeof(di_characterData_t));
	// save seperate mission data (if any)
	if( mc->tempCharacterData->missionStateCount )
	{
		mc->tempCharacterData->missionStateData = (di_CharacterMissionData*)malloc(sizeof(di_CharacterMissionData) * mc->tempCharacterData->missionStateCount);
		memcpy(mc->tempCharacterData->missionStateData, jobData->outCharacterData->missionStateData, sizeof(di_CharacterMissionData) * mc->tempCharacterData->missionStateCount);
	}
	else
	{
		mc->tempCharacterData->missionStateData = NULL;
	}
	// add to player to mapChannel (synced)
	EnterCriticalSection(&mapChannel->criticalSection);
	mapChannel->playerList[mapChannel->playerCount] = mc;
	mapChannel->playerCount++;
	hashTable_set(&mapChannel->ht_socketToClient, (unsigned int)mc->cgm->socket, mc);
	LeaveCriticalSection(&mapChannel->criticalSection);
}

void mapChannel_addNewPlayer(mapChannel_t *mapChannel, clientGamemain_t *cgm)
{
	mapChannelClient_t *mc = (mapChannelClient_t*)malloc(sizeof(mapChannelClient_t));
	memset((void*)mc, 0x00, sizeof(mapChannelClient_t));
	mc->cgm = cgm;
	mc->clientEntityId = entityMgr_getFreeEntityIdForClient(); // generate a entityId for the client instance
	mc->mapChannel = mapChannel;
	mc->player = NULL;
	dataInterface_Character_getCharacterData(cgm->userID, cgm->mapLoadSlotId, _cb_mapChannel_addNewPlayer, mc);
	// register mapChannelClient
	entityMgr_registerEntity(mc->clientEntityId, mc);

	//// add to the serverlist
	//if( mapChannel->playerCount == mapChannel->playerLimit )
	//{
	//	printf("TODO#addNewPlayer\n");
	//	return;
	//}
	//mapChannel->playerList[mapChannel->playerCount] = mc;
	//mapChannel->playerCount++;
	//hashTable_set(&mapChannel->ht_socketToClient, (unsigned int)cgm->socket, mc);
	//// create new actor...
	//
	//// void dataInterface_Character_getCharacterData(unsigned long long userID, unsigned int slotIndex, void (*cb)(void *param, diJob_characterData_t *jobData), void *param)
}

void mapChannel_removePlayer(mapChannelClient_t *client)
{
	// unregister mapChannelClient
	entityMgr_unregisterEntity(client->clientEntityId);

	communicator_unregisterPlayer(client);
	EnterCriticalSection(&client->cgm->cs_general);
	cellMgr_removeFromWorld(client);
	manifestation_removePlayerCharacter(client->mapChannel, client);
	if( client->disconnected == false )
		GameMain_PassClientToCharacterSelection(client->cgm);
	// remove from list
	for(int i=0; i<client->mapChannel->playerCount; i++)
	{
		if( client == client->mapChannel->playerList[i] )
		{
			if( i == client->mapChannel->playerCount-1 )
			{
				client->mapChannel->playerCount--;
			}
			else
			{
				client->mapChannel->playerList[i] = client->mapChannel->playerList[client->mapChannel->playerCount-1];
				client->mapChannel->playerCount--;
			}
			break;
		}
	}
	// delete data
	LeaveCriticalSection(&client->cgm->cs_general);
	free(client->cgm);
	free(client);
}

void mapChannel_registerTimer(mapChannel_t *mapChannel, int period, void *param, bool (*cb)(mapChannel_t *mapChannel, void *param, int timePassed))
{
	mapChannelTimer_t *timer = (mapChannelTimer_t*)malloc(sizeof(mapChannelTimer_t));
	timer->period = period;
	timer->timeLeft = period;
	timer->param = param;
	timer->cb = cb;
	hashTable_set(&mapChannel->ht_timerList, (unsigned int)timer, timer);
}

void mapChannel_registerAutoFireTimer(mapChannel_t *mapChannel, int delay, manifestation_t* origin, int type)
{
	mapChannelAutoFireTimer_t timer;
	timer.delay = delay;
	timer.timeLeft = delay;
	timer.origin = origin;
	timer.type = type;
	mapChannel->autoFire_timers.push_back(timer);
}

void mapChannel_removeAutoFireTimer(mapChannel_t* mapChannel, manifestation_t* origin)
{
	std::vector<mapChannelAutoFireTimer_t>::iterator timer = mapChannel->autoFire_timers.begin();
	while (timer != mapChannel->autoFire_timers.end())
	{
		if (timer->origin == origin)
		{
			timer = mapChannel->autoFire_timers.erase(timer);
		}
		else { ++timer; }
	}
}

void mapChannel_check_AutoFireTimers(mapChannel_t* mapChannel)
{
	std::vector<mapChannelAutoFireTimer_t>::iterator timer;
	for(timer = mapChannel->autoFire_timers.begin(); timer < mapChannel->autoFire_timers.end(); timer++)
	{
		timer->timeLeft -= 100;
		if (timer->timeLeft <= 0)
		{
			if (timer->origin->actor->inCombatMode == false)
			{ continue; /* TODO: delete timer here */ }
			if (timer->origin->targetEntityId)
			{ missile_launch(mapChannel, timer->origin->actor, timer->origin->targetEntityId, timer->type, 10); }
			timer->timeLeft = timer->delay;
		}
	}
}

//20110827 @dennton
bool CheckTempCharacter(di_characterData_t *tcd)
{
   bool valid = true;   
   if(tcd == NULL) valid = false;
   if(tcd->missionStateData == NULL) valid = false;
   return valid;
}


void mapChannel_recv_mapLoaded(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	manifestation_createPlayerCharacter(cm->mapChannel, cm, cm->tempCharacterData);
	communicator_registerPlayer(cm);
	communicator_playerEnterMap(cm);
	inventory_initForClient(cm);
	mission_initForClient(cm);
	// free temporary character data	
	if( CheckTempCharacter(cm->tempCharacterData) != 0 )// 20110827 @dennton
	{
		if( cm->tempCharacterData->missionStateData )
			free(cm->tempCharacterData->missionStateData);
		free(cm->tempCharacterData);
	}
}

void mapChannel_recv_LogoutRequest(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	pyMarshalString_t pms;
	// send time remaining to logout
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10*1000); // milliseconds
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, 5, METHODID_LOGOUTTIMEREMAINING, pym_getData(&pms), pym_getLen(&pms));
	cm->logoutRequestedLast = GetTickCount();
	cm->logoutActive = true;
}

void mapChannel_recv_CharacterLogout(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	pyMarshalString_t pms;
	// pass to character selection
	if( cm->logoutActive == false )
		return;
	cm->removeFromMap = true;
}


void mapChannel_processPythonRPC(mapChannelClient_t *cm, unsigned int methodID, unsigned char *pyString, int pyStringLen)
{
	// check if 'O'
	if( *pyString != 'O' )
		__debugbreak(); // oh shit...
	pyString++; pyStringLen--;

	switch( methodID )
	{
	case METHODID_WHISPER: // Whisper
		communicator_recv_whisper(cm, pyString, pyStringLen);
		return;
	case 43: // ClearTargetId
		manifestation_recv_ClearTargetId(cm, pyString, pyStringLen);
		return;
	case METHODID_RADIALCHAT: // RadialChat
		communicator_recv_radialChat(cm, pyString, pyStringLen);
		return;
	case METHODID_MAPLOADED: // MapLoaded
		mapChannel_recv_mapLoaded(cm, pyString, pyStringLen);
		return;
	case 129: // Ping
		// ignore (todo)
		return;
	case METHODID_REQUESTTOGGLERUN: // ToggleRun
		manifestation_recv_ToggleRun(cm, pyString, pyStringLen);
		return;
	case 201: // SetTargetId
		manifestation_recv_SetTargetId(cm, pyString, pyStringLen);
		return;
	case METHODID_SHOUT: // Shout
		communicator_recv_shout(cm, pyString, pyStringLen);
		return;
	case 343: // RequestPerformAbility
		manifestation_recv_RequestPerformAbility(cm, pyString, pyStringLen);
		return;
	case METHODID_REQUESTUSEOBJECT: // RequestUseObject
		dynamicObject_recv_RequestUseObject(cm, pyString, pyStringLen);
		return;
	case 407: //AssignNPCMission
		npc_recv_AssignNPCMission(cm, pyString, pyStringLen);
		return;
	case METHODID_PERSONALINVENTORY_MOVEITEM: // PersonalInventory_MoveItem
		item_recv_PersonalInventoryMoveItem(cm, pyString, pyStringLen);
		return;
	case 506: // RequestArmAbility
		manifestation_recv_RequestArmAbility(cm, pyString, pyStringLen);
		return;
	case 507: // RequestArmWeapon
		item_recv_RequestArmWeapon(cm, pyString, pyStringLen);
		return;
	case METHODID_REQUESTEQUIPARMOR: // RequestEquipArmor
		item_recv_RequestEquipArmor(cm, pyString, pyStringLen);
		return;
	case METHODID_REQUESTEQUIPWEAPON: // RequestEquipWeapon
		item_recv_RequestEquipWeapon(cm, pyString, pyStringLen);
		return;
	case 518: // RequestNPCConverse
		npc_recv_RequestNPCConverse(cm, pyString, pyStringLen);
		return;
	case METHODID_REQUESTNPCVENDING: // RequestNPCVending
		npc_recv_RequestNPCVending(cm, pyString, pyStringLen);
		return;
	case 522: // RequestSetAbilitySlot
		manifestation_recv_RequestSetAbilitySlot(cm, pyString, pyStringLen);
		return;
	case 530: // RequestWeaponDraw
		item_recv_RequestWeaponDraw(cm, pyString, pyStringLen);
		return;
	case 531: // RequestWeaponReload
		item_recv_RequestWeaponReload(cm, pyString, pyStringLen);
		return;
	case 532: // RequestWeaponStow
		item_recv_RequestWeaponStow(cm, pyString, pyStringLen);
		return;
	case 549: // StartAutoFire
		manifestation_recv_StartAutoFire(cm, pyString, pyStringLen);
		return;
	case 550: // StopAutoFire
		manifestation_recv_StopAutoFire(cm, pyString, pyStringLen);
		return;
	case 410: // AutoFireKeepAlive
		manifestation_recv_AutoFireKeepAlive(cm, pyString, pyStringLen);
		return;
	case 573: // WeaponDrawerInventory_MoveItem
		printf("TODO: WeaponDrawerInventory MoveItem\n");
		return;
	case METHODID_REQUESTTOOLTIPFORITEMTEMPLATEID: // RequestTooltipForItemTemplateId
		item_recv_RequestTooltipForItemTemplateId(cm, pyString, pyStringLen);
		return;
	case 753: // RequestVisualCombatMode
		manifestation_recv_RequestVisualCombatMode(cm, pyString, pyStringLen);
		return;
	case 759: // RequestActionInterrupt
		dynamicObject_recv_RequestActionInterrupt(cm, pyString, pyStringLen);
		return;
	case METHODID_REQUESTLOGOUT: // RequestLogout
		mapChannel_recv_LogoutRequest(cm, pyString, pyStringLen);
		return;
	case METHODID_CHANNELCHAT: // ChannelChat
		communicator_recv_channelChat(cm, pyString, pyStringLen);
		return;
	case METHODID_CHARACTERLOGOUT: // CharacterLogout
		mapChannel_recv_CharacterLogout(cm, pyString, pyStringLen);
		return;
	case METHODID_REQUESTWEAPONATTACK://player melee
		missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, MELEE_PISTOL, 20, 2);
		return;
	default:
		printf("MapChannel_UnknownMethodID: %d\n", methodID);
		printf("MapChannel_RPCPacket - Size: %d\n", pyStringLen);
		HexOut(pyString, pyStringLen);
		printf("\n\n");
		return;
		// no handler for that
	};

	// 149
	// 00001AA7     64 - LOAD_CONST          'RequestCharacterName'


	return;
}

#pragma pack(1)
typedef struct  
{
	int contextId;
	int pX;
	int pY;
	int pZ;
}movementLogEntry_t;
#pragma pack()

HANDLE hMovementLogFile = NULL;
void mapChannel_logMovement(int contextId, int x, int y, int z)
{
	return;
	if( hMovementLogFile == NULL )
	{
		hMovementLogFile = CreateFile("movementlog.bin", FILE_ALL_ACCESS, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
		SetFilePointer(hMovementLogFile, 0, 0, FILE_END);
	}
	// setup entry
	movementLogEntry_t entry;
	entry.contextId = contextId;
	entry.pX = x;
	entry.pY = y;
	entry.pZ = z;
	// write entry
	DWORD bytesWritten;
	WriteFile(hMovementLogFile, (LPCVOID)&entry, sizeof(movementLogEntry_t), &bytesWritten, NULL);
}


void mapChannel_decodeMovementPacket(mapChannelClient_t *mc, unsigned char *data, unsigned int len)
{
	if( mc->removeFromMap )
		return;
	if( mc->player == NULL )
		return;
	netCompressedMovement_t netMovement;
	unsigned int pIdx = 0;
	unsigned int counterA = *(unsigned int*)(data+pIdx); pIdx += 4;
	unsigned int ukn1 = *(unsigned int*)(data+pIdx); pIdx += 4;
	unsigned int counterB = *(unsigned int*)(data+pIdx); pIdx += 4;
	if( data[pIdx] != 2 )
		__debugbreak();
	pIdx++;
	if( data[pIdx] != 6 ) // subOpcode?
		__debugbreak();
	pIdx++;
	// skip unknown
	pIdx += 10;
	// for performance we actually dont parse the packet
	// posX
	if( data[pIdx] != 0x29 )
		__debugbreak();
	if( data[pIdx+7] != 0x2A )
		__debugbreak();
	int val24b = (data[pIdx+2]<<16) | (data[pIdx+4]<<8) | (data[pIdx+6]);
	if( val24b&0x00800000 )
		val24b |= 0xFF000000;
	netMovement.posX24b = val24b;
	float posX = (float)val24b / 256.0f;
	int vLogX = val24b;
	mc->player->actor->posX = posX;
	pIdx += 8;
	// posY
	if( data[pIdx] != 0x29 )
		__debugbreak();
	if( data[pIdx+7] != 0x2A )
		__debugbreak();
	val24b = (data[pIdx+2]<<16) | (data[pIdx+4]<<8) | (data[pIdx+6]);
	if( val24b&0x00800000 )
		val24b |= 0xFF000000;
	netMovement.posY24b = val24b;
	float posY = (float)val24b / 256.0f;
	int vLogY = val24b;
	mc->player->actor->posY = posY;
	pIdx += 8;
	// posZ
	if( data[pIdx] != 0x29 )
		__debugbreak();
	if( data[pIdx+7] != 0x2A )
		__debugbreak();
	val24b = (data[pIdx+2]<<16) | (data[pIdx+4]<<8) | (data[pIdx+6]);
	if( val24b&0x00800000 )
		val24b |= 0xFF000000;
	netMovement.posZ24b = val24b;
	float posZ = (float)val24b / 256.0f;
	int vLogZ = val24b;
	mc->player->actor->posZ = posZ;
	pIdx += 8;
	// read velocity
	//29 05 00 1A 2A velocity?	/1024.0
	if( data[pIdx] != 0x29 )
		__debugbreak();
	val24b = *(signed short*)(data+pIdx+2);
	netMovement.velocity = val24b;
	float velocity = (float)val24b / 1024.0f;
	if( data[pIdx+4] != 0x2A )
		__debugbreak();
	pIdx += 5;

	mapChannel_logMovement(mc->mapChannel->mapInfo->contextId, vLogX, vLogY, vLogZ);
	// read flag
	netMovement.flag = *(unsigned char*)(data+pIdx+1);
	pIdx += 2;
	// read viewX, viewY
	if( data[pIdx] != 0x29 )
		__debugbreak();
	val24b = *(signed short*)(data+pIdx+2);
	netMovement.viewX = val24b;
	float viewX = (float)val24b / 1024.0f; // factor guessed ??? find real
	val24b = *(signed short*)(data+pIdx+5);
	netMovement.viewY = val24b;
	float viewY = (float)val24b / 1024.0f; // factor guessed ???
	/*
	03 08
	29 05 04 66 05 62 08 2A 	???
	2A
	2A 31
	*/

	netMovement.entityId = mc->player->actor->entityId;

	//netMgr_broadcastEntityMovement(mc->mapChannel, &netMovement, true);
	netMgr_cellDomain_sendEntityMovement(mc, &netMovement, true);
	// void netMgr_broadcastEntityMovement(mapChannel_t *broadCastChannel, netCompressedMovement_t *movement, bool skipOwner)
	// print info
	//printf("move %f %f %f v: %f rXY: %f %f\n", posX, posY, posZ, velocity, viewX, viewY);
}

int mapChannel_decodePacket(mapChannelClient_t *mc, unsigned char *data, unsigned int len)
{
	if( mc->removeFromMap )
		return 1;
	if( len >= 0xFFFF )
		__debugbreak();
	if( len < 4 )
		return 0;


	if( len >= 0xFFFF )
		__debugbreak();
	if( len < 4 )
		return 0;

	int pIdx = 0;
	// read subSize
	unsigned int subSize = *(unsigned short*)(data+pIdx); pIdx += 2; // redundancy with param len
	// read major opcode
	unsigned int majorOpc = *(unsigned short*)(data+pIdx); pIdx += 2;
	if( majorOpc == 1 )
	{
		mapChannel_decodeMovementPacket(mc, data+pIdx, subSize-pIdx);
		return 1;
	}
	else if( majorOpc )
	{
		return 1; // ignore the packet
	}
	// read header A
	unsigned char ukn1 = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( ukn1 != 2 )
		__debugbreak();

	unsigned char opcode = *(unsigned char*)(data+pIdx); pIdx +=1; // not 100% sure
	unsigned char ukn2 = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( ukn2 != 0 )
		__debugbreak();
	unsigned char xorCheckA = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( xorCheckA != 3 ) // we only know headerA length of 3 for now
		__debugbreak();

	unsigned int hdrB_start = pIdx;
	unsigned char ukn3 = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( ukn3 != 3 )
		__debugbreak();
	// different handling now (dont support subOpc 2 here anymore)
	if( opcode == 0x0C )
	{
		// expect header B part 1 (0x29)
		if( *(unsigned char*)(data+pIdx) == 0x00 )
			return 1; // empty packet?
		if( *(unsigned char*)(data+pIdx) != 0x29 )
			__debugbreak(); // wrong
		pIdx++;
		unsigned char ukn0C_1 = *(unsigned char*)(data+pIdx); pIdx++;
		if( ukn0C_1 != 3 ) __debugbreak();
		unsigned char ukn0C_2 = *(unsigned char*)(data+pIdx); pIdx++;
		//if( ukn0C_2 != 1 && ukn0C_2 != 3 && ukn0C_2 != 9 ) __debugbdfdsfreak(); // server entityId?
		if( ukn0C_2 == 0 || ukn0C_2 > 0x10 ) __debugbreak(); // server entityId?
		unsigned char preffix0C_1 = *(unsigned char*)(data+pIdx); pIdx++;
		if( preffix0C_1 != 7 ) __debugbreak(); // 7 --> 32-bit int
		unsigned int methodID = *(unsigned int*)(data+pIdx); pIdx += 4;
		unsigned char ukn0C_3 = *(unsigned char*)(data+pIdx); pIdx++; // entityID?
		if( ukn0C_3 != 1 ) __debugbreak();
		// part 2 (0xCB)
		if( *(unsigned char*)(data+pIdx) != 0xCB )
			__debugbreak(); // wrong
		pIdx++;
		unsigned int dataLen = 0;
		unsigned int lenMask = *(unsigned char*)(data+pIdx); pIdx++;
		if( (lenMask>>6) == 0 )
		{
			// 6 bit length
			dataLen = lenMask&0x3F;
		}
		else if( (lenMask>>6) == 1 )
		{
			// 14 bit length
			dataLen = (lenMask&0x3F);
			dataLen |= ((*(unsigned char*)(data+pIdx))<<6);
			pIdx++;
		}
		else
			__debugbreak();
		mapChannel_processPythonRPC(mc, methodID, data+pIdx, dataLen);
		pIdx += dataLen;
		// xor check...
	}
	else
		return 1;
	return 1;
}

void mapChannel_readData(mapChannelClient_t *mc)
{
	// todo: disconnect client on error...
	clientGamemain_t *cgm = mc->cgm;
	if( cgm->RecvState < 4 )
	{
		int r = recv(cgm->socket, (char*)cgm->RecvBuffer+cgm->RecvState, 4-cgm->RecvState, 0);
		if( r == 0 || r == SOCKET_ERROR )
		{
			mc->removeFromMap = true;
			mc->disconnected = true;
			return;
		}
		cgm->RecvState += r;
		if( cgm->RecvState == 4 )
			cgm->RecvSize = *(unsigned int*)cgm->RecvBuffer + 4;
		return;
	}
	int r = recv(cgm->socket, (char*)cgm->RecvBuffer+cgm->RecvState, cgm->RecvSize-cgm->RecvState, 0);
	if( r == 0 || r == SOCKET_ERROR )
	{
		mc->removeFromMap = true;
		mc->disconnected = true;
		return;
	}
	cgm->RecvState += r;

	if( cgm->RecvState == cgm->RecvSize )
	{
		// full packet received
		// everything is encrypted, so do decryption job here
		Tabula_Decrypt2(&cgm->tbc2, (unsigned int*)(cgm->RecvBuffer+4), cgm->RecvSize);
		int r = 0;
		int AlignBytes = cgm->RecvBuffer[4]%9;

		unsigned char *Buffer = cgm->RecvBuffer + 4 + AlignBytes;
		int Size = cgm->RecvSize - 4 - AlignBytes;
		do{
			unsigned short Subsize = *(unsigned short*)Buffer;
			mapChannel_decodePacket(mc, Buffer, Subsize);
			Buffer += Subsize;
			Size -= Subsize;
		}while(Size > 0);
		cgm->RecvState = 0;
		return;
	}
	return;
}

int mapChannel_worker(mapChannelList_t *channelList)
{
	
	FD_SET fd;
	timeval sTimeout;
	sTimeout.tv_sec = 0;
	sTimeout.tv_usec = 10000;
	global_channelList = channelList; //20110827 @dennton

	// init mapchannel
	//
	printf("init mapchannels..\n");
	for(int chan=0; chan<channelList->mapChannelCount; chan++)
	{
		mapChannel_t *mapChannel = channelList->mapChannelArray+chan;
		if( cellMgr_initForMapChannel(mapChannel) == false )
		{
			printf("Error on map-cell init in mapContextId %d\n", mapChannel->mapInfo->contextId);
			Sleep(5000);
			ExitThread(-1);
		}
		dynamicObject_init(mapChannel);
		mission_initForChannel(mapChannel);
		npc_initForMapChannel(mapChannel); //---db use
		missile_initForMapchannel(mapChannel);
		spawnPool_initForMapChannel(mapChannel); //---todo:db use
		controller_initForMapChannel(mapChannel);
		printf("Map: [%s]\n",mapChannel->mapInfo->name);
	}


	printf("MapChannels started...\n");

	while( true )
	{
		for(int chan=0; chan<channelList->mapChannelCount; chan++)
		{
			mapChannel_t *mapChannel = channelList->mapChannelArray+chan;
			// check for new players in queue (one per round)
			if( mapChannel->rb_playerQueueReadIndex != mapChannel->rb_playerQueueWriteIndex )
			{
				mapChannel_addNewPlayer(mapChannel, mapChannel->rb_playerQueue[mapChannel->rb_playerQueueReadIndex]);
				mapChannel->rb_playerQueueReadIndex = (mapChannel->rb_playerQueueReadIndex+1)%MAPCHANNEL_PLAYERQUEUE;
			}
			// recv client data
			FD_ZERO(&fd);
			for(int i=0; i<mapChannel->playerCount; i++)
			{
				FD_SET(mapChannel->playerList[i]->cgm->socket, &fd);
			}
			int r = select(0, &fd, 0, 0, &sTimeout);
			if( r )
			{
				for(int i=0; i<fd.fd_count; i++)
				{
					mapChannelClient_t *mc = (mapChannelClient_t*)hashTable_get(&mapChannel->ht_socketToClient, (unsigned int)fd.fd_array[i]);
					if( mc )
						mapChannel_readData(mc);
					else
					{
						continue;
					}
					if( mc->removeFromMap )
					{
						communicator_playerExitMap(mc);
						mapChannel_removePlayer(mc);
					}
				}
			}
			
			if (mapChannel->playerCount > 0)
			{
				// do other work
				cellMgr_doWork(mapChannel);
				// check timers
				unsigned int currentTime = GetTickCount();
				if( (currentTime - mapChannel->timer_clientEffectUpdate) >= 500 )
				{
					gameEffect_checkForPlayers(mapChannel->playerList, mapChannel->playerCount, 500);
					mapChannel->timer_clientEffectUpdate += 500;
				}
				if (mapChannel->cp_trigger.cb != NULL)
				{
					if ((currentTime - mapChannel->cp_trigger.period) >= 100)
					{
						mapChannel->cp_trigger.timeLeft -= 100;
						mapChannel->cp_trigger.period = currentTime;
						if (mapChannel->cp_trigger.timeLeft <= 0)
						{
							mapChannel->cp_trigger.cb(mapChannel, mapChannel->cp_trigger.param, 1);
							mapChannel->cp_trigger.cb = NULL;
						}
					}
				}
				if( (currentTime - mapChannel->timer_missileUpdate) >= 100 )
				{
					missile_check(mapChannel, 100);
					mapChannel->timer_missileUpdate += 100;
				}
				if( (currentTime - mapChannel->timer_dynObjUpdate) >= 100 )
				{
					dynamicObject_check(mapChannel, 100);
					mapChannel->timer_dynObjUpdate += 100;
				}
				if( (currentTime - mapChannel->timer_controller) >= 250 )
				{
					controller_mapChannelThink(mapChannel);
					mapChannel->timer_controller += 250;
				}
				if( (currentTime - mapChannel->timer_generalTimer) >= 100 )
				{
					int timePassed = 100;
					
					mapChannel_check_AutoFireTimers(mapChannel);
					// parse through all timers
					int count = hashTable_getCount(&mapChannel->ht_timerList);
					mapChannelTimer_t **timerList = (mapChannelTimer_t**)hashTable_getValueArray(&mapChannel->ht_timerList);
					for(int i=0; i<count; i++)
					{
						mapChannelTimer_t *entry = timerList[i];
						entry->timeLeft -= timePassed;
						if( entry->timeLeft <= 0 )
						{
							int objTimePassed = entry->period - entry->timeLeft;
							entry->timeLeft += entry->period;
							// trigger object
							bool remove = entry->cb(mapChannel, entry->param, objTimePassed);//dynamicObject_process(mapChannel, dynObjectWorkEntry->object, objTimePassed);
							if( remove == false )
								__debugbreak(); // todo!
						}
					}
					mapChannel->timer_generalTimer += 100;
				}
			} // (mapChannel->playerCount > 0)
		}
		Sleep(1); // eventually remove/replace this (dont sleep when too busy)
	}
	return 0;
}

void mapChannel_start(int *contextIdList, int contextCount)
{
	mapChannelList_t *mapList = (mapChannelList_t*)malloc(sizeof(mapChannelList_t));
	mapList->mapChannelArray = (mapChannel_t*)malloc(sizeof(mapChannel_t)*contextCount);
	mapList->mapChannelCount = 0;
	RtlZeroMemory(mapList->mapChannelArray, sizeof(mapChannel_t)*contextCount);
	for(int i=0; i<contextCount; i++)
	{
		int f = -1;
		// find by context
		for(int m=0; m<mapInfoCount; m++)
		{
			if( mapInfoArray[m].contextId == contextIdList[i] )
			{
				f = m;
				break;
			}
		}
		if( f == -1 )
		{
			printf("context %d not found in mapInfo.txt\n", contextIdList[i]);
		}
		// load all maps
		mapList->mapChannelArray[i].mapInfo = &mapInfoArray[f];
		hashTable_init(&mapList->mapChannelArray[i].ht_socketToClient, 128);
		hashTable_init(&mapList->mapChannelArray[i].ht_timerList, 8);
		mapList->mapChannelArray[i].timer_clientEffectUpdate = GetTickCount();
		mapList->mapChannelArray[i].timer_missileUpdate = GetTickCount();
		mapList->mapChannelArray[i].timer_dynObjUpdate = GetTickCount();
		mapList->mapChannelArray[i].timer_generalTimer = GetTickCount();
		mapList->mapChannelArray[i].timer_controller = GetTickCount();
		mapList->mapChannelArray[i].playerCount = 0;
		mapList->mapChannelArray[i].playerLimit = 128;
		mapList->mapChannelArray[i].playerList = (mapChannelClient_t**)malloc(sizeof(mapChannelClient_t*)*mapList->mapChannelArray[i].playerLimit);
		InitializeCriticalSection(&mapList->mapChannelArray[i].criticalSection);
		mapList->mapChannelCount++;
		// register mapChannel
		hashTable_set(&ht_mapChannelsByContextId, contextIdList[i], &mapList->mapChannelArray[i]);
	}
	// start the thread!
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mapChannel_worker, (LPVOID)mapList, 0, NULL);
}

void mapChannel_init()
{
	hashTable_init(&ht_mapChannelsByContextId, 8);
}

mapChannel_t *mapChannel_findByContextId(int contextId)
{
	return (mapChannel_t*)hashTable_get(&ht_mapChannelsByContextId, contextId);
}

bool mapChannel_pass(mapChannel_t *mapChannel, clientGamemain_t *cgm)
{
	int newWriteIndex = ((mapChannel->rb_playerQueueWriteIndex+1)%MAPCHANNEL_PLAYERQUEUE);
	if( newWriteIndex == mapChannel->rb_playerQueueReadIndex )
		return false; // error queue full
	mapChannel->rb_playerQueue[mapChannel->rb_playerQueueWriteIndex] = cgm;
	mapChannel->rb_playerQueueWriteIndex = newWriteIndex;
	return true;
}

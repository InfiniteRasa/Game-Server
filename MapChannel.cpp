#include "Global.h"

/*
	A MapChannel in general is a isolated thread of the server that is 100% responsible for one or multiple maps
	Whenever a client selects a character the clientinstance is passed to the MapChannel and is no more processed by the main thread.
	If a client changes map his instance is passed to the target MapChannel
*/

hashTable_t ht_mapChannelsByContextId;
mapChannelList_t *global_channelList; //20110827 @dennton

void mapteleporter_teleportEntity(sint32 destX,sint32 destY, sint32 destZ, sint32 mapContextId, mapChannelClient_t *player)
{
    destY += 700;
	printf("teleport to: x y z map - %d %d %d %d \n",destX,destY,destZ,mapContextId);
	
	//remove entity from old map - remove client from all channels
			communicator_playerExitMap(player);
			//unregister player
			//communicator_unregisterPlayer(cm);
			//remove visible entity
			Thread::LockMutex(&player->cgm->cs_general);
			cellMgr_removeFromWorld(player);
			// remove from list
			for(sint32 i=0; i<player->mapChannel->playerCount; i++)
			{
				if( player == player->mapChannel->playerList[i] )
				{
					if( i == player->mapChannel->playerCount-1 )
					{
						player->mapChannel->playerCount--;
					}
					else
					{
						player->mapChannel->playerList[i] = player->mapChannel->playerList[player->mapChannel->playerCount-1];
						player->mapChannel->playerCount--;
					}
					break;
				}
			}
			Thread::UnlockMutex(&player->cgm->cs_general);
				
			//############## map loading stuff ##############
			// send PreWonkavate (clientMethod.134)
			pyMarshalString_t pms;
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, 0); // wonkType - actually not used by the game
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(player->cgm, 5, 134, pym_getData(&pms), pym_getLen(&pms));
			// send Wonkavate (inputstateRouter.242)
			player->cgm->mapLoadContextId = mapContextId;
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, mapContextId);	// gameContextId (alias mapId)
			pym_addInt(&pms, 0);	// instanceId ( not important for now )
			// find map version
			sint32 mapVersion = 0; // default = 0;
			for(sint32 i=0; i<mapInfoCount; i++)
			{
				if( mapInfoArray[i].contextId == mapContextId )
				{
					mapVersion = mapInfoArray[i].version;
					break;
				}
			}
			pym_addInt(&pms, mapVersion);	// templateVersion ( from the map file? )
			pym_tuple_begin(&pms);  // startPosition
			pym_addFloat(&pms, destX); // x (todo: send as float)
			pym_addFloat(&pms, destY); // y (todo: send as float)
			pym_addFloat(&pms, destZ); // z (todo: send as float)
			pym_tuple_end(&pms);
			pym_addInt(&pms, 0);	// startRotation (todo, read from db and send as float)
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(player->cgm, 6, METHODID_WONKAVATE, pym_getData(&pms), pym_getLen(&pms));
			
			//################## player assigning ###############
			communicator_loginOk(player->mapChannel, player);
			communicator_playerEnterMap(player);
			//add entity to new map
			player->player->actor->posX = destX; 
			player->player->actor->posY = destY;
			player->player->actor->posZ = destZ;
			//cm->mapChannel->mapInfo->contextId = telepos.mapContextId;
		  
			
			player->player->controllerUser->inventory = player->inventory;
			player->player->controllerUser->mission = player->mission;
			player->tempCharacterData = player->player->controllerUser->tempCharacterData;

				
			//---search new mapchannel
			for(sint32 chan=0; chan < global_channelList->mapChannelCount; chan++)
			{
               mapChannel_t *mapChannel = global_channelList->mapChannelArray+chan;
			   if(mapChannel->mapInfo->contextId == mapContextId)
			   {
				   player->mapChannel = mapChannel;
				   break;
			   }
			}

			mapChannel_t *mapChannel = player->mapChannel;
			Thread::LockMutex(&player->mapChannel->criticalSection);
			mapChannel->playerList[mapChannel->playerCount] = player;
			mapChannel->playerCount++;
			hashTable_set(&mapChannel->ht_socketToClient, (uint32)player->cgm->socket, player);
			Thread::UnlockMutex(&mapChannel->criticalSection);
			
			player->player->actor->posX = destX; 
			player->player->actor->posY = destY;
			player->player->actor->posZ = destZ;
			cellMgr_addToWorld(player); //cellsint32roducing to player /from players
			// setCurrentContextId (clientMethod.362)
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, player->mapChannel->mapInfo->contextId);
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(player->cgm, 5, 362, pym_getData(&pms), pym_getLen(&pms));

}

void mapteleporter_checkForEntityInRange(mapChannel_t *mapChannel)
{
    
	pyMarshalString_t pms;

	sint32 tCount =0;
	float minimumRange = 1.8f;
	float difX = 0.0f;
	float difY = 0.0f;
	float difZ = 0.0f;
	float dist = 0.0f;
	minimumRange *= minimumRange;
	//test zoneteleporters map. should be builded from db
	sint32 **porting_locs = new sint32*[4];
	// values 0-9: source-contextid, source xyz ,dest xyz , dest-contextid, cell-x, cell-z
	porting_locs[0] = new sint32 [10]; // zone teleporter #1: wilderness -> divide
	porting_locs[0][0] = 1220;
	porting_locs[0][1] = 300;
	porting_locs[0][2] = 142;
	porting_locs[0][3] = -580;
	porting_locs[0][4] = -965;
	porting_locs[0][5] = 176;
	porting_locs[0][6] = 634;
	porting_locs[0][7] = 1148;
	porting_locs[0][8] = (uint32)((porting_locs[0][1] / CELL_SIZE) + CELL_BIAS);
	porting_locs[0][9] = (uint32)((porting_locs[0][3] / CELL_SIZE) + CELL_BIAS);
	porting_locs[1] = new sint32 [10]; // zone teleporter #1: divide -> wilderness
	porting_locs[1][0] = 1148;
	porting_locs[1][1] = -1008;
	porting_locs[1][2] = 180;
	porting_locs[1][3] = 671;
	porting_locs[1][4] = 280;
	porting_locs[1][5] = 152;
	porting_locs[1][6] = -538;
	porting_locs[1][7] = 1220;
	porting_locs[1][8] = (uint32)((porting_locs[1][1] / CELL_SIZE) + CELL_BIAS);
	porting_locs[1][9] = (uint32)((porting_locs[1][3] / CELL_SIZE) + CELL_BIAS);
	porting_locs[2] = new sint32 [10]; //zone zeleporter #2: wilderness -> divide
	porting_locs[2][0] = 1220;
	porting_locs[2][1] = 891;
	porting_locs[2][2] = 268;
	porting_locs[2][3] = 32;
	porting_locs[2][4] = 436;
	porting_locs[2][5] = 173;
	porting_locs[2][6] = 1193;
	porting_locs[2][7] = 1148;
	porting_locs[2][8] = (uint32)((porting_locs[2][1] / CELL_SIZE) + CELL_BIAS);
	porting_locs[2][9] = (uint32)((porting_locs[2][3] / CELL_SIZE) + CELL_BIAS);
	porting_locs[3] = new sint32 [10]; //zone teleporter #2: divide -> wilderness
	porting_locs[3][0] = 1148;
	porting_locs[3][1] = 499;
	porting_locs[3][2] = 184;
	porting_locs[3][3] = 1202;
	porting_locs[3][4] = 905;
	porting_locs[3][5] = 273;
	porting_locs[3][6] = 65;
	porting_locs[3][7] = 1220;
	porting_locs[3][8] = (uint32)((porting_locs[3][1] / CELL_SIZE) + CELL_BIAS);
	porting_locs[3][9] = (uint32)((porting_locs[3][3] / CELL_SIZE) + CELL_BIAS);
 
	//---search through the whole teleporter list
	for (sint32 x =0; x < 4; x++)
	{
		
	    float mPosX = porting_locs[x][1]; //teleporter x-pos 
		float mPosZ = porting_locs[x][3]; //		   z-pos
		//############ get teleporter mapcell ###################################
		mapCell_t *mapCell = cellMgr_tryGetCell(mapChannel, 
			                                    porting_locs[x][8], 
												porting_locs[x][9]);
		if(mapCell == NULL) continue;
		//############ get all players in current celllocation ###################
		mapChannelClient_t **playerList = NULL;
		tCount = mapCell->ht_playerNotifyList.size();
		playerList = &mapCell->ht_playerNotifyList[0];

		// check players in range
		for(sint32 i=0; i<tCount; i++)
		{
			if( playerList == NULL) break; //no player found
			mapChannelClient_t *player = playerList[i];
			if(player->player->actor->stats.healthCurrent<=0) break;
			difX = (sint32)(player->player->actor->posX) - mPosX;
			difZ = (sint32)(player->player->actor->posZ) - mPosZ;
			dist = difX*difX + difZ*difZ;
			//player(s) in range: do teleporting
			if( (dist <= minimumRange) &&   (porting_locs[x][0] == player->mapChannel->mapInfo->contextId))
			{
			    
				 mapteleporter_teleportEntity( porting_locs[x][4],
											   porting_locs[x][5],
											   porting_locs[x][6],
											   porting_locs[x][7],
											   player);
					
			}
		}//---for: playercount
	}//---for: all teleporter locations       				
}

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
	Thread::LockMutex(&mapChannel->criticalSection);
	mapChannel->playerList[mapChannel->playerCount] = mc;
	mapChannel->playerCount++;
	hashTable_set(&mapChannel->ht_socketToClient, (uint32)mc->cgm->socket, mc);
	Thread::UnlockMutex(&mapChannel->criticalSection);
}

void mapChannel_addNewPlayer(mapChannel_t *mapChannel, clientGamemain_t *cgm)
{
	mapChannelClient_t *mc = (mapChannelClient_t*)malloc(sizeof(mapChannelClient_t));
	memset((void*)mc, 0x00, sizeof(mapChannelClient_t));
	mc->cgm = cgm;
	mc->clientEntityId = entityMgr_getFreeEntityIdForClient(); // generate a entityId for the client instance
	mc->mapChannel = mapChannel;
	mc->player = NULL;
	DataInterface_Character_getCharacterData(cgm->userID, cgm->mapLoadSlotId, _cb_mapChannel_addNewPlayer, mc);
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
	//hashTable_set(&mapChannel->ht_socketToClient, (uint32)cgm->socket, mc);
	//// create new actor...
	//
	//// void DataInterface_Character_getCharacterData(unsigned long long userID, uint32 slotIndex, void (*cb)(void *param, diJob_characterData_t *jobData), void *param)
}

void mapChannel_removePlayer(mapChannelClient_t *client)
{
	// unregister mapChannelClient
	entityMgr_unregisterEntity(client->clientEntityId);

	communicator_unregisterPlayer(client);
	Thread::LockMutex(&client->cgm->cs_general);
	cellMgr_removeFromWorld(client);
	manifestation_removePlayerCharacter(client->mapChannel, client);
	if( client->disconnected == false )
		GameMain_PassClientToCharacterSelection(client->cgm);
	// remove from list
	for(sint32 i=0; i<client->mapChannel->playerCount; i++)
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
	Thread::UnlockMutex(&client->cgm->cs_general);
	free(client->cgm);
	free(client);
}

void mapChannel_registerTimer(mapChannel_t *mapChannel, sint32 period, void *param, bool (*cb)(mapChannel_t *mapChannel, void *param, sint32 timePassed))
{
	mapChannelTimer_t *timer = (mapChannelTimer_t*)malloc(sizeof(mapChannelTimer_t));
	timer->period = period;
	timer->timeLeft = period;
	timer->param = param;
	timer->cb = cb;
	mapChannel->timerList.push_back(timer);
}

void mapChannel_launchMissileForWeapon(mapChannelClient_t* client, item_t* weapon)
{
	switch(weapon->itemTemplate->weapon.toolType)
	{
	case 9:
		if(weapon->itemTemplate->item.classId == 29395)
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 120, 1, 287); 
		else
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 40, 1, 121); 
		break;
	case 10:
		missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 90, 1, 6); 
		break;
	case 8:
		missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 15, 1, 133); 
		break;
	case 7:
		missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 25, 1, 134); 
		break;
	case 15:
		if(weapon->itemTemplate->item.classId == 29757)
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 20, 149, 7); 
		else
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 20, 149, 1); 
		break;
	case 22:
		missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, 20, 1, 3); 
		break;
	default:
		printf("mapChannel_launchMissileForWeapon(): unknown weapontype\n");
		return;
	}
}


void mapChannel_registerAutoFireTimer(mapChannel_t *mapChannel, sint32 delay, manifestation_t* origin, item_t* weapon)
{
	mapChannelAutoFireTimer_t timer;
	timer.delay = delay;
	timer.timeLeft = delay;
	timer.origin = origin;
	timer.weapon = weapon;
	mapChannel->autoFire_timers.push_back(timer);
	// launch missile
	mapChannel_launchMissileForWeapon(origin->actor->owner, weapon);
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
			{
				mapChannel_launchMissileForWeapon(timer->origin->actor->owner, timer->weapon);
			}
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


void mapChannel_recv_mapLoaded(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
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

void mapChannel_recv_LogoutRequest(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyMarshalString_t pms;
	// send time remaining to logout
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0*1000); // milliseconds
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, 5, METHODID_LOGOUTTIMEREMAINING, pym_getData(&pms), pym_getLen(&pms));
	cm->logoutRequestedLast = GetTickCount();
	cm->logoutActive = true;
}

void mapChannel_recv_CharacterLogout(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyMarshalString_t pms;
	// pass to character selection
	if( cm->logoutActive == false )
		return;
	cm->removeFromMap = true;
}

void mapChannel_recv_ClearTrackingTarget(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyMarshalString_t pms;
	// send new tracking target
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addLong(&pms, 0); // tracking target - none
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, METHODID_SETTRACKINGTARGET, pym_getData(&pms), pym_getLen(&pms));
}

void mapChannel_recv_SetTrackingTarget(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	// unpack new tracking target
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	long long trackingTargetEntityId = pym_unpackLongLong(&pums);
	// send new tracking target
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addLong(&pms, trackingTargetEntityId); // tracking target
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, METHODID_SETTRACKINGTARGET, pym_getData(&pms), pym_getLen(&pms));
}

void mapChannel_processPythonRPC(mapChannelClient_t *cm, uint32 methodID, uint8 *pyString, sint32 pyStringLen)
{
	// check if 'O'
	if( *pyString != 'O' )
		__debugbreak(); // oh shit...
	pyString++; pyStringLen--;

	switch( methodID )
	{
	case METHODID_ALLOCATEATTRIBUTEPOINTS:
		manifestation_recv_AllocateAttributePoints(cm, pyString, pyStringLen);
		return;
	case METHODID_LEVELSKILLS:
		manifestation_recv_LevelSkills(cm, pyString, pyStringLen);
		return;
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
		// todo
		return;
	case METHODID_REQUESTTOGGLERUN: // ToggleRun
		manifestation_recv_ToggleRun(cm, pyString, pyStringLen);
		return;
	case 201: // SetTargetId
		manifestation_recv_SetTargetId(cm, pyString, pyStringLen);
		return;
	case METHODID_CLEARTRACKINGTARGET:
		mapChannel_recv_ClearTrackingTarget(cm, pyString, pyStringLen);
		return;
	case METHODID_SETTRACKINGTARGET:
		mapChannel_recv_SetTrackingTarget(cm, pyString, pyStringLen);
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
	case RequestVendorSale:
		npc_recv_RequestVendorSale(cm, pyString, pyStringLen);
		return;
	case RequestVendorPurchase:
		npc_recv_RequestVendorPurchase(cm, pyString, pyStringLen);
		return;
	case 431: // CompleteNPCObjective
		npc_recv_CompleteNPCObjective(cm, pyString, pyStringLen);
		return;
	case 430: // CompleteNPCMission
		npc_recv_CompleteNPCMission(cm, pyString, pyStringLen);
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
		printf("VisualCombatMode:\n");
		HexOut(pyString, pyStringLen);
		printf("\n\n");
		manifestation_recv_RequestVisualCombatMode(cm, pyString, pyStringLen);
		return;
	case 759: // RequestActioninterrupt
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
		if(inventory_CurrentWeapon(cm)->itemTemplate->item.classId == 27220)
			missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 5); // rifle
		else if(inventory_CurrentWeapon(cm)->itemTemplate->item.classId == 27320)
			missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 6); // shotgun
		else if(inventory_CurrentWeapon(cm)->itemTemplate->item.classId == 28066)
			missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 3); // machinegun
		else
			missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 4); // pistol
		return;
	case METHODID_REVIVEME: // dead player wish to go to the hospital
		manifestation_recv_Revive(cm, pyString, pyStringLen);
		return;
	case METHODID_SELECTWAYPOINT: // waypoint selected
		waypoint_recv_SelectWaypoint(cm, pyString, pyStringLen);
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
	sint32 contextId;
	sint32 pX;
	sint32 pY;
	sint32 pZ;
}movementLogEntry_t;
#pragma pack()

HANDLE hMovementLogFile = NULL;
void mapChannel_logMovement(sint32 contextId, sint32 x, sint32 y, sint32 z)
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


void mapChannel_decodeMovementPacket(mapChannelClient_t *mc, uint8 *data, uint32 len)
{
	if( mc->removeFromMap )
		return;
	if( mc->player == NULL )
		return;
	netCompressedMovement_t netMovement;
	uint32 pIdx = 0;
	uint32 counterA = *(uint32*)(data+pIdx); pIdx += 4;
	uint32 ukn1 = *(uint32*)(data+pIdx); pIdx += 4;
	uint32 counterB = *(uint32*)(data+pIdx); pIdx += 4;
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
	sint32 val24b = (data[pIdx+2]<<16) | (data[pIdx+4]<<8) | (data[pIdx+6]);
	if( val24b&0x00800000 )
		val24b |= 0xFF000000;
	netMovement.posX24b = val24b;
	float posX = (float)val24b / 256.0f;
	sint32 vLogX = val24b;
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
	sint32 vLogY = val24b;
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
	sint32 vLogZ = val24b;
	mc->player->actor->posZ = posZ;
	pIdx += 8;
	// read velocity
	//29 05 00 1A 2A velocity?	/1024.0
	if( data[pIdx] != 0x29 )
		__debugbreak();
	val24b = *(sint16*)(data+pIdx+2);
	netMovement.velocity = val24b;
	float velocity = (float)val24b / 1024.0f;
	if( data[pIdx+4] != 0x2A )
		__debugbreak();
	pIdx += 5;

	mapChannel_logMovement(mc->mapChannel->mapInfo->contextId, vLogX, vLogY, vLogZ);
	// read flag
	netMovement.flag = *(uint8*)(data+pIdx+1);
	pIdx += 2;
	// read viewX, viewY
	if( data[pIdx] != 0x29 )
		__debugbreak();
	val24b = *(sint16*)(data+pIdx+2);
	netMovement.viewX = val24b;
	float viewX = (float)val24b / 1024.0f; // factor guessed ??? find real
	val24b = *(sint16*)(data+pIdx+5);
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
	// prsint32 info
	//printf("move %f %f %f v: %f rXY: %f %f\n", posX, posY, posZ, velocity, viewX, viewY);
}

sint32 mapChannel_decodePacket(mapChannelClient_t *mc, uint8 *data, uint32 len)
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

	sint32 pIdx = 0;
	// read subSize
	uint32 subSize = *(uint16*)(data+pIdx); pIdx += 2; // redundancy with param len
	// read major opcode
	uint32 majorOpc = *(uint16*)(data+pIdx); pIdx += 2;
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
	uint8 ukn1 = *(uint8*)(data+pIdx); pIdx +=1;
	if( ukn1 != 2 )
		__debugbreak();

	uint8 opcode = *(uint8*)(data+pIdx); pIdx +=1; // not 100% sure
	uint8 ukn2 = *(uint8*)(data+pIdx); pIdx +=1;
	if( ukn2 != 0 )
		__debugbreak();
	uint8 xorCheckA = *(uint8*)(data+pIdx); pIdx +=1;
	if( xorCheckA != 3 ) // we only know headerA length of 3 for now
		__debugbreak();

	uint32 hdrB_start = pIdx;
	uint8 ukn3 = *(uint8*)(data+pIdx); pIdx +=1;
	if( ukn3 != 3 )
		__debugbreak();
	// different handling now (dont support subOpc 2 here anymore)
	if( opcode == 0x0C )
	{
		// expect header B part 1 (0x29)
		if( *(uint8*)(data+pIdx) == 0x00 )
			return 1; // empty packet?
		if( *(uint8*)(data+pIdx) != 0x29 )
			__debugbreak(); // wrong
		pIdx++;
		uint8 ukn0C_1 = *(uint8*)(data+pIdx); pIdx++;
		if( ukn0C_1 != 3 ) __debugbreak();
		uint8 ukn0C_2 = *(uint8*)(data+pIdx); pIdx++;
		//if( ukn0C_2 != 1 && ukn0C_2 != 3 && ukn0C_2 != 9 ) __debugbdfdsfreak(); // server entityId?
		if( ukn0C_2 == 0 || ukn0C_2 > 0x10 ) __debugbreak(); // server entityId?
		uint8 preffix0C_1 = *(uint8*)(data+pIdx); pIdx++;
		if( preffix0C_1 != 7 ) __debugbreak(); // 7 --> 32-bit sint32
		uint32 methodID = *(uint32*)(data+pIdx); pIdx += 4;
		uint8 ukn0C_3 = *(uint8*)(data+pIdx); pIdx++; // entityID?
		if( ukn0C_3 != 1 ) __debugbreak();
		// part 2 (0xCB)
		if( *(uint8*)(data+pIdx) != 0xCB )
			__debugbreak(); // wrong
		pIdx++;
		uint32 dataLen = 0;
		uint32 lenMask = *(uint8*)(data+pIdx); pIdx++;
		if( (lenMask>>6) == 0 )
		{
			// 6 bit length
			dataLen = lenMask&0x3F;
		}
		else if( (lenMask>>6) == 1 )
		{
			// 14 bit length
			dataLen = (lenMask&0x3F);
			dataLen |= ((*(uint8*)(data+pIdx))<<6);
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
		sint32 r = recv(cgm->socket, (char*)cgm->RecvBuffer+cgm->RecvState, 4-cgm->RecvState, 0);
		if( r == 0 || r == SOCKET_ERROR )
		{
			mc->removeFromMap = true;
			mc->disconnected = true;
			return;
		}
		cgm->RecvState += r;
		if( cgm->RecvState == 4 )
			cgm->RecvSize = *(uint32*)cgm->RecvBuffer + 4;
		return;
	}
	sint32 r = recv(cgm->socket, (char*)cgm->RecvBuffer+cgm->RecvState, cgm->RecvSize-cgm->RecvState, 0);
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
		Tabula_Decrypt2(&cgm->tbc2, (uint32*)(cgm->RecvBuffer+4), cgm->RecvSize);
		sint32 r = 0;
		sint32 AlignBytes = cgm->RecvBuffer[4]%9;

		uint8 *Buffer = cgm->RecvBuffer + 4 + AlignBytes;
		sint32 Size = cgm->RecvSize - 4 - AlignBytes;
		do{
			uint16 Subsize = *(uint16*)Buffer;
			mapChannel_decodePacket(mc, Buffer, Subsize);
			Buffer += Subsize;
			Size -= Subsize;
		}while(Size > 0);
		cgm->RecvState = 0;
		return;
	}
	return;
}

sint32 mapChannel_worker(mapChannelList_t *channelList)
{
	
	FD_SET fd;
	timeval sTimeout;
	sTimeout.tv_sec = 0;
	sTimeout.tv_usec = 10000;
	global_channelList = channelList; //20110827 @dennton

	// init mapchannel (map instance)
	printf("Initializing MapChannel...\n");
	for(sint32 chan=0; chan<channelList->mapChannelCount; chan++)
	{
		mapChannel_t *mapChannel = channelList->mapChannelArray+chan;
		if( cellMgr_initForMapChannel(mapChannel) == false )
		{
			printf("Error on map-cell init in mapContextId %d\n", mapChannel->mapInfo->contextId);
			Sleep(5000);
			ExitThread(-1);
		}
		navmesh_initForMapChannel(mapChannel);
		dynamicObject_init(mapChannel);
		mission_initForChannel(mapChannel);
		missile_initForMapchannel(mapChannel);
		spawnPool_initForMapChannel(mapChannel); //---todo:db use -done
		controller_initForMapChannel(mapChannel);
		teleporter_initForMapChannel(mapChannel); //---load teleporters
		printf("  Map: [%s]\n",mapChannel->mapInfo->name);
	}

	printf("MapChannel started...\n");

	while( true )
	{
		for(sint32 chan=0; chan<channelList->mapChannelCount; chan++)
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
			for(sint32 i=0; i<mapChannel->playerCount; i++)
			{
				FD_SET(mapChannel->playerList[i]->cgm->socket, &fd);
			}
			sint32 r = select(0, &fd, 0, 0, &sTimeout);
			if( r )
			{
				for(sint32 i=0; i<fd.fd_count; i++)
				{
					mapChannelClient_t *mc = (mapChannelClient_t*)hashTable_get(&mapChannel->ht_socketToClient, (uint32)fd.fd_array[i]);
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
				uint32 currentTime = GetTickCount();
				if( (currentTime - mapChannel->timer_clientEffectUpdate) >= 500 )
				{
					gameEffect_checkForPlayers(mapChannel->playerList, mapChannel->playerCount, 500);
					mapChannel->timer_clientEffectUpdate += 500;
				}
				//if (mapChannel->cp_trigger.cb != NULL)
				//{
				//	if ((currentTime - mapChannel->cp_trigger.period) >= 100)
				//	{
				//		mapChannel->cp_trigger.timeLeft -= 100;
				//		mapChannel->cp_trigger.period = currentTime;
				//		if (mapChannel->cp_trigger.timeLeft <= 0)
				//		{
				//			mapChannel->cp_trigger.cb(mapChannel, mapChannel->cp_trigger.param, 1);
				//			mapChannel->cp_trigger.cb = NULL;
				//		}
				//	}
				//}
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
					mapteleporter_checkForEntityInRange(mapChannel);
					controller_mapChannelThink(mapChannel);
					mapChannel->timer_controller += 250;
				}
				if( (currentTime - mapChannel->timer_playerUpdate) >= 1000 )
				{
					uint32 playerUpdateTick = currentTime - mapChannel->timer_playerUpdate;
					mapChannel->timer_playerUpdate = currentTime;
					for(uint32 i=0; i<mapChannel->playerCount; i++)
					{
						manifestation_updatePlayer(mapChannel->playerList[i], playerUpdateTick);
					}
				}
				if( (currentTime - mapChannel->timer_generalTimer) >= 100 )
				{
					sint32 timePassed = 100;
					// parse through all timers
					mapChannel_check_AutoFireTimers(mapChannel);
					std::vector<mapChannelTimer_t*>::iterator timer = mapChannel->timerList.begin();
					while (timer != mapChannel->timerList.end())
					{
						(*timer)->timeLeft -= timePassed;
						if( (*timer)->timeLeft <= 0 )
						{
							sint32 objTimePassed = (*timer)->period - (*timer)->timeLeft;
							(*timer)->timeLeft += (*timer)->period;
							// trigger object
							bool remove = (*timer)->cb(mapChannel, (*timer)->param, objTimePassed);
							if( remove == false )
								__debugbreak(); // todo!
						}
						timer++;
					}

					//sint32 count = hashTable_getCount(&mapChannel->list_timerList);
					//mapChannelTimer_t **timerList = (mapChannelTimer_t**)hashTable_getValueArray(&mapChannel->list_timerList);
					//for(sint32 i=0; i<count; i++)
					//{
					//	mapChannelTimer_t *entry = timerList[i];
					//	entry->timeLeft -= timePassed;
					//	if( entry->timeLeft <= 0 )
					//	{
					//		sint32 objTimePassed = entry->period - entry->timeLeft;
					//		entry->timeLeft += entry->period;
					//		// trigger object
					//		bool remove = entry->cb(mapChannel, entry->param, objTimePassed);//dynamicObject_process(mapChannel, dynObjectWorkEntry->object, objTimePassed);
					//		if( remove == false )
					//			__debugbreak(); // todo!
					//	}
					//}
					mapChannel->timer_generalTimer += 100;
				}
			} // (mapChannel->playerCount > 0)
		}
		Sleep(1); // eventually remove/replace this (dont sleep when too busy)
	}
	return 0;
}

void mapChannel_start(sint32 *contextIdList, sint32 contextCount)
{
	mapChannelList_t *mapList = (mapChannelList_t*)malloc(sizeof(mapChannelList_t));
	mapList->mapChannelArray = (mapChannel_t*)malloc(sizeof(mapChannel_t)*contextCount);
	mapList->mapChannelCount = 0;
	RtlZeroMemory(mapList->mapChannelArray, sizeof(mapChannel_t)*contextCount);
	for(sint32 i=0; i<contextCount; i++)
	{
		// call constructor to init std::vectors
		new(&mapList->mapChannelArray[i]) mapChannel_t();

		sint32 f = -1;
		// find by context
		for(sint32 m=0; m<mapInfoCount; m++)
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
		mapList->mapChannelArray[i].timer_clientEffectUpdate = GetTickCount();
		mapList->mapChannelArray[i].timer_missileUpdate = GetTickCount();
		mapList->mapChannelArray[i].timer_dynObjUpdate = GetTickCount();
		mapList->mapChannelArray[i].timer_generalTimer = GetTickCount();
		mapList->mapChannelArray[i].timer_controller = GetTickCount();
		mapList->mapChannelArray[i].timer_playerUpdate = GetTickCount();
		mapList->mapChannelArray[i].playerCount = 0;
		mapList->mapChannelArray[i].playerLimit = 128;
		mapList->mapChannelArray[i].playerList = (mapChannelClient_t**)malloc(sizeof(mapChannelClient_t*)*mapList->mapChannelArray[i].playerLimit);
		Thread::InitMutex(&mapList->mapChannelArray[i].criticalSection);
		mapList->mapChannelCount++;
		// register mapChannel
		hashTable_set(&ht_mapChannelsByContextId, contextIdList[i], &mapList->mapChannelArray[i]);
	}
	// start the thread!
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mapChannel_worker, (LPVOID)mapList, 0, NULL);
	Thread::New(NULL, (THREAD_ROUTINE)mapChannel_worker, mapList);
}

void mapChannel_init()
{
	hashTable_init(&ht_mapChannelsByContextId, 8);
}

mapChannel_t *mapChannel_findByContextId(sint32 contextId)
{
	return (mapChannel_t*)hashTable_get(&ht_mapChannelsByContextId, contextId);
}

bool mapChannel_pass(mapChannel_t *mapChannel, clientGamemain_t *cgm)
{
	sint32 newWriteIndex = ((mapChannel->rb_playerQueueWriteIndex+1)%MAPCHANNEL_PLAYERQUEUE);
	if( newWriteIndex == mapChannel->rb_playerQueueReadIndex )
		return false; // error queue full
	mapChannel->rb_playerQueue[mapChannel->rb_playerQueueWriteIndex] = cgm;
	mapChannel->rb_playerQueueWriteIndex = newWriteIndex;
	return true;
}

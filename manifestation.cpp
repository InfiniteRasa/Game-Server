#include"global.h"

/*
	manifestation is a player object (actor with more settings)
*/


/*
 * Called for the owner of the manifestation when it enters the map (after character selection)
 */
void manifestation_assignPlayer(mapChannel_t *mapChannel, mapChannelClient_t *owner, manifestation_t *m)
{
	pyMarshalString_t pms;
	// Recv_SetControlledActorId
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, m->actor->entityId); // actorId
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(owner->cgm, 5, METHODID_SETCONTROLLEDACTORID, pym_getData(&pms), pym_getLen(&pms));
	// set gameMap timeOfDay (Recv_SetSkyTime 198)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 6666666); // number of seconds the map has been up
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(owner->cgm, 7, 198, pym_getData(&pms), pym_getLen(&pms));
	// setCurrentContextId (clientMethod.362)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, owner->mapChannel->mapInfo->contextId);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(owner->cgm, 5, 362, pym_getData(&pms), pym_getLen(&pms));
	// Recv_UpdateRegions (568)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms);
	pym_addInt(&pms, owner->mapChannel->mapInfo->baseRegionId); //map with region support
	//pym_addInt(&pms, 1); - luna cavern
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(owner->cgm, owner->player->actor->entityId, 568, pym_getData(&pms), pym_getLen(&pms));
	// credits
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1); // creditType 1 -> fund
	pym_addInt(&pms, owner->player->credits);
	pym_tuple_end(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 2); // creditType 2 -> prestige
	pym_addInt(&pms, owner->player->prestige);
	pym_tuple_end(&pms);
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(owner->cgm, owner->player->actor->entityId, METHODID_ALLCREDITS, pym_getData(&pms), pym_getLen(&pms));
}

void manifestation_removeAppearanceItem(manifestation_t *manifestation, sint32 itemClassId)
{
	sint32 equipmentSlotId = gameData_getEquipmentClassIdSlot(itemClassId);
	if( equipmentSlotId == 0 )
	return;
	manifestation->actor->appearanceData[equipmentSlotId-1].classId = 0;
}

void manifestation_setAppearanceItem(manifestation_t *manifestation, sint32 itemClassId, uint32 hueAARRGGBB)
{
	sint32 equipmentSlotId = gameData_getEquipmentClassIdSlot(itemClassId);
	if( equipmentSlotId == 0 )
		return;
	manifestation->actor->appearanceData[equipmentSlotId-1].classId = itemClassId;
	manifestation->actor->appearanceData[equipmentSlotId-1].hue = hueAARRGGBB;
}

void manifestation_updateAppearance(mapChannelClient_t *owner)
{
	if( owner->player == NULL )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	for(sint32 i=0; i<21; i++)
	{
		if( owner->player->actor->appearanceData[i].classId == 0 )
			continue;
		pym_addInt(&pms, i+1); // index(equipmentSlotId)
		pym_tuple_begin(&pms);
			pym_addInt(&pms, owner->player->actor->appearanceData[i].classId); // classId
			// hue
			pym_tuple_begin(&pms);
				pym_addInt(&pms, (sint32)(owner->player->actor->appearanceData[i].hue&0xFF));
				pym_addInt(&pms, (sint32)((owner->player->actor->appearanceData[i].hue>>8)&0xFF));
				pym_addInt(&pms, (sint32)((owner->player->actor->appearanceData[i].hue>>16)&0xFF));
				pym_addInt(&pms, (sint32)((owner->player->actor->appearanceData[i].hue>>24)&0xFF));
			pym_tuple_end(&pms);
			pym_tuple_begin(&pms);
				pym_addInt(&pms, (sint32)(owner->player->actor->appearanceData[i].hue&0xFF));
				pym_addInt(&pms, (sint32)((owner->player->actor->appearanceData[i].hue>>8)&0xFF));
				pym_addInt(&pms, (sint32)((owner->player->actor->appearanceData[i].hue>>16)&0xFF));
				pym_addInt(&pms, (sint32)((owner->player->actor->appearanceData[i].hue>>24)&0xFF));
			pym_tuple_end(&pms);
		pym_tuple_end(&pms);
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(owner, owner->player->actor->entityId, 27, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Updates all stats values depending on level, spent attribute points, etc.
 * Does not send values to clients
 */
void manifestation_updateStatsValues(mapChannelClient_t *client)
{
	// used reference:
	// http://zeus.jrq.ch/trcb/ 
	// body
	client->player->actor->stats.bodyCurrent = 10 + (client->player->actor->stats.level-1)*2;
	client->player->actor->stats.bodyCurrentMax = client->player->actor->stats.bodyCurrent;
	client->player->actor->stats.bodyNormalMax = client->player->actor->stats.bodyCurrent;
	// mind
	client->player->actor->stats.mindCurrent = 10 + (client->player->actor->stats.level-1)*2;
	client->player->actor->stats.mindCurrentMax = client->player->actor->stats.mindCurrent;
	client->player->actor->stats.mindNormalMax = client->player->actor->stats.mindCurrent;
	// spirit
	client->player->actor->stats.spiritCurrent = 10 + (client->player->actor->stats.level-1)*2;
	client->player->actor->stats.spiritCurrentMax = client->player->actor->stats.spiritCurrent;
	client->player->actor->stats.spiritNormalMax = client->player->actor->stats.spiritCurrent;
	// health
	client->player->actor->stats.healthCurrentMax = client->player->actor->stats.bodyCurrent * 10;
	client->player->actor->stats.healthNormalMax = client->player->actor->stats.bodyCurrent * 10;
	client->player->actor->stats.healthCurrent = client->player->actor->stats.healthCurrentMax;
	// chi/power
	client->player->actor->stats.chiCurrentMax = 103 + (client->player->actor->stats.spiritCurrent-10) * 5;
	client->player->actor->stats.chiNormalMax = 103 + (client->player->actor->stats.spiritCurrent-10) * 5;
	client->player->actor->stats.chiCurrent = client->player->actor->stats.chiCurrentMax;
}

void manifestation_createPlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner, di_characterData_t *characterData)
{
	manifestation_t *manifestation = (manifestation_t*)malloc(sizeof(manifestation_t));
	memset(manifestation, 0x00, sizeof(manifestation_t));
	manifestation->actor = (actor_t*)malloc(sizeof(actor_t));
	memset(manifestation->actor, 0x00, sizeof(actor_t));
	for(sint32 i=0; i<SWAPSET_SIZE; i++)
	{
		manifestation->actor->appearanceData[i].classId = characterData->appearanceData[i].classId;
		manifestation->actor->appearanceData[i].hue = characterData->appearanceData[i].hue;
	}
	manifestation->controllerUser = owner;
	manifestation->actor->entityId = entityMgr_getFreeEntityIdForPlayer(); // generate an entityId
	manifestation->actor->owner = owner;
	// todo: Read values from db
	strcpy(manifestation->actor->name, characterData->unicodeName);
	strcpy(manifestation->actor->family, characterData->unicodeFamily);
	manifestation->actor->posX = characterData->posX;
	manifestation->actor->posY = characterData->posY;
	manifestation->actor->posZ = characterData->posZ;
	manifestation->actor->rotation = 0.0f;
	manifestation->actor->entityClassId = characterData->genderIsMale?692:691;
	manifestation->actor->activeEffects = NULL;
	manifestation->genderIsMale = characterData->genderIsMale;
	manifestation->raceId = characterData->raceID;
	manifestation->classId = 1; // Recruit
	/*
		1: 'RECRUIT',
		2: 'SOLDIER',
		3: 'SPECIALIST',
		4: 'COMMANDO',
		5: 'RANGER',
		6: 'SAPPER',
		7: 'BIOTECHNICIAN',
		8: 'GRENADIER',
		9: 'GUARDIAN',
		10: 'SNIPER',
		11: 'SPY',
		12: 'DEMOLITIONIST',
		13: 'ENGINEER',
		14: 'MEDIC',
		15: 'EXOBIOLOGIST'}
	*/
	manifestation->actor->isRunning = true;
	manifestation->actor->inCombatMode = false;
	manifestation->targetEntityId = 0;
	manifestation->currentAbilityDrawer = 0;
	memset(manifestation->abilityDrawer, 0, sizeof(manifestation->abilityDrawer));
	owner->player = manifestation;

	// set stats (should read them from the db)
	owner->player->actor->stats.level = 1;
	owner->player->credits = 0;
	owner->player->prestige = 0;
	owner->player->experience = 0;
	manifestation_updateStatsValues(owner);
	


	// introduce player to myself
	//manifestation_sint32roducteEntity(mapChannel, manifestation, owner);
	//// introduce others to new player
	//EnterCriticalSection(&mapChannel->criticalSection);
	//for(sint32 i=0; i<mapChannel->playerCount; i++)
	//{
	//	if( mapChannel->playerList[i]->player != manifestation )
	//		if( mapChannel->playerList[i]->player )
	//			manifestation_sint32roducteEntity(mapChannel, mapChannel->playerList[i]->player, owner);
	//}
	//LeaveCriticalSection(&mapChannel->criticalSection);
	// set controller and other local player information

	
	communicator_loginOk(mapChannel, owner);
	cellMgr_addToWorld(owner); // will introduce the player to all clients, including the current owner
	manifestation_assignPlayer(mapChannel, owner, manifestation);
}

void manifestation_removePlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner)
{
	// dont save character here, that is already done on logout
	if( owner->player == NULL )
		return;
	// destroy entity is already called by cellMgr
	// free manifest data
	free(owner->player->actor);
	free(owner->player);
}

void manifestation_cellIntroducePlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount)
{
	pyMarshalString_t pms;
	for(sint32 i=0; i<playerCount; i++)
	{
		if( playerList[i] == client )
			continue;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		mapChannelClient_t *tempClient = playerList[i];
		pym_addInt(&pms, tempClient->player->actor->entityId); // entityID
		pym_addInt(&pms, tempClient->player->actor->entityClassId); // classID
		pym_addNoneStruct(&pms); // entityData (dunno)
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
		// set attributes - Recv_AttributeInfo (29)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_dict_begin(&pms);
		// for all attributes the following values are send:
		// current, currentMax, normalMax, refreshIncrement, refreshPeriod
		// body
		pym_addInt(&pms, 1);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.bodyCurrent); // current (current value)
		pym_addInt(&pms, client->player->actor->stats.bodyCurrentMax); // currentMax (current max)
		pym_addInt(&pms, client->player->actor->stats.bodyNormalMax); // normalMax (max without bonus)
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod
		pym_tuple_end(&pms);
		// mind
		pym_addInt(&pms, 2);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.mindCurrent); // current (current value)
		pym_addInt(&pms, client->player->actor->stats.mindCurrentMax); // currentMax (current max)
		pym_addInt(&pms, client->player->actor->stats.mindNormalMax); // normalMax (max without bonus)
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod
		pym_tuple_end(&pms);
		// spirit
		pym_addInt(&pms, 3);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.spiritCurrent); // current (current value)
		pym_addInt(&pms, client->player->actor->stats.spiritCurrentMax); // currentMax (current max)
		pym_addInt(&pms, client->player->actor->stats.spiritNormalMax); // normalMax (max without bonus)
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod
		pym_tuple_end(&pms);
		// health
		pym_addInt(&pms, 4);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.healthCurrent); // current (current value)
	    pym_addInt(&pms, client->player->actor->stats.healthCurrentMax); // currentMax (current max)
		pym_addInt(&pms, client->player->actor->stats.healthNormalMax); // normalMax (max without bonus)
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod (seconds, float?)
		pym_tuple_end(&pms);
		// chi
		pym_addInt(&pms, 5);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.chiCurrent); // current (current value)
		pym_addInt(&pms, client->player->actor->stats.chiCurrentMax); // currentMax (current max)
		pym_addInt(&pms, client->player->actor->stats.chiNormalMax); // normalMax (max without bonus)
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod
		pym_tuple_end(&pms);
		// ???
		for(sint32 i=6; i<=10; i++)
		{
			pym_addInt(&pms, i);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, 0 + i*100); // current
			pym_addInt(&pms, 22800); // currentMax
			pym_addInt(&pms, 22800); // normalMax
			pym_addInt(&pms, 0); // refreshIncrement
			pym_addInt(&pms, 0); // refreshPeriod
			pym_tuple_end(&pms);
		}
		pym_dict_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, METHODID_ATTRIBUTEINFO, pym_getData(&pms), pym_getLen(&pms));
		// PreloadData
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 0); // weaponId
		pym_addInt(&pms, 25); // abilities
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 622, pym_getData(&pms), pym_getLen(&pms));
		// Recv_AppearanceData (27)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_dict_begin(&pms);
		for(sint32 i=0; i<21; i++)
		{
			if( tempClient->player->actor->appearanceData[i].classId == 0 )
				continue;
			pym_addInt(&pms, i+1); // index(equipmentSlotId)
			pym_tuple_begin(&pms);
			pym_addInt(&pms, tempClient->player->actor->appearanceData[i].classId); // classId
			// hue
			pym_tuple_begin(&pms);
				pym_addInt(&pms, (sint32)(tempClient->player->actor->appearanceData[i].hue&0xFF));
				pym_addInt(&pms, (sint32)((tempClient->player->actor->appearanceData[i].hue>>8)&0xFF));
				pym_addInt(&pms, (sint32)((tempClient->player->actor->appearanceData[i].hue>>16)&0xFF));
				pym_addInt(&pms, (sint32)((tempClient->player->actor->appearanceData[i].hue>>24)&0xFF));
			pym_tuple_end(&pms);
			// test .16
			pym_tuple_begin(&pms);
				pym_addInt(&pms, (sint32)(tempClient->player->actor->appearanceData[i].hue&0xFF));
				pym_addInt(&pms, (sint32)((tempClient->player->actor->appearanceData[i].hue>>8)&0xFF));
				pym_addInt(&pms, (sint32)((tempClient->player->actor->appearanceData[i].hue>>16)&0xFF));
				pym_addInt(&pms, (sint32)((tempClient->player->actor->appearanceData[i].hue>>24)&0xFF));
			pym_tuple_end(&pms);
			// end test .16
			pym_tuple_end(&pms);
		}
		pym_dict_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 27, pym_getData(&pms), pym_getLen(&pms));
		// set controller (Recv_ActorControllerInfo )
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true); // isPlayer
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 14, pym_getData(&pms), pym_getLen(&pms));
		// set level
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, tempClient->player->actor->stats.level); // level
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 103, pym_getData(&pms), pym_getLen(&pms));
		// set class
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, tempClient->player->classId); // class
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 40, pym_getData(&pms), pym_getLen(&pms));
		// set charname  - Recv_CharacterName (427)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addUnicode(&pms, tempClient->player->actor->name); // firstName
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 427, pym_getData(&pms), pym_getLen(&pms));
		// set actor name
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addUnicode(&pms, tempClient->player->actor->family); // lastName
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 16, pym_getData(&pms), pym_getLen(&pms));
		// set running
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, tempClient->player->actor->isRunning);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 96, pym_getData(&pms), pym_getLen(&pms));
		// set skills
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_list_begin(&pms);
		// skill firearms
			pym_tuple_begin(&pms);
				pym_addInt(&pms, 1);  // id
				pym_addInt(&pms, 5);// level
			pym_tuple_end(&pms);
		pym_list_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 548, pym_getData(&pms), pym_getLen(&pms));
		// Recv_WorldLocationDescriptor (243)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_tuple_begin(&pms); // position
		pym_addInt(&pms, tempClient->player->actor->posX); // x 
		pym_addInt(&pms, tempClient->player->actor->posY); // y 
		pym_addInt(&pms, tempClient->player->actor->posZ); // z 
		pym_tuple_end(&pms); 
		pym_tuple_begin(&pms); // rotation quaterninion
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 0.0f);
		pym_addFloat(&pms, 1.0f);
		pym_tuple_end(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 243, pym_getData(&pms), pym_getLen(&pms));
		// set target category
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 0); // 'FRIENDLY'
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, tempClient->player->actor->entityId, 211, pym_getData(&pms), pym_getLen(&pms));
		// send inital movement packet
		netCompressedMovement_t netMovement = {0};
		netMovement.entityId = tempClient->player->actor->entityId;
		netMovement.posX24b = tempClient->player->actor->posX * 256.0f;
		netMovement.posY24b = tempClient->player->actor->posY * 256.0f;
		netMovement.posZ24b = tempClient->player->actor->posZ * 256.0f;
		netMgr_sendEntityMovement(client->cgm, &netMovement);
	}
}

void manifestation_cellIntroduceClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount)
{
	pyMarshalString_t pms;
	// create manifestation/actor entity
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->entityId); // entityID
	pym_addInt(&pms, client->player->actor->entityClassId); // classID
	pym_addNoneStruct(&pms); // entityData (dunno)
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	}
	// set attributes - Recv_AttributeInfo (29)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	// body
	pym_addInt(&pms, 1);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->stats.bodyCurrent); // current (current value)
	pym_addInt(&pms, client->player->actor->stats.bodyCurrentMax); // currentMax (current max)
	pym_addInt(&pms, client->player->actor->stats.bodyNormalMax); // normalMax (max without bonus)
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// mind
	pym_addInt(&pms, 2);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->stats.mindCurrent); // current (current value)
	pym_addInt(&pms, client->player->actor->stats.mindCurrentMax); // currentMax (current max)
	pym_addInt(&pms, client->player->actor->stats.mindNormalMax); // normalMax (max without bonus)
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// spirit
	pym_addInt(&pms, 3);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->stats.spiritCurrent); // current (current value)
	pym_addInt(&pms, client->player->actor->stats.spiritCurrentMax); // currentMax (current max)
	pym_addInt(&pms, client->player->actor->stats.spiritNormalMax); // normalMax (max without bonus)
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// health
	pym_addInt(&pms, 4);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->stats.healthCurrent); // current (current value)
	pym_addInt(&pms, client->player->actor->stats.healthCurrentMax); // currentMax (current max)
	pym_addInt(&pms, client->player->actor->stats.healthNormalMax); // normalMax (max without bonus)
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod (seconds, float?)
	pym_tuple_end(&pms);
	// chi
	pym_addInt(&pms, 5);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->stats.chiCurrent); // current (current value)
	pym_addInt(&pms, client->player->actor->stats.chiCurrentMax); // currentMax (current max)
	pym_addInt(&pms, client->player->actor->stats.chiNormalMax); // normalMax (max without bonus)
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// misc
	for(sint32 i=6; i<=10; i++)
	{
		pym_addInt(&pms, i);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 0 + i*100); // current
		pym_addInt(&pms, 1000); // currentMax
		pym_addInt(&pms, 1000); // normalMax
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod
		pym_tuple_end(&pms);
	}
	/*
	Receive the initialization data for the attributes
	(BODY, MIND, SPIRIT, HEALTH, CHI) of this actor.
	@param attrDict - dict keyed by attr type, containing tuples of 
	(current, currentMax, normalMax, refreshIncrement, refreshPeriod)
	*/
	pym_dict_end(&pms);
	pym_tuple_end(&pms);

	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 29, pym_getData(&pms), pym_getLen(&pms));
	}

	// PreloadData
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0); // weaponId
	pym_addInt(&pms, 0); // abilities
	pym_tuple_end(&pms);


	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 622, pym_getData(&pms), pym_getLen(&pms));
	}
	// Recv_AppearanceData (27)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	for(sint32 i=0; i<21; i++)
	{
		if( client->player->actor->appearanceData[i].classId == 0 )
			continue;
		pym_addInt(&pms, i+1); // index(equipmentSlotId)
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->appearanceData[i].classId); // classId
		// hue
		pym_tuple_begin(&pms);
			pym_addInt(&pms, (sint32)(client->player->actor->appearanceData[i].hue&0xFF));
			pym_addInt(&pms, (sint32)((client->player->actor->appearanceData[i].hue>>8)&0xFF));
			pym_addInt(&pms, (sint32)((client->player->actor->appearanceData[i].hue>>16)&0xFF));
			pym_addInt(&pms, (sint32)((client->player->actor->appearanceData[i].hue>>24)&0xFF));
		pym_tuple_end(&pms);
		// test .16
		pym_tuple_begin(&pms);
			pym_addInt(&pms, (sint32)(client->player->actor->appearanceData[i].hue&0xFF));
			pym_addInt(&pms, (sint32)((client->player->actor->appearanceData[i].hue>>8)&0xFF));
			pym_addInt(&pms, (sint32)((client->player->actor->appearanceData[i].hue>>16)&0xFF));
			pym_addInt(&pms, (sint32)((client->player->actor->appearanceData[i].hue>>24)&0xFF));
		pym_tuple_end(&pms);
		// end test .16
		pym_tuple_end(&pms);
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 27, pym_getData(&pms), pym_getLen(&pms));
	}
	// set controller (Recv_ActorControllerInfo )
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, true); // isPlayer
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 14, pym_getData(&pms), pym_getLen(&pms));
	}
	// set level
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->stats.level); // level
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 103, pym_getData(&pms), pym_getLen(&pms));
	}
	// set class
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->classId); // class
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 40, pym_getData(&pms), pym_getLen(&pms));
	}
	// set charname  - Recv_CharacterName (427)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, client->player->actor->name); // firstName
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 427, pym_getData(&pms), pym_getLen(&pms));
	}
	// set actor name
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, client->player->actor->family); // lastName
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 16, pym_getData(&pms), pym_getLen(&pms));
	}
	// set running
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->isRunning);
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 96, pym_getData(&pms), pym_getLen(&pms));
	}
	// set logos tabula
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms);
	pym_addInt(&pms, 23); //power
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 477, pym_getData(&pms), pym_getLen(&pms));
	}
	// set skills (see skillData.pyo)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms);
		// skill firearms
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 4);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	// skill firearms
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 1);  // id
	pym_addInt(&pms, 5);	// level
		pym_tuple_end(&pms);
	// skill hand to hand
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 8);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	//	T1_RECRUIT_MOTOR_ASSIST_ARMOR
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 19);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	//T2_SOLDIER_REFLECTIVE_ARMOR
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 21);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	//T2_SOLDIER_RAGE
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 147);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	// T2_SOLDIER_MACHINE_GUN
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 22);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	// T3_COMMANDO_LAUNCHER
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 24);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	// T4_GRENADIER_PROPELLANT
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 40);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	//	T3_COMMANDO_GRAVITON_ARMOR
	pym_tuple_begin(&pms);
		pym_addInt(&pms, 39);  // id
		pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);
	//T1_SPRsint32
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 165);  // T1_RECRUIT_SPRsint32
	pym_addInt(&pms, 5);	// level
	pym_tuple_end(&pms);

	pym_tuple_begin(&pms);
	pym_addInt(&pms, 49);  // T1_RECRUIT_LIGHTNING
	pym_addInt(&pms, 1);// level
	pym_tuple_end(&pms);
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 548, pym_getData(&pms), pym_getLen(&pms));
	}
	// Recv_Abilities (id: 10, desc: must only be sent for the local manifestation)
	// set skills
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms);
	// ability sprsint32
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 401); // id
	pym_addInt(&pms, 5); // level
	pym_tuple_end(&pms);
	// ability lightning
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 194); // id
	pym_addInt(&pms, 1); // level
	pym_tuple_end(&pms);
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 10, pym_getData(&pms), pym_getLen(&pms));
	}
	// Recv_WorldLocationDescriptor (243)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_tuple_begin(&pms); // position
	pym_addInt(&pms, client->player->actor->posX); // x 
	pym_addInt(&pms, client->player->actor->posY); // y 
	pym_addInt(&pms, client->player->actor->posZ); // z 
	pym_tuple_end(&pms); 
	pym_tuple_begin(&pms); // rotation quaterninion
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 1.0f);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 243, pym_getData(&pms), pym_getLen(&pms));
	}
	// set target category
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0); // 'FRIENDLY'
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 211, pym_getData(&pms), pym_getLen(&pms));
	}
	// player flags (710)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0xFFFFFFF); // actually should be a tuple or list or values?
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, client->player->actor->entityId, 710, pym_getData(&pms), pym_getLen(&pms));
	}
	// send inital movement packet
	netCompressedMovement_t netMovement = {0};
	netMovement.entityId = client->player->actor->entityId;
	netMovement.posX24b = client->player->actor->posX * 256.0f;
	netMovement.posY24b = client->player->actor->posY * 256.0f;
	netMovement.posZ24b = client->player->actor->posZ * 256.0f;
	for(sint32 i=0; i<playerCount; i++)
	{
		netMgr_sendEntityMovement(playerList[i]->cgm, &netMovement);
	}
}

void manifestation_cellDiscardClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount)
{
	pyMarshalString_t pms;
	// destroy manifestation/actor entity
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->entityId); // entityID
	pym_tuple_end(&pms);
	for(sint32 i=0; i<playerCount; i++)
	{
		if( playerList[i] == client )
			continue;
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
	}
}

void manifestation_cellDiscardPlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount)
{
	pyMarshalString_t pms;
	// destroy manifestation/actor entity
	for(sint32 i=0; i<playerCount; i++)
	{
		if( playerList[i]->player == NULL )
			continue;
		if( playerList[i] == client )
			continue;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, playerList[i]->player->actor->entityId); // entityID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
	}
}

void manifestation_recv_ToggleRun(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	if( cm->player == NULL )
		return;
	// toggle running/walking
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	cm->player->actor->isRunning = !cm->player->actor->isRunning;
	pym_addInt(&pms, cm->player->actor->isRunning);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(cm, cm->player->actor->entityId, 96, pym_getData(&pms), pym_getLen(&pms));
}

void manifestation_recv_SetTargetId(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned long long entityId = pym_unpackLongLong(&pums);
	if( pums.unpackErrorEncountered )
		return;
	cm->player->targetEntityId = entityId;
}

void manifestation_recv_ClearTargetId(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	cm->player->targetEntityId = 0;
}

void manifestation_GainCredits(mapChannelClient_t *cm, sint32 credits)
{
	cm->player->credits += credits;
	// inform owner
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1); // creditType (1 -> fund)	
	pym_addInt(&pms, cm->player->credits); // new amount
	pym_addInt(&pms, credits); // delta
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, METHODID_UPDATECREDITS, pym_getData(&pms), pym_getLen(&pms));
}

void manifestation_GainPrestige(mapChannelClient_t *cm, sint32 prestige)
{
	cm->player->prestige += prestige;
	// inform owner
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 2); // creditType (2 -> prestige)	
	pym_addInt(&pms, cm->player->prestige); // new amount
	pym_addInt(&pms, prestige); // delta
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, METHODID_UPDATECREDITS, pym_getData(&pms), pym_getLen(&pms));
}


void manifestation_GainExperience(mapChannelClient_t *cm, sint32 experience)
{
	cm->player->experience += experience;
	// inform owner
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_tuple_begin(&pms); // values are packed
	pym_addInt(&pms, cm->player->experience); // total
	pym_addInt(&pms, experience); // gained
	pym_addInt(&pms, experience); // baseGained (maybe the value without all the modifiers?)
	pym_addInt(&pms, 0); // groupMod
	pym_addInt(&pms, 0); // streakMod
	pym_addInt(&pms, 0); // boosterMod
	pym_addInt(&pms, 0); // wasCritKill
	pym_addInt(&pms, 0); // wasTeamCritKill
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, METHODID_EXPERIENCECHANGED, pym_getData(&pms), pym_getLen(&pms));
	// check for level up 
	// since we can level up multiple levels at once, check as long as there is a level up possible
	while( true )
	{
		sint32 xpForLevelUp = manifestation_getLevelNeededExperience(cm->player->actor->stats.level+1);
		if( xpForLevelUp == -1 )
			return; // no level up possible
		if( cm->player->experience >= xpForLevelUp )
		{
			// level up
			cm->player->actor->stats.level++;
			// update stats
			// inform client of level up
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, cm->player->actor->stats.level); // newLevel
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, METHODID_LEVELUP, pym_getData(&pms), pym_getLen(&pms));
			// todo: For all others send Recv_setLevel() to update level display for this player
		}
		else
			break;
	}

}

void manifestation_SendAbilityDrawerBar(mapChannelClient_t *cm)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	sint32 start = cm->player->currentAbilityDrawer;
	sint32 end = start + 4;
	//printf("Sending Drawer\n");
	//printf("Start: %i - End: %i\n", start, end);
	for (sint32 i = start; i <= end; i++)
	{
		if (cm->player->abilityDrawer[i] != 0)
		{
			//printf("Slot %i -> ability id %i\n", i, cm->player->abilityDrawer[i]);
			pym_addInt(&pms, i);								// slotIdx
			pym_tuple_begin(&pms);
			pym_addInt(&pms, cm->player->abilityDrawer[i]);		// abilityId
			pym_addInt(&pms, cm->player->abilityLvDrawer[i]);	// level
			pym_addNoneStruct(&pms);							// itemId ( unknown purpose )
			pym_tuple_end(&pms);
		}
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 393, pym_getData(&pms), pym_getLen(&pms));
}

void manifestation_SendAbilityDrawerFull(mapChannelClient_t *cm)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	for (sint32 i = 0; i <= 24; i++)
	{
		if (cm->player->abilityDrawer[i] != 0)
		{
			pym_addInt(&pms, i);								// slotIdx
			pym_tuple_begin(&pms);
			pym_addInt(&pms, cm->player->abilityDrawer[i]);		// abilityId
			pym_addInt(&pms, cm->player->abilityLvDrawer[i]);	// level
			pym_addNoneStruct(&pms);							// itemId ( unknown purpose )
			pym_tuple_end(&pms);
		}
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 393, pym_getData(&pms), pym_getLen(&pms));
}

void manifestation_recv_RequestArmAbility(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 slot = pym_unpackInt(&pums);
	cm->player->currentAbilityDrawer = (sint8)slot;
	// Recv_AbilityDrawerSlot(self, slotNum, bRequested = True): 394
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, slot);
	pym_addBool(&pms, true);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 394, pym_getData(&pms), pym_getLen(&pms));
}

void manifestation_recv_RequestSetAbilitySlot(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	//printf("requesting set ability slot\n");
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 slot = pym_unpackInt(&pums);
	unsigned long long abilityId = pym_unpackLongLong(&pums);
	uint32 abilityLevel = (uint32)pym_unpackLongLong(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// todo: check if ability is available
	cm->player->abilityDrawer[slot] = (sint32)abilityId;
	cm->player->abilityLvDrawer[slot] = (sint32)abilityLevel;
	
	manifestation_SendAbilityDrawerFull(cm);
}
// 



/* weapon stats and combat handling */

void manifestation_recv_RequestVisualCombatMode(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;

	bool setting = pym_unpackBool(&pums);

	pyMarshalString_t pms;
	
	if (setting == true) // Enter combat mode
	{
		client->player->actor->inCombatMode = true;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true);
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, client->player->actor, client->player->actor->entityId, 753, pym_getData(&pms), pym_getLen(&pms));
	}
	else // Exit combat mode
	{
		client->player->actor->inCombatMode = false;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, false);
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, client->player->actor, client->player->actor->entityId, 753, pym_getData(&pms), pym_getLen(&pms));
	}
}

void manifestation_recv_StartAutoFire(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	float yaw = pym_unpackFloat(&pums);
	
	pyMarshalString_t pms;
	client->player->actor->inCombatMode = true;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, true);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, client->player->actor, client->player->actor->entityId, 753, pym_getData(&pms), pym_getLen(&pms));


 

	printf("%f at %I64d\n", yaw, client->player->targetEntityId);
	// TODO!

	printf("TODO: "); puts(__FUNCTION__);
	printf("target: %u\n", client->player->targetEntityId);

	//##################### Begin: if target is Mapchannel Client #################
	//desc: have to use mapchannel-client id instead of player-entity-id because
	//player-entity-id isnt registered, when new player is created(enter world)
	
	sint32 targetType = entityMgr_getEntityType(client->player->targetEntityId);
	sint32 newTargetEntityId = 0;
	if(targetType == 0) //1:client-type,0=player-type
	{
        mapCell_t *mapCell = cellMgr_tryGetCell(client->mapChannel, 
												client->player->actor->cellLocation.x, 
			                                    client->player->actor->cellLocation.z);	
		if(mapCell)
		{
             mapChannelClient_t **playerList = NULL;
			 sint32 tCount = mapCell->ht_playerNotifyList.size();
			 playerList = &mapCell->ht_playerNotifyList[0];
			 for(sint32 i=0; i < tCount; i++)
			 {

				 mapChannelClient_t *targetPlayer = playerList[i];
				 if(targetPlayer->player->actor->entityId == client->player->targetEntityId)
				 {
					 client->player->targetEntityId = targetPlayer->clientEntityId;
					 break; //player-entity-id found, now use client-entity-id for missile trigger
				 }
			 }
			 //void *entity = entityMgr_get();
		}
		
	}
	//##################### End: if target is Mapchannel Client #################

	if( client->player->targetEntityId )
	{
		
		switch(inventory_CurrentWeapon(client)->itemTemplate->toolType)
		{

		case 9:
			if(inventory_CurrentWeapon(client)->itemTemplate->classId == 29395)
			{
				missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_SHOTGUN_V3, 120); 
				mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player, MISSILE_SHOTGUN_V3);
			
			}
			else
			{
				missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_SHOTGUN, 40); 
				mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player, MISSILE_SHOTGUN);
			}
		break;
		case 10:
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_GRENADE, 90); 
  			mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player, MISSILE_NETGUN);
			//gameEffect_attach(client->mapChannel, client->player->targetEntityId, 258, 5, 3000);
			break;

		case 8:
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_PISTOL, 15); 
			mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player, MISSILE_PISTOL);
			break;

		case 7:
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_RIFLE, 25); 
  			mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player, MISSILE_RIFLE);
			break;
		case 15:
			if(inventory_CurrentWeapon(client)->itemTemplate->classId == 29757)
			{
				missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_MACHINEGUN_V3, 20); 
  				mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player,MISSILE_MACHINEGUN_V3);
			}
			else
			{
				missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_MACHINEGUN, 15); 
  				mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player,MISSILE_MACHINEGUN);
			}
			break;
		case 22:
			missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_PROPELLANT_ICE, 20); 
  			mapChannel_registerAutoFireTimer(client->mapChannel, inventory_CurrentWeapon(client)->itemTemplate->refireTime, client->player,MISSILE_MACHINEGUN);
			break;
         
		default:
			printf("unknown weapontype \n");
			return;

		}
		
	}//--if: targed id	
}

void manifestation_recv_StopAutoFire(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	
	pyMarshalString_t pms;
	client->player->actor->inCombatMode = false;
	pym_init(&pms);
	pym_tuple_begin(&pms);
  	pym_addBool(&pms, false);
	pym_tuple_end(&pms);
  	netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, client->player->actor, client->player->actor->entityId, 753, pym_getData(&pms), pym_getLen(&pms));

  	mapChannel_removeAutoFireTimer(client->mapChannel, client->player);
	printf("TODO: "); puts(__FUNCTION__);
	
}

void manifestation_recv_AutoFireKeepAlive(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;

	sint32 keepAliveDelay = pym_unpackInt(&pums);

	printf("TODO: "); puts(__FUNCTION__);
	printf("KeepAliveDelay: %i\r\n", keepAliveDelay);
	//if( client->player->targetEntityId )
	//missile_launch(client->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_PISTOL, 10);
}

void manifestation_updateWeaponReadyState(mapChannelClient_t *client)
{
	// todo: check weapon drawers
	// send weapon ready state
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, true);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(client, client->player->actor->entityId, 575, pym_getData(&pms), pym_getLen(&pms));

	// test enter combat state
	/*pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(client, client->player->actor->entityId, 718, pym_getData(&pms), pym_getLen(&pms));
*/

	// set state (206)
	// StateChange
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_list_begin(&pms);
	//pym_addInt(&pms, 4); // combat
	//pym_list_end(&pms);
	//pym_tuple_end(&pms);
	//netMgr_cellDomain_pythonAddMethodCallRaw(client, client->player->actor->entityId, 206, pym_getData(&pms), pym_getLen(&pms));	
	// test ... send windup (126)
	
	//for(sint32 i=0; i<100; i++)
	//{
	//	printf("%d\n", i);
	//	// _SetCurrentAction
	//	pym_init(&pms);
	//	pym_tuple_begin(&pms);
	//	pym_addInt(&pms, 1); // actionId
	//	pym_addInt(&pms, 81); // actionArg (use object?)
	//	pym_tuple_end(&pms);
	//	netMgr_pythonAddMethodCallRaw(client->mapChannel, client->player->actor->entityId, 126, pym_getData(&pms), pym_getLen(&pms));
	//	Sleep(500);
	//}

}


void manifestion_recv_revive(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	printf("Revive me requested- Size: %d\n", pyStringLen);


		
		// remove player from cell
		cellMgr_removeFromWorld(cm);
		// rebuild players  health/status /etc..
		cm->player->actor->stats.healthCurrent = cm->player->actor->stats.healthCurrentMax;
		// todo: use manifestation_updateStatsValues() to reset health?
		// update health (Recv_UpdateHealth 380) or 285
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, cm->player->actor->stats.healthCurrent); // current
		pym_addInt(&pms, cm->player->actor->stats.healthCurrentMax); // currentMax
		pym_addInt(&pms, 0); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel,
												 cm->player->actor, 
			                                     cm->player->actor->entityId, 380, 
												 pym_getData(&pms), pym_getLen(&pms));

		//cm->player->actor->state = -51;
		//	// alive!
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addNoneStruct(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel, 
				                                    cm->player->actor, 
													cm->player->actor->entityId, 
													METHODID_SWAPMESHREVIVE, 
													pym_getData(&pms), pym_getLen(&pms));
			// alive!
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_list_begin(&pms);
			pym_addInt(&pms, ACTOR_STATE_ALIVE); // standing
			pym_list_end(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel, 
				                                    cm->player->actor, 
													cm->player->actor->entityId, 
													METHODID_STATECHANGE, 
													pym_getData(&pms), pym_getLen(&pms));



			// send "revived" to client
			pym_init(&pms);			
			netMgr_cellDomain_pythonAddMethodCallRaw(cm->mapChannel, 
				                                    cm->player->actor, 
													cm->player->actor->entityId, 
													552, 
													pym_getData(&pms), pym_getLen(&pms));

			// introduce him back to cell.
			cellMgr_addToWorld(cm);
}

/*
 * Calculates the level from the current xp
 * Returns 1-50
 */
sint32 manifestation_getLevelByExperience(sint32 xp)
{
	// clamp xp value to valid range
	xp = min(max(xp,0),103410000);
	// do binary search to find value fast
	sint32 arraySize = sizeof(experienceTable)/sizeof(experienceTable[0]);
	sint32 jumpFactor = arraySize/4;
	sint32 index = arraySize/2;
	while( true )
	{
		if( xp >= experienceTable[index] && xp < experienceTable[index+1] )
			return index+1;
		if( xp < experienceTable[index] )
		{
			index -= jumpFactor;
			jumpFactor /= 2;
			jumpFactor = max(jumpFactor, 1); // needed since the array does not have a size of 2^n
		}
		else
		{
			index += jumpFactor;
			jumpFactor /= 2;
			jumpFactor = max(jumpFactor, 1); // needed since the array does not have a size of 2^n
		}
	}
	return 1; // cannot be reached
}

/*
 * Returns the amount needed to get the given level
 * level must be in range 1-50
 * returns -1 if level value impossible
 * If you want to know how much experience is needed to level up use
 * manifestation_getLevelNeededExperience(currentLevel+1)
 */
sint32 manifestation_getLevelNeededExperience(sint32 level)
{
	level--; // -1 as we pass the actual level (1-50) and need to translate it to array index (0-49)
	if( level < 0 )
		return -1;
	if( level > 49 )
		return -1;
	return experienceTable[level];
}
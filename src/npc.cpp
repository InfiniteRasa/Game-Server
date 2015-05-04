#include "global.h"


//void npc_test(mapChannelClient_t *client)
//{
//	//unsigned long long entityId = entityMgr_getFreeEntityIdForNPC();
//
///*
//20972
//00036286     64 - LOAD_CONST          'Vendor_Human_Female'
//*/
//
//	npc_t *npc = npc_createNPC(client->mapChannel, 20972, "Blah");
//	npc_setLocation(npc, client->player->actor->posX, client->player->actor->posY, client->player->actor->posZ, 0.0f, 0.0f);
//	
//	if( rand()%2 )
//	{
//		communicator_systemMessage(client, "type: VENDOR");
//		npc_setType(npc, NPC_TYPE_VENDOR);
//	}
//	else
//	{
//		communicator_systemMessage(client, "type: NONE");
//		npc_setType(npc, NPC_TYPE_NONE);
//	}
//	//if( rand()%2 )
//	//{
//	//	communicator_systemMessage(client, "hasMission: YES");
//
//	//	missionList_t *missionListData = (missionList_t*)malloc(sizeof(missionList_t)*2);
//	//	missionList_t *missionList = missionListData;
//	//	mission_t *missionData = (mission_t*)malloc(sizeof(mission_t)*2);
//	//	// mission 1
//	//	missionList->next = missionListData+1;
//	//	missionList->mission = missionData;
//	//	missionList->mission->firstCondition = NULL;
//	//	missionList->mission->missionId = 1449;
//	//	// mission 2
//	//	missionList = missionList->next;
//	//	missionList->next = NULL;
//	//	missionList->mission = missionData+1;
//	//	missionList->mission->firstCondition = NULL;
//	//	missionList->mission->missionId = 1450;
//
//	//	npc_setMissionList(npc, missionListData);
//	//}
//	//else
//	//	communicator_systemMessage(client, "hasMission: NO");
//
//	//npc_setMissionList(npc, )
//	cellMgr_addToWorld(client->mapChannel, npc);
//
//	/*
//		
//		1	CONVO_STATUS_NONE
//		2	CONVO_STATUS_UNAVAILABLE
//		3	CONVO_STATUS_AVAILABLE
//		4	CONVO_STATUS_OBJ_COMP
//		5	CONVO_STATUS_MIS_COMP
//		6	CONVO_STATUS_REWARD
//		7	CONVO_STATUS_MIS_REMINDER
//		8	CONVO_STATUS_OBJ_CHOICE
//		9	CONVO_STATUS_OBJ_AMB
//		10	CONVO_STATUS_VENDING
//		11	CONVO_STATUS_TRAIN
//		12	CONVO_STATUS_GREETING
//		13	CONVO_STATUS_IMPORTANT_GREETING
//		14	CONVO_STATUS_CLAN
//		15	CONVO_STATUS_AUCTIONEER (14 tested)
//		CONVO_STATUS_ESCORT
//		CONVO_STATUS_CRIT_DEATH
//		CONVO_STATUS_WARGAME_HOSTILE
//		CONVO_STATUS_WARGAME_FRIENDLY
//		CONVO_STATUS_WARGAME_NEUTRAL
//		CONVO_STATUS_WARGAME_HOSTILE_SAFE
//		CONVO_STATUS_WARGAME_FRIENDLY_SAFE
//		CONVO_STATUS_WARGAME_NEUTRAL_SAFE'
//
//	*/

/*
 * Updates the conversation status of all NPCs in sight range of the player
 */
void npc_globalUpdateConversationStatus(mapChannelClient_t *client)
{
	sint32 viewX1 = client->player->actor->cellLocation.x-CELL_VIEWRANGE;
	sint32 viewX2 = client->player->actor->cellLocation.x+CELL_VIEWRANGE;
	sint32 viewZ1 = client->player->actor->cellLocation.z-CELL_VIEWRANGE;
	sint32 viewZ2 = client->player->actor->cellLocation.z+CELL_VIEWRANGE;
	for(sint32 ix=viewX1; ix<=viewX2; ix++)
	{
		for(sint32 iz=viewZ1; iz<=viewZ2; iz++)
		{
			mapCell_t *nMapCell = cellMgr_tryGetCell(client->mapChannel, ix, iz);
			if( nMapCell )
			{
				// find all NPCs (creatures with NPC extension)
				std::vector<creature_t*>::iterator itrCreature = nMapCell->ht_creatureList.begin();
				while (itrCreature != nMapCell->ht_creatureList.end())
				{
					creature_t* creature = itrCreature[0];
					++itrCreature;
					if( creature->actor.stats.healthCurrent <= 0 )
						continue;
					// check if npc
					if( creature->type->npcData )
					{
						npc_creature_updateConversationStatus(client, creature);
					}
				}		
			}
		}
	}
}

/*
 * Used to send NPC specific data
 */
void npc_creature_updateConversationStatus(mapChannelClient_t *client, creature_t *creature)
{
	pyMarshalString_t pms;
	npcData_t* npcData = creature->type->npcData;
	// Recv_NPCConversationStatus (489)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	bool statusSet = false;
	for(uint32 i=0; i<npcData->relatedMissionCount; i++)
	{
		missionLogEntry_t* missionLogEntry = mission_findPlayerMission(client, npcData->relatedMissions[i].missionIndex);
		if( missionLogEntry )
		{
			// check specific state of mission
			mission_t* mission = mission_getByIndex(missionLogEntry->missionIndex);
			if( mission == NULL )
				continue;
			if( missionLogEntry->state >= mission->stateCount )
				continue;
			// search for objective or mission related updates
			sint32 scriptlineStart = mission->stateMapping[missionLogEntry->state];
			sint32 scriptlineEnd = mission->stateMapping[missionLogEntry->state+1];
			for(sint32 l=scriptlineStart; l<scriptlineEnd; l++)
			{
				missionScriptLine_t* scriptline = mission->scriptLines + l;
				if( scriptline->command == M_OP_COMPLETEOBJECTIVE )
				{
					if( creature->type->typeId == scriptline->value1 ) // same NPC?
					{
						// objective already completed?
						if( missionLogEntry->missionData[scriptline->storageIndex] == 1 )
							continue;
						// send objective completable flag
						pym_addInt(&pms, CONVO_STATUS_OBJ_COMP); // status - complete objective
						pym_tuple_begin(&pms); // data
						pym_tuple_end(&pms);
						statusSet = true;
						break;
					}
				}
				else if( scriptline->command == M_OP_COLLECTOR )
				{
					if( creature->type->typeId == scriptline->value1 ) // same NPC?
					{
						// mission already completed?
						if( missionLogEntry->state != (mission->stateCount-1) )
							continue;
						// send mission completable flag
						pym_addInt(&pms, CONVO_STATUS_MIS_COMP); // status - complete mission
						pym_tuple_begin(&pms); // data
						pym_tuple_end(&pms);
						statusSet = true;
						break;
					}
				}
			}
		}
		else if( mission_isCompletedByPlayer(client, npcData->relatedMissions[i].missionIndex) == false )
		{
			// check if the npc is actually the mission dispenser and not only a objective related npc
			if( mission_isCreatureMissionDispenser(mission_getByIndex(npcData->relatedMissions[i].missionIndex), creature) )
			{
				// mission available overwrites any other converse state
				pym_addInt(&pms, CONVO_STATUS_AVAILABLE); // status - available
				pym_tuple_begin(&pms); // data
				pym_tuple_end(&pms);
				statusSet = true;
				break;
			}
		}
	}
	// is NPC vendor?
	if( creature->type->vendorData && statusSet == false )
	{
		// creature->npcData.isVendor
		pym_addInt(&pms, CONVO_STATUS_VENDING); // status - vending
		pym_tuple_begin(&pms); // data
		pym_addInt(&pms, creature->type->vendorData->vendorPackageId); // vendorPackageId
		pym_tuple_end(&pms);
		statusSet = true;
	}
	// no status set yet? Send NONE conversation status
	if( statusSet == false )
	{
		// no other status, set NONE status
		pym_addInt(&pms, CONVO_STATUS_NONE); // status - none
		pym_tuple_begin(&pms); // data
		pym_tuple_end(&pms);
		statusSet = true;
	}
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 489, pym_getData(&pms), pym_getLen(&pms)); // Recv_NPCConversationStatus
}

/*
 * Send by the client when the 'continue' button is pressed when talking to a mission objective relevant NPC
 */
void npc_recv_CompleteNPCObjective(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	//npcId, missionId, objectiveId, playerFlagId
	unsigned long long entityId = pym_unpackLongLong(&pums);
	uint32 missionId = pym_unpackInt(&pums);
	//uint32 objectiveId = pym_unpackInt(&pums); not needed
	//uint32 playerFlagId = pym_unpackInt(&pums); not needed
	if( pums.unpackErrorEncountered )
		return;
	// get NPC from entityId
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_CREATURE )
		return;
	creature_t* creature = (creature_t*)entityMgr_get(entityId);
	if( creature == NULL )
		return;
	npcData_t* npcData = creature->type->npcData;
	if( npcData == NULL )
		return;
	// find the related mission
	mission_t* mission = NULL;
	for(sint32 i=0; i<npcData->relatedMissionCount; i++)
	{
		mission_t* rMission = mission_getByIndex(npcData->relatedMissions[i].missionIndex);
		if( rMission == NULL )
			continue;
		if( rMission->missionId == missionId )
		{
			mission = rMission;
			break;
		}
	}
	if( mission == NULL )
		return;
	// check if the player really is in the correct mission state to complete objective
	missionLogEntry_t* missionLogEntry = mission_findPlayerMission(client, mission->missionIndex);
	if( missionLogEntry == NULL )
		return; // player does not have mission
	// find objective
	if( missionLogEntry->state >= mission->stateCount )
		return;
	sint32 scriptlineStart = mission->stateMapping[missionLogEntry->state];
	sint32 scriptlineEnd = mission->stateMapping[missionLogEntry->state+1];
	for(sint32 l=scriptlineStart; l<scriptlineEnd; l++)
	{
		missionScriptLine_t* scriptline = mission->scriptLines + l;
		if( scriptline->command == M_OP_COMPLETEOBJECTIVE )
		{
			if( creature->type->typeId == scriptline->value1 )
			{
				// related scriptline for objective found
				// set tracking state in storage to 1
				missionLogEntry->missionData[scriptline->storageIndex] = 1; // 1 means 'completed'
				// try to progress current mission state
				mission_tryProgressMissionState(client, mission, missionLogEntry);
				break;
			}
		}
	}
	// update conversation status
	npc_creature_updateConversationStatus(client, creature);
}

/*
 * Send by the client when completing a mission at the debriefing/collector NPC
 */
void npc_recv_CompleteNPCMission(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	// npcId, missionId, selectionIdx, rating
	unsigned long long entityId = pym_unpackLongLong(&pums);
	uint32 missionId = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// get NPC from entityId
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_CREATURE )
		return;
	creature_t* creature = (creature_t*)entityMgr_get(entityId);
	if( creature == NULL )
		return;
	npcData_t* npcData = creature->type->npcData;
	if( npcData == NULL )
		return;
	// find the related mission
	mission_t* mission = NULL;
	for(sint32 i=0; i<npcData->relatedMissionCount; i++)
	{
		mission_t* rMission = mission_getByIndex(npcData->relatedMissions[i].missionIndex);
		if( rMission == NULL )
			continue;
		if( rMission->missionId == missionId )
		{
			mission = rMission;
			break;
		}
	}
	if( mission == NULL )
		return;
	// check if the player really is in the correct mission state to complete the mission
	missionLogEntry_t* missionLogEntry = mission_findPlayerMission(client, mission->missionIndex);
	if( missionLogEntry == NULL )
		return; // player does not have mission
	if( missionLogEntry->state != (mission->stateCount-1) )
		return; // mission not complete
	// everything ok, give reward
	sint32 rewardXp = mission_getReward_experience(mission);
	sint32 rewardPrestige = mission_getReward_prestige(mission);
	sint32 rewardCredits = mission_getReward_credits(mission);
	if( rewardXp > 0 )
		manifestation_GainExperience(client, rewardXp);
	if( rewardPrestige > 0 )
		manifestation_GainPrestige(client, rewardPrestige);
	if( rewardCredits > 0 )
		manifestation_GainCredits(client, rewardCredits);
	// todo: Item rewards (and item script commands too)
	// update mission state
	sint32 missionLogEntryIndex = -1;
	for(sint32 i=0; i<client->player->activeMissionCount; i++)
	{
		if( client->player->missionLog[i].missionIndex == mission->missionIndex )
		{
			missionLogEntryIndex = i;
			break;
		}
	}
	if( missionLogEntryIndex >= 0 )
	{
		// remove missionLogEntry
		client->player->activeMissionCount--;
		for(sint32 i=missionLogEntryIndex; i<client->player->activeMissionCount; i++)
		{
			memcpy(&client->player->missionLog[i], &client->player->missionLog[i+1], sizeof(missionLogEntry_t));
		}
	}
	client->player->missionStateMap[mission->missionIndex/8] |= (1<<(mission->missionIndex%8)); // set bit to 1 to mark mission as complete
	// send Recv_MissionRewarded (mission is done, remove from mission log)
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, mission->missionId);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, 486, pym_getData(&pms), pym_getLen(&pms));
	// update all NPCs around player in case there are suddenly new follow-up missions available
	npc_globalUpdateConversationStatus(client);
}

/*
 * Send by the client when the 'accept' button on the mission briefing window is pressed.
 */
void npc_recv_AssignNPCMission(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen) // alias 'AcceptMission'
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned long long entityId = pym_unpackLongLong(&pums);
	uint32 missionId = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_CREATURE )
		return;
	creature_t* creature = (creature_t*)entityMgr_get(entityId);
	if( creature == NULL )
		return;
	npcData_t* npcData = creature->type->npcData;
	if( npcData == NULL )
		return;
	// does have player have space for more missions?
	if( client->player->activeMissionCount >= MISSION_LOG_LIMIT )
	{
		// tell player that his mission log is full
		// todo: how did the original server tell the client?
		communicator_systemMessage(client, "Mission log is full.");
		return;
	}
	// find mission
	for(sint32 i=0; i<npcData->relatedMissionCount; i++)
	{
		mission_t* mission = mission_getByIndex(npcData->relatedMissions[i].missionIndex);
		if( mission == NULL )
			continue;
		if( mission->missionId == missionId )
		{
			// add mission to player mission list
			missionLogEntry_t* missionLogEntry = &client->player->missionLog[client->player->activeMissionCount];
			memset(missionLogEntry, 0x00, sizeof(missionLogEntry_t));
			missionLogEntry->missionIndex = mission->missionIndex;
			missionLogEntry->state = 1; // state 1 is forced when accepting mission
			client->player->activeMissionCount++;
			missionLog_clientGainedMission(client, mission);
			npc_creature_updateConversationStatus(client, creature);
			// todo: Update all NPCs in sight range that are related to this mission
			return;
		}
	}
}

// mission states only used in npc_recv_RequestNPCConverse()
#define MISSION_STATE_NOTACCEPTED	-1	// mission not yet accepted
#define MISSION_STATE_COMPLETED		-2	// mission already done (no more available)
#define MISSION_STATE_COMPLETEABLE	-3	// completed mission can be delivered to NPC

void npc_recv_RequestNPCConverse(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 actionId = pym_unpackInt(&pums);
	sint32 actionArgId = pym_unpackInt(&pums);
	unsigned long long entityId = pym_unpackLongLong(&pums);
	if( pums.unpackErrorEncountered )
		return;
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_CREATURE )
		return;
	creature_t* creature = (creature_t*)entityMgr_get(entityId);
	if( creature == NULL )
		return;
	npcData_t* npcData = creature->type->npcData;
	if( npcData == NULL )
		return;
	// Recv_Converse
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	/*
		convo types:
		CONVO_TYPE_GREETING = 0
		CONVO_TYPE_FORCETOPIC = 1
		CONVO_TYPE_MISSIONDISPENSE = 2
		CONVO_TYPE_MISSIONCOMPLETE = 3
		CONVO_TYPE_MISSIONREMINDER = 4
		CONVO_TYPE_OBJECTIVEAMBIENT = 5
		CONVO_TYPE_OBJECTIVECOMPLETE = 6
		CONVO_TYPE_MISSIONREWARD = 7
		CONVO_TYPE_OBJECTIVECHOICE = 8
		CONVO_TYPE_ENDCONVERSATION = 9
		CONVO_TYPE_TRAINING = 10
		CONVO_TYPE_VENDING = 11
		CONVO_TYPE_IMPORTANT_GREETING = 12
		CONVO_TYPE_CLAN = 13
		CONVO_TYPE_AUCTIONEER = 14
		CONVO_TYPE_FORCED_BY_SCRIPT = 15
	 */

	if( npcData->relatedMissionCount > 64 )
	{
		printf("NPC has more than 64 missions! Truncating list...\n"); 
		npcData->relatedMissionCount = 64;
	}
	// collect player state info about provided missions
	sint32 missionState[64]; // we assume no NPC will ever offer more than 64 missions, -2 means mission completed, -1 means not accepted
	for(uint32 i=0; i<npcData->relatedMissionCount; i++)
	{
		missionState[i] = MISSION_STATE_NOTACCEPTED;
		missionLogEntry_t* missionLogEntry = mission_findPlayerMission(client, npcData->relatedMissions[i].missionIndex);
		if( missionLogEntry )
		{
			// check specific state of mission
			mission_t* mission = mission_getByIndex(missionLogEntry->missionIndex);
			if( mission == NULL )
			{
				missionState[i] = MISSION_STATE_COMPLETED;
				continue;
			}
			if( missionLogEntry->state >= (mission->stateCount-1) )
			{
				missionState[i] = MISSION_STATE_COMPLETEABLE;
				continue;
			}
			// check specific state of mission
			missionState[i] = missionLogEntry->state;
		}
		else if( mission_isCompletedByPlayer(client, npcData->relatedMissions[i].missionIndex) == true )
		{
			missionState[i] = MISSION_STATE_COMPLETED;
		}
	}

	// count mission types
	sint32 countMissionAvailable = 0;
	sint32 countMissionCompletable = 0;
	sint32 countMissionObjectiveCompletable = 0;
	for(uint32 i=0; i<npcData->relatedMissionCount; i++)
	{
		if( missionState[i] == MISSION_STATE_COMPLETED )
			; // completed mission not available
		else if( missionState[i] == MISSION_STATE_NOTACCEPTED )
			countMissionAvailable++;
		else if( missionState[i] == MISSION_STATE_COMPLETEABLE )
			countMissionCompletable++;
		else if( missionState[i] > 0 )
			countMissionObjectiveCompletable++;
	}
	// build info about available missions
	if( countMissionAvailable > 0 )
	{
		pym_addInt(&pms, 2); // key: CONVO_TYPE_MISSIONDISPENSE
		pym_dict_begin(&pms); // mission list
		for(uint32 i=0; i<npcData->relatedMissionCount; i++)
		{
			if( missionState[i] == MISSION_STATE_NOTACCEPTED )
			{
				mission_t* mission = mission_getByIndex(npcData->relatedMissions[i].missionIndex);
				if( mission == NULL )
					continue;
				pym_addInt(&pms, mission->missionId); // missionID
				pym_tuple_begin(&pms);	// mission info
				pym_addInt(&pms, 1);	// level
				mission_buildRewardInfoTuple(mission, &pms);
				pym_addNoneStruct(&pms); // offerVOAudioSetId (NoneStruct for no-audio)
				pym_list_begin(&pms);	// itemsRequired
				pym_list_end(&pms);
				pym_list_begin(&pms);	// objectives
				pym_list_end(&pms);
				pym_addInt(&pms, MISSION_GROUPTYPE_SOLO); // groupType
				pym_tuple_end(&pms);
			}
		}
		pym_dict_end(&pms);
	}
	// build info about objectives
	if( countMissionObjectiveCompletable > 0 )
	{
		pym_addInt(&pms, 6); // key: CONVO_TYPE_OBJECTIVECOMPLETE
		pym_list_begin(&pms); // mission list
		for(uint32 i=0; i<npcData->relatedMissionCount; i++)
		{
			if( missionState[i] <= 0 )
				continue;
			mission_t* mission = mission_getByIndex(npcData->relatedMissions[i].missionIndex);
			sint32 scriptlineStart = mission->stateMapping[missionState[i]];
			sint32 scriptlineEnd = mission->stateMapping[missionState[i]+1];
			for(sint32 l=scriptlineStart; l<scriptlineEnd; l++)
			{
				missionScriptLine_t* scriptline = mission->scriptLines + l;
				if( scriptline->command == M_OP_COMPLETEOBJECTIVE )
				{
					if( creature->type->typeId == scriptline->value1 )
					{
						pym_tuple_begin(&pms);	// mission info
						pym_addInt(&pms, mission->missionId); // missionID
						pym_addInt(&pms, scriptline->value2); // objectiveId
						pym_addInt(&pms, scriptline->value3); // playerFlagId
						pym_tuple_end(&pms);
					}
				}
			}
		}
		pym_list_end(&pms);
	}
	// build info about completable missions
	if( countMissionCompletable > 0 )
	{
		pym_addInt(&pms, 3); // key: CONVO_TYPE_MISSIONCOMPLETE
		pym_dict_begin(&pms); // mission list
		for(uint32 i=0; i<npcData->relatedMissionCount; i++)
		{
			if( missionState[i] == MISSION_STATE_COMPLETEABLE )
			{
				mission_t* mission = mission_getByIndex(npcData->relatedMissions[i].missionIndex);
				if( mission == NULL )
					continue;
				pym_addInt(&pms, mission->missionId); // missionID
				mission_buildRewardInfoTuple(mission, &pms);
			}
		}
		pym_dict_end(&pms);
	}
	// build info about vendor data
	if( creature->type->vendorData )
	{
		pym_addInt(&pms, 11); // key: CONVO_TYPE_VENDING
		pym_list_begin(&pms); // vendor packageId list
		pym_addInt(&pms, creature->type->vendorData->vendorPackageId); // vendorPackageId (why do some parts support more than one vendorPackageId?)
		pym_list_end(&pms);
	}

	//mission_t *missionAvailableList[16];
	//sint32 missionAvailableCount; 
	//// send greeting
	//pym_addInt(&pms, 0); // key: CONVO_TYPE_GREETING
	////pym_tuple_begin(&pms); // greeting data
	//pym_addInt(&pms, 6); // blah

	//missionAvailableCount = 16; // limit
	//if( mission_completeableAvailableForClient(npc->missionList, client, npc, missionAvailableList, &missionAvailableCount) )
	//{
	//	if( missionAvailableCount > 0 )
	//	{
	//		// CONVO_TYPE_MISSIONCOMPLETE (3)
	//		pym_addInt(&pms, 3); // key: CONVO_TYPE_MISSIONCOMPLETE
	//		pym_dict_begin(&pms); // mission list
	//		for(sint32 i=0; i<missionAvailableCount; i++)
	//		{
	//			mission_t *mission = missionAvailableList[i];
	//			if( !mission )
	//				continue;
	//			pym_addInt(&pms, mission->missionId); // missionID
	//			pym_tuple_begin(&pms); // rewardInfo
	//			  pym_tuple_begin(&pms); // fixed redward
	//			     pym_list_begin(&pms); // fixedReward-credits
	//			     pym_list_end(&pms); // fixedReward-credits
	//			     pym_list_begin(&pms); // fixedReward-items
	//			     pym_list_end(&pms); // fixedReward-items
	//			  pym_tuple_end(&pms); // fixed redward
	//		      pym_list_begin(&pms); // selectionList
	//			  pym_list_end(&pms);
	//			pym_tuple_end(&pms); 
	//			/*
	//				rewardInfo(T):
	//					fixedReward(T)
	//						credits(List)
	//							Number of credits, can have multiple or no entry?
	//						fixedItems(List)
	//							items (	'itemTemplateId'
	//									'itemClassId'
	//									'quantity'
	//									'hue'
	//									'moduleIds'
	//									'qualityId' )
	//					selectionList(List)
	//							items (see above)
	//			*/
	//		}
	//		pym_dict_end(&pms); // mission list
	//	}
	//}

	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, creature->actor.entityId, 433, pym_getData(&pms), pym_getLen(&pms));
}

void npc_recv_RequestNPCVending(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned long long entityId = pym_unpackLongLong(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// get creature
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_CREATURE )
	{
		printf("npc_recv_RequestNPCVending: Target entity not a creature\n");
		return;
	}
	creature_t* creature = (creature_t*)entityMgr_get(entityId);
	if( creature == NULL || creature->type->vendorData == NULL )
	{
		printf("npc_recv_RequestNPCVending: Target entity is not valid\n");
		return;
	}
	vendorData_t* vendorData = creature->type->vendorData;
	// Vend (vendor augmentation specific)
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	// iterate sold item list
	for(sint32 i=0; i<vendorData->numberOfSoldItems; i++)
	{
		// note: We create the items in this function (on-demand) to avoid generating lots of possibly unused vendor items at server startup
		// do we need to create the 'display-item'?
		if( vendorData->soldItemList[i].itemInstance == NULL )
		{
			vendorData->soldItemList[i].itemInstance = item_createFromTemplateId(vendorData->soldItemList[i].itemTemplateId, 1);
			if( vendorData->soldItemList[i].itemInstance == NULL )
			{
				printf("npc_recv_RequestNPCVending: Failed to create item from itemTemplateId %d for vendor creatureTypeId %d\n", vendorData->soldItemList[i].itemTemplateId, creature->type->typeId);
				continue;
			}
		}
		// tell client about item entity
		item_t* vendingItem = vendorData->soldItemList[i].itemInstance;
		// dont forget to destroy the entity once the client stops shopping
		item_sendItemCreation(client, vendingItem);
		// add item data to vending list
		pym_addInt(&pms, vendingItem->entityId); // itemId
		pym_tuple_begin(&pms); // itemInfo
		pym_addInt(&pms, vendingItem->itemTemplate->item.buyPrice); // price
		pym_addInt(&pms, vendorData->soldItemList[i].sequence); // sequence? Probably the order of the items in the list...
		pym_tuple_end(&pms);
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, entityId, Vend, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Called by the client when an item is sold to a vendor
 * RPC parameters passed: (vendorEntityId, itemId, quantity)
 */
void npc_recv_RequestVendorSale(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned long long vendorEntityId = pym_unpackLongLong(&pums);
	unsigned long long itemEntityId = pym_unpackLongLong(&pums);
	sint32 itemQuantity = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// note: Players can only sell items directly from their personal inventory
	//       so we only have to scan there for the item entityId
	sint32 slotIndex = -1;
	for(sint32 i=0; i<250; i++)
	{
		if( (sint64)client->inventory.personalInventory[i] == (sint64)itemEntityId )
		{
			slotIndex = i;
			break;
		}
	}
	if( slotIndex == -1 )
	{
		printf("npc_recv_RequestVendorSale: Item entity not found in player's inventory\n");
		return;
	}
	// get item handle
	item_t* soldItem = (item_t*)entityMgr_get(itemEntityId);
	if( soldItem == NULL )
	{
		printf("npc_recv_RequestVendorSale: Item reference found but item instance does not exist\n");
		return;
	}
	// get sell price
	sint32 realItemQuantity = min(itemQuantity, soldItem->stacksize);
	sint32 sellPrice = soldItem->itemTemplate->item.sellPrice * realItemQuantity;
	// remove item
	// todo: Handle stacksizes correctly and only decrease item by quantity parameter
	inventory_removeItemBySlot(client, INVENTORY_PERSONAL, slotIndex);
	item_sendItemDestruction(client, soldItem);
	item_free(soldItem);
	// add credits to player
	manifestation_GainCredits(client, sellPrice);
}

/*
 * Called by the client when an item is bought from a vendor
 * RPC parameters passed: (vendorEntityId, itemId, quantity)
 */
void npc_recv_RequestVendorPurchase(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned long long vendorEntityId = pym_unpackLongLong(&pums);
	unsigned long long itemEntityId = pym_unpackLongLong(&pums);
	sint32 itemQuantity = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	if( itemQuantity <= 0 )
		return; // a quantity of less than 1 is not allowed
	// get the instance of the bought item
	item_t* selectedVendorItem = (item_t*)entityMgr_get(itemEntityId);
	if( selectedVendorItem == NULL )
	{
		printf("npc_recv_RequestVendorPurchase: The item instance does not exist\n");
		return;
	}
	// has the player enough credits?
	sint32 buyPrice = selectedVendorItem->itemTemplate->item.buyPrice * itemQuantity;
	if( client->player->credits < buyPrice )
		return; // not enough credits
	// duplicate item
	item_t* boughtItem = item_duplicate(selectedVendorItem, itemQuantity);
	if( boughtItem == NULL )
		return; // could not duplicate item
	// tell client about item
	item_sendItemCreation(client, boughtItem);
	// add item to inventory
	item_t* tempItemPtr = boughtItem;
	sint32 desiredStacksize = tempItemPtr->stacksize;
	boughtItem = inventory_addItemToInventory(client, boughtItem);
	if( boughtItem == NULL )
	{
		// item could not be added to inventory
		sint32 appliedRestStackSize = tempItemPtr->stacksize;
		if( appliedRestStackSize == desiredStacksize )
		{
			// not even 1x item could be added to the inventory
			item_sendItemDestruction(client, tempItemPtr);
			item_free(tempItemPtr);
			return;
		}
		// we were able to at least get a part of the stack into the inventory
		// destroy the rest and update the stacksize for the price
		item_sendItemDestruction(client, tempItemPtr);
		item_free(tempItemPtr);
		itemQuantity = (desiredStacksize - appliedRestStackSize);
	}
	// get correct buy price
	buyPrice = boughtItem->itemTemplate->item.buyPrice * itemQuantity;
	// remove credits
	manifestation_GainCredits(client, -buyPrice); // todo: maybe we should have a separate method for removing credits
}

/*
 * jobdata->outNpcList: contains a list with npc specific data like appearance (and later vendor data, auctioneer data etc.)
 * It is important that NPC data is loaded after creature types (npc data extends creature data)
 */
sint8 npcLoadState;
void _cb_npc_init(void *param, diJob_npcListData_t *jobData)
{
	for(sint32 i=0; i<jobData->outNpcCount; i++)
	{
		di_npcData_t *npcData = jobData->outNpcList+i;
		// find the creature type
		creatureType_t* creatureType = creatureType_findType(npcData->creatureTypeId);
		if( creatureType == NULL )
		{
			printf("Failed to find creature type %d for npc data\n", npcData->creatureTypeId);
			break;
		}
		// allocate and init npc data
		npcData_t* npc = (npcData_t*)malloc(sizeof(npcData_t));
		memset(npc, 0x00, sizeof(npcData_t));
		creatureType->npcData = npc;
		// set appearance info (maybe move appearance data into actor table?)
		for(sint32 s=0; s<SWAPSET_SIZE; s++)
		{
			creatureType->appearanceData[s].classId = npcData->appearanceData[s].classId;
			creatureType->appearanceData[s].hue = npcData->appearanceData[s].hue;
		}
		// set npcPackageInfo (needed only for mission objectives?)
		npc->npcPackageId = npcData->npcPackageId;
	}
	npcLoadState = 1;
}

// synchronous loading of npcs from the database
void npc_init()
{
	npcLoadState = 0;
	DataInterface_NPC_getNPCList(_cb_npc_init, NULL);
	while( npcLoadState == 0 ) Sleep(100);
}
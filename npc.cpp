#include "global.h"

void npc_test(mapChannelClient_t *client)
{
	//unsigned long long entityId = entityMgr_getFreeEntityIdForNPC();

/*
20972
00036286     64 - LOAD_CONST          'Vendor_Human_Female'
*/

	npc_t *npc = npc_createNPC(client->mapChannel, 20972, "Blah");
	npc_setLocation(npc, client->player->actor->posX, client->player->actor->posY, client->player->actor->posZ, 0.0f, 0.0f);
	
	if( rand()%2 )
	{
		communicator_systemMessage(client, "type: VENDOR");
		npc_setType(npc, NPC_TYPE_VENDOR);
	}
	else
	{
		communicator_systemMessage(client, "type: NONE");
		npc_setType(npc, NPC_TYPE_NONE);
	}
	//if( rand()%2 )
	//{
	//	communicator_systemMessage(client, "hasMission: YES");

	//	missionList_t *missionListData = (missionList_t*)malloc(sizeof(missionList_t)*2);
	//	missionList_t *missionList = missionListData;
	//	mission_t *missionData = (mission_t*)malloc(sizeof(mission_t)*2);
	//	// mission 1
	//	missionList->next = missionListData+1;
	//	missionList->mission = missionData;
	//	missionList->mission->firstCondition = NULL;
	//	missionList->mission->missionId = 1449;
	//	// mission 2
	//	missionList = missionList->next;
	//	missionList->next = NULL;
	//	missionList->mission = missionData+1;
	//	missionList->mission->firstCondition = NULL;
	//	missionList->mission->missionId = 1450;

	//	npc_setMissionList(npc, missionListData);
	//}
	//else
	//	communicator_systemMessage(client, "hasMission: NO");

	//npc_setMissionList(npc, )
	cellMgr_addToWorld(client->mapChannel, npc);

	/*
		
		1	CONVO_STATUS_NONE
		2	CONVO_STATUS_UNAVAILABLE
		3	CONVO_STATUS_AVAILABLE
		4	CONVO_STATUS_OBJ_COMP
		5	CONVO_STATUS_MIS_COMP
		6	CONVO_STATUS_REWARD
		7	CONVO_STATUS_MIS_REMINDER
		8	CONVO_STATUS_OBJ_CHOICE
		9	CONVO_STATUS_OBJ_AMB
		10	CONVO_STATUS_VENDING
		11	CONVO_STATUS_TRAIN
		12	CONVO_STATUS_GREETING
		13	CONVO_STATUS_IMPORTANT_GREETING
		14	CONVO_STATUS_CLAN
		15	CONVO_STATUS_AUCTIONEER (14 tested)
		CONVO_STATUS_ESCORT
		CONVO_STATUS_CRIT_DEATH
		CONVO_STATUS_WARGAME_HOSTILE
		CONVO_STATUS_WARGAME_FRIENDLY
		CONVO_STATUS_WARGAME_NEUTRAL
		CONVO_STATUS_WARGAME_HOSTILE_SAFE
		CONVO_STATUS_WARGAME_FRIENDLY_SAFE
		CONVO_STATUS_WARGAME_NEUTRAL_SAFE'

	*/
}

npc_t* npc_createNPC(mapChannel_t *mapChannel, int classId, char *name)
{
	npc_t *npc = (npc_t*)malloc(sizeof(npc_t));
	memset(npc, 0, sizeof(npc_t));
	npc->actor.entityClassId = classId;
	npc->entityId = entityMgr_getFreeEntityIdForNPC();
	npc->actor.entityId = npc->entityId;
	npc->missionList = NULL;

	strcpy(npc->actor.name, name);
	return npc;
}

npc_t* npc_createNPC(mapChannel_t *mapChannel, unsigned long long entityId, int classId, char *name)
{
	npc_t *npc = (npc_t*)malloc(sizeof(npc_t));
	memset(npc, 0, sizeof(npc_t));
	npc->actor.entityClassId = classId;
	npc->entityId = entityId;
	npc->actor.entityId = npc->entityId;
	npc->missionList = NULL;
	strcpy(npc->actor.name, name);
	// if entityId is known, also search for missions
	npc->missionList = mission_generateMissionListForNPC(npc);
	return npc;
}

void npc_setLocation(npc_t *npc, float x, float y, float z, float rX, float rY)
{
	npc->actor.posX = x;
	npc->actor.posY = y;
	npc->actor.posZ = z;
}

void npc_setType(npc_t *npc, int npcType)
{
	npc->npcType = npcType;
}

void npc_setAppearanceEntry(npc_t *npc, int entryIndex, int classId, unsigned int hue)
{
	if( entryIndex < 0 || entryIndex >= SWAPSET_SIZE )
		return;
	npc->actor.appearanceData[entryIndex].classId = classId;
	npc->actor.appearanceData[entryIndex].hue = hue;
}

void npc_setMissionList(npc_t *npc, missionList_t *missionList)
{
	npc->missionList = missionList;
}


void npc_createNPCOnClient(mapChannelClient_t *client, npc_t *npc)
{
	if( !npc )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, npc->entityId); // entityID
	pym_addInt(&pms, npc->actor.entityClassId); // classID
	pym_addNoneStruct(&pms); // entityData (dunno)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	// set attributes - Recv_AttributeInfo (29)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	// body
	pym_addInt(&pms, 1);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 15); // current
	pym_addInt(&pms, 15); // currentMax
	pym_addInt(&pms, 15); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// mind
	pym_addInt(&pms, 2);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10); // current
	pym_addInt(&pms, 0); // currentMax
	pym_addInt(&pms, 0); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// spirit
	pym_addInt(&pms, 3);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10); // current
	pym_addInt(&pms, 0); // currentMax
	pym_addInt(&pms, 0); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// health
	pym_addInt(&pms, 4);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 100); // current (current Max, base)
	pym_addInt(&pms, 100); // currentMax (modfierTarget?)
	pym_addInt(&pms, 2); // normalMax (current Value)
	pym_addInt(&pms, 8); // refreshIncrement
	pym_addInt(&pms, 3); // refreshPeriod (seconds, float?)
	pym_tuple_end(&pms);
	// chi
	pym_addInt(&pms, 5);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10); // current
	pym_addInt(&pms, 50); // currentMax
	pym_addInt(&pms, 100); // normalMax
	pym_addInt(&pms, 0); // refreshIncrement
	pym_addInt(&pms, 0); // refreshPeriod
	pym_tuple_end(&pms);
	// shit
	for(int i=6; i<=10; i++)
	{
		pym_addInt(&pms, i);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 10); // current
		pym_addInt(&pms, 50); // currentMax
		pym_addInt(&pms, 100); // normalMax
		pym_addInt(&pms, 0); // refreshIncrement
		pym_addInt(&pms, 0); // refreshPeriod
		pym_tuple_end(&pms);
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 29, pym_getData(&pms), pym_getLen(&pms));
	// PreloadData
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0); // weaponId
	pym_addInt(&pms, 0); // abilities
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 622, pym_getData(&pms), pym_getLen(&pms));
	// Recv_AppearanceData (27)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	for(int i=0; i<21; i++)
	{
		if( npc->actor.appearanceData[i].classId == 0 )
			continue;
		pym_addInt(&pms, i+1); // index(equipmentSlotId)
		pym_tuple_begin(&pms);
		pym_addInt(&pms, npc->actor.appearanceData[i].classId); // classId
		pym_tuple_begin(&pms);
		// hue
		pym_addInt(&pms, (int)(npc->actor.appearanceData[i].hue&0xFF));
		pym_addInt(&pms, (int)((npc->actor.appearanceData[i].hue>>8)&0xFF));
		pym_addInt(&pms, (int)((npc->actor.appearanceData[i].hue>>16)&0xFF));
		pym_addInt(&pms, (int)((npc->actor.appearanceData[i].hue>>24)&0xFF));
		pym_tuple_end(&pms);
		pym_tuple_end(&pms);
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 27, pym_getData(&pms), pym_getLen(&pms));
	// set level
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1); // level
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 103, pym_getData(&pms), pym_getLen(&pms));
	// set actor name
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, npc->actor.name); // name
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 16, pym_getData(&pms), pym_getLen(&pms));
	// set running
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 96, pym_getData(&pms), pym_getLen(&pms));
	// set skills
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms);
	// skill firearms
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1);  // id
	pym_addInt(&pms, 1);// level
	pym_tuple_end(&pms);
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 548, pym_getData(&pms), pym_getLen(&pms));
	// Recv_WorldLocationDescriptor (243)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_tuple_begin(&pms); // position
	pym_addInt(&pms, npc->actor.posX); // x 
	pym_addInt(&pms, npc->actor.posY); // y 
	pym_addInt(&pms, npc->actor.posZ); // z 
	pym_tuple_end(&pms); 
	pym_tuple_begin(&pms); // rotation quaterninion
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 1.0f);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 243, pym_getData(&pms), pym_getLen(&pms));
	// Test for weapons
	if( npc->actor.appearanceData[13].classId != 0 && strcmp(npc->actor.name, "Catherine Jones"))
	{
		/*pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true);
		pym_tuple_end(&pms);
		// Recv_RequestVisualCombatMode
		//RequestVisualCombatMode = 753
		netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 753, pym_getData(&pms), pym_getLen(&pms));*/

		// weapon ready
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 575, pym_getData(&pms), pym_getLen(&pms));
		/*pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 718, pym_getData(&pms), pym_getLen(&pms));*/
	}
	// set conversation status
	npc_updateConversationStatus(client, npc);
}

void npc_updateName(mapChannel_t *mapChannel, npc_t *npc, char *newName)
{
	pyMarshalString_t pms;
	strcpy(npc->actor.name, newName);
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, npc->actor.name); // name
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &npc->actor, npc->entityId, 16, pym_getData(&pms), pym_getLen(&pms));
}

void npc_updateAppearanceItem(mapChannel_t *mapChannel, npc_t *npc, unsigned int itemClassId, unsigned int hue)
{
	pyMarshalString_t pms;
	int equipmentSlotId = gameData_getEquipmentClassIdSlot(itemClassId);
	if( equipmentSlotId == 0 && equipmentSlotId >= SWAPSET_SIZE )
		return; // unknown starter item class id
	npc->actor.appearanceData[equipmentSlotId].classId = itemClassId;
	npc->actor.appearanceData[equipmentSlotId].hue = hue;
	// send update appearance
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	for(int i=0; i<21; i++)
	{
		if( npc->actor.appearanceData[i].classId == 0 )
			continue;
		pym_addInt(&pms, i+1);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, npc->actor.appearanceData[i].classId); // classId
		pym_tuple_begin(&pms);
		// hue
		pym_addInt(&pms, (int)(npc->actor.appearanceData[i].hue&0xFF));
		pym_addInt(&pms, (int)((npc->actor.appearanceData[i].hue>>8)&0xFF));
		pym_addInt(&pms, (int)((npc->actor.appearanceData[i].hue>>16)&0xFF));
		pym_addInt(&pms, (int)((npc->actor.appearanceData[i].hue>>24)&0xFF));
		pym_tuple_end(&pms);
		pym_tuple_end(&pms);
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &npc->actor, npc->entityId, 27, pym_getData(&pms), pym_getLen(&pms));
}

void npc_updateConversationStatus(mapChannelClient_t *client, npc_t *npc)
{
	pyMarshalString_t pms;
	// Recv_NPCConversationStatus (489)
	pym_init(&pms);
	pym_tuple_begin(&pms);

	mission_t *missionAvailableList[16];
	int missionAvailableCount = 16; 
	int missionCompletableCount = 16; 
	if( mission_completeableAvailableForClient(npc->missionList, client, npc, missionAvailableList, &missionAvailableCount) )
	{
		// mission available overwrites any other converse state
		pym_addInt(&pms, 4); // status - CONVO_TYPE_MISSIONCOMPLETE
		pym_tuple_begin(&pms); // data
		pym_tuple_end(&pms);
	}
	else if( mission_newAvailableForClient(npc->missionList, client, npc, missionAvailableList, &missionCompletableCount) )
	{
		// mission available overwrites any other converse state
		pym_addInt(&pms, 2); // status - available
		pym_tuple_begin(&pms); // data
		//pym_addInt(&pms, 10); // package: weapons
		pym_tuple_end(&pms);
	}
	else
	{
		// check for other
		if( npc->npcType == NPC_TYPE_VENDOR )
		{
			// status vending
			pym_addInt(&pms, 9); // status - vending
			pym_tuple_begin(&pms); // data
			pym_addInt(&pms, 10); // package: weapons
			pym_tuple_end(&pms);
		}
		else if( npc->npcType == NPC_TYPE_NONE )
		{
			// status none
			pym_addInt(&pms, 0); // status - none
			pym_tuple_begin(&pms); // data
			pym_tuple_end(&pms);
		}
		else
		{
			printf("Unknown NPC type(%d)\n", npc->npcType);
			return;
		}
	}

	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, npc->entityId, 489, pym_getData(&pms), pym_getLen(&pms));

}


void npc_recv_AssignNPCMission(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen) // alias 'AcceptMission'
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned long long entityId = pym_unpackLongLong(&pums);
	unsigned int missionId = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_NPC )
		return;
	npc_t *npc = (npc_t*)entityMgr_get(entityId);
	if( npc == NULL )
		return;
	// check if mission is available
	mission_t *missionAvailableList[16];
	int missionAvailableCount = 16; 
	// todo: check for mission completed/update available
	if( mission_newAvailableForClient(npc->missionList, client, npc, missionAvailableList, &missionAvailableCount) == false )
		return;
	mission_t *mission = NULL;
	for(int i=0; i<missionAvailableCount; i++)
	{
		if( missionAvailableList[i] == NULL )
			continue;
		if( missionAvailableList[i]->missionId == missionId )
		{
			mission = missionAvailableList[i];
			break;
		}
	}
	if( mission == NULL )
		return; // mission not available
	// accept mission
	mission_acceptForClient(client, mission);
	// update conversation status (mission state change)
	npc_updateConversationStatus(client, npc);
	// also update collector NPC if different, existing and visible
	if( mission->npcCollectorEntityId )
	{
		if( mission->npcCollectorEntityId != mission->npcDispenserEntityId )
		{
			if( entityMgr_getEntityType(mission->npcCollectorEntityId) != ENTITYTYPE_NPC )
				return;
			npc = (npc_t*)entityMgr_get(mission->npcCollectorEntityId);
			if( npc == NULL )
				return;
			// should check for visibility?
			npc_updateConversationStatus(client, npc);
		}
	}
}

void npc_recv_RequestNPCConverse(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int actionId = pym_unpackInt(&pums);
	int actionArgId = pym_unpackInt(&pums);
	unsigned long long entityId = pym_unpackLongLong(&pums);
	if( pums.unpackErrorEncountered )
		return;

	if( entityMgr_getEntityType(entityId) != ENTITYTYPE_NPC )
		return;
	npc_t *npc = (npc_t*)entityMgr_get(entityId);
	if( npc == NULL )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);

	mission_t *missionAvailableList[16];
	int missionAvailableCount; 
	// send greeting
	pym_addInt(&pms, 0); // key: CONVO_TYPE_GREETING
	//pym_tuple_begin(&pms); // greeting data
	pym_addInt(&pms, 6); // blah

	missionAvailableCount = 16; // limit
	if( mission_completeableAvailableForClient(npc->missionList, client, npc, missionAvailableList, &missionAvailableCount) )
	{
		if( missionAvailableCount > 0 )
		{
			// CONVO_TYPE_MISSIONCOMPLETE (3)
			pym_addInt(&pms, 3); // key: CONVO_TYPE_MISSIONCOMPLETE
			pym_dict_begin(&pms); // mission list
			for(int i=0; i<missionAvailableCount; i++)
			{
				mission_t *mission = missionAvailableList[i];
				if( !mission )
					continue;
				pym_addInt(&pms, mission->missionId); // missionID
				pym_tuple_begin(&pms); // rewardInfo
				  pym_tuple_begin(&pms); // fixed redward
				     pym_list_begin(&pms); // fixedReward-credits
				     pym_list_end(&pms); // fixedReward-credits
				     pym_list_begin(&pms); // fixedReward-items
				     pym_list_end(&pms); // fixedReward-items
				  pym_tuple_end(&pms); // fixed redward
			      pym_list_begin(&pms); // selectionList
				  pym_list_end(&pms);
				pym_tuple_end(&pms); 
				/*
					rewardInfo(T):
						fixedReward(T)
							credits(List)
								Number of credits, can have multiple or no entry?
							fixedItems(List)
								items (	'itemTemplateId'
										'itemClassId'
										'quantity'
										'hue'
										'moduleIds'
										'qualityId' )
						selectionList(List)
								items (see above)
				*/
			}
			pym_dict_end(&pms); // mission list
		}
	}
	missionAvailableCount = 16; // limit
	if( mission_newAvailableForClient(npc->missionList, client, npc, missionAvailableList, &missionAvailableCount) )
	{
		if( missionAvailableCount > 0 )
		{
			// mission info
			pym_addInt(&pms, 2); // key: CONVO_TYPE_MISSIONDISPENSE
			pym_dict_begin(&pms); // mission list

				for(int i=0; i<missionAvailableCount; i++)
				{
					mission_t *mission = missionAvailableList[i];
					if( !mission )
						continue;
					pym_addInt(&pms, mission->missionId); // missionID
					pym_tuple_begin(&pms); // mission info
					pym_addInt(&pms, 1);		// level
					pym_tuple_begin(&pms); // rewardInfo
					pym_tuple_begin(&pms); // fixed redward
					pym_list_begin(&pms); // fixedReward-credits
					pym_list_end(&pms); // fixedReward-credits
					pym_list_begin(&pms); // fixedReward-items
					pym_list_end(&pms); // fixedReward-items
					pym_tuple_end(&pms); // fixed redward
					pym_list_begin(&pms); // selectionList
					pym_list_end(&pms);
					pym_tuple_end(&pms); 
					pym_addNoneStruct(&pms);	// offerVOAudioSetId (NoneStruct for no-audio)
					pym_list_begin(&pms); // itemsRequired
					pym_list_end(&pms);
					pym_list_begin(&pms); // objectives
					pym_list_end(&pms);
					pym_addInt(&pms, 1); // groupType
					/*
					GroupType:
					1: 'SOLO',
					2: 'DUO',
					3: 'GROUP',
					4: 'RAID'}
					*/
					pym_tuple_end(&pms);
				}
			pym_dict_end(&pms);
		}
	}
	if( npc->npcType == NPC_TYPE_VENDOR )
	{
		// vending info
		pym_addInt(&pms, 11); // key: CONVO_TYPE_VENDING
		pym_tuple_begin(&pms); // vendor data: packageIds
			pym_addInt(&pms, 10); // weapon
		pym_tuple_end(&pms);
	}
	else if( npc->npcType == NPC_TYPE_NONE )
	{
		// nothing
	}
	else
		printf("Unknown NPC type (RequestNPCConverse)\n");
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, entityId, 433, pym_getData(&pms), pym_getLen(&pms));
}

void npc_recv_RequestNPCVending(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned long long entityId = pym_unpackLongLong(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// Vend (vendor augmentation specific)
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
		// test Item A
		pym_addInt(&pms, client->inventory.homeInventory[0]); // itemId
		pym_tuple_begin(&pms); // itemInfo
			pym_addInt(&pms, 1); // price
			pym_addInt(&pms, 1); // entityId?
		pym_tuple_end(&pms);
	// list of item entityIds?
	//pym_addInt(&pms, 10); // weapon
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, entityId, 569, pym_getData(&pms), pym_getLen(&pms));
}


// 1:n
void npc_cellIntroduceNPCToClients(mapChannel_t *mapChannel, npc_t *npc, mapChannelClient_t **playerList, int playerCount)
{
	for(int i=0; i<playerCount; i++)
	{
		npc_createNPCOnClient(playerList[i], npc);
	}
}

// n:1
void npc_cellIntroduceNPCsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, npc_t **npcList, int npcCount)
{
	for(int i=0; i<npcCount; i++)
	{
		npc_createNPCOnClient(client, npcList[i]);
	}
}

void npc_cellDiscardNPCToClients(mapChannel_t *mapChannel, npc_t *npc, mapChannelClient_t **playerList, int playerCount)
{
	if( !npc )
		return;
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, npc->actor.entityId); // entityID
	pym_tuple_end(&pms);
	for(int i=0; i<playerCount; i++)
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
}

void npc_cellDiscardNPCsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, npc_t **npcList, int npcCount)
{
	if( !client )
		return;
	pyMarshalString_t pms;
	for(int i=0; i<npcCount; i++)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, npcList[i]->actor.entityId); // entityID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, 5, 56, pym_getData(&pms), pym_getLen(&pms));
	}
}

void _cb_npc_initForMapChannel(void *param, diJob_npcListData_t *jobData)
{
	mapChannel_t *mapChannel = (mapChannel_t*)param;
	for(int i=0; i<jobData->outNpcCount; i++)
	{
		di_npcData_t *npcData = jobData->outNpcList+i;
		// create npc
		npc_t *npc = npc_createNPC(mapChannel, npcData->npcID, npcData->entityClassID, npcData->unicodeName);
		if( npc == NULL )
			continue;
		// set npc info
		for(int a=0; a<SWAPSET_SIZE; a++)
			npc_setAppearanceEntry(npc, a, npcData->appearanceData[a].classId, npcData->appearanceData[a].hue);
		npc_setLocation(npc, (float)npcData->posX, (float)npcData->posY, (float)npcData->posZ, 0.0f, 0.0f);
		npc_setType(npc, NPC_TYPE_NONE);
		//npc_setMissionList(npc, NULL);
		// add npc to world
		cellMgr_addToWorld(mapChannel, npc);
	}
	mapChannel->loadState = 1; // loading is done
}

// synchronous loading of npcs from the database
void npc_initForMapChannel(mapChannel_t *mapChannel)
{
	mapChannel->loadState = 0;
	dataInterface_NPC_getNPCList(mapChannel->mapInfo->contextId, _cb_npc_initForMapChannel, mapChannel);
	while( mapChannel->loadState == 0 ) Sleep(100);
}
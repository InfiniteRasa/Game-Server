#include "global.h"

typedef struct  
{
	TMutex cs;
	TMutex cs_missionList; // synchronous access to any npc mission list
	//hashTable_t ht_mission;
	uint8 loadState;
	/*
		Important notice:
			After loading the mission Hashtable is accessed unsynchronized.
			Therefore only read access is safe. Never write to the Hashtable during runtime.
	*/	
	missionScriptLine_t* missionScriptLines;
	uint32 missionScriptLineCount;
	// mission table (used for missionIndex -> mission mapping)
	mission_t** missions;
	sint32 missionCount;
}missionEnv_t;

missionEnv_t missionEnv;

void _mission_loadScriptData(void *param, di_missionScript_t *jobData)
{
	missionEnv.missionScriptLineCount = jobData->scriptLineCount;
	if( jobData->scriptLineCount > 0 )
	{
		missionEnv.missionScriptLines = (missionScriptLine_t*)malloc(sizeof(missionScriptLine_t) * jobData->scriptLineCount);
		memset(missionEnv.missionScriptLines, 0x00, sizeof(missionScriptLine_t) * jobData->scriptLineCount);
		for(uint32 i=0; i<jobData->scriptLineCount; i++)
		{
			missionEnv.missionScriptLines[i].missionId = jobData->scriptLines[i].missionId;
			missionEnv.missionScriptLines[i].command = jobData->scriptLines[i].command;
			missionEnv.missionScriptLines[i].state = jobData->scriptLines[i].state;
			missionEnv.missionScriptLines[i].flags = jobData->scriptLines[i].flags;
			missionEnv.missionScriptLines[i].value1 = jobData->scriptLines[i].value1;
			missionEnv.missionScriptLines[i].value2 = jobData->scriptLines[i].value2;
			missionEnv.missionScriptLines[i].value3 = jobData->scriptLines[i].value3;
		}
	}
	// notify of finished loading
	missionEnv.loadState = 1;
}

int _mission_qsort_compareScriptLine(const void* p1, const void* p2)
{
	missionScriptLine_t* scriptLineA = (missionScriptLine_t*)p1;
	missionScriptLine_t* scriptLineB = (missionScriptLine_t*)p2;
	if( scriptLineA->missionId != scriptLineB->missionId )
		return scriptLineA->missionId - scriptLineB->missionId;
	return scriptLineA->state - scriptLineB->state;
}

/*
 * Returns true if the given command requires storage space for objective tracking
 */
bool mission_commandRequiresStorage(uint16 command)
{
	if( command == M_OP_COMPLETEOBJECTIVE )
		return true;
	return false;
}

void mission_generateMissionFromScriptLines(sint32 missionId, sint32 startIndex, sint32 lineCount)
{
	printf("Prepare mission %d Script-Start: %d Script-Length: %d\n", missionId, startIndex, lineCount);
	// do some validation
	if( missionEnv.missionScriptLines[startIndex].state != 0 )
	{
		printf("Mission %d does not begin with state 0!\n", missionId);
		return;
	}
	if( missionEnv.missionScriptLines[startIndex].state != 0 )
	{
		printf("Mission %d does not begin with state 0!\n", missionId);
		return;
	}
	sint32 validateCurrentState = 0;
	for(sint32 i=0; i<lineCount; i++)
	{
		if( missionEnv.missionScriptLines[startIndex+i].state == validateCurrentState )
			continue; // state ok
		if( missionEnv.missionScriptLines[startIndex+i].state == (validateCurrentState+1) )
		{
			validateCurrentState++; // progress state
			continue; // state progressed and ok
		}
		else
		{
			printf("Mission %d has unused states!\n", missionId);
			return;
		}
	}
	// get state count
	sint32 stateCount = missionEnv.missionScriptLines[startIndex+lineCount-1].state - missionEnv.missionScriptLines[startIndex+0].state + 1;
	// init mission struct
	mission_t* mission = (mission_t*)malloc(sizeof(mission_t));
	memset(mission, 0x00, sizeof(mission_t));
	mission->missionId = missionId;
	mission->scriptLines = missionEnv.missionScriptLines + startIndex;
	mission->stateCount = stateCount;
	// set some default values
	mission->level = 1;
	mission->categoryId = 1;
	// find and process mission info script lines
	for(sint32 i=0; i<lineCount; i++)
	{
		if( mission->scriptLines[i].command == M_OP_MISSIONINFO_CATEGORYID )
			mission->categoryId = mission->scriptLines[i].value1;
		else if( mission->scriptLines[i].command == M_OP_MISSIONINFO_LEVEL )
			mission->level = mission->scriptLines[i].value1;
	}
	// create state mapping
	mission->stateMapping = (uint16*)malloc(sizeof(uint16) * (stateCount+1));
	sint16 currentState = -1;
	uint16 stateMapIndex = 0;
	for(sint32 i=0; i<lineCount; i++)
	{
		if( mission->scriptLines[i].state == currentState )
			continue;
		currentState = mission->scriptLines[i].state;
		mission->stateMapping[stateMapIndex] = i;
		stateMapIndex++;
	}
	if( stateMapIndex != stateCount )
		__debugbreak(); // what? some of the validation code above must be wrong if this happens
	mission->stateMapping[stateMapIndex] = lineCount;
	mission->scriptLineCount = lineCount;
	mission->missionIndex = missionEnv.missionCount;
	// calculate storage indices per state
	// mission_commandRequiresStorage
	for(sint32 s=0; s<mission->stateCount; s++)
	{
		sint32 scriptlineFrom = mission->stateMapping[s];
		sint32 scriptlineTo = mission->stateMapping[s+1];
		sint32 currentStorageIndex = 0;
		for(sint32 i=scriptlineFrom; i<scriptlineTo; i++)
		{
			missionScriptLine_t* scriptline = mission->scriptLines + i;
			if( mission_commandRequiresStorage(scriptline->command) )
			{
				if( currentStorageIndex == 8 )
				{
					printf("Too many objectives to track in state %d of mission %d\n", s, mission->missionId);
					continue;
				}
				scriptline->storageIndex = currentStorageIndex;
				currentStorageIndex++;
			}
		}
	}
	// add mission to mission list
	missionEnv.missions[missionEnv.missionCount] = mission;
	missionEnv.missionCount++;
}

void _mission_addNPCMissionRelation(npcData_t* npcData, sint32 missionIndex)
{
	for(uint32 i=0; i<npcData->relatedMissionCount; i++)
	{
		if( npcData->relatedMissions[i].missionIndex == missionIndex )
			return; // mission already in list
	}
	npcData->relatedMissions = (npcDataRelatedMission_t*)realloc(npcData->relatedMissions, sizeof(npcDataRelatedMission_t) * (npcData->relatedMissionCount+1));
	npcData->relatedMissions[npcData->relatedMissionCount].missionIndex = missionIndex;
	npcData->relatedMissionCount++;
}

void mission_prepareMission(sint32 missionIndex)
{
	mission_t* mission = missionEnv.missions[missionIndex];
	// process all commands and find related creatures/npcs
	for(uint32 i=0; i<mission->scriptLineCount; i++)
	{
		missionScriptLine_t* scriptline = mission->scriptLines + i;
		if( scriptline->command == M_OP_DISPENSER )
		{
			creatureType_t* creatureType = creatureType_findType(scriptline->value1);
			if( creatureType == NULL )
			{
				printf("Dispenser NPC for mission %d not found\n", scriptline->missionId);
				continue;
			}
			if( creatureType->npcData == NULL )
			{
				printf("Dispenser creature for mission %d is not defined as NPC\n", scriptline->missionId);
				continue;
			}
			_mission_addNPCMissionRelation(creatureType->npcData, missionIndex);
		}
		else if( scriptline->command == M_OP_COLLECTOR )
		{
			creatureType_t* creatureType = creatureType_findType(scriptline->value1);
			if( creatureType == NULL )
			{
				printf("Collector NPC for mission %d not found\n", scriptline->missionId);
				continue;
			}
			if( creatureType->npcData == NULL )
			{
				printf("Collector creature for mission %d is not defined as NPC\n", scriptline->missionId);
				continue;
			}
			_mission_addNPCMissionRelation(creatureType->npcData, missionIndex);
		}
		else if( scriptline->command == M_OP_COMPLETEOBJECTIVE )
		{
			creatureType_t* creatureType = creatureType_findType(scriptline->value1);
			if( creatureType == NULL )
			{
				printf("Objective NPC for mission %d not found\n", scriptline->missionId);
				continue;
			}
			if( creatureType->npcData == NULL )
			{
				printf("Objective creature for mission %d is not defined as NPC\n", scriptline->missionId);
				continue;
			}
			_mission_addNPCMissionRelation(creatureType->npcData, missionIndex);
		}
	}
}

void mission_prepareScriptLines()
{
	// first sort the script lines by missionId and state
	qsort(missionEnv.missionScriptLines, missionEnv.missionScriptLineCount, sizeof(missionScriptLine_t), _mission_qsort_compareScriptLine);
	// count number of missions to init missions array
	sint32 missionCount = 0;
	sint32 currentMissionId = -1;
	for(uint32 i=0; i<missionEnv.missionScriptLineCount; i++)
	{
		if( currentMissionId != missionEnv.missionScriptLines[i].missionId )
		{
			missionCount++;
			currentMissionId = missionEnv.missionScriptLines[i].missionId;
		}
	}
	missionEnv.missionCount = 0; // we set this to 0 for now, it is increased by mission_generateMissionFromScriptLines()
	missionEnv.missions = (mission_t**)malloc(sizeof(mission_t*) * missionCount);
	memset(missionEnv.missions, 0x00, sizeof(mission_t*) * missionCount);
	// process the script lines to generate mission structs
	currentMissionId = -1;
	sint32 lastMissionIdChange = -1;
	for(uint32 i=0; i<missionEnv.missionScriptLineCount; i++)
	{
		if( currentMissionId != missionEnv.missionScriptLines[i].missionId )
		{
			// new mission
			if( lastMissionIdChange != -1 )
			{
				sint32 currentMissionLineCount = i - lastMissionIdChange;
				mission_generateMissionFromScriptLines(currentMissionId, lastMissionIdChange, currentMissionLineCount);
			}
			currentMissionId = missionEnv.missionScriptLines[i].missionId;
			lastMissionIdChange = i;
		}
	}
	if( lastMissionIdChange != -1 )
	{
		sint32 currentMissionLineCount = missionEnv.missionScriptLineCount - lastMissionIdChange;
		if( currentMissionLineCount > 0 )
			mission_generateMissionFromScriptLines(currentMissionId, lastMissionIdChange, currentMissionLineCount);
	}
	// process the missions as a whole and register all the callbacks used to track mission updates
	for(sint32 i=0; i<missionEnv.missionCount; i++)
	{
		mission_prepareMission(i);
	}
}

sint32 mission_getNumberOfMissions()
{
	return missionEnv.missionCount;
}

void mission_init()
{
	Thread::InitMutex(&missionEnv.cs);
	
	// load mission locations

	// load mission text
	
	// load mission script lines
	missionEnv.loadState = 0;
	DataInterface_Mission_getMissionScriptData(_mission_loadScriptData, NULL);
	while( missionEnv.loadState == 0 ) Sleep(100);
	mission_prepareScriptLines();
}

void mission_initForClient(mapChannelClient_t *client)
{
	// todo: setup client->mission hashTable with missiondata from client->tempCharacter
	hashTable_init(&client->mission, client->tempCharacterData->missionStateCount+2);
	// ...


}

// load missions and create hashTable ( called only once, on init )
void _cb_mission_initForChannel(void *param, diJob_missionListData_t *jobData)
{
	//mapChannel_t *mapChannel = (mapChannel_t*)param;
	//// alloc memory, init hashtable
	//mission_t *missionDataList = (mission_t*)malloc(sizeof(mission_t)*jobData->outMissionCount);
	//hashTable_init(&missionEnv.ht_mission, jobData->outMissionCount + 16);
	//for(sint32 i=0; i<jobData->outMissionCount; i++)
	//{
	//	di_missionData_t *missionData = jobData->outMissionList+i;
	//	mission_t *mission = missionDataList+i;
	//	// set mission data
	//	mission->missionId = missionData->missionId;
	//	mission->firstCondition = NULL;
	//	mission->npcCollectorEntityId = missionData->collectorNPC;
	//	mission->npcDispenserEntityId = missionData->dispenserNPC;
	//	mission->objectiveCount = 0;
	//	mission->objectiveList = NULL;
	//	// register in hashTable
	//	hashTable_set(&missionEnv.ht_mission, mission->missionId, mission);
	//}
	//mapChannel->loadState = 1;
}

bool missionsLoaded = false;
void mission_initForChannel(mapChannel_t *mapChannel)
{
	//Thread::LockMutex(&missionEnv.cs);
	//if( missionsLoaded == true )
	//{
	//	Thread::UnlockMutex(&missionEnv.cs);
	//	return;
	//}
	//missionsLoaded = true;
	//mapChannel->loadState = 0;
	//DataInterface_Mission_getMissionList(_cb_mission_initForChannel, mapChannel);
	//while( mapChannel->loadState == 0 ) Sleep(10);
	//Thread::UnlockMutex(&missionEnv.cs);
}

/*
 * Will check if all objectives of the current mission state are completed
 * If yes it will add one to the state and send some updates if necessary (new objectives)
 */
void mission_tryProgressMissionState(mapChannelClient_t *client, mission_t* mission, missionLogEntry_t* missionLogEntry)
{
	sint32 currentState = missionLogEntry->state;
	if( currentState >= (mission->stateCount-1) )
		return; // already at the last state
	sint32 scriptlineFrom = mission->stateMapping[currentState];
	sint32 scriptlineTo = mission->stateMapping[currentState+1];
	// process all commands for the current state
	for(sint32 i=scriptlineFrom; i<scriptlineTo; i++)
	{
		missionScriptLine_t* missionScriptLine = mission->scriptLines + i;
		if( missionScriptLine->command == M_OP_COMPLETEOBJECTIVE )
		{
			// is objective incomplete?
			if( missionLogEntry->missionData[missionScriptLine->storageIndex] != 1 )
				return; // not completed
		}
	}
	// if we have not left the method yet, we can progress
	missionLogEntry->state++;
	// reset mission objective counters
	missionLogEntry->missionData[0] = 0;
	missionLogEntry->missionData[1] = 0;
	missionLogEntry->missionData[2] = 0;
	missionLogEntry->missionData[3] = 0;
	missionLogEntry->missionData[4] = 0;
	missionLogEntry->missionData[5] = 0;
	missionLogEntry->missionData[6] = 0;
	missionLogEntry->missionData[7] = 0;
	//// debug: inform client
	//char text[256];
	//sprintf(text, "progressed mission to state %d", missionLogEntry->state);
	//communicator_systemMessage(client, text);
	// process commands for the given state
	pyMarshalString_t pms;
	for(sint32 q=mission->stateMapping[missionLogEntry->state]; q<mission->stateMapping[missionLogEntry->state+1]; q++)
	{
		missionScriptLine_t* scriptline = mission->scriptLines + q;
		if( scriptline->command == M_OP_OBJECTIVE_ANNOUNCE )
		{
			// send Recv_ObjectiveRevealed
			// note that the objective must already be in the mission log
			pym_init(&pms);
			pym_tuple_begin(&pms);
			// missionId
			pym_addInt(&pms, mission->missionId);
			// objectiveId
			pym_addInt(&pms, scriptline->value1);
			// missionInfo
			missionLog_buildMissionInfo(client, mission, &pms);
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, 494, pym_getData(&pms), pym_getLen(&pms));
		}
		else if( scriptline->command == M_OP_OBJECTIVE_COMPLETE )
		{
			// only needed when there are multiple objectives or there is no new objective (else Recv_ObjectiveRevealed will update all objectives at once)
			// it makes more sense to move this to the objective processing methods
			//// send Recv_ObjectiveCompleted
			//pym_init(&pms);
			//pym_tuple_begin(&pms);
			//// missionId
			//pym_addInt(&pms, mission->missionId);
			//// objectiveId
			//pym_addInt(&pms, scriptline->value1);
			//pym_tuple_end(&pms);
			//netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, 492, pym_getData(&pms), pym_getLen(&pms));
		}
	}
}

void missionLog_buildMissionInfo(mapChannelClient_t *client, mission_t *mission, pyMarshalString_t* pms)
{
	pym_tuple_begin(pms); // missionInfo
	pym_addInt(pms, MISSION_STATUS_ACTIVE); // missionStatus
	if( mission->stateCount == 2 ) // only two states means there cant be any mission objectives
		pym_addBool(pms, true);// bCompletable		
	else
		pym_addBool(pms, false);// bCompletable
	pym_tuple_begin(pms); // constantData
	pym_addInt(pms, mission->level); // missionLevel
	pym_addInt(pms, MISSION_GROUPTYPE_SOLO); // groupType
	pym_addInt(pms, mission->categoryId); // missionCategoryId
	pym_addInt(pms, 0); // bShareable
	pym_addInt(pms, 0); // bRadioCompletable
	mission_buildRewardInfoTuple(mission, pms);
	pym_tuple_end(pms);
	pym_addInt(pms, 0); // changeTime
	pym_list_begin(pms); // objectiveList

	missionLogEntry_t* missionLogEntry = mission_findPlayerMission(client, mission->missionIndex);
	sint32 currentState = 1;
	if( missionLogEntry )
		currentState = missionLogEntry->state;
	if( mission->stateCount > 1 )
	{
		for(sint32 q=0; q<mission->scriptLineCount; q++)
		{
			missionScriptLine_t* scriptline = mission->scriptLines + q;
			if( scriptline->state > currentState )
				continue;
			if( scriptline->command == M_OP_OBJECTIVE_ANNOUNCE )
			{
				// add objectives
				// isRequired, indicatorList
				pym_tuple_begin(pms);
				// objectiveId
				pym_addInt(pms, scriptline->value1);
				// objective status
				if( scriptline->state == currentState )
					pym_addInt(pms, MISSION_OBJECTIVE_STATUS_INCOMPLETE);
				else
					pym_addInt(pms, MISSION_OBJECTIVE_STATUS_COMPLETED);
				// ordinal
				pym_addInt(pms, q); // we use the script line index as the ordinal (objective sort index)
				// time remaining
				pym_addNoneStruct(pms);
				// counterDict
				pym_dict_begin(pms);
				pym_dict_end(pms);
				// itemCounters (dict)
				pym_dict_begin(pms);
				pym_dict_end(pms);
				// isRequired
				pym_addBool(pms, true);
				// indicatorList
				pym_list_begin(pms);
				pym_list_end(pms);
				pym_tuple_end(pms);
			}
		}
	}
	pym_list_end(pms);
	pym_tuple_end(pms);
}

void missionLog_clientGainedMission(mapChannelClient_t *client, mission_t *mission)
{
	pyMarshalString_t pms;
	if( client->player == NULL )
		return;
	if( client->player->actor == NULL )
		return;
	// mission gained
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, mission->missionId);
	missionLog_buildMissionInfo(client, mission, &pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, 485, pym_getData(&pms), pym_getLen(&pms));
}

missionLogEntry_t* mission_findPlayerMission(mapChannelClient_t *client, uint16 missionIndex)
{
	// for now we do a simple straight forward search
	// todo: replace by binary search
	for(uint32 i=0; i<client->player->activeMissionCount; i++)
	{
		if( client->player->missionLog[i].missionIndex == missionIndex )
			return &client->player->missionLog[i];
	}
	return NULL;
}

bool mission_isCompletedByPlayer(mapChannelClient_t *client, uint16 missionIndex)
{
	// simple bit set test, missionIndex is the bit index
	if( client->player->missionStateMap[missionIndex/8] & (1<<(missionIndex%8)) )
		return true;
	return false;
}

mission_t* mission_getByIndex(uint16 missionIndex)
{
	if( missionIndex >= missionEnv.missionCount )
		return NULL;
	return missionEnv.missions[missionIndex];
}

sint32 mission_getReward_credits(mission_t* mission)
{
	sint32 scriptLineFrom = mission->stateMapping[mission->stateCount-1];
	sint32 scriptLineTo = mission->stateMapping[mission->stateCount];

	for(sint32 i=scriptLineFrom; i<scriptLineTo; i++)
	{
		missionScriptLine_t* scriptline = mission->scriptLines + i;
		if( scriptline->command == M_OP_COLLECTOR_REWARD_CREDITS )
			return scriptline->value1;
	}
	return MISSION_REWARD_NONE;
}

sint32 mission_getReward_prestige(mission_t* mission)
{
	sint32 scriptLineFrom = mission->stateMapping[mission->stateCount-1];
	sint32 scriptLineTo = mission->stateMapping[mission->stateCount];

	for(sint32 i=scriptLineFrom; i<scriptLineTo; i++)
	{
		missionScriptLine_t* scriptline = mission->scriptLines + i;
		if( scriptline->command == M_OP_COLLECTOR_REWARD_PRESTIGE )
			return scriptline->value1;
	}
	return MISSION_REWARD_NONE;
}

sint32 mission_getReward_experience(mission_t* mission)
{
	sint32 scriptLineFrom = mission->stateMapping[mission->stateCount-1];
	sint32 scriptLineTo = mission->stateMapping[mission->stateCount];

	for(sint32 i=scriptLineFrom; i<scriptLineTo; i++)
	{
		missionScriptLine_t* scriptline = mission->scriptLines + i;
		if( scriptline->command == M_OP_COLLECTOR_REWARD_XP )
			return scriptline->value1;
	}
	return MISSION_REWARD_NONE;
}

void mission_buildRewardInfoTuple(mission_t* mission, pyMarshalString_t* pms)
{
	pym_tuple_begin(pms);	// rewardInfo
	// fixed redward tuple
	pym_tuple_begin(pms);
	// credits list 
	pym_list_begin(pms);
	sint32 rewardCredits = mission_getReward_credits(mission);
	if( rewardCredits != MISSION_REWARD_NONE )
	{
		pym_tuple_begin(pms);
		pym_addInt(pms, 1); // FUND(1)
		pym_addInt(pms, rewardCredits); // amount
		pym_tuple_end(pms);
	}
	sint32 rewardPrestige = mission_getReward_prestige(mission);
	if( rewardPrestige != MISSION_REWARD_NONE )
	{
		pym_tuple_begin(pms);
		pym_addInt(pms, 2); // PRESTIGE(2)
		pym_addInt(pms, rewardPrestige); // amount
		pym_tuple_end(pms);
	}
	pym_list_end(pms);
	// fixed reward items
	pym_list_begin(pms);	
	pym_list_end(pms);
	pym_tuple_end(pms);
	// selection list (selectable reward)
	pym_list_begin(pms);
	pym_list_end(pms);		
	pym_tuple_end(pms); 

}
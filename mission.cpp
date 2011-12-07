#include "global.h"

typedef struct  
{
	CRITICAL_SECTION cs;
	CRITICAL_SECTION cs_missionList; // synchronous access to any npc mission list
	HashTable_uint32Iterable_t ht_mission;
	/*
		Important notice:
			After loading the mission Hashtable is accessed unsynchronized.
			Therefore only read access is safe. Never write to the Hashtable during runtime.
	*/	

}missionEnv_t;

missionEnv_t missionEnv;

void mission_init()
{
	InitializeCriticalSection(&missionEnv.cs);
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
	mapChannel_t *mapChannel = (mapChannel_t*)param;
	// alloc memory, init hashtable
	mission_t *missionDataList = (mission_t*)malloc(sizeof(mission_t)*jobData->outMissionCount);
	hashTable_init(&missionEnv.ht_mission, jobData->outMissionCount + 16);
	for(int i=0; i<jobData->outMissionCount; i++)
	{
		di_missionData_t *missionData = jobData->outMissionList+i;
		mission_t *mission = missionDataList+i;
		// set mission data
		mission->missionId = missionData->missionId;
		mission->firstCondition = NULL;
		mission->npcCollectorEntityId = missionData->collectorNPC;
		mission->npcDispenserEntityId = missionData->dispenserNPC;
		mission->objectiveCount = 0;
		mission->objectiveList = NULL;
		// register in hashTable
		hashTable_set(&missionEnv.ht_mission, mission->missionId, mission);
	}
	mapChannel->loadState = 1;
}

bool missionsLoaded = false;
void mission_initForChannel(mapChannel_t *mapChannel)
{
	EnterCriticalSection(&missionEnv.cs);
	if( missionsLoaded == true )
	{
		LeaveCriticalSection(&missionEnv.cs);
		return;
	}
	missionsLoaded = true;
	mapChannel->loadState = 0;
	dataInterface_Mission_getMissionList(_cb_mission_initForChannel, mapChannel);
	while( mapChannel->loadState == 0 ) Sleep(10);
	LeaveCriticalSection(&missionEnv.cs);
}

missionList_t *mission_generateMissionListForNPC(npc_t *npc)
{
	// todo: cache mission lists 
	mission_t *missions[32];
	int missionCount = 0;
	mission_t **fullMissionList = (mission_t**)hashTable_getValueArray(&missionEnv.ht_mission);
	int fullMissionCount = hashTable_getCount(&missionEnv.ht_mission);
	for(int i=0; i<fullMissionCount; i++)
	{
		if( fullMissionList[i] == NULL )
			continue;
		if( fullMissionList[i]->npcDispenserEntityId == npc->entityId ||
			fullMissionList[i]->npcCollectorEntityId == npc->entityId )
		{
			missions[missionCount] = fullMissionList[i];
			missionCount++;
			if( missionCount >= 32 )
			{
				printf("NPC %I64u has too many missions\n", npc->entityId);
				break;
			}
		}
	}
	// return NULL if no mission for NPC exists
	if( missionCount == 0 )
		return NULL;
	// create mission list
	missionList_t *missionList;
	missionList = (missionList_t*)malloc(sizeof(missionList_t) + sizeof(mission_t*) * missionCount);
	missionList->missionCount = missionCount;
	missionList->missionReferences = (mission_t **)(missionList+1);
	for(int i=0; i<missionCount; i++)
		missionList->missionReferences[i] = missions[i];
	return missionList;
}

bool mission_newAvailableForClient(missionList_t *missionList, mapChannelClient_t *client, npc_t *npc, mission_t **outMissionList, int *outLimit)
{
	mission_t *mission;
	int c = 0;
	int limit = *outLimit;
	*outLimit = 0;
	if( missionList == NULL )
		return false;
	for(int i=0; i<missionList->missionCount; i++)
	{
		mission = missionList->missionReferences[i];
		if( mission )
		{
			if( mission->npcDispenserEntityId != npc->entityId )
				continue;
			// todo: check conditions, is quest available?
			unsigned int missionValue = (unsigned int)hashTable_get(&client->mission, mission->missionId);
			unsigned int missionState = missionValue&MISSION_STATE_MASK;
			if( missionState == MISSION_STATE_NOTACCEPTED )
			{
				outMissionList[c] = mission;
				c++;
				if( c >= limit )
				{
					*outLimit = c;
					return true;
				}
			}
		}
	}
	*outLimit = c;
	if( c )
		return true;
	return false;
}

bool mission_completeableAvailableForClient(missionList_t *missionList, mapChannelClient_t *client, npc_t *npc, mission_t **outMissionList, int *outLimit)
{
	mission_t *mission;
	int c = 0;
	int limit = *outLimit;
	*outLimit = 0;
	if( missionList == NULL )
		return false;
	for(int i=0; i<missionList->missionCount; i++)
	{
		mission = missionList->missionReferences[i];
		if( mission )
		{
			if( mission->npcCollectorEntityId != npc->entityId )
				continue;
			// todo: check conditions, is quest available?
			unsigned int missionValue = (unsigned int)hashTable_get(&client->mission, mission->missionId);
			unsigned int missionState = missionValue&MISSION_STATE_MASK;
			if( missionState == MISSION_STATE_DONE )
			{
				outMissionList[c] = mission;
				c++;
				if( c >= limit )
				{
					*outLimit = c;
					return true;
				}
			}
		}
	}
	*outLimit = c;
	if( c )
		return true;
	return false;
}
void _missionLog_clientGainedMission(mapChannelClient_t *client, mission_t *mission)
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
	pym_tuple_begin(&pms); // missionInfo
		pym_addInt(&pms, 0); // missionStatus
		pym_addBool(&pms, true);// bCompleteable
		pym_tuple_begin(&pms); // constantData
			pym_addInt(&pms, 1); // missionLevel
			pym_addInt(&pms, 1); // groupType
			pym_addInt(&pms, 1); // missionCategoryId
			pym_addInt(&pms, 0); // bShareable
			pym_addInt(&pms, 0); // bRadioCompleteable
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
		pym_tuple_end(&pms);

		pym_addInt(&pms, 0); // changeTime
		pym_list_begin(&pms); // objectiveList
		pym_list_end(&pms);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, 485, pym_getData(&pms), pym_getLen(&pms));
}

void mission_changeMissionStateForClient(mapChannelClient_t *client, mission_t *mission, unsigned int newState, unsigned int newValue)
{
	unsigned int missionValue = (unsigned int)hashTable_get(&client->mission, mission->missionId);
	unsigned int newMissionValue = newState | (newValue<<4);
	if( (missionValue&MISSION_STATE_MASK) != (newMissionValue&MISSION_STATE_MASK) )
	{
		// state changed
		unsigned int oldState = missionValue&MISSION_STATE_MASK;
		if( oldState == MISSION_STATE_NOTACCEPTED )
		{
			// quest gained, add to mission log
			_missionLog_clientGainedMission(client, mission);
		}
		// send mission updates

	}
	hashTable_set(&client->mission, mission->missionId, (void*)newMissionValue);
}

void mission_acceptForClient(mapChannelClient_t *client, mission_t *mission)
{
	unsigned int missionState = MISSION_STATE_ACTIVE;
	// if mission has no objectives, immediately mark as done
	if( mission->objectiveCount == 0 )
	{
		missionState = MISSION_STATE_DONE;
	}
	else
		__debugbreak(); // todo
	mission_changeMissionStateForClient(client, mission, missionState, 0);
	//hashTable_set(&client->mission, mission->missionId, (void*)missionValue);

	
}
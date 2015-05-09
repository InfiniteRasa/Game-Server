

// mission script flags
#define M_FLAG_MIN_STATE	(1) // must have min this state
#define M_FLAG_MAX_STATE	(2) // must have at least this state
#define M_FLAG_PROGRESS		(4) // progress state by 1 when this command executed successfully
#define M_FLAG_EXACT_STATE	(M_FLAG_MIN_STATE|M_FLAG_MAX_STATE)


// mission script commands
#define M_OP_DISPENSER					1	// value1: typeId of giver NPC
#define M_OP_COMPLETEOBJECTIVE			2	// value1: typeId of talk-to-me NPC, value2: objectiveId value3: playerFlagId
#define M_OP_COLLECTOR					3	// value1: typeId of collector NPC
#define M_OP_COLLECTOR_REWARD_ITEM		4	// value1: templateId of item, value2: item count
#define M_OP_COLLECTOR_REWARD_XP		5	// value1: xp
#define M_OP_COLLECTOR_REWARD_CREDITS	6	// value1: credits
#define M_OP_COLLECTOR_REWARD_PRESTIGE	7	// value1: prestige
#define M_OP_MISSIONINFO_CATEGORYID		8	// value1: categoryId (missioncategorylanguage.py)
#define M_OP_MISSIONINFO_LEVEL			9	// value1: mission level (not to be confused with required level)
#define M_OP_NO_OP						10	// does nothing (placeholder)
#define M_OP_OBJECTIVE_ANNOUNCE			11	// value1: objectiveId
#define M_OP_OBJECTIVE_COUNTER			12	// value1: objectiveId value2: counterId value3: maxCount
#define M_OP_OBJECTIVE_COMPLETE			13	// value1: objectiveId

#define MISSION_OBJECTIVE_COUNTER_LIMIT	(8)

typedef struct  
{
	sint16 counterId;
	sint16 maxCount;
}missionObjectiveCounter;

typedef struct  
{
	uint16 state; // min state
	sint32 objectiveId;
	uint8 counterCount;
	missionObjectiveCounter counter[MISSION_OBJECTIVE_COUNTER_LIMIT];
}missionObjective_t;


typedef struct  
{
	sint32	missionId;
	sint16	command;
	sint16	state;
	sint8	flags;
	sint32	value1;
	sint32	value2;
	sint32	value3;
	// calculated during preprocessing
	uint8	storageIndex; // indicates the index for this state
}missionScriptLine_t;

typedef struct  
{
	uint16 missionIndex; // index of the mission in the mission table
	sint32 missionId; // the missionId used internally by the game
	sint32 stateCount; // number of states
	uint16* stateMapping; // fast state -> line conversion array (plus one entry at the end that stores the value of stateCount, used to allow fast access to number of lines for a state)
	missionScriptLine_t* scriptLines;
	sint32 scriptLineCount;
	//sint32 missionId;
	//unsigned long long npcDispenserEntityId; // the NPC that offers the mission
	//unsigned long long npcCollectorEntityId; // the NPC that the mission can be completed at
	//missionCondition_t *firstCondition; // NULL if no conditions, else points to the first condition
	sint32 objectiveCount;
	missionObjective_t *objectiveList;
	// mission info
	sint32 categoryId;
	sint32 level; // level of the mission (not required level)
}mission_t;

#define MISSION_LOG_LIMIT	30

typedef struct  
{
	uint16 missionIndex; // use index instead of id for faster access, but if we save to the DB we have to translate this back to id
	uint16 state; // the state the mission currently is at
	uint16 missionData[8]; // 8 16-bit values of mission data (stores kill count, completed objectives..)
}missionLogEntry_t; // used by manifestation_t

//typedef struct _missionList_t
//{
//	sint32 missionCount;
//	mission_t **missionReferences;
//	//struct _missionList_t *next;
//}missionList_t; // a list of missions related to a single NPC

//#define MISSION_STATE_MASK			0xF
//#define MISSION_STATE_NOTACCEPTED	0
//#define MISSION_STATE_ACTIVE		1	// mission running, still have to do objectives
//#define MISSION_STATE_DONE			2	// mission objectives done, must talk to NPC
//#define MISSION_STATE_COMPLETED		3	// mission completed, no more in mission log

// shared defines
#define MISSION_GROUPTYPE_SOLO		1
#define MISSION_GROUPTYPE_DUO		2
#define MISSION_GROUPTYPE_GROUP		3
#define MISSION_GROUPTYPE_RAID		4

#define MISSION_STATUS_ACTIVE		0
#define MISSION_STATUS_SUCCESS		1
#define MISSION_STATUS_FAILED		2
#define MISSION_STATUS_NOT_ASSIGNED	3
#define MISSION_STATUS_COMPLETED	4

#define MISSION_OBJECTIVE_STATUS_NOT_ASSIGNED	0
#define MISSION_OBJECTIVE_STATUS_INCOMPLETE	1
#define MISSION_OBJECTIVE_STATUS_COMPLETED		2
#define MISSION_OBJECTIVE_STATUS_FAILED		3
#define MISSION_OBJECTIVE_STATUS_INACTIVE		4

// server only defines
#define MISSION_REWARD_NONE	-1


void mission_init();
void mission_initForClient(mapChannelClient_t *client);
void mission_initForChannel(mapChannel_t *mapChannel);

//missionList_t *mission_generateMissionListForNPC(npc_t *npc);


//bool mission_completeableAvailableForClient(missionList_t *missionList, mapChannelClient_t *client, npc_t *npc, mission_t **outMissionList, sint32 *outLimit);
//bool mission_newAvailableForClient(missionList_t *missionList, mapChannelClient_t *client, npc_t *npc, mission_t **outMissionList, sint32 *outLimit);

mission_t* mission_getByIndex(uint16 missionIndex);

sint32 mission_getNumberOfMissions();

missionLogEntry_t* mission_findPlayerMission(mapChannelClient_t *client, uint16 missionIndex);
bool mission_isCompletedByPlayer(mapChannelClient_t *client, uint16 missionIndex);

sint32 mission_getReward_credits(mission_t* mission);
sint32 mission_getReward_prestige(mission_t* mission);
sint32 mission_getReward_experience(mission_t* mission);
void mission_buildRewardInfoTuple(mission_t* mission, pyMarshalString_t* pms);

bool mission_isCreatureMissionDispenser(mission_t* mission, creature_t* creatureOrNPC);

void mission_tryProgressMissionState(mapChannelClient_t *client, mission_t* mission, missionLogEntry_t* missionLogEntry);

// mission log
void missionLog_buildMissionInfo(mapChannelClient_t *client, mission_t *mission, pyMarshalString_t* pms);
void missionLog_clientGainedMission(mapChannelClient_t *client, mission_t *mission);
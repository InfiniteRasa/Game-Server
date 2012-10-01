
/*
	
*/

typedef struct _missionCondition_t
{
	sint32 conditionCode;
	sint32 conditionValue;
	struct _missionCondition_t *next; // if there is more than one condition
}missionCondition_t; // the condition(s) that must be true so the quest can be accepted

typedef struct _missionObjective_t
{
	sint32 temp;
	//sint32 conditionCode;
	//sint32 conditionValue;
	//struct _missionCondition_t *next; // if there is more than one condition
}missionObjective_t; // the objective(s) that must be true so the quest can be completed


typedef struct  
{
	sint32 missionId;
	unsigned long long npcDispenserEntityId; // the NPC that offers the mission
	unsigned long long npcCollectorEntityId; // the NPC that the mission can be completed at
	missionCondition_t *firstCondition; // NULL if no conditions, else points to the first condition
	sint32 objectiveCount;
	missionObjective_t *objectiveList;
}mission_t;

typedef struct _missionList_t
{
	sint32 missionCount;
	mission_t **missionReferences;
	//struct _missionList_t *next;
}missionList_t; // a list of missions related to a single NPC

#define MISSION_STATE_MASK			0xF
#define MISSION_STATE_NOTACCEPTED	0
#define MISSION_STATE_ACTIVE		1	// mission running, still have to do objectives
#define MISSION_STATE_DONE			2	// mission objectives done, must talk to NPC
#define MISSION_STATE_COMPLETED		3	// mission completed, no more in mission log

void mission_init();
void mission_initForClient(mapChannelClient_t *client);
void mission_initForChannel(mapChannel_t *mapChannel);

missionList_t *mission_generateMissionListForNPC(npc_t *npc);


bool mission_completeableAvailableForClient(missionList_t *missionList, mapChannelClient_t *client, npc_t *npc, mission_t **outMissionList, sint32 *outLimit);
bool mission_newAvailableForClient(missionList_t *missionList, mapChannelClient_t *client, npc_t *npc, mission_t **outMissionList, sint32 *outLimit);

void mission_acceptForClient(mapChannelClient_t *client, mission_t *mission);
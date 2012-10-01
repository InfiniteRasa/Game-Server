

/*
	NPCs are all sint32eractable characters.
	
*/


#define NPC_TYPE_NONE	0
#define NPC_TYPE_VENDOR	1

typedef struct  
{
	sint8 vendorType;

}metaNpc_vendor_t;


typedef struct _npc_t 
{
	unsigned long long entityId;		// the generated unique entityId
	//sint32 classId;						// the classId (defined in entityClass.pyo) (is stored inside actor)
	actor_t actor;						// the base actor object
	missionList_t *missionList;			// available missions
	sint8 npcType;						// the npc type (see above)
	union{								// union for easy pointer access of type specific data
		metaNpc_vendor_t *metaVendor;
	};
	// behavior controller
	behaviorState_t controller;
}npc_t;

void npc_test(mapChannelClient_t *client);

npc_t* npc_createNPC(mapChannel_t *mapChannel, sint32 classId, sint8 *name);

// set (must not be called after addToWorld, does not take effect on client instantly)
void npc_setLocation(npc_t *npc, float x, float y, float z, float rX, float rY);
void npc_setType(npc_t *npc, sint32 npcType);
void npc_setMissionList(npc_t *npc, missionList_t *missionList);
void npc_setAppearanceEntry(npc_t *npc, sint32 entryIndex, sint32 classId, uint32 hue);

// update
void npc_updateConversationStatus(mapChannelClient_t *client, npc_t *npc);
void npc_updateAppearanceItem(mapChannel_t *mapChannel, npc_t *npc, uint32 itemClassId, uint32 hue);
void npc_updateName(mapChannel_t *mapChannel, npc_t *npc, sint8 *newName);


void npc_recv_RequestNPCConverse(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void npc_recv_RequestNPCVending(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);

void npc_recv_AssignNPCMission(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);


void npc_cellIntroduceNPCToClients(mapChannel_t *mapChannel, npc_t *npc, mapChannelClient_t **playerList, sint32 playerCount);
void npc_cellIntroduceNPCsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, npc_t **npcList, sint32 npcCount);

void npc_cellDiscardNPCToClients(mapChannel_t *mapChannel, npc_t *npc, mapChannelClient_t **playerList, sint32 playerCount);
void npc_cellDiscardNPCsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, npc_t **npcList, sint32 npcCount);

void npc_initForMapChannel(mapChannel_t *mapChannel);
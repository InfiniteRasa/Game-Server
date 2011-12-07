

/*
	NPCs are all interactable characters.
	
*/


#define NPC_TYPE_NONE	0
#define NPC_TYPE_VENDOR	1

typedef struct  
{
	char vendorType;

}metaNpc_vendor_t;


typedef struct _npc_t 
{
	unsigned long long entityId;		// the generated unique entityId
	//int classId;						// the classId (defined in entityClass.pyo) (is stored inside actor)
	actor_t actor;						// the base actor object
	missionList_t *missionList;			// available missions
	char npcType;						// the npc type (see above)
	union{								// union for easy pointer access of type specific data
		metaNpc_vendor_t *metaVendor;
	};
	// behavior controller
	behaviorState_t controller;
}npc_t;

void npc_test(mapChannelClient_t *client);

npc_t* npc_createNPC(mapChannel_t *mapChannel, int classId, char *name);

// set (must not be called after addToWorld, does not take effect on client instantly)
void npc_setLocation(npc_t *npc, float x, float y, float z, float rX, float rY);
void npc_setType(npc_t *npc, int npcType);
void npc_setMissionList(npc_t *npc, missionList_t *missionList);
void npc_setAppearanceEntry(npc_t *npc, int entryIndex, int classId, unsigned int hue);

// update
void npc_updateConversationStatus(mapChannelClient_t *client, npc_t *npc);
void npc_updateAppearanceItem(mapChannel_t *mapChannel, npc_t *npc, unsigned int itemClassId, unsigned int hue);
void npc_updateName(mapChannel_t *mapChannel, npc_t *npc, char *newName);


void npc_recv_RequestNPCConverse(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen);
void npc_recv_RequestNPCVending(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen);

void npc_recv_AssignNPCMission(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen);


void npc_cellIntroduceNPCToClients(mapChannel_t *mapChannel, npc_t *npc, mapChannelClient_t **playerList, int playerCount);
void npc_cellIntroduceNPCsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, npc_t **npcList, int npcCount);

void npc_cellDiscardNPCToClients(mapChannel_t *mapChannel, npc_t *npc, mapChannelClient_t **playerList, int playerCount);
void npc_cellDiscardNPCsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, npc_t **npcList, int npcCount);

void npc_initForMapChannel(mapChannel_t *mapChannel);
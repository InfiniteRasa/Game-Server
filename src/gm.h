
typedef struct  
{
	float x;
	float y;
	float z;
	sint64 entityId_nodeDebugObject;
}gmPathNode_t;

typedef struct  
{
	std::vector<gmPathNode_t> pathNodes;
	sint32 contextId;
}gmPathCreation_t;

typedef struct  
{
	// additional data that is necessary only for GMs
	// can be accessed by cgm->manifestation->gmData
	gmPathCreation_t* pathCreation;
	sint32 lastCreatedSpawnpool; // contains the id of the last created spawnpool (or zero if there is none)
}gmData_t;

void gm_startPathCreation(mapChannelClient_t *cm);
void gm_addPathNode(mapChannelClient_t *cm);

bool gm_parseGmCommands(mapChannelClient_t *cm, char *textMsg);
void dynamicObject_init(mapChannel_t *mapChannel);

void dynamicObject_recv_RequestUseObject(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen);

bool _dynamicObject_controlpoint_callback(mapChannel_t *mapChannel, void *param, int timePassed);

void dynamicObject_recv_RequestActionInterrupt(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen);

typedef struct  
{
	int phaseTimeleft;
	char phase;
	spawnPool_t *spawnPool; // spawnPool the dropship belongs to
	char spawnCount;
	creatureType_t *spawnTypeList[24]; // maximal 24 creatures can be spawned at once
}dynObject_baneDropship_t;

typedef struct _dynObject_t
{
	unsigned long long entityId;
	unsigned short objectType;
	unsigned long long entityClassId;
	void *objectData;
	// universal position data
	float x;
	float y;
	float z;
	float rotX;
	float rotY;
	float rotZ;
	//float velocity;
	//float viewX;
	//float viewY;
	// useable data
	int stateId;
	// todo: rotation quarterninion
	mapCellLocation_t cellLocation;
}dynObject_t;

typedef struct
{
	int period;
	int timeLeft;
	dynObject_t *object;
	unsigned long long entityId;
}dynObject_workEntry_t; 

void dynamicObject_cellIntroduceObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, int playerCount);
void dynamicObject_cellIntroduceObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, int objectCount);
void dynamicObject_cellDiscardObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, int playerCount);
void dynamicObject_cellDiscardObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, int objectCount);

void dynamicObject_check(mapChannel_t *mapChannel, int timePassed);
bool dynamicObject_process(mapChannel_t *mapChannel, dynObject_t *dynObject, int timePassed);


// developer / testing
void dynamicObject_developer_createFootlocker(mapChannel_t *mapChannel, float x, float y, float z);
void dynamicObject_developer_createControlPoint(mapChannel_t *mapChannel, float x, float y, float z);
void dynamicObject_developer_createCustom(mapChannel_t *mapChannel, int classId, float x, float y, float z);

void dynamicObject_createBaneDropship(mapChannel_t *mapChannel, float x, float y, float z, int spawnCount, creatureType_t **spawnTypeList, spawnPool_t *spawnPool);
void dynamicObject_createBaneDropship(mapChannel_t *mapChannel, float x, float y, float z, int spawnCount, creatureType_t **spawnTypeList);

void dynamicObject_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject);
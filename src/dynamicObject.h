// ids are used only for the database
#define OBJECTTYPE_FOOTLOCKER		1
#define OBJECTTYPE_UNDEFINED		2
#define OBJECTTYPE_BANE_DROPSHIP	3
#define OBJECTTYPE_CONTROL_POINT	4
#define OBJECTTYPE_LOGOS			5
#define OBJECTTYPE_HUMAN_DROPSHIP	6
#define OBJECTTYPE_TELEPORTER		7
#define OBJECTTYPE_WAYPOINT			8
#define OBJECTTYPE_DOOR				9
#define OBJECTTYPE_FORCEFIELD		10
#define OBJECTTYPE_AFS_TURRET		11
#define OBJECTTYPE_BASEWORMHOLE		14
#define OBJECTTYPE_TORIODCANNON		15

void dynamicObject_init(mapChannel_t *mapChannel);

void dynamicObject_recv_RequestUseObject(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);

bool _dynamicObject_controlpoint_callback(mapChannel_t *mapChannel, void *param, sint32 timePassed);
void dynamicObject_createLogosObject(mapChannel_t *mapChannel, float x, float y, float z);
//void dynamicObject_createHumanDropship(mapChannel_t *mapChannel, float x, float y, float z);
void dynamicObject_forceState(clientGamemain_t* cgm, uint32 entityId, sint32 state);

void dynamicObject_recv_RequestActionInterrupt(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);

typedef struct  
{
	//void (*init)(mapChannel_t *mapChannel, dynObject_t *dynObject); // called right after creation of the object
	void (*destroy)(mapChannel_t *mapChannel, dynObject_t *dynObject); // called before free()
	void (*appearForPlayers)(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount);	// enter sight, created
	void (*disappearForPlayers)(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount); // leave sight, deleted
	bool (*periodicCallback)(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed); // timerId can be used to distinguish between multiple timers on the same object, timePassed is the time since the last callback in MS 	
	void (*useObject)(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg);
	void (*interruptUse)(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg);
	//void (*completeUse)(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg);
}dynObjectType_t;

typedef struct _dynObject_t
{
	unsigned long long entityId;
	//uint16 objectType;
	dynObjectType_t* type;
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
	sint32 stateId;
	// todo: rotation quarterninion
	mapCellLocation_t cellLocation;
}dynObject_t;

typedef struct
{
	sint32 period;
	sint32 timeLeft;
	dynObject_t *object;
	uint8 timerID;
	unsigned long long entityId;
}dynObject_workEntry_t; 

void dynamicObject_cellIntroduceObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, sint32 playerCount);
void dynamicObject_cellIntroduceObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, sint32 objectCount);
void dynamicObject_cellDiscardObjectToClients(mapChannel_t *mapChannel, dynObject_t *dynObj, mapChannelClient_t **playerList, sint32 playerCount);
void dynamicObject_cellDiscardObjectsToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, dynObject_t **objectList, sint32 objectCount);

void dynamicObject_check(mapChannel_t *mapChannel, sint32 timePassed);
bool dynamicObject_process(mapChannel_t *mapChannel, dynObject_t *dynObject, sint32 timePassed);

// useable states (for force state)
#define USE_ID_STATE_ACTIVE		81	 // augmentation [34]
#define USE_ID_STATE_INACTIVE	82	 // augmentation [34]


// developer / testing

dynObject_t *_dynamicObject_create(uint32 classId, dynObjectType_t* objectType);
void dynamicObject_setPosition(dynObject_t *dynamicObject, float x, float y, float z);
void dynamicObject_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject);

void dynamicObject_setRotation(dynObject_t *dynamicObject, float pitch, float yaw, float roll);
void dynamicObject_setPeriodicUpdate(mapChannel_t *mapChannel, dynObject_t *dynamicObject, uint8 timerID, sint32 periodInMS);
void dynamicObject_stopPeriodicUpdate(mapChannel_t *mapChannel, dynObject_t *dynamicObject, uint8 timerID);

#include"defaultObject.h"
#include"banedropship.h"
#include"humandropship.h"
#include"waypoint.h"
#include"controlpoint.h"
#include"logosObject.h"
#include"lootdispenser.h"
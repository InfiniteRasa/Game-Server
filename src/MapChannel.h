
typedef struct _mapChannel_t mapChannel_t;
typedef struct _clientGamemain_t clientGamemain_t;
typedef struct _manifestation_t manifestation_t;

#define CHANNEL_LIMIT 14

typedef struct  
{
	uint64 entityId;
	sint32 actionId;
	sint32 actionArg;
}mapChannelClient_objectUse_t;

typedef struct _mapChannelClient_t
{
	unsigned long long clientEntityId;
	clientGamemain_t *cgm;
	manifestation_t *player;
	mapChannel_t *mapChannel;
	di_characterData_t *tempCharacterData; // used while loading
	bool logoutActive;
	unsigned long long logoutRequestedLast;
	bool removeFromMap; // should be removed from the map at end of processing
	bool disconnected; // client disconnected (do not pass to other)
	// chat
	uint32 joinedChannels;
	uint32 channelHashes[CHANNEL_LIMIT];
	// inventory data
	inventory_t inventory;
	// mission data
	hashTable_t mission;
	// object interaction
	mapChannelClient_objectUse_t usedObject;
	// gm flags
	bool gmFlagAlwaysFriendly;
}mapChannelClient_t;

#define MAPCHANNEL_PLAYERQUEUE 32

typedef struct  
{
	sint32 period;
	sint32 timeLeft;
	void *param;
	bool (*cb)(mapChannel_t *mapChannel, void *param, sint32 timePassed);
}mapChannelTimer_t;

typedef struct  
{
	sint32 delay;
	sint32 timeLeft;
	//manifestation_t* origin;
	//item_t* weapon;
	mapChannelClient_t* client;
}mapChannelAutoFireTimer_t;

/*
 * For every map instance, there exists 1 map channel.
 * But there can exist multiple map instances for a single map.
 */
typedef struct _mapChannel_t
{
	gameData_mapInfo_t *mapInfo;
	mapChannelClient_t **playerList;
	sint32 loadState; // temporary variable used for multithreaded but synchronous operations
	sint32 playerCount;
	sint32 playerLimit;
	hashTable_t ht_socketToClient; // maps socket to client structure
	// ringbuffer for passed players
	clientGamemain_t *rb_playerQueue[MAPCHANNEL_PLAYERQUEUE];
	sint32 rb_playerQueueReadIndex;
	sint32 rb_playerQueueWriteIndex;
	TMutex criticalSection;
	// timers
	uint32 timer_clientEffectUpdate;
	uint32 timer_missileUpdate;
	uint32 timer_dynObjUpdate;
	uint32 timer_generalTimer;
	uint32 timer_controller;
	uint32 timer_playerUpdate;
	//mapChannelTimer_t cp_trigger; 
	// other timers
	std::vector<mapChannelAutoFireTimer_t> autoFire_timers;
	std::vector<mapChannelTimer_t*> timerList;
	// cell data
	mapCellInfo_t mapCellInfo;
	// missile data
	missileInfo_t missileInfo;
	// dynamic object info ( contains only objects that require regular updates )
	std::vector<dynObject_workEntry_t*> updateObjectList;
	// list of all waypoints on this map
	std::vector<dynObject_t*> waypoints;
	// list of all wormholes on this map
	std::vector<dynObject_t*> wormholes;
	// navmesh
	dtNavMesh* navMesh;
	dtNavMeshQuery* navMeshQuery;
	// effect
	uint32 currentEffectID; // increases with every spawned game effect
}mapChannel_t;

typedef struct 
{
	mapChannel_t *mapChannelArray;
	sint32 mapChannelCount;
}mapChannelList_t;

void mapChannel_init();
void mapChannel_start(sint32 *contextIdList, sint32 contextCount);
mapChannel_t *mapChannel_findByContextId(sint32 contextId);
bool mapChannel_pass(mapChannel_t *mapChannel, clientGamemain_t *cgm);

// timer
//void mapChannel_registerTimer(mapChannel_t *mapChannel, sint32 period, void *param, bool (*cb)(mapChannel_t *mapChannel, void *param, sint32 timePassed));
void mapChannel_registerTimer(mapChannel_t *mapChannel, sint32 period, void *param, bool (*cb)(mapChannel_t *mapChannel, void *param, sint32 timePassed));
void mapChannel_registerAutoFireTimer(mapChannelClient_t* cm);
void mapChannel_removeAutoFireTimer(mapChannelClient_t* cm);
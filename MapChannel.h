
typedef struct _mapChannel_t mapChannel_t;
typedef struct _clientGamemain_t clientGamemain_t;
typedef struct _manifestation_t manifestation_t;

#define CHANNEL_LIMIT 14

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
	unsigned int joinedChannels;
	unsigned int channelHashes[CHANNEL_LIMIT];
	// inventory data
	inventory_t inventory;
	// mission data
	HashTable_uint32Iterable_t mission;
}mapChannelClient_t;

#define MAPCHANNEL_PLAYERQUEUE 32

typedef struct  
{
	int period;
	int timeLeft;
	void *param;
	bool (*cb)(mapChannel_t *mapChannel, void *param, int timePassed);
}mapChannelTimer_t;

typedef struct _mapChannel_t
{
	gameData_mapInfo_t *mapInfo;
	mapChannelClient_t **playerList;
	int loadState; // temporary variable used for multithreaded but synchronous operations
	int playerCount;
	int playerLimit;
	HashTable_uint32_t ht_socketToClient; // maps socket to client structure
	// ringbuffer for passed players
	clientGamemain_t *rb_playerQueue[MAPCHANNEL_PLAYERQUEUE];
	int rb_playerQueueReadIndex;
	int rb_playerQueueWriteIndex;
	CRITICAL_SECTION criticalSection;
	// timers
	unsigned int timer_clientEffectUpdate;
	unsigned int timer_missileUpdate;
	unsigned int timer_dynObjUpdate;
	unsigned int timer_generalTimer;
	unsigned int timer_controller;
	mapChannelTimer_t cp_trigger;
	// other timers
	HashTable_uint32Iterable_t ht_timerList; // Todo: relace this with a list implementation
	// cell data
	mapCellInfo_t mapCellInfo;
	// missile data
	missileInfo_t missileInfo;
	// dynamic object info ( contains only objects that require regular updates )
	HashTable_uint32Iterable_t ht_updateObjectList;
}mapChannel_t;

typedef struct 
{
	mapChannel_t *mapChannelArray;
	int mapChannelCount;
}mapChannelList_t;

void mapChannel_init();
void mapChannel_start(int *contextIdList, int contextCount);
mapChannel_t *mapChannel_findByContextId(int contextId);
bool mapChannel_pass(mapChannel_t *mapChannel, clientGamemain_t *cgm);

// timer
void mapChannel_registerTimer(mapChannel_t *mapChannel, int period, void *param, bool (*cb)(mapChannel_t *mapChannel, void *param, int timePassed));
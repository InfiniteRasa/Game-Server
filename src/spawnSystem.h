#define SPAWNTYPE_ANIMATION_NONE 0
#define SPAWNTYPE_ANIMATION_BEAM 1
#define SPAWNTYPE_ANIMATION_DROPSHIP 2
 

typedef struct  
{
	float x;
	float y;
	float z;
	//uint16 creatureTypeFilter; // a mask that enables the specific creatures defined by the spawn pool
	// used during runtime
	//uint32 lastSpawnTime;
}spawnLocation_t;

typedef struct  
{
	sint16 creatureType;
	sint8 countMin;
	sint8 countMax;
}spawnPool_spawnSlot_t;

#define SPAWNPOOL_SPAWNSLOTS	6

#define SPAWNPOOL_ANIMTYPE_NONE				0
#define SPAWNPOOL_ANIMTYPE_BANE_DROPSHIP	1
#define SPAWNPOOL_ANIMTYPE_HUMAN_DROPSHIP	2

#define SPAWNPOOL_MODE_AUTOMATIC			0

typedef struct _spawnPool_t
{
	sint32 id; // id of the spawnpool
	spawnPool_spawnSlot_t spawnSlot[SPAWNPOOL_SPAWNSLOTS];
	// different spawn points
	sint32 locationCount;
	spawnLocation_t *locationList;
	// currently the DB structure only supports 1 spawn location, but the spawn system code already allows for multiple
	// spawn settings
	//sint32 maxCreatures; // use depends on criteria
	//sint32 spawnLocationLockTime; // number of milliseconds that have to pass until a location can be used again for spawning
	sint8 mode; // automatic spawning, CP spawn, scripted spawn (manual trigger)
	sint8 animType; // which effect is used to spawn creatures (bane dropship, no effect, human dropship)
	//sint8 criteria;
	// spawn runtime info
	sint32 dropshipQueue; // number of dropships that are currently delivering units
	sint32 queuedCreatures; // number of creatures that are spawning right now (i.e. delivered via dropship)
	sint32 aliveCreatures; // number of spawned creatures that are alive
	sint32 deadCreatures; // number of spawned creatures that are dead (either killed or spawned dead)

	// respawn lock
	uint32 spawnLockTime;
	uint32 respawnTime;
	// paths
	sint32 pathCount;
	aiPath_t** pathList;



	//baseBehavior_baseNode *pathnodes;
	//sint32 attackspeed;
	//sint32 attackanim;
	//float velocity;
	//sint32 attackstyle;
	//sint32 actionid;
	// default action assignment
	// nothing here until 
}spawnPool_t;

//#define SPAWN_CRITERIA_CELLCREATURECOUNT		1	// amount of living creatures in the same cell as the spawnLocation
//#define SPAWN_CRITERIA_SPAWNPOOLCREATURECOUNT	2	// todo
//#define SPAWN_CRITERIA_BOSSSPAWNER			3	// todo


void spawnPool_initForMapChannel(mapChannel_t *mapChannel);

spawnPool_t *spawnPool_create();
void spawnPool_setCriteriaCellCreatureCount(spawnPool_t *spawnPool, sint32 maxCount);
bool spawnPool_setCreatureVariant(spawnPool_t *spawnPool, sint32 index, sint8 *creatureTypeName);
void spawnPool_initSpawnLocations(spawnPool_t *spawnPool, sint32 number);
void spawnPool_setSpawnLocation(spawnPool_t *spawnPool, sint32 index, float x, float y, float z, uint16 typeMask);
void spawnPool_activate(mapChannel_t *mapChannel, spawnPool_t *spawnPool);

// count dropships
void spawnPool_increaseQueueCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_decreaseQueueCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);

// count creatures
void spawnPool_increaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, sint32 count);
void spawnPool_increaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_increaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_decreaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, sint32 count);
void spawnPool_decreaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_decreaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
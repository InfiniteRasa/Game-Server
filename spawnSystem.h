#define SPAWNTYPE_ANIMATION_NONE 0
#define SPAWNTYPE_ANIMATION_BEAM 1
#define SPAWNTYPE_ANIMATION_DROPSHIP 2
 

typedef struct  
{
	float x;
	float y;
	float z;
	unsigned short creatureTypeFilter; // a mask that enables the specific creatures defined by the spawn pool
	// used during runtime
	unsigned int lastSpawnTime;
}spawnLocation_t;

typedef struct _spawnPool_t
{
	creatureType_t *spawnTypes[16]; // 16 different creature types available at max, NULL if not set
	char spawnCountBase[16];	// minimum creatures to spawn at once
	char spawnCountRandom[16]; // additional random factor of creatures to spawn
	char spawnVariantCount; // 0 - 16
	// different spawn points
	int locationCount;
	spawnLocation_t *locationList;
	// spawn settings
	int maxCreatures; // use depends on criteria
	int maxQueueLength; // maximal number of delivering dropships
	int spawnLocationLockTime; // number of milliseconds that have to pass until a location can be used again for spawning
	char criteria;
	// spawn runtime info
	int dropshipQueue; // number of dropships that are currently delivering units
	int queuedCreatures; // number of creatures that are spawning right now (i.e. delivered via dropship)
	int aliveCreatures; // number of spawned creatures that are alive
	int deadCreatures; // number of spawned creatures that are dead (either killed or spawned dead)
	int spawnAnimationType; //which type of animation introduces creatures
	int faction;
	baseBehavior_baseNode *pathnodes;
	int attackspeed;
	int attackanim;
	float velocity;
	int attackstyle;
	int actionid;
	// default action assignment
	// nothing here until 
}spawnPool_t;

#define SPAWN_CRITERIA_CELLCREATURECOUNT		1	// amount of living creatures in the same cell as the spawnLocation
#define SPAWN_CRITERIA_SPAWNPOOLCREATURECOUNT	2	// todo
#define SPAWN_CRITERIA_BOSSSPAWNER				3	// todo


void spawnPool_initForMapChannel(mapChannel_t *mapChannel);

spawnPool_t *spawnPool_create();
void spawnPool_setCriteriaCellCreatureCount(spawnPool_t *spawnPool, int maxCount);
bool spawnPool_setCreatureVariant(spawnPool_t *spawnPool, int index, char *creatureTypeName);
void spawnPool_initSpawnLocations(spawnPool_t *spawnPool, int number);
void spawnPool_setSpawnLocation(spawnPool_t *spawnPool, int index, float x, float y, float z, unsigned short typeMask);

// count dropships
void spawnPool_increaseQueueCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_decreaseQueueCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);

// count creatures
void spawnPool_increaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, int count);
void spawnPool_increaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_increaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_decreaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, int count);
void spawnPool_decreaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
void spawnPool_decreaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool);
#include"Global.h"

spawnPool_t *spawnPool_create()
{
	spawnPool_t *spawnPool = (spawnPool_t*)malloc(sizeof(spawnPool_t));
	memset(spawnPool, 0x00, sizeof(spawnPool_t));
	// setup some default values
	spawnPool->maxQueueLength = 3;
	spawnPool->spawnLocationLockTime = 15000; // 15 seconds
	return spawnPool;
}

void spawnPool_setCriteriaCellCreatureCount(spawnPool_t *spawnPool, int maxCount)
{
	spawnPool->criteria = SPAWN_CRITERIA_CELLCREATURECOUNT;
	spawnPool->maxCreatures = maxCount; // maximum creatures per cell
}

bool spawnPool_setCreatureVariant(spawnPool_t *spawnPool, int index, char *creatureTypeName)
{
	creatureType_t *creatureType = creatureType_findType(creatureTypeName);
	if( index<0 || index >= 16 )
		return false;
	if( creatureType == NULL )
		return false;
	if( spawnPool->spawnTypes[index] == NULL )
		spawnPool->spawnVariantCount++;
	spawnPool->spawnTypes[index] = creatureType;
	return true;
}

void spawnPool_initSpawnLocations(spawnPool_t *spawnPool, int number)
{
	// free previous
	if( spawnPool->locationList )
	{
		free(spawnPool->locationList);
		spawnPool->locationList = NULL;
		spawnPool->locationCount = 0;
	}
	// early escape if no creatures
	if( number == 0 )
		return;
	// init list
	spawnLocation_t *locationList = (spawnLocation_t*)malloc(sizeof(spawnLocation_t)*number);
	for(int i=0; i<number; i++)
	{
		locationList[i].creatureTypeFilter = 0; //deactivated
		locationList[i].x = 0.0f;
		locationList[i].y = 0.0f;
		locationList[i].z = 0.0f;
	}
	// set list and count
	spawnPool->locationList = locationList;
	spawnPool->locationCount = number;
}

void spawnPool_setSpawnLocation(spawnPool_t *spawnPool, int index, float x, float y, float z, unsigned short typeMask)
{
	if( index < 0 )
		return;
	if( index >= spawnPool->locationCount )
		return;
	// set settings
	spawnPool->locationList[index].x = x;
	spawnPool->locationList[index].y = y;
	spawnPool->locationList[index].z = z;
	spawnPool->locationList[index].lastSpawnTime = GetTickCount();
	spawnPool->locationList[index].creatureTypeFilter = typeMask;
}

bool _spawnPool_callback(mapChannel_t *mapChannel, void *param, int timePassed)
{
	spawnPool_t *spawnPool = (spawnPool_t*)param; 
	// check simple spawn conditions
	if( spawnPool->spawnVariantCount == 0 ) // nothing to spawn?
		return true;
	if( spawnPool->dropshipQueue >= spawnPool->maxQueueLength ) // creaturespawner queue full?
		return true;
	// check criteria based spawnconditions
	// (todo: criteria switch)
	int totalCreaturesActive = spawnPool->aliveCreatures + spawnPool->queuedCreatures;
	if( totalCreaturesActive >= 5 ) // no more than 5 creatures at once
		return true;
	// get time
	unsigned int time = GetTickCount(); // todo: rather read the time from a global object time variable than via api everytime
	// get random spawnpoint
	int rSpawnLoc = rand()%spawnPool->locationCount;
	spawnLocation_t *location = spawnPool->locationList+rSpawnLoc;
	// is spawnpoint cooled down?
	if( (time-location->lastSpawnTime) < spawnPool->spawnLocationLockTime)
		return true;
	location->lastSpawnTime = time;
	// create list of units to spawn
	creatureType_t *spawnTypeList[24];
	int spawnCount = 0 + rand()%5;
	// TODO: location->creatureTypeFilter (bitmask of spawnPool variants)
	for(int i=0; i<spawnCount; i++)
	{
		spawnTypeList[i] = spawnPool->spawnTypes[rand()%spawnPool->spawnVariantCount];
	}
	// nothing to spawn, then also no dropship
	if( spawnCount == 0 )
		return true;
	// create dropship
	dynamicObject_createBaneDropship(mapChannel, location->x, location->y, location->z, spawnCount, spawnTypeList, spawnPool);
	return true;
}

void spawnPool_activate(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	mapChannel_registerTimer(mapChannel, 3000, spawnPool, _spawnPool_callback);
}

void spawnPool_increaseQueueCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->dropshipQueue++;
}

void spawnPool_decreaseQueueCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->dropshipQueue--;
}

void spawnPool_increaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->aliveCreatures++;
}

void spawnPool_increaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->deadCreatures++;
}

void spawnPool_increaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, int count)
{
	spawnPool->queuedCreatures+=count;
}

void spawnPool_decreaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->aliveCreatures--;
}

void spawnPool_decreaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->deadCreatures--;
}

void spawnPool_decreaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, int count)
{
	spawnPool->queuedCreatures-=count;
}



/*
 * Should load spawn data from database
 * Creature types are already initialized
 */
void spawnPool_initForMapChannel(mapChannel_t *mapChannel)
{
	if( mapChannel->mapInfo->contextId != 0x7C1 )
		return; 
	// only init the test pool for the bootcamp map
	// test pool
	/*
	-200 95 -24
	-197 96 -44
	-217 99 -54
	*/

	spawnPool_t *spawnPool = spawnPool_create();
	spawnPool_setCriteriaCellCreatureCount(spawnPool, 5);
	spawnPool_setCreatureVariant(spawnPool, 0, "TEST25580");
	spawnPool_setCreatureVariant(spawnPool, 1, "TEST25581");
	spawnPool_initSpawnLocations(spawnPool, 3);
	spawnPool_setSpawnLocation(spawnPool, 0, -200.0f, 95.0f, -24.0f, 0xFFFF);
	spawnPool_setSpawnLocation(spawnPool, 1, -197.0f, 96.0f, -44.0f, 0xFFFF);
	spawnPool_setSpawnLocation(spawnPool, 2, -217.0f, 99.0f, -54.0f, 0xFFFF);

	spawnPool_activate(mapChannel, spawnPool);
}
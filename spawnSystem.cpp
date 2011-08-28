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
	if( totalCreaturesActive >= 10 ) // no more than 10 creatures at once
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
	int spawnCount = 0 + rand()%spawnPool->maxCreatures;
	// TODO: location->creatureTypeFilter (bitmask of spawnPool variants)
	for(int i=0; i<spawnCount; i++)
	{
		spawnTypeList[i] = spawnPool->spawnTypes[rand()%spawnPool->spawnVariantCount];
	}
	// nothing to spawn, then also no dropship
	if( spawnCount == 0 )
		return true;
	//---type==0; spawn without animation
	if(spawnPool->spawnAnimationType == 0)
	{
		if( spawnPool )
		{
			spawnPool_increaseQueueCount(mapChannel, spawnPool);
			spawnPool_increaseQueuedCreatureCount(mapChannel, spawnPool, spawnCount);
		}
		//__debugbreak();	
		// spawn creatures 
		srand(GetTickCount());
		for(int i=0; i< spawnCount;i++)
		{
			creature_t *creature = creature_createCreature(mapChannel, spawnTypeList[i], spawnPool,1);
			if( creature == NULL )
				continue;
			
			creature_setLocation(creature, location->x+(float)(rand() % 5 +1), location->y, location->z+(float)(rand() % 5 +1), 0.0f, 0.0f);
			cellMgr_addToWorld(mapChannel, creature);
			
		}
		if( spawnPool )
		spawnPool_decreaseQueuedCreatureCount(mapChannel, spawnPool, spawnCount);
		if( spawnPool )
			spawnPool_decreaseQueueCount(mapChannel,spawnPool);
	}
	//---type==2; bane dropship animation
	if(spawnPool->spawnAnimationType == 2)
	{
         // create dropship
	    dynamicObject_createBaneDropship(mapChannel, location->x, location->y, location->z, spawnCount, spawnTypeList, spawnPool);
	}
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

//###################################### init spawnpool #####################################
void _cb_spawnPool_initForMapChannel(void *param, diJob_spawnTypeW2_t *jobData)
{
	//__debugbreak();
	mapChannel_t *mapChannel = (mapChannel_t*)param;
	//---allocate spawnpools
	spawnPool_t *spawnPool = (spawnPool_t*)malloc(sizeof(spawnPool_t) * jobData->scount);	
	memset(spawnPool, 0x00, sizeof(spawnPool_t) * jobData->scount);

	
	for(int i=0; i<jobData->scount; i++)
	{
		di_spawnTypeW2_t *npcData = jobData->spawnType+i;
		if(npcData->locationlist == NULL || npcData->spawnlocCount <=0) continue;
		spawnPool_t *tPool = spawnPool+i;
		tPool->maxQueueLength = npcData->activeSpawnCount; //simultaneously used spawns
		tPool->spawnLocationLockTime = npcData->locktime;
		tPool->spawnAnimationType = npcData->anim_type;
		tPool->faction = npcData->faction;
		spawnPool_setCriteriaCellCreatureCount(tPool,npcData->maxcreatures);
		char tname[70];
		sprintf(tname,"TEST%s",npcData->creatures);
		spawnPool_setCreatureVariant(tPool, 0, tname);
		spawnPool_initSpawnLocations(tPool,npcData->spawnlocCount);
		int tspawncount = npcData->spawnlocCount;
		//---allocate spawn locations
		di_spawnDataW2_t *locationlist = (di_spawnDataW2_t*)malloc(sizeof(di_spawnDataW2_t) * tspawncount);	
		locationlist = npcData->locationlist;
		//memcpy(locationlist,npcData->locationlist,sizeof(di_spawnDataW2_t) * tspawncount);
		for( int sc = 0; sc < tspawncount; sc ++)
		{
            float tx = (locationlist+sc)->posX;
			float ty = (locationlist+sc)->posY;
			float tz = (locationlist+sc)->posZ;
			spawnPool_setSpawnLocation(tPool,sc,tx,ty,tz, 0xFFFF);

		}		

        spawnPool_activate(mapChannel,tPool);
	}

	mapChannel->loadState = 1; // loading is done
}


/*
 * Should load spawn data from database
 * Creature types are already initialized
 */
void spawnPool_initForMapChannel(mapChannel_t *mapChannel)
{

	/*
	  1) load spawndata/type from db
	     +cellcreaturecount,creaturevariant,spawntype_id
		 +for each db-entry: spawnPool_create() 
		                     setCriteriaCellCreatureCount(db-value)
							 loop(cntcreatures):setCreatureVariant(token)
							 cnt = number of spawnloc with ID=spawntype;initSpawnLocations(cnt)
							 finally:spawnPool_activate() after set location
	  2) load spawnlocations from db
	     +for each db-entry with ID = spawntype: spawnPool_setSpawnLocation()
	
	*/
	//__debugbreak();	
	mapChannel->loadState = 0;
	dataInterface_Spawn_getSpawnpool(mapChannel->mapInfo->contextId,_cb_spawnPool_initForMapChannel, mapChannel);
	while( mapChannel->loadState == 0 ) Sleep(100);
	
}
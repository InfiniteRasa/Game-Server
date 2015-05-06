#include"Global.h"

spawnPool_t *spawnPool_create()
{
	spawnPool_t *spawnPool = (spawnPool_t*)malloc(sizeof(spawnPool_t));
	memset(spawnPool, 0x00, sizeof(spawnPool_t));
	// setup some default values
	//spawnPool->maxQueueLength = 3;
	//spawnPool->spawnLocationLockTime = 35000; // 15 seconds
	return spawnPool;
}

//void spawnPool_setCriteriaCellCreatureCount(spawnPool_t *spawnPool, sint32 maxCount)
//{
//	//spawnPool->criteria = SPAWN_CRITERIA_CELLCREATURECOUNT;
//	//spawnPool->maxCreatures = maxCount; // maximum creatures per cell
//}
//
//bool spawnPool_setCreatureVariant(spawnPool_t *spawnPool, sint32 index, sint8 *creatureTypeName)
//{
//	creatureType_t *creatureType = creatureType_findType(creatureTypeName);
//	if( index<0 || index >= 16 )
//		return false;
//	if( creatureType == NULL )
//		return false;
//	if( spawnPool->spawnTypes[index] == NULL )
//		spawnPool->spawnVariantCount++;
//	spawnPool->spawnTypes[index] = creatureType;
//	return true;
//}

//void spawnPool_initSpawnLocations(spawnPool_t *spawnPool, sint32 number)
//{
//	// free previous
//	if( spawnPool->locationList )
//	{
//		free(spawnPool->locationList);
//		spawnPool->locationList = NULL;
//		spawnPool->locationCount = 0;
//	}
//	// early escape if no creatures
//	if( number == 0 )
//		return;
//	// init list
//	spawnLocation_t *locationList = (spawnLocation_t*)malloc(sizeof(spawnLocation_t)*number);
//	for(sint32 i=0; i<number; i++)
//	{
//		locationList[i].creatureTypeFilter = 0; //deactivated
//		locationList[i].x = 0.0f;
//		locationList[i].y = 0.0f;
//		locationList[i].z = 0.0f;
//	}
//	// set list and count
//	spawnPool->locationList = locationList;
//	spawnPool->locationCount = number;
//}
//
//void spawnPool_setSpawnLocation(spawnPool_t *spawnPool, sint32 index, float x, float y, float z, uint16 typeMask)
//{
//	if( index < 0 )
//		return;
//	if( index >= spawnPool->locationCount )
//		return;
//	// set settings
//	spawnPool->locationList[index].x = x;
//	spawnPool->locationList[index].y = y;
//	spawnPool->locationList[index].z = z;
//	spawnPool->locationList[index].lastSpawnTime = GetTickCount();
//	spawnPool->locationList[index].creatureTypeFilter = typeMask;
//}

bool _spawnPool_callback(mapChannel_t *mapChannel, void *param, sint32 timePassed)
{
	spawnPool_t *spawnPool = (spawnPool_t*)param; 
	// check simple spawn conditions
	//if( spawnPool->spawnVariantCount == 0 ) // nothing to spawn?
	//	return true;
	//if( spawnPool->dropshipQueue > spawnPool->maxQueueLength ) // creaturespawner queue full? >=
	//	return true;
	// check criteria based spawnconditions
	// (todo: criteria switch)
	sint32 totalCreaturesActive = spawnPool->aliveCreatures + spawnPool->queuedCreatures;
	//if( totalCreaturesActive >= spawnPool->maxCreatures ) // max. creatures per spawnpoint >=
	//	return true;
	if( totalCreaturesActive )
		return true;
	// get time
	uint32 time = GetTickCount(); // todo: read the time from a global object time variable than via api everytime
	// get random spawnpoint
	sint32 rSpawnLoc = rand()%spawnPool->locationCount;
	spawnLocation_t *location = spawnPool->locationList+rSpawnLoc;
	// is spawnpoint cooled down?
	if( time < spawnPool->spawnLockTime )
		return true;
	// create list of units to spawn
	creatureType_t* spawnTypeList[64];
	sint32 spawnTypeCount = 0;
	//sint32 spawnCount = spawnPool->maxCreatures; 
	for(sint32 i=0; i<SPAWNPOOL_SPAWNSLOTS; i++)
	{
		if( spawnPool->spawnSlot[i].creatureType == 0 )
			continue;
		sint32 typeSpawnCount = spawnPool->spawnSlot[i].countMin + ((rand())%(spawnPool->spawnSlot[i].countMax-spawnPool->spawnSlot[i].countMin+1));
		creatureType_t* creatureType = creatureType_findType(spawnPool->spawnSlot[i].creatureType);
		if( creatureType == NULL )
		{
			printf("Could not find creature typeId %d for spawnpool\n", spawnPool->spawnSlot[i].creatureType);
			continue;
		}
		//spawnTypeIDList[i] = spawnPool->spawnTypes[rand()%spawnPool->spawnVariantCount];
		for(sint32 x=0; x<typeSpawnCount; x++)
		{
			spawnTypeList[spawnTypeCount] = creatureType;
			spawnTypeCount++;
			if( spawnTypeCount == 64 )
				break; // cannot spawn more than 64 creatures at once
		}
		if( spawnTypeCount == 64 )
			break;
	}
	// nothing to spawn, then also no dropship
	if( spawnTypeCount == 0 )
		return true;
	//---type==0; spawn without animation
	if(spawnPool->animType == SPAWNPOOL_ANIMTYPE_NONE)
	{
		if( spawnPool )
		{
			spawnPool_increaseQueueCount(mapChannel, spawnPool);
			spawnPool_increaseQueuedCreatureCount(mapChannel, spawnPool, spawnTypeCount);
		}
		//__debugbreak();	
		// spawn creatures 
		for(sint32 i=0; i<spawnTypeCount;i++)
		{
			creature_t *creature = creature_createCreature(mapChannel, spawnTypeList[i], spawnPool);
			if( creature == NULL )
				continue;
			// set ai path if spawnpool has any
			if( spawnPool->pathCount > 0 )
				creature->controller.aiPathFollowing.generalPath = spawnPool->pathList[rand()%spawnPool->pathCount]; // select random path
			// no random location if we spawn only one creature
			if( spawnTypeCount == 1 )
				creature_setLocation(creature, location->x, location->y, location->z, 0.0f, 0.0f);
			else
				creature_setLocation(creature, location->x+(float)((rand() % 5) - 2), location->y, location->z+(float)((rand() % 5) - 2), 0.0f, 0.0f);
			cellMgr_addToWorld(mapChannel, creature);
		}
		if( spawnPool )
			spawnPool_decreaseQueuedCreatureCount(mapChannel, spawnPool, spawnTypeCount);
		if( spawnPool )
			spawnPool_decreaseQueueCount(mapChannel,spawnPool);
	}
	//---type==2; bane dropship animation
	else if(spawnPool->animType == SPAWNPOOL_ANIMTYPE_BANE_DROPSHIP)
	{
        // create dropship
	    banedropship_create(mapChannel, location->x, location->y, location->z, spawnTypeCount, spawnTypeList, spawnPool);
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
	if( (spawnPool->dropshipQueue + spawnPool->queuedCreatures + spawnPool->aliveCreatures) == 0 )
		spawnPool->spawnLockTime = GetTickCount() + spawnPool->respawnTime;
}

void spawnPool_increaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->aliveCreatures++;
}

void spawnPool_increaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->deadCreatures++;
}

void spawnPool_increaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, sint32 count)
{
	spawnPool->queuedCreatures+=count;
}

void spawnPool_decreaseAliveCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->aliveCreatures--;
	if( (spawnPool->dropshipQueue + spawnPool->queuedCreatures + spawnPool->aliveCreatures) == 0 )
		spawnPool->spawnLockTime = GetTickCount() + spawnPool->respawnTime;
}

void spawnPool_decreaseDeadCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool)
{
	spawnPool->deadCreatures--;
}

void spawnPool_decreaseQueuedCreatureCount(mapChannel_t *mapChannel, spawnPool_t *spawnPool, sint32 count)
{
	spawnPool->queuedCreatures-=count;
	if( (spawnPool->dropshipQueue + spawnPool->queuedCreatures + spawnPool->aliveCreatures) == 0 )
		spawnPool->spawnLockTime = GetTickCount() + spawnPool->respawnTime;
}

//###################################### init spawnpool #####################################
void _cb_spawnPool_initForMapChannel(void *param, diJob_spawnpool_t *jobData)
{
	mapChannel_t *mapChannel = (mapChannel_t*)param;
	if( jobData == NULL )
	{
		// indicates that we are done
		mapChannel->loadState = 1;
		return;
	}
	//__debugbreak();
	if( jobData->contextId != mapChannel->mapInfo->contextId )
		return; // spawnpool is not for this map
	spawnPool_t *spawnPool = (spawnPool_t*)malloc(sizeof(spawnPool_t));
	memset(spawnPool, 0x00, sizeof(spawnPool_t));
	spawnPool->locationCount = 1; // eventually add support for multiple spawn locations
	spawnPool->locationList = (spawnLocation_t*)malloc(sizeof(spawnLocation_t) * spawnPool->locationCount);
	spawnPool->locationList[0].x = jobData->posX;
	spawnPool->locationList[0].y = jobData->posY;
	spawnPool->locationList[0].z = jobData->posZ;
	spawnPool->id = jobData->id;
	spawnPool->animType = jobData->animType;
	spawnPool->mode = jobData->mode;

	spawnPool->respawnTime = jobData->respawnTime * 1000; // convert to ms

	for(sint32 spawnSlot=0; spawnSlot<SPAWNPOOL_SPAWNSLOTS; spawnSlot++)
	{
		spawnPool->spawnSlot[spawnSlot].creatureType = jobData->spawnSlot[spawnSlot].creatureType;
		spawnPool->spawnSlot[spawnSlot].countMin = jobData->spawnSlot[spawnSlot].countMin;
		spawnPool->spawnSlot[spawnSlot].countMax = jobData->spawnSlot[spawnSlot].countMax;
	}

	// attach paths (if there are any)
	controller_attachPathsToSpawnpool(spawnPool);
	// enable spawnpool
	spawnPool_activate(mapChannel, spawnPool);

	//for(sint32 i=0; i<jobData->scount; i++)
	//{
	//	di_spawnTypeW2_t *npcData = jobData->spawnType+i;
	//	if(npcData->locationlist == NULL || npcData->spawnlocCount <=0) continue;
	//
	//	//---allocate spawnpools per location
	//	sint32 tspawncount = npcData->spawnlocCount;
	//    spawnPool_t *spawnPool = (spawnPool_t*)malloc(sizeof(spawnPool_t) * tspawncount);	
	//    memset(spawnPool, 0x00, sizeof(spawnPool_t) * tspawncount);
	//	
	//	//---allocate spawn locations
	//	di_spawnDataW2_t *locationlist = (di_spawnDataW2_t*)malloc(sizeof(di_spawnDataW2_t) * tspawncount);	
	//	locationlist = npcData->locationlist;
	//	
	//	for(sint32 sp = 0; sp < tspawncount; sp++)
	//	{
	//		spawnPool_t *tPool = spawnPool+sp;

	//		//---allocate pathnodes for current spawnpoint
	//        //tPool->pathnodes = (baseBehavior_baseNode*) malloc(sizeof(baseBehavior_baseNode) * (locationlist+sp)->nodeCount);
	//		//tPool->pathnodes = (locationlist+sp)->pathnodes;
	//		//tPool->maxQueueLength = npcData->activeSpawnCount; //simultaneously used spawns
	//		tPool->spawnLocationLockTime = npcData->locktime;
	//		tPool->spawnAnimationType = npcData->anim_type;
	//		tPool->faction = npcData->faction;
	//		tPool->attackanim = npcData->attackaction;
	//		tPool->attackspeed = npcData->attackspeed;
	//		tPool->velocity = npcData->velocity;
	//		tPool->attackstyle = npcData->attackstyle;
	//		tPool->actionid = npcData->actionid;
	//		spawnPool_setCriteriaCellCreatureCount(tPool,npcData->maxcreatures);
	//		sint8 tname[70];
	//		sprintf(tname,"TEST%s",npcData->creatures);
	//		spawnPool_setCreatureVariant(tPool, 0, tname);
	//		//use only one spawnlocation per spawnpoint
	//		spawnPool_initSpawnLocations(tPool,1); 
	//		
	//		//memcpy(locationlist,npcData->locationlist,sizeof(di_spawnDataW2_t) * tspawncount);
	//        float tx = (locationlist+sp)->posX;
	//		float ty = (locationlist+sp)->posY;
	//	    float tz = (locationlist+sp)->posZ;
	//		//use only one spawnlocation per spawnpoint	
	//	    spawnPool_setSpawnLocation(tPool,0,tx,ty,tz, 0xFFFF); 			
	//
	//        spawnPool_activate(mapChannel,tPool);
	//	}//---for:spawnpoints
	//}//---for: spawntypes

	
}


/*
 * Should load spawn data from database
 * Creature types are already initialized
 */
void spawnPool_initForMapChannel(mapChannel_t *mapChannel)
{
	mapChannel->loadState = 0;
	DataInterface_SpawnSystem_getSpawnPoolList(_cb_spawnPool_initForMapChannel, mapChannel);
	while( mapChannel->loadState == 0 ) Sleep(100);
}
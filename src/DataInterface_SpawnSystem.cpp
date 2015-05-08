#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"

sint32 _nextSpawnPoolId = 1;

void cb_DataInterface_SpawnSystem_getSpawnPoolList(MYSQL *dbCon, diJob_spawnpool_t *job, void *cb, void *param)
{
	sint8 queryText[4096];
	sprintf(queryText, "SELECT "
		"id,mode,animType,respawnTime,"
		"posx,posy,posz,contextid,"
		// creature slots
		"creatureType1,creatureMinCount1,creatureMaxCount1,"
		"creatureType2,creatureMinCount2,creatureMaxCount2,"
		"creatureType3,creatureMinCount3,creatureMaxCount3,"
		"creatureType4,creatureMinCount4,creatureMaxCount4,"
		"creatureType5,creatureMinCount5,creatureMaxCount5,"
		"creatureType6,creatureMinCount6,creatureMaxCount6"
		" FROM spawnpool");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// parse rows
	sint32 rowCount = mysql_num_rows(dbResult);
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		//"id,mode,animType,"
		//"posx,posy,posz,contextid,"
		//"creatureType1,creatureMinCount1,creatureMaxCount1,"
		//"creatureType2,creatureMinCount2,creatureMaxCount2,"
		//"creatureType3,creatureMinCount3,creatureMaxCount3,"
		//"creatureType4,creatureMinCount4,creatureMaxCount4,"
		//"creatureType5,creatureMinCount5,creatureMaxCount5,"
		//"creatureType6,creatureMinCount6,creatureMaxCount6"
		sint32 idx = 0;
		// general data
		sscanf(dbRow[idx], "%d", &job->id); idx++;
		sscanf(dbRow[idx], "%d", &job->mode); idx++;
		sscanf(dbRow[idx], "%d", &job->animType); idx++;
		sscanf(dbRow[idx], "%d", &job->respawnTime); idx++;
		// location data
		sscanf(dbRow[idx], "%f", &job->posX); idx++;
		sscanf(dbRow[idx], "%f", &job->posY); idx++;
		sscanf(dbRow[idx], "%f", &job->posZ); idx++;
		sscanf(dbRow[idx], "%d", &job->contextId); idx++;
		// update spawnpool id
		_nextSpawnPoolId = max(_nextSpawnPoolId, job->id+1);
		// creature spawn slots
		for(sint32 spawnSlot=0; spawnSlot<6; spawnSlot++)
		{
			if( dbRow[idx] == NULL || dbRow[idx+1] == NULL || dbRow[idx+2] == NULL )
			{
				job->spawnSlot[spawnSlot].creatureType = 0;
				idx += 3;
				continue;
			}
			sscanf(dbRow[idx], "%d", &job->spawnSlot[spawnSlot].creatureType); idx++;
			sscanf(dbRow[idx], "%d", &job->spawnSlot[spawnSlot].countMin); idx++;
			sscanf(dbRow[idx], "%d", &job->spawnSlot[spawnSlot].countMax); idx++;
		}
		// do callback param1: mapchannel param2: list of spawnpool data
		((void (*)(void*,void*))cb)(param, job);
	}
	// call with NULL job to inform callback that we are done
	((void (*)(void*,void*))cb)(param, NULL);

	mysql_free_result(dbResult);
	DataInterface_freeJob(job);
}

void DataInterface_SpawnSystem_getSpawnPoolList(void (*cb)(void *param, diJob_spawnpool_t *jobData), void *param)
{	
	diJob_spawnpool_t *job = (diJob_spawnpool_t*)DataInterface_allocJob(sizeof(diJob_spawnpool_t));
	DataInterface_queueJob(job, cb_DataInterface_SpawnSystem_getSpawnPoolList, cb, param);
}



void cb_DataInterface_SpawnSystem_addSpawnPoint(MYSQL *dbCon, diData_spawnEntry_t *job, void *cb, void *param)
{
	char queryText[4096];
	char subText[128];
	sprintf(queryText, "INSERT INTO spawnpool "
		"(id,mode,animType,respawnTime,"
		"posx,posy,posz,contextid,"
		// creature slots
		"creatureType1,creatureMinCount1,creatureMaxCount1,"
		"creatureType2,creatureMinCount2,creatureMaxCount2,"
		"creatureType3,creatureMinCount3,creatureMaxCount3,"
		"creatureType4,creatureMinCount4,creatureMaxCount4,"
		"creatureType5,creatureMinCount5,creatureMaxCount5,"
		"creatureType6,creatureMinCount6,creatureMaxCount6)"
		" VALUES (%d,%d,%d,%d,%f,%f,%f,%d", _nextSpawnPoolId, job->mode, job->animType, job->respawnTime, job->posX, job->posY, job->posZ, job->contextId);
	_nextSpawnPoolId++;
	// add creature spawn types and amount
	for(sint32 i=0; i<6; i++)
	{
		if( job->spawnSlot[i].countMax == 0 )
		{
			strcat(queryText, ",NULL,NULL,NULL");
		}
		else
		{
			sprintf(subText, ",%d,%d,%d", job->spawnSlot[i].creatureType, job->spawnSlot[i].countMin, job->spawnSlot[i].countMax);
			strcat(queryText, subText);
		}
	}
	strcat(queryText, ");");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	DataInterface_freeJob(job);
}

/*
 * Adds a new spawn entry to the database
 * Returns the id of the spawnpool
 */
sint32 DataInterface_SpawnSystem_addSpawnPoint(diData_spawnEntry_t* spawnEntry)
{	
	sint32 usedSpawnpoolId = _nextSpawnPoolId; // todo: In rare cases the spawnpool id can be mixed up if there are several pools added at once by multiple people
	diData_spawnEntry_t *job = (diData_spawnEntry_t*)DataInterface_allocJob(sizeof(diData_spawnEntry_t));
	RtlCopyMemory(job, spawnEntry, sizeof(diData_spawnEntry_t));
	DataInterface_queueJob(job, cb_DataInterface_SpawnSystem_addSpawnPoint, NULL, NULL);
	return usedSpawnpoolId;
}
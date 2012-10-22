#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"


void cb_DataInterface_Creature_getCreatureActionList(MYSQL *dbCon, diJob_creatureAction_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"id,description,actionId,actionArgId,"
		"rangeMin,rangeMax,"
		"recoverTime,recoverTimeGlobal,windupTime,minDamage,maxDamage"
		" FROM creature_action");
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
		//"id,description,actionId,actionArgId,"
		//"recoverTime,recoverTimeGlobal,windupTime,"
		sint32 idx = 0;
		sscanf(dbRow[idx], "%d", &job->id); idx++;
		strcpy(job->description, dbRow[idx]); idx++;
		sscanf(dbRow[idx], "%d", &job->actionId); idx++;
		sscanf(dbRow[idx], "%d", &job->actionArgId); idx++;
		sscanf(dbRow[idx], "%f", &job->rangeMin); idx++;
		sscanf(dbRow[idx], "%f", &job->rangeMax); idx++;
		sscanf(dbRow[idx], "%d", &job->recoverTime); idx++;
		sscanf(dbRow[idx], "%d", &job->recoverTimeGlobal); idx++;
		sscanf(dbRow[idx], "%d", &job->windupTime); idx++;
		sscanf(dbRow[idx], "%d", &job->minDamage); idx++;
		sscanf(dbRow[idx], "%d", &job->maxDamage); idx++;
		// do callback param1: mapchannel param2: list of missile type data
		((void (*)(void*,void*))cb)(param, job);
	}
	mysql_free_result(dbResult);
	DataInterface_freeJob(job);
}


void DataInterface_Creature_getCreatureActionList(void (*cb)(void *param, diJob_creatureAction_t *jobData), void *param)
{	
	diJob_creatureAction_t *job = (diJob_creatureAction_t*)DataInterface_allocJob(sizeof(diJob_creatureAction_t));
	DataInterface_queueJob(job, cb_DataInterface_Creature_getCreatureActionList, cb, param);
}


void cb_DataInterface_Creature_getCreatureTypeList(MYSQL *dbCon, diJob_creatureType_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"id,name,nameId,classId,faction,"
		"walkspeed,runspeed,hitpoints,"
		"action1,action2,action3,action4,action5,action6,action7,action8,"
		"wanderDistance"
		" FROM creature_type");
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
		//"id,name,classId,faction,"
		//"walkspeed,runspeed,hitpoints,"
		//"melee_damageMin,melee_damageMax,melee_attackSpeed,melee_attackRange,"
		//"range_damageMin,range_damageMax,range_missile,range_attackSpeed,range_attackRange"
		sint32 idx = 0;
		sscanf(dbRow[idx], "%d", &job->id); idx++;
		strcpy(job->name, dbRow[idx]); idx++;
		sscanf(dbRow[idx], "%d", &job->nameId); idx++;
		sscanf(dbRow[idx], "%d", &job->classId); idx++;
		sscanf(dbRow[idx], "%d", &job->faction); idx++;
		sscanf(dbRow[idx], "%f", &job->walkspeed); idx++;
		sscanf(dbRow[idx], "%f", &job->runspeed); idx++;
		sscanf(dbRow[idx], "%d", &job->hitpoints); idx++;
		for(sint32 i=0; i<8; i++)
		{
			if( dbRow[idx] == NULL )
			{
				job->missile[i] = 0;
				idx++;
			}
			else
			{
				sscanf(dbRow[idx], "%d", &job->missile[i]); idx++;	
			}
		}
		sscanf(dbRow[idx], "%f", &job->wanderDistance); idx++;
		// do callback param1: mapchannel param2: list of creature type data
		((void (*)(void*,void*))cb)(param, job);
	}
	mysql_free_result(dbResult);
	DataInterface_freeJob(job);
}

void DataInterface_Creature_getCreatureTypeList(void (*cb)(void *param, diJob_creatureType_t *jobData), void *param)
{	
	diJob_creatureType_t *job = (diJob_creatureType_t*)DataInterface_allocJob(sizeof(diJob_creatureType_t));
	DataInterface_queueJob(job, cb_DataInterface_Creature_getCreatureTypeList, cb, param);
}
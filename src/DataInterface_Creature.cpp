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
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
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

void cb_DataInterface_Creature_getCreatureTypeList_LootTable(MYSQL *dbCon, diJob_creatureType_t *job)
{
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"itemTemplateId,chance,stacksizeMin,stacksizeMax"
		" FROM creature_type_loot WHERE creatureTypeId = %d", job->id);
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// parse rows
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
	// allocate table
	if( rowCount > 0 )
	{
		job->numLootEntries = rowCount;
		job->lootTable = (diData_lootTable_t*)malloc(sizeof(diData_lootTable_t) * rowCount);
		memset(job->lootTable, 0x00, sizeof(diData_lootTable_t) * rowCount);
	}
	else
	{
		job->numLootEntries = 0;
		job->lootTable = NULL;
	}
	sint32 lootIndex = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		//"itemTemplateId,chance,stacksizeMin,stacksizeMax"
		sint32 loot_itemTemplateId = 0;
		float loot_chance = 0;
		sint32 loot_stacksizeMin = 0;
		sint32 loot_stacksizeMax = 0;
		sscanf(dbRow[0], "%d", &loot_itemTemplateId);
		sscanf(dbRow[1], "%f", &loot_chance);
		sscanf(dbRow[2], "%d", &loot_stacksizeMin);
		sscanf(dbRow[3], "%d", &loot_stacksizeMax);
		job->lootTable[lootIndex].itemTemplateId = loot_itemTemplateId;
		job->lootTable[lootIndex].chance = loot_chance;
		job->lootTable[lootIndex].stacksizeMin = loot_stacksizeMin;
		job->lootTable[lootIndex].stacksizeMax = max(loot_stacksizeMin, loot_stacksizeMax);
		// next
		lootIndex++;
	}
	mysql_free_result(dbResult);
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
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
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
		// load the loot table for this creature
		cb_DataInterface_Creature_getCreatureTypeList_LootTable(dbCon, job);
		// do callback param1: mapchannel param2: list of creature type data
		((void (*)(void*,void*))cb)(param, job);
		// free temp data
		if( job->lootTable )
			free(job->lootTable);
	}
	mysql_free_result(dbResult);
	DataInterface_freeJob(job);
}

/*
 * Queries the database creature_type table
 */
void DataInterface_Creature_getCreatureTypeList(void (*cb)(void *param, diJob_creatureType_t *jobData), void *param)
{	
	diJob_creatureType_t *job = (diJob_creatureType_t*)DataInterface_allocJob(sizeof(diJob_creatureType_t));
	DataInterface_queueJob(job, cb_DataInterface_Creature_getCreatureTypeList, cb, param);
}



void cb_DataInterface_Creature_savePath(MYSQL *dbCon, diJob_path_t *job, void *cb, void *param)
{
	diData_path_t* pathData = job->pathData;
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"max(pathId)"
		" FROM creature_ai_path");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// parse rows
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
	if( rowCount == NULL )
		__debugbreak();
	dbRow = mysql_fetch_row(dbResult);
	sint32 newPathId = 0;
	if( dbRow[0] )
		newPathId = atoi(dbRow[0]);
	mysql_free_result(dbResult);
	newPathId++;
	pathData->pathId = newPathId;
	// now that we have a valid pathId -> insert path
	sprintf(queryText, "INSERT INTO creature_ai_path (`pathId`,`contextId`,`mode`,`spawnpool`) VALUES('%d','%d','%d','%d')", pathData->pathId, pathData->contextId, pathData->mode, pathData->spawnpool);
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	// add all the path nodes
	for(sint32 i=0; i<pathData->numberOfNodes; i++)
	{
		sprintf(queryText, "INSERT INTO creature_ai_pathnodes (`pathId`,`nodeIndex`,`x`,`y`,`z`) VALUES('%d','%d','%f','%f','%f')", pathData->pathId, i+1, pathData->pathnodes[i].pos[0], pathData->pathnodes[i].pos[1], pathData->pathnodes[i].pos[2]);
		// execute query
		if( mysql_query(dbCon, queryText) )
		{
			printf("Error in query\n");
			while(1) Sleep(1000);	
		}
	}
	// free job and leave
	pathData->_isFinished = true;
	DataInterface_freeJob(job);
}

/*
 * SYNCHRONOUSLY adding a path to the db (table creature_ai_path and creature_ai_pathnodes)
 * This method is dangerous, 
 */
void DataInterface_Creature_savePath(diData_path_t* pathData)
{	
	diJob_path_t *job = (diJob_path_t*)DataInterface_allocJob(sizeof(diJob_path_t));
	job->pathData = pathData;
	pathData->_isFinished = false;
	DataInterface_queueJob(job, cb_DataInterface_Creature_savePath, NULL, NULL);
	while( pathData->_isFinished == false ) Sleep(1);
}


/*
 * Queries data from creature_ai_pathnodes table for a specific path
 */
void _cb_DataInterface_Creature_getPathNodes(MYSQL *dbCon, diData_path_t* pathData)
{
	sint8 queryText[1024];
	wsprintf(queryText, "SELECT "
		"x,y,z"
		" FROM creature_ai_pathnodes WHERE pathId = %d ORDER BY nodeIndex ASC", pathData->pathId);
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate path nodes
	sint32 pathNodeCount = (sint32)mysql_num_rows(dbResult);
	diData_pathNodes_t *pathNodeList = (diData_pathNodes_t*)malloc(sizeof(diData_pathNodes_t) * pathNodeCount);
	// parse mysql data
	diData_pathNodes_t* pathNodeData = pathNodeList;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		float pathnode_x = 0.0f;
		float pathnode_y = 0.0f;
		float pathnode_z = 0.0f;
		sscanf(dbRow[0], "%f", &pathnode_x);
		sscanf(dbRow[1], "%f", &pathnode_y);
		sscanf(dbRow[2], "%f", &pathnode_z);;
		pathNodeData->pos[0] = pathnode_x;
		pathNodeData->pos[1] = pathnode_y;
		pathNodeData->pos[2] = pathnode_z;
		// next
		pathNodeData++;
	}
	mysql_free_result(dbResult);
	pathData->pathnodes = pathNodeList;
	pathData->numberOfNodes = pathNodeCount;
}

/*
 * Queries data from creature_ai_path table for all paths
 * Also, queries each paths's node list
 */
void cb_DataInterface_Vendor_getVendorList(MYSQL *dbCon, diJob_path_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	wsprintf(queryText, "SELECT "
		"pathId,contextId,mode,spawnpool,nodeOffsetRandomization"
		" FROM creature_ai_path");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate path data
	sint32 pathCount = (sint32)mysql_num_rows(dbResult);
	diData_path_t *pathDataList = (diData_path_t*)malloc(sizeof(diData_path_t) * pathCount);
	// parse mysql data
	diData_path_t* path = pathDataList;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		// "pathId,contextId,mode"
		sint32 path_pathId = 0;
		sint32 path_contextId = 0;
		sint32 path_mode = 0;
		sint32 path_spawnpool = 0;
		float path_nodeOffsetRandomization = 0.0f;
		sscanf(dbRow[0], "%d", &path_pathId);
		sscanf(dbRow[1], "%d", &path_contextId);
		sscanf(dbRow[2], "%d", &path_mode);
		if( dbRow[3] )
			sscanf(dbRow[3], "%d", &path_spawnpool);
		else
			path_spawnpool = 0;
		if( dbRow[4] )
			sscanf(dbRow[4], "%f", &path_nodeOffsetRandomization);
		else
			path_nodeOffsetRandomization = 0.0f;
		path->pathId = path_pathId;
		path->contextId = path_contextId;
		path->mode = path_mode;
		path->spawnpool = path_spawnpool;
		path->nodeOffsetRandomization = path_nodeOffsetRandomization;
		// for each vendor, query item data
		_cb_DataInterface_Creature_getPathNodes(dbCon, path);
		// next
		path++;
	}
	mysql_free_result(dbResult);
	job->pathList = pathDataList;
	job->pathCount = pathCount;
	// do callback param1: mapchannel param2: list of job path data
	((void (*)(void*,void*))cb)(param, job);
	// free all path node lists
	for(sint32 i=0; i<pathCount; i++)
	{
		if( pathDataList[i].pathnodes )
			free(pathDataList[i].pathnodes);
	}
	// free path list
	free(pathDataList);
	// free job
	DataInterface_freeJob(job);
}

/*
 * Queries all ai paths and their associates nodes from the db
 */
void DataInterface_Creature_getPathList(void (*cb)(void *param, diJob_path_t *jobData), void *param)
{	
	diJob_path_t *job = (diJob_path_t*)DataInterface_allocJob(sizeof(diJob_path_t));
	DataInterface_queueJob(job, cb_DataInterface_Vendor_getVendorList, cb, param);
}
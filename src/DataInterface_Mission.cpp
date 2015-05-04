#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"

void cb_DataInterface_Mission_getMissionList(MYSQL *dbCon, diJob_missionListData_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	job->outMissionCount = 0;
	job->outMissionList = NULL;

	wsprintf(queryText, "SELECT "
		"missionId,dispenserNPC,collectorNPC"
		" FROM mission");

	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate mission data
	sint32 missionCount = (sint32)mysql_num_rows(dbResult);
	di_missionData_t *missionDataList = (di_missionData_t*)malloc(sizeof(di_missionData_t) * missionCount);

	sint32 idx = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		di_missionData_t *missionData = missionDataList+idx;
		idx++;

		uint32 mission_id;
		unsigned long long mission_dispenserNPC;
		unsigned long long mission_collectorNPC;

		sint32 idx = 0;
		sscanf(dbRow[idx], "%u", &mission_id); idx++;
		sscanf(dbRow[idx], "%I64u", &mission_dispenserNPC); idx++;
		sscanf(dbRow[idx], "%I64u", &mission_collectorNPC); idx++;
		missionData->missionId = mission_id;
		missionData->collectorNPC = mission_collectorNPC;
		missionData->dispenserNPC = mission_dispenserNPC;
	}
	mysql_free_result(dbResult);
	job->outMissionList = missionDataList;
	job->outMissionCount = missionCount;
	// do callback
	((void (*)(void*,void*))cb)(param, job);
	// free data
	free(missionDataList);
	DataInterface_freeJob(job);
}

void DataInterface_Mission_getMissionList(void (*cb)(void *param, diJob_missionListData_t *jobData), void *param)
{	
	diJob_missionListData_t *job = (diJob_missionListData_t*)DataInterface_allocJob(sizeof(diJob_missionListData_t));
	DataInterface_queueJob(job, cb_DataInterface_Mission_getMissionList, cb, param);
}

void cb_DataInterface_Mission_getMissionScriptData(MYSQL *dbCon, di_missionScript_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	job->scriptLineCount = 0;

	wsprintf(queryText, "SELECT "
		"id,missionId,command,state,flags,value1,value2,value3"
		" FROM mission_script");

	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate mission data
	sint32 missionScriptLineCount = (sint32)mysql_num_rows(dbResult);
	di_missionScriptLine_t *missionDataList = (di_missionScriptLine_t*)malloc(sizeof(di_missionScriptLine_t) * missionScriptLineCount);
	sint32 idx = 0;
	job->scriptLines = missionDataList;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		di_missionScriptLine_t *missionScriptLine = missionDataList+idx;
		idx++;
		missionScriptLine->id = atoi(dbRow[0]);
		missionScriptLine->missionId = atoi(dbRow[1]);
		missionScriptLine->command = atoi(dbRow[2]);
		missionScriptLine->state = atoi(dbRow[3]);
		missionScriptLine->flags = atoi(dbRow[4]);
		missionScriptLine->value1 = atoi(dbRow[5]);
		missionScriptLine->value2 = atoi(dbRow[6]);
		missionScriptLine->value3 = atoi(dbRow[7]);
	}
	mysql_free_result(dbResult);
	job->scriptLineCount = missionScriptLineCount;
	// do callback
	((void (*)(void*,void*))cb)(param, job);
	// free data
	free(missionDataList);
	DataInterface_freeJob(job);
}

void DataInterface_Mission_getMissionScriptData(void (*cb)(void *param, di_missionScript_t *jobData), void *param)
{	
	di_missionScript_t *job = (di_missionScript_t*)DataInterface_allocJob(sizeof(di_missionScript_t));
	DataInterface_queueJob(job, cb_DataInterface_Mission_getMissionScriptData, cb, param);
}
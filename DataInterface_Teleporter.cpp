#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"


void cb_dataInterface_teleporter_getList(MYSQL *dbCon, diJob_teleporterData *job, void *cb, void *param)
{
  
	char queryText[1024];
	job->scount = 0;
	job->tdata = NULL;
	
	//__debugbreak();
	wsprintf(queryText, "SELECT id,type,px,py,pz,modelid FROM teleporter");
	
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate spawn data
	int teleporterCount = mysql_num_rows(dbResult);
	di_teleporterData *teleporterList = (di_teleporterData*)malloc(sizeof(di_teleporterData) * teleporterCount);
	int x = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
           //float temp_dx,temp_dy,temp_dx,temp_sx,temp_sy,temp_sz,temp_bx,temp_bz;
		   //unsigned int temp_id,temp_type;
		   //char temp_label[50];
		   di_teleporterData *teleporter = teleporterList+x;
		   x++;
		   sscanf(dbRow[0], "%d", &teleporter->id);
		   sscanf(dbRow[1], "%d", &teleporter->type);
		   //strcpy(teleporter->label, dbRow[2]); 
		   sscanf(dbRow[2], "%f", &teleporter->sx);
		   sscanf(dbRow[3], "%f", &teleporter->sy);
		   sscanf(dbRow[4], "%f", &teleporter->sz);
		   sscanf(dbRow[5], "%d", &teleporter->modelid);
	}//---while
	mysql_free_result(dbResult);
	job->tdata = teleporterList;
	job->scount = teleporterCount;

	// do callback param1: mapchannel param2: list of npcdata
	((void (*)(void*,void*))cb)(param, job);

	if(teleporterList != NULL)free(teleporterList);
	dataInterface_freeJob(job);
}

void dataInterface_teleporter_getList(unsigned int mapContextId, void (*cb)(void *param,  diJob_teleporterData *jobData), void *param)
{

  diJob_teleporterData *job = (diJob_teleporterData*)dataInterface_allocJob(sizeof(diJob_teleporterData));
  job->mapContextId = mapContextId;
  dataInterface_queueJob(job, cb_dataInterface_teleporter_getList, cb, param);

}

void cb_dataInterface_teleporter_updateList(MYSQL *dbCon, diJob_teleporterData *job, void *cb, void *param)
{
	char queryText[4 * 1024];
	di_teleporterData *worldObject = job->tdata;

	printf("DB-GameObject-Insert: type: %i classid: %i\n",worldObject->type,worldObject->modelid);

	sprintf(queryText, "INSERT INTO teleporter ("
			"`type`,`px`,`py`,`pz`,`modelid`"
			") VALUES (%d,%f,%f,%f,%d)", 
			worldObject->type, worldObject->sx, worldObject->sy,
			worldObject->sz,worldObject->modelid);
	if( mysql_query(dbCon, queryText) )
	{
			//characterData->error = true;
			printf("Error in query\n");
			puts(queryText);
			puts(mysql_error(dbCon));
			return;
	}
	
	dataInterface_freeJob(job);
}

void dataInterface_teleporter_updateList( di_teleporterData *objectData, void (*cb)(void *param, diJob_teleporterData *jobData), void *param)
{	
		
	diJob_teleporterData *job = (diJob_teleporterData*)dataInterface_allocJob(sizeof(diJob_teleporterData));
	job->tdata = objectData;
	dataInterface_queueJob(job, cb_dataInterface_teleporter_updateList, cb, param);
}
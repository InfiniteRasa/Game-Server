#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"

uint32 DataInterfaceSpawnpoolId;

void cb_DataInterface_NPC_getNPCList(MYSQL *dbCon, diJob_npcListData_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	job->outNpcCount = 0;
	job->outNpcList = NULL;
	
	wsprintf(queryText, "SELECT "
	"id,name,entityClassId,"
	"mapContextId,posX,posY,posZ,rotation,"
	"ad1_classId,ad1_hue,"
	"ad2_classId,ad2_hue,"
	"ad3_classId,ad3_hue,"
	"ad4_classId,ad4_hue,"
	"ad5_classId,ad5_hue,"
	"ad6_classId,ad6_hue,"
	"ad7_classId,ad7_hue,"
	"ad8_classId,ad8_hue,"
	"ad9_classId,ad9_hue,"
	"ad10_classId,ad10_hue,"
	"ad11_classId,ad11_hue,"
	"ad12_classId,ad12_hue,"
	"ad13_classId,ad13_hue,"
	"ad14_classId,ad14_hue,"
	"ad15_classId,ad15_hue,"
	"ad16_classId,ad16_hue,"
	"ad17_classId,ad17_hue,"
	"ad18_classId,ad18_hue,"
	"ad19_classId,ad19_hue,"
	"ad20_classId,ad20_hue,"
	"ad21_classId,ad21_hue"
	" FROM npc WHERE mapContextId=%d", job->mapContextId);
	//else
	//{
	//	wsprintf(queryText, "SELECT "
	//		"id,name,slotId,gender,raceId,classId,"
	//		"currentContextId,posX,posY,posZ,"
	//		"ad1_classId,ad1_hue,"
	//		"ad2_classId,ad2_hue,"
	//		"ad3_classId,ad3_hue,"
	//		"ad4_classId,ad4_hue,"
	//		"ad5_classId,ad5_hue,"
	//		"ad6_classId,ad6_hue,"
	//		"ad7_classId,ad7_hue,"
	//		"ad8_classId,ad8_hue,"
	//		"ad9_classId,ad9_hue,"
	//		"ad10_classId,ad10_hue,"
	//		"ad11_classId,ad11_hue,"
	//		"ad12_classId,ad12_hue,"
	//		"ad13_classId,ad13_hue,"
	//		"ad14_classId,ad14_hue,"
	//		"ad15_classId,ad15_hue,"
	//		"ad16_classId,ad16_hue,"
	//		"ad17_classId,ad17_hue,"
	//		"ad18_classId,ad18_hue,"
	//		"ad19_classId,ad19_hue,"
	//		"ad20_classId,ad20_hue,"
	//		"ad21_classId,ad21_hue"
	//		" FROM characters WHERE userId=%I64u AND slotId=%d", job->userId, job->slotIndex);
	//}

	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate npc data
	sint32 npcCount = mysql_num_rows(dbResult);
	di_npcData_t *npcDataList = (di_npcData_t*)malloc(sizeof(di_npcData_t) * npcCount);
	//allocate the spawntypes/pools the same way
	sint32 idx = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		di_npcData_t *npcData = npcDataList+idx;
		idx++;

		unsigned long long npc_id;
		sint32 npc_entityClassId;
		sint32 npc_currentContextId;
		double npc_posX;
		double npc_posY;
		double npc_posZ;
		double npc_rotation;
		
		sint32 idx = 0;
		sscanf(dbRow[idx], "%I64u", &npcData->npcID); idx++;
		strcpy(npcData->unicodeName, dbRow[idx]); idx++;
		sscanf(dbRow[idx], "%d", &npc_entityClassId); idx++;
		npcData->entityClassID = npc_entityClassId;
		sscanf(dbRow[idx], "%d", &npc_currentContextId); idx++;
		npcData->currentContextId = npc_currentContextId;
		sscanf(dbRow[idx], "%lf", &npc_posX); idx++;
		sscanf(dbRow[idx], "%lf", &npc_posY); idx++;
		sscanf(dbRow[idx], "%lf", &npc_posZ); idx++;
		sscanf(dbRow[idx], "%lf", &npc_rotation); idx++;
		npcData->posX = npc_posX;
		npcData->posY = npc_posY;
		npcData->posZ = npc_posZ;
		//npcData->rot = npc_rotation;

		for(sint32 i=0; i<21; i++)
		{
			sscanf(dbRow[idx+0], "%d", &npcData->appearanceData[i].classId);
			sscanf(dbRow[idx+1], "%u", &npcData->appearanceData[i].hue);
			idx += 2;
		}
	}
	mysql_free_result(dbResult);
	job->outNpcList = npcDataList;
	job->outNpcCount = npcCount;
	// do callback param1: mapchannel param2: list of npcdata
	((void (*)(void*,void*))cb)(param, job);
	//// free data
	//for(sint32 i=0; i<16; i++)
	//{
	//	free(job->outPreviewData[i]);
	//}
	free(npcDataList);
	DataInterface_freeJob(job);
}

void DataInterface_NPC_getNPCList(uint32 mapContextId, void (*cb)(void *param, diJob_npcListData_t *jobData), void *param)
{	
	diJob_npcListData_t *job = (diJob_npcListData_t*)DataInterface_allocJob(sizeof(diJob_npcListData_t));
	job->mapContextId = mapContextId;
	DataInterface_queueJob(job, cb_DataInterface_NPC_getNPCList, cb, param);
}

void cb_DataInterface_NPC_updateNPC(MYSQL *dbCon, diJob_updateNPC_t *job, void *cb, void *param)
{
	sint8 queryText[1024*4];
	di_npcData_t *npcData = job->npcData;
	// check if npc exists first (maybe should lock table)
	bool npcAlreadyExists = false;
	sprintf(queryText, "SELECT id FROM npc WHERE id=%I64d", job->npcData->npcID);
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void (*)(void*,void*))cb)(param, job);
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	if( mysql_num_rows(dbResult) )
		npcAlreadyExists = true;
	mysql_free_result(dbResult);
	if( npcAlreadyExists == false )
	{
		// create npc
		sprintf(queryText, "INSERT INTO npc ("
			"`id`,`name`,`entityClassId`,"
			"`mapContextId`,`posX`,`posY`,`posZ`,"
			"`ad1_classId`,`ad1_hue`,"
			"`ad2_classId`,`ad2_hue`,"
			"`ad3_classId`,`ad3_hue`,"
			"`ad4_classId`,`ad4_hue`,"
			"`ad5_classId`,`ad5_hue`,"
			"`ad6_classId`,`ad6_hue`,"
			"`ad7_classId`,`ad7_hue`,"
			"`ad8_classId`,`ad8_hue`,"
			"`ad9_classId`,`ad9_hue`,"
			"`ad10_classId`,`ad10_hue`,"
			"`ad11_classId`,`ad11_hue`,"
			"`ad12_classId`,`ad12_hue`,"
			"`ad13_classId`,`ad13_hue`,"
			"`ad14_classId`,`ad14_hue`,"
			"`ad15_classId`,`ad15_hue`,"
			"`ad16_classId`,`ad16_hue`,"
			"`ad17_classId`,`ad17_hue`,"
			"`ad18_classId`,`ad18_hue`,"
			"`ad19_classId`,`ad19_hue`,"
			"`ad20_classId`,`ad20_hue`,"
			"`ad21_classId`,`ad21_hue`"
			") VALUES"
			"(%I64u,'%s',%d,"
			"%d,%f,%f,%f,"
			"%d,%u,%d,%u,%d,%u,%d,%u," // 1
			"%d,%u,%d,%u,%d,%u,%d,%u," // 5
			"%d,%u,%d,%u,%d,%u,%d,%u," // 9
			"%d,%u,%d,%u,%d,%u,%d,%u," // 13
			"%d,%u,%d,%u,%d,%u,%d,%u," // 17
			"%d,%u"					   // 21
			")", 
			npcData->npcID, npcData->unicodeName, npcData->entityClassID,
			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue
			);
		// execute query
		if( mysql_query(dbCon, queryText) )
		{
			//characterData->error = true;
			printf("Error in query\n");
			puts(queryText);
			puts(mysql_error(dbCon));
			if( cb )
				((void (*)(void*,void*))cb)(param, job);
			return;
		}
		// do callback
		if( cb )
			((void (*)(void*,void*))cb)(param, job);
	}
	else
	{
		// update npc
		sprintf(queryText, "UPDATE npc SET "
			"`name`='%s',`entityClassId`=%d,"
			"`mapContextId`=%d,`posX`=%lf,`posY`=%lf,`posZ`=%lf,"
			"`ad1_classId`=%u,`ad1_hue`=%u,"
			"`ad2_classId`=%u,`ad2_hue`=%u,"
			"`ad3_classId`=%u,`ad3_hue`=%u,"
			"`ad4_classId`=%u,`ad4_hue`=%u,"
			"`ad5_classId`=%u,`ad5_hue`=%u,"
			"`ad6_classId`=%u,`ad6_hue`=%u,"
			"`ad7_classId`=%u,`ad7_hue`=%u,"
			"`ad8_classId`=%u,`ad8_hue`=%u,"
			"`ad9_classId`=%u,`ad9_hue`=%u,"
			"`ad10_classId`=%u,`ad10_hue`=%u,"
			"`ad11_classId`=%u,`ad11_hue`=%u,"
			"`ad12_classId`=%u,`ad12_hue`=%u,"
			"`ad13_classId`=%u,`ad13_hue`=%u,"
			"`ad14_classId`=%u,`ad14_hue`=%u,"
			"`ad15_classId`=%u,`ad15_hue`=%u,"
			"`ad16_classId`=%u,`ad16_hue`=%u,"
			"`ad17_classId`=%u,`ad17_hue`=%u,"
			"`ad18_classId`=%u,`ad18_hue`=%u,"
			"`ad19_classId`=%u,`ad19_hue`=%u,"
			"`ad20_classId`=%u,`ad20_hue`=%u,"
			"`ad21_classId`=%u,`ad21_hue`=%u"
			" WHERE id=%I64u LIMIT 1", 
			npcData->unicodeName, npcData->entityClassID,
			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue,
			npcData->npcID
			);
		// execute query
		if( mysql_query(dbCon, queryText) )
		{
			//characterData->error = true;
			printf("Error in query\n");
			puts(queryText);
			puts(mysql_error(dbCon));
			if( cb )
				((void (*)(void*,void*))cb)(param, job);
			return;
		}
		// do callback
		if( cb )
			((void (*)(void*,void*))cb)(param, job);
	}
	DataInterface_freeJob(job);
}

void DataInterface_NPC_updateNPC(di_npcData_t *npcData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param)
{	
	diJob_updateNPC_t *job = (diJob_updateNPC_t*)DataInterface_allocJob(sizeof(diJob_updateNPC_t));
	job->npcData = npcData;
	DataInterface_queueJob(job, cb_DataInterface_NPC_updateNPC, cb, param);
}

//############################# TEST #########################################
void cb_DataInterface_Entity_updateEntityW(MYSQL *dbCon, diJob_updateEntityW_t *job, void *cb, void *param)
{
	sint8 queryText[4 * 1024];
	di_entityDataW_t *npcData = job->entityData;
	// check if npc exists first (maybe should lock table)
	bool npcAlreadyExists = false;
	sprintf(queryText, "SELECT id FROM npc WHERE id=%I64d", job->entityData->npcID);
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void (*)(void*,void*))cb)(param, job);
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	if( mysql_num_rows(dbResult) )
		npcAlreadyExists = true;
	mysql_free_result(dbResult);
	if( npcAlreadyExists == false )
	{
		// create npc
		sprintf(queryText, "INSERT INTO npc ("
			"`id`,`name`,`entityClassId`,"
			"`mapContextId`,`posX`,`posY`,`posZ`,`rotation`,"
			"`ad1_classId`,`ad1_hue`,"
			"`ad2_classId`,`ad2_hue`,"
			"`ad3_classId`,`ad3_hue`,"
			"`ad4_classId`,`ad4_hue`,"
			"`ad5_classId`,`ad5_hue`,"
			"`ad6_classId`,`ad6_hue`,"
			"`ad7_classId`,`ad7_hue`,"
			"`ad8_classId`,`ad8_hue`,"
			"`ad9_classId`,`ad9_hue`,"
			"`ad10_classId`,`ad10_hue`,"
			"`ad11_classId`,`ad11_hue`,"
			"`ad12_classId`,`ad12_hue`,"
			"`ad13_classId`,`ad13_hue`,"
			"`ad14_classId`,`ad14_hue`,"
			"`ad15_classId`,`ad15_hue`,"
			"`ad16_classId`,`ad16_hue`,"
			"`ad17_classId`,`ad17_hue`,"
			"`ad18_classId`,`ad18_hue`,"
			"`ad19_classId`,`ad19_hue`,"
			"`ad20_classId`,`ad20_hue`,"
			"`ad21_classId`,`ad21_hue`"
			") VALUES"
			"(%I64u,'%s',%d,"
			"%d,%f,%f,%f,%f,"
			"%d,%u,%d,%u,%d,%u,%d,%u," // 1
			"%d,%u,%d,%u,%d,%u,%d,%u," // 5
			"%d,%u,%d,%u,%d,%u,%d,%u," // 9
			"%d,%u,%d,%u,%d,%u,%d,%u," // 13
			"%d,%u,%d,%u,%d,%u,%d,%u," // 17
			"%d,%u"					   // 21
			")", 
			npcData->npcID, npcData->unicodeName, npcData->entityClassID,
			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
			(double)npcData->rotation,
			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue
			);
		// execute query
		if( mysql_query(dbCon, queryText) )
		{
			//characterData->error = true;
			printf("Error in query\n");
			puts(queryText);
			puts(mysql_error(dbCon));
			if( cb )
				((void (*)(void*,void*))cb)(param, job);
			return;
		}
		// do callback
		if( cb )
			((void (*)(void*,void*))cb)(param, job);
	}
	else
	{
		// update npc
		sprintf(queryText, "UPDATE npc SET "
			"`name`='%s',`entityClassId`=%d,"
			"`mapContextId`=%d,`posX`=%lf,`posY`=%lf,`posZ`=%lf,`rotation=%f`,"
			"`ad1_classId`=%u,`ad1_hue`=%u,"
			"`ad2_classId`=%u,`ad2_hue`=%u,"
			"`ad3_classId`=%u,`ad3_hue`=%u,"
			"`ad4_classId`=%u,`ad4_hue`=%u,"
			"`ad5_classId`=%u,`ad5_hue`=%u,"
			"`ad6_classId`=%u,`ad6_hue`=%u,"
			"`ad7_classId`=%u,`ad7_hue`=%u,"
			"`ad8_classId`=%u,`ad8_hue`=%u,"
			"`ad9_classId`=%u,`ad9_hue`=%u,"
			"`ad10_classId`=%u,`ad10_hue`=%u,"
			"`ad11_classId`=%u,`ad11_hue`=%u,"
			"`ad12_classId`=%u,`ad12_hue`=%u,"
			"`ad13_classId`=%u,`ad13_hue`=%u,"
			"`ad14_classId`=%u,`ad14_hue`=%u,"
			"`ad15_classId`=%u,`ad15_hue`=%u,"
			"`ad16_classId`=%u,`ad16_hue`=%u,"
			"`ad17_classId`=%u,`ad17_hue`=%u,"
			"`ad18_classId`=%u,`ad18_hue`=%u,"
			"`ad19_classId`=%u,`ad19_hue`=%u,"
			"`ad20_classId`=%u,`ad20_hue`=%u,"
			"`ad21_classId`=%u,`ad21_hue`=%u"
			" WHERE id=%I64u LIMIT 1", 
			npcData->unicodeName, npcData->entityClassID,
			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
			(double)npcData->rotation,
			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue
			);
		// execute query
		if( mysql_query(dbCon, queryText) )
		{
			//characterData->error = true;
			printf("Error in query\n");
			puts(queryText);
			puts(mysql_error(dbCon));
			if( cb )
				((void (*)(void*,void*))cb)(param, job);
			return;
		}
		// do callback
		if( cb )
			((void (*)(void*,void*))cb)(param, job);
	}
	DataInterface_freeJob(job);
}


void DataInterface_Entity_updateEntityW(di_entityDataW_t *entityData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param)
{	
		
	diJob_updateEntityW_t *job = (diJob_updateEntityW_t*)DataInterface_allocJob(sizeof(diJob_updateEntityW_t));
	job->entityData = entityData;
	DataInterface_queueJob(job, cb_DataInterface_Entity_updateEntityW, cb, param);
}

//################################################## test # 2 ##################################################
void cb_DataInterface_Spawnpool_updateSpawnW2(MYSQL *dbCon, diJob_updateSpawnW2_t *job, void *cb, void *param)
{
	sint8 queryText[4 * 1024];
	di_spawnDataW2_t *npcData = job->spawnData;
	// check if spawnlocation exists first (maybe should lock table)
	float currentX = npcData->posX;
	float currentZ = npcData->posZ;
	//--position
	bool positionUsed = false;

	sprintf(queryText, "SELECT posx,posz FROM spawnpool WHERE contextid=%d", job->spawnData->currentContextId);
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void (*)(void*,void*))cb)(param, job);
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
    //check spawnpool for double used spawnlocations
	while((dbRow = mysql_fetch_row(dbResult)))
	{
        float tmpx,tmpz;  
		sscanf(dbRow[0], "%f", &tmpx);
		sscanf(dbRow[1], "%f", &tmpz);
        
          if( (currentX == tmpx)  &&  (currentZ == tmpz) )
          positionUsed = true;	
	}
	mysql_free_result(dbResult);


	if( positionUsed == false )
	{
		// create new spawnlocation
		sprintf(queryText, "INSERT INTO spawnpool ("
			"`type`,`posx`,`posy`,`posz`,`contextid`"
			") VALUES (%d,%f,%f,%f,%d)", 
			npcData->spawntype, npcData->posX, npcData->posY,
			npcData->posZ,npcData->currentContextId);
		// execute query
		if( mysql_query(dbCon, queryText) )
		{
			//characterData->error = true;
			printf("Error in query\n");
			puts(queryText);
			puts(mysql_error(dbCon));
			if( cb )
				((void (*)(void*,void*))cb)(param, job);
			return;
		}
		// do callback
		if( cb )
			((void (*)(void*,void*))cb)(param, job);
	}
	
	DataInterface_freeJob(job);
}


void DataInterface_Spawnpool_updateSpawnW2(di_spawnDataW2_t *spawnData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param)
{	
		
	diJob_updateSpawnW2_t *job = (diJob_updateSpawnW2_t*)DataInterface_allocJob(sizeof(diJob_updateSpawnW2_t));
	job->spawnData = spawnData;
	DataInterface_queueJob(job, cb_DataInterface_Spawnpool_updateSpawnW2, cb, param);
}

//################################################### test3 ##############################################
void cb_DataInterface_Spawn_getSpawnpool(MYSQL *dbCon, diJob_spawnTypeW2_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	job->scount = 0;
	job->spawnType = NULL;
	
	//__debugbreak();
	wsprintf(queryText, "SELECT id,maxcreatures,spawnname,creatures,faction,spawnanim,locktime,attackspeed,velocity,attackaction,attakstyle,actionid,melee_damage,range_damage,HitPoints FROM spawntype");
	
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate spawn data
	sint32 spawnCount = mysql_num_rows(dbResult);
	di_spawnDataW2_t *spawnLocationList = NULL;
	baseBehavior_baseNode *pNodesList = NULL;
	di_spawnTypeW2_t *spawnTypeList = (di_spawnTypeW2_t*)malloc(sizeof(di_spawnTypeW2_t) * spawnCount);
	
	//allocate the spawntypes/pools the same way
	sint32 idx = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		di_spawnTypeW2_t *npcData = spawnTypeList+idx;
		idx++;
		sint32 tspawntype,maxcreatures,tspawnanim,tsctiveswawns,tlocktime;
		sint32 tfaction,tattckspeed,tattckanim,tattackstyle,tactionid,rng_dmg,melee_dmg,hps;
		float tvelocity;
		sint8 tlabel[50];
		sint8 tvariant[70];

		
		sscanf(dbRow[0], "%d", &tspawntype);
		npcData->spawntype = tspawntype;
		sscanf(dbRow[1], "%d", &maxcreatures); 
		npcData->maxcreatures = maxcreatures;
		strcpy(npcData->label, dbRow[2]); 
		strcpy(npcData->creatures, dbRow[3]); 
		sscanf(dbRow[4], "%d", &tfaction); 
		npcData->faction = tfaction;
		sscanf(dbRow[5], "%d", &tspawnanim); 
		npcData->anim_type = tspawnanim;
		//sscanf(dbRow[6], "%d", &tsctiveswawns); 
		//npcData->activeSpawnCount = tsctiveswawns;
		sscanf(dbRow[6], "%d", &tlocktime); 
		npcData->locktime = tlocktime;
		sscanf(dbRow[7], "%d", &tattckspeed); 
		npcData->attackspeed = tattckspeed;
		sscanf(dbRow[8], "%f", &tvelocity); 
		npcData->velocity = tvelocity;
		sscanf(dbRow[9], "%d", &tattckanim); 
		npcData->attackaction = tattckanim;
		sscanf(dbRow[10], "%d", &tattackstyle); 
		npcData->attackstyle = tattackstyle;
		sscanf(dbRow[11], "%d", &tactionid); 
		npcData->actionid = tactionid;
		sscanf(dbRow[12], "%d", &melee_dmg); 
		npcData->dmg_melee = melee_dmg;
		sscanf(dbRow[13], "%d", &rng_dmg); 
		npcData->dmg_range = rng_dmg;
		sscanf(dbRow[14], "%d", &hps); 
		npcData->HitPoints = hps;
		
        //---get spawnlocations 
		wsprintf(queryText, "SELECT type,posx,posy,posz,contextid,id FROM spawnpool WHERE type=%d AND contextid=%d", 
			                tspawntype,job->mapContextId);
		// execute query
		if( mysql_query(dbCon, queryText) )
		{
			printf("Error in query\n");
			while(1) Sleep(1000);	
		}
		MYSQL_RES *dbResult2 = mysql_store_result(dbCon);
		MYSQL_ROW dbRow2;
		//--- allocate spawnlocation data
		sint32 spawnLocCount = mysql_num_rows(dbResult2);

		npcData->spawnlocCount = spawnLocCount;
		if(spawnLocCount<= 0)
		{
			mysql_free_result(dbResult2);
			continue;
		}
		spawnLocationList = (di_spawnDataW2_t*)malloc(sizeof(di_spawnDataW2_t) * spawnLocCount);
		sint32 cnt2 = 0;
		while((dbRow2 = mysql_fetch_row(dbResult2)))
		{
           di_spawnDataW2_t *t_spawnData = spawnLocationList + cnt2;
		   cnt2++;
		   float tposx,tposy,tposz;
		   sint32 tcontextid,ttype,tid;

		   
		   sscanf(dbRow2[0],"%d",&ttype);
		   t_spawnData->spawntype = ttype;
		   sscanf(dbRow2[1],"%f",&tposx);
		   t_spawnData->posX = tposx;
		   sscanf(dbRow2[2],"%f",&tposy);
		   t_spawnData->posY = tposy;
		   sscanf(dbRow2[3],"%f",&tposz);
		   t_spawnData->posZ = tposz;
		   sscanf(dbRow2[4],"%d",&tcontextid);
		   t_spawnData->currentContextId = tcontextid;

		   //--- get pathnodes for current spawnlocation/-point
		   sint32 tspawnid = 0;
		   sscanf(dbRow2[5],"%d",&tspawnid);
		   wsprintf(queryText, "SELECT spawnid,contextid,posx,posy,posz,pindex FROM pathnodes WHERE spawnid=%d", 
			   tspawnid);
		    //--- execute query
			if( mysql_query(dbCon, queryText) )
			{
				printf("Error in query\n");
				while(1) Sleep(1000);	
			}
		   MYSQL_RES *dbResult3 = mysql_store_result(dbCon);
		   MYSQL_ROW dbRow3;
		   sint32 pathNodeCount = mysql_num_rows(dbResult3);
		   pNodesList = (baseBehavior_baseNode*) malloc(sizeof(baseBehavior_baseNode) * pathNodeCount);
		   sint32 cnt3 = 0;
		   while( dbRow3 = mysql_fetch_row(dbResult3) )
		   {
              baseBehavior_baseNode *tNode =  pNodesList + cnt3;
			  cnt3++;
			  float tx,ty,tz;
			  sint32 tspawnid,tcontext,tpathindex;
			  sscanf(dbRow3[0],"%d",&tspawnid);
		      sscanf(dbRow3[1],"%d",&tcontextid);
		      tNode->contextid = tcontextid;
		      sscanf(dbRow3[2],"%f",&tx);
		      tNode->x = tx;
		      sscanf(dbRow3[3],"%f",&ty);
		      tNode->y = ty;
		      sscanf(dbRow3[4],"%f",&tz);
		      tNode->z = tz;
			  sscanf(dbRow3[5],"%d",&tpathindex);
			  tNode->pindex = tpathindex;
		   }//---while pathnodes
		   mysql_free_result(dbResult3);
		   if(pathNodeCount > 0)
		   {
               t_spawnData->pathnodes = (baseBehavior_baseNode*) malloc(sizeof(baseBehavior_baseNode) * pathNodeCount);
			   t_spawnData->pathnodes = pNodesList;
			   t_spawnData->nodeCount = pathNodeCount;
		   }

		}//---while(spawnlocations)
		mysql_free_result(dbResult2);
		if(spawnLocCount > 0)
		{
		npcData->locationlist = (di_spawnDataW2_t*)malloc(sizeof(di_spawnDataW2_t)* cnt2);
		npcData->locationlist = spawnLocationList;
		}

	}//---while(spanwtypes)
	mysql_free_result(dbResult);
	job->spawnType = spawnTypeList;
	job->scount = spawnCount;
	// do callback param1: mapchannel param2: list of npcdata
	((void (*)(void*,void*))cb)(param, job);
	
	if(spawnTypeList != NULL)free(spawnTypeList);
	if(spawnLocationList !=NULL)free(spawnLocationList);
	if(pNodesList != NULL) free (pNodesList);
	DataInterface_freeJob(job);
}
void DataInterface_Spawn_getSpawnpool(uint32 mapContextId, void (*cb)(void *param, diJob_spawnTypeW2_t *jobData), void *param)
{
 
  diJob_spawnTypeW2_t *job = (diJob_spawnTypeW2_t*)DataInterface_allocJob(sizeof(diJob_spawnTypeW2_t));
  job->mapContextId = mapContextId;
  DataInterface_queueJob(job, cb_DataInterface_Spawn_getSpawnpool, cb, param);
}

//###################### test 4 ###################
void cb_DataInterface_PathNode_updatePathNode(MYSQL *dbCon, diJob_updatePathNodeW2_t *job, void *cb, void *param)
{

	__debugbreak();
	sint8 queryText[4 * 1024];
	di_pathNodeDataW2_t *pNodeData = job->pnodedata;
    sint32 tcontextid,tspawnid,tpnodeindex;
	float tx,ty,tz;
	tcontextid = pNodeData->currentContextId;
	tspawnid = pNodeData ->spawntype;
	tpnodeindex = pNodeData ->pathindex;
	tx = pNodeData ->posX;
	ty = pNodeData ->posY;
	tz = pNodeData ->posZ;
	sprintf(queryText, "SELECT * FROM pathnodes WHERE spawnid ='%d' AND pindex='%d'", 
	pNodeData->spawntype,pNodeData->pathindex);
	if( mysql_query(dbCon, queryText) )
	{
		//--- characterData->error = true;
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		if( cb )
			((void (*)(void*,void*))cb)(param, job);
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	sint32 spawnCount = mysql_num_rows(dbResult);
	
	if(spawnCount)
	{
		printf("Pathnode already in use\n");
		mysql_free_result(dbResult);
		return;
	}

	//--- create new pathnode
	sprintf(queryText, "INSERT INTO pathnodes (spawnid,contextid,posx,posy,posz,pindex)"
		" VALUES (%d,%d,%f,%f,%f,%d)", 
	tspawnid,tcontextid,tx,ty,tz,tpnodeindex);
	//--- execute query
	if( mysql_query(dbCon, queryText) )
	{
		//--- characterData->error = true;
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		if( cb )
			((void (*)(void*,void*))cb)(param, job);
		return;
	}
}

void DataInterface_PathNode_setPathnode(di_pathNodeDataW2_t *pnodedata, void (*cb)(void *param, diJob_spawnTypeW2_t *jobData), void *param)
{
 
  diJob_updatePathNodeW2_t *job = (diJob_updatePathNodeW2_t*)DataInterface_allocJob(sizeof(diJob_updatePathNodeW2_t));
  job->pnodedata = pnodedata;
  DataInterface_queueJob(job, cb_DataInterface_PathNode_updatePathNode, cb, param);
}

void cb_DataInterface_NPC_getLastNPCEntityID(MYSQL *dbCon, diJob_getLastNPCEntityID_t *job, void *cb, void *param)
{
	sint8 queryText[512];
	// find highest entityId of npcs
	bool npcAlreadyExists = false;
	sprintf(queryText, "SELECT max(id) FROM npc");
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		job->outState = 2;
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	if( mysql_num_rows(dbResult) )
	{
		MYSQL_ROW dbRow;
		dbRow = mysql_fetch_row(dbResult);
		if( dbRow[0] )
			sscanf(dbRow[0], "%I64u", &job->outEntityId);
		else
			job->outEntityId = 0;
	}
	else
		job->outEntityId = 0;
	mysql_free_result(dbResult);

	job->outState = 1;
	// job is freed outside
}

unsigned long long DataInterface_NPC_getLastNPCEntityID()
{	
	diJob_getLastNPCEntityID_t *job = (diJob_getLastNPCEntityID_t*)DataInterface_allocJob(sizeof(diJob_getLastNPCEntityID_t));
	job->outEntityId = 0;
	job->outState = 0;
	DataInterface_queueJob(job, cb_DataInterface_NPC_getLastNPCEntityID, NULL, NULL);
	while( job->outState == 0 ) Sleep(10);
	unsigned long long entityId = job->outEntityId;
	if( job->outState == 2 ) // error
		entityId = 0;
	DataInterface_freeJob(job);
	return entityId;
}
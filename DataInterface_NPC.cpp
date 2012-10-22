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
	"id,creatureTypeId,"
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
	"ad21_classId,ad21_hue,npcPackageId"
	" FROM creature_type_npc");
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
		sint32 npc_creatureTypeId;
		sint32 idx = 0;
		sscanf(dbRow[idx], "%I64u", &npcData->npcID); idx++;
		sscanf(dbRow[idx], "%d", &npc_creatureTypeId); idx++;
		npcData->creatureTypeId = npc_creatureTypeId;
		for(sint32 i=0; i<21; i++)
		{
			sscanf(dbRow[idx+0], "%d", &npcData->appearanceData[i].classId);
			sscanf(dbRow[idx+1], "%u", &npcData->appearanceData[i].hue);
			idx += 2;
		}
		sscanf(dbRow[idx], "%d", &npcData->npcPackageId); idx++;
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

void DataInterface_NPC_getNPCList(void (*cb)(void *param, diJob_npcListData_t *jobData), void *param)
{	
	diJob_npcListData_t *job = (diJob_npcListData_t*)DataInterface_allocJob(sizeof(diJob_npcListData_t));
	DataInterface_queueJob(job, cb_DataInterface_NPC_getNPCList, cb, param);
}
//
//void cb_DataInterface_NPC_updateNPC(MYSQL *dbCon, diJob_updateNPC_t *job, void *cb, void *param)
//{
//	sint8 queryText[1024*4];
//	di_npcData_t *npcData = job->npcData;
//	// check if npc exists first (maybe should lock table)
//	bool npcAlreadyExists = false;
//	sprintf(queryText, "SELECT id FROM npc WHERE id=%I64d", job->npcData->npcID);
//	if( mysql_query(dbCon, queryText) )
//	{
//		printf("Error in query\n");
//		puts(queryText);
//		puts(mysql_error(dbCon));
//		((void (*)(void*,void*))cb)(param, job);
//		return;
//	}
//	MYSQL_RES *dbResult = mysql_store_result(dbCon);
//	if( mysql_num_rows(dbResult) )
//		npcAlreadyExists = true;
//	mysql_free_result(dbResult);
//	if( npcAlreadyExists == false )
//	{
//		// create npc
//		sprintf(queryText, "INSERT INTO npc ("
//			"`id`,`name`,`entityClassId`,"
//			"`mapContextId`,`posX`,`posY`,`posZ`,"
//			"`ad1_classId`,`ad1_hue`,"
//			"`ad2_classId`,`ad2_hue`,"
//			"`ad3_classId`,`ad3_hue`,"
//			"`ad4_classId`,`ad4_hue`,"
//			"`ad5_classId`,`ad5_hue`,"
//			"`ad6_classId`,`ad6_hue`,"
//			"`ad7_classId`,`ad7_hue`,"
//			"`ad8_classId`,`ad8_hue`,"
//			"`ad9_classId`,`ad9_hue`,"
//			"`ad10_classId`,`ad10_hue`,"
//			"`ad11_classId`,`ad11_hue`,"
//			"`ad12_classId`,`ad12_hue`,"
//			"`ad13_classId`,`ad13_hue`,"
//			"`ad14_classId`,`ad14_hue`,"
//			"`ad15_classId`,`ad15_hue`,"
//			"`ad16_classId`,`ad16_hue`,"
//			"`ad17_classId`,`ad17_hue`,"
//			"`ad18_classId`,`ad18_hue`,"
//			"`ad19_classId`,`ad19_hue`,"
//			"`ad20_classId`,`ad20_hue`,"
//			"`ad21_classId`,`ad21_hue`"
//			") VALUES"
//			"(%I64u,'%s',%d,"
//			"%d,%f,%f,%f,"
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 1
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 5
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 9
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 13
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 17
//			"%d,%u"					   // 21
//			")", 
//			npcData->npcID, npcData->unicodeName, npcData->entityClassID,
//			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
//			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
//			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
//			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
//			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
//			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
//			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
//			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
//			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
//			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
//			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
//			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
//			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
//			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
//			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
//			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
//			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
//			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
//			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
//			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
//			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
//			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue
//			);
//		// execute query
//		if( mysql_query(dbCon, queryText) )
//		{
//			//characterData->error = true;
//			printf("Error in query\n");
//			puts(queryText);
//			puts(mysql_error(dbCon));
//			if( cb )
//				((void (*)(void*,void*))cb)(param, job);
//			return;
//		}
//		// do callback
//		if( cb )
//			((void (*)(void*,void*))cb)(param, job);
//	}
//	else
//	{
//		// update npc
//		sprintf(queryText, "UPDATE npc SET "
//			"`name`='%s',`entityClassId`=%d,"
//			"`mapContextId`=%d,`posX`=%lf,`posY`=%lf,`posZ`=%lf,"
//			"`ad1_classId`=%u,`ad1_hue`=%u,"
//			"`ad2_classId`=%u,`ad2_hue`=%u,"
//			"`ad3_classId`=%u,`ad3_hue`=%u,"
//			"`ad4_classId`=%u,`ad4_hue`=%u,"
//			"`ad5_classId`=%u,`ad5_hue`=%u,"
//			"`ad6_classId`=%u,`ad6_hue`=%u,"
//			"`ad7_classId`=%u,`ad7_hue`=%u,"
//			"`ad8_classId`=%u,`ad8_hue`=%u,"
//			"`ad9_classId`=%u,`ad9_hue`=%u,"
//			"`ad10_classId`=%u,`ad10_hue`=%u,"
//			"`ad11_classId`=%u,`ad11_hue`=%u,"
//			"`ad12_classId`=%u,`ad12_hue`=%u,"
//			"`ad13_classId`=%u,`ad13_hue`=%u,"
//			"`ad14_classId`=%u,`ad14_hue`=%u,"
//			"`ad15_classId`=%u,`ad15_hue`=%u,"
//			"`ad16_classId`=%u,`ad16_hue`=%u,"
//			"`ad17_classId`=%u,`ad17_hue`=%u,"
//			"`ad18_classId`=%u,`ad18_hue`=%u,"
//			"`ad19_classId`=%u,`ad19_hue`=%u,"
//			"`ad20_classId`=%u,`ad20_hue`=%u,"
//			"`ad21_classId`=%u,`ad21_hue`=%u"
//			" WHERE id=%I64u LIMIT 1", 
//			npcData->unicodeName, npcData->entityClassID,
//			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
//			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
//			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
//			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
//			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
//			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
//			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
//			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
//			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
//			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
//			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
//			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
//			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
//			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
//			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
//			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
//			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
//			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
//			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
//			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
//			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
//			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue,
//			npcData->npcID
//			);
//		// execute query
//		if( mysql_query(dbCon, queryText) )
//		{
//			//characterData->error = true;
//			printf("Error in query\n");
//			puts(queryText);
//			puts(mysql_error(dbCon));
//			if( cb )
//				((void (*)(void*,void*))cb)(param, job);
//			return;
//		}
//		// do callback
//		if( cb )
//			((void (*)(void*,void*))cb)(param, job);
//	}
//	DataInterface_freeJob(job);
//}
//
//void DataInterface_NPC_updateNPC(di_npcData_t *npcData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param)
//{	
//	diJob_updateNPC_t *job = (diJob_updateNPC_t*)DataInterface_allocJob(sizeof(diJob_updateNPC_t));
//	job->npcData = npcData;
//	DataInterface_queueJob(job, cb_DataInterface_NPC_updateNPC, cb, param);
//}

////############################# TEST #########################################
//void cb_DataInterface_Entity_updateEntityW(MYSQL *dbCon, diJob_updateEntityW_t *job, void *cb, void *param)
//{
//	sint8 queryText[4 * 1024];
//	di_entityDataW_t *npcData = job->entityData;
//	// check if npc exists first (maybe should lock table)
//	bool npcAlreadyExists = false;
//	sprintf(queryText, "SELECT id FROM npc WHERE id=%I64d", job->entityData->npcID);
//	if( mysql_query(dbCon, queryText) )
//	{
//		printf("Error in query\n");
//		puts(queryText);
//		puts(mysql_error(dbCon));
//		((void (*)(void*,void*))cb)(param, job);
//		return;
//	}
//	MYSQL_RES *dbResult = mysql_store_result(dbCon);
//	if( mysql_num_rows(dbResult) )
//		npcAlreadyExists = true;
//	mysql_free_result(dbResult);
//	if( npcAlreadyExists == false )
//	{
//		// create npc
//		sprintf(queryText, "INSERT INTO npc ("
//			"`id`,`name`,`entityClassId`,"
//			"`mapContextId`,`posX`,`posY`,`posZ`,`rotation`,"
//			"`ad1_classId`,`ad1_hue`,"
//			"`ad2_classId`,`ad2_hue`,"
//			"`ad3_classId`,`ad3_hue`,"
//			"`ad4_classId`,`ad4_hue`,"
//			"`ad5_classId`,`ad5_hue`,"
//			"`ad6_classId`,`ad6_hue`,"
//			"`ad7_classId`,`ad7_hue`,"
//			"`ad8_classId`,`ad8_hue`,"
//			"`ad9_classId`,`ad9_hue`,"
//			"`ad10_classId`,`ad10_hue`,"
//			"`ad11_classId`,`ad11_hue`,"
//			"`ad12_classId`,`ad12_hue`,"
//			"`ad13_classId`,`ad13_hue`,"
//			"`ad14_classId`,`ad14_hue`,"
//			"`ad15_classId`,`ad15_hue`,"
//			"`ad16_classId`,`ad16_hue`,"
//			"`ad17_classId`,`ad17_hue`,"
//			"`ad18_classId`,`ad18_hue`,"
//			"`ad19_classId`,`ad19_hue`,"
//			"`ad20_classId`,`ad20_hue`,"
//			"`ad21_classId`,`ad21_hue`"
//			") VALUES"
//			"(%I64u,'%s',%d,"
//			"%d,%f,%f,%f,%f,"
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 1
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 5
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 9
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 13
//			"%d,%u,%d,%u,%d,%u,%d,%u," // 17
//			"%d,%u"					   // 21
//			")", 
//			npcData->npcID, npcData->unicodeName, npcData->entityClassID,
//			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
//			(double)npcData->rotation,
//			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
//			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
//			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
//			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
//			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
//			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
//			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
//			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
//			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
//			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
//			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
//			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
//			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
//			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
//			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
//			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
//			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
//			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
//			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
//			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
//			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue
//			);
//		// execute query
//		if( mysql_query(dbCon, queryText) )
//		{
//			//characterData->error = true;
//			printf("Error in query\n");
//			puts(queryText);
//			puts(mysql_error(dbCon));
//			if( cb )
//				((void (*)(void*,void*))cb)(param, job);
//			return;
//		}
//		// do callback
//		if( cb )
//			((void (*)(void*,void*))cb)(param, job);
//	}
//	else
//	{
//		// update npc
//		sprintf(queryText, "UPDATE npc SET "
//			"`name`='%s',`entityClassId`=%d,"
//			"`mapContextId`=%d,`posX`=%lf,`posY`=%lf,`posZ`=%lf,`rotation=%f`,"
//			"`ad1_classId`=%u,`ad1_hue`=%u,"
//			"`ad2_classId`=%u,`ad2_hue`=%u,"
//			"`ad3_classId`=%u,`ad3_hue`=%u,"
//			"`ad4_classId`=%u,`ad4_hue`=%u,"
//			"`ad5_classId`=%u,`ad5_hue`=%u,"
//			"`ad6_classId`=%u,`ad6_hue`=%u,"
//			"`ad7_classId`=%u,`ad7_hue`=%u,"
//			"`ad8_classId`=%u,`ad8_hue`=%u,"
//			"`ad9_classId`=%u,`ad9_hue`=%u,"
//			"`ad10_classId`=%u,`ad10_hue`=%u,"
//			"`ad11_classId`=%u,`ad11_hue`=%u,"
//			"`ad12_classId`=%u,`ad12_hue`=%u,"
//			"`ad13_classId`=%u,`ad13_hue`=%u,"
//			"`ad14_classId`=%u,`ad14_hue`=%u,"
//			"`ad15_classId`=%u,`ad15_hue`=%u,"
//			"`ad16_classId`=%u,`ad16_hue`=%u,"
//			"`ad17_classId`=%u,`ad17_hue`=%u,"
//			"`ad18_classId`=%u,`ad18_hue`=%u,"
//			"`ad19_classId`=%u,`ad19_hue`=%u,"
//			"`ad20_classId`=%u,`ad20_hue`=%u,"
//			"`ad21_classId`=%u,`ad21_hue`=%u"
//			" WHERE id=%I64u LIMIT 1", 
//			npcData->unicodeName, npcData->entityClassID,
//			npcData->currentContextId,(double)npcData->posX,(double)npcData->posY,(double)npcData->posZ,
//			(double)npcData->rotation,
//			npcData->appearanceData[0].classId,npcData->appearanceData[0].hue,
//			npcData->appearanceData[1].classId,npcData->appearanceData[1].hue,
//			npcData->appearanceData[2].classId,npcData->appearanceData[2].hue,
//			npcData->appearanceData[3].classId,npcData->appearanceData[3].hue,
//			npcData->appearanceData[4].classId,npcData->appearanceData[4].hue,
//			npcData->appearanceData[5].classId,npcData->appearanceData[5].hue,
//			npcData->appearanceData[6].classId,npcData->appearanceData[6].hue,
//			npcData->appearanceData[7].classId,npcData->appearanceData[7].hue,
//			npcData->appearanceData[8].classId,npcData->appearanceData[8].hue,
//			npcData->appearanceData[9].classId,npcData->appearanceData[9].hue,
//			npcData->appearanceData[10].classId,npcData->appearanceData[10].hue,
//			npcData->appearanceData[11].classId,npcData->appearanceData[11].hue,
//			npcData->appearanceData[12].classId,npcData->appearanceData[12].hue,
//			npcData->appearanceData[13].classId,npcData->appearanceData[13].hue,
//			npcData->appearanceData[14].classId,npcData->appearanceData[14].hue,
//			npcData->appearanceData[15].classId,npcData->appearanceData[15].hue,
//			npcData->appearanceData[16].classId,npcData->appearanceData[16].hue,
//			npcData->appearanceData[17].classId,npcData->appearanceData[17].hue,
//			npcData->appearanceData[18].classId,npcData->appearanceData[18].hue,
//			npcData->appearanceData[19].classId,npcData->appearanceData[19].hue,
//			npcData->appearanceData[20].classId,npcData->appearanceData[20].hue
//			);
//		// execute query
//		if( mysql_query(dbCon, queryText) )
//		{
//			//characterData->error = true;
//			printf("Error in query\n");
//			puts(queryText);
//			puts(mysql_error(dbCon));
//			if( cb )
//				((void (*)(void*,void*))cb)(param, job);
//			return;
//		}
//		// do callback
//		if( cb )
//			((void (*)(void*,void*))cb)(param, job);
//	}
//	DataInterface_freeJob(job);
//}
//
//
//void DataInterface_Entity_updateEntityW(di_entityDataW_t *entityData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param)
//{	
//		
//	diJob_updateEntityW_t *job = (diJob_updateEntityW_t*)DataInterface_allocJob(sizeof(diJob_updateEntityW_t));
//	job->entityData = entityData;
//	DataInterface_queueJob(job, cb_DataInterface_Entity_updateEntityW, cb, param);
//}


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
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
	sint32 npcCount = (sint32)mysql_num_rows(dbResult);
	di_npcData_t *npcDataList = (di_npcData_t*)malloc(sizeof(di_npcData_t) * npcCount);
	//allocate the spawntypes/pools the same way
	di_npcData_t* npcData = npcDataList;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		//unsigned long long npc_id;
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
		npcData++;
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

/*
 * Queries data from vendor_items table for a specific vendor
 */
void _cb_DataInterface_Vendor_getVendorItemList(MYSQL *dbCon, di_vendorData_t* vendorData)
{
	sint8 queryText[1024];
	wsprintf(queryText, "SELECT "
		"itemTemplateId,stacksize,sequence"
		" FROM vendor_items WHERE creatureTypeId = %u", vendorData->creatureTypeId);
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate vendor item data
	sint32 itemCount = (sint32)mysql_num_rows(dbResult);
	di_vendorItemData_t *vendorItemDataList = (di_vendorItemData_t*)malloc(sizeof(di_vendorItemData_t) * itemCount);
	// parse mysql data
	di_vendorItemData_t* vendorItemData = vendorItemDataList;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		sint32 vendorItem_itemTemplateId = 0;
		sint32 vendorItem_stacksize = 0;
		sint32 vendorItem_sequence = 0;
		sint32 idx = 0;
		sscanf(dbRow[idx], "%d", &vendorItem_itemTemplateId); idx++;
		sscanf(dbRow[idx], "%d", &vendorItem_stacksize); idx++;
		sscanf(dbRow[idx], "%d", &vendorItem_sequence); idx++;
		vendorItemData->itemTemplateId = vendorItem_itemTemplateId;
		vendorItemData->stacksize = vendorItem_stacksize;
		vendorItemData->sequence = vendorItem_sequence;
		// next
		vendorItemData++;
	}
	mysql_free_result(dbResult);
	vendorData->itemList = vendorItemDataList;
	vendorData->numberOfItems = itemCount;
}

/*
 * Queries data from vendor table for all vendors
 * Also, queries each vendor's item list
 */
void cb_DataInterface_Vendor_getVendorList(MYSQL *dbCon, diJob_vendorListData_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	job->outVendorCount = 0;
	job->outVendorList = NULL;

	wsprintf(queryText, "SELECT "
		"creatureTypeId,vendorPackageId"
		" FROM vendor");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	// allocate vendor data
	sint32 vendorCount = (sint32)mysql_num_rows(dbResult);
	di_vendorData_t *vendorDataList = (di_vendorData_t*)malloc(sizeof(di_vendorData_t) * vendorCount);
	// parse mysql data
	di_vendorData_t* vendorData = vendorDataList;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		sint32 vendor_creatureTypeId = 0;
		sint32 vendor_packageId = 0;
		sint32 idx = 0;
		sscanf(dbRow[idx], "%d", &vendor_creatureTypeId); idx++;
		sscanf(dbRow[idx], "%d", &vendor_packageId); idx++;
		vendorData->creatureTypeId = vendor_creatureTypeId;
		vendorData->vendorPackageId = vendor_packageId;
		// for each vendor, query item data
		_cb_DataInterface_Vendor_getVendorItemList(dbCon, vendorData);
		// next
		vendorData++;
	}
	mysql_free_result(dbResult);
	job->outVendorList = vendorDataList;
	job->outVendorCount = vendorCount;
	// do callback param1: mapchannel param2: list of vendordata
	((void (*)(void*,void*))cb)(param, job);
	// free all item lists
	for(sint32 i=0; i<vendorCount; i++)
	{
		if( job->outVendorList[i].itemList )
			free(job->outVendorList[i].itemList);
	}
	// free vendor list
	free(vendorDataList);
	// free job
	DataInterface_freeJob(job);
}

void DataInterface_Vendor_getVendorList(void (*cb)(void *param, diJob_vendorListData_t *jobData), void *param)
{	
	diJob_vendorListData_t *job = (diJob_vendorListData_t*)DataInterface_allocJob(sizeof(diJob_vendorListData_t));
	DataInterface_queueJob(job, cb_DataInterface_Vendor_getVendorList, cb, param);
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

//
////###################### test 4 ###################
//void cb_DataInterface_PathNode_updatePathNode(MYSQL *dbCon, diJob_updatePathNodeW2_t *job, void *cb, void *param)
//{
//
//	__debugbreak();
//	sint8 queryText[4 * 1024];
//	di_pathNodeDataW2_t *pNodeData = job->pnodedata;
//    sint32 tcontextid,tspawnid,tpnodeindex;
//	float tx,ty,tz;
//	tcontextid = pNodeData->currentContextId;
//	tspawnid = pNodeData ->spawntype;
//	tpnodeindex = pNodeData ->pathindex;
//	tx = pNodeData ->posX;
//	ty = pNodeData ->posY;
//	tz = pNodeData ->posZ;
//	sprintf(queryText, "SELECT * FROM pathnodes WHERE spawnid ='%d' AND pindex='%d'", 
//	pNodeData->spawntype,pNodeData->pathindex);
//	if( mysql_query(dbCon, queryText) )
//	{
//		printf("Error in query\n");
//		puts(queryText);
//		puts(mysql_error(dbCon));
//		if( cb )
//			((void (*)(void*,void*))cb)(param, job);
//		return;
//	}
//	MYSQL_RES *dbResult = mysql_store_result(dbCon);
//	MYSQL_ROW dbRow;
//	sint32 spawnCount = mysql_num_rows(dbResult);
//	
//	if(spawnCount)
//	{
//		printf("Pathnode already in use\n");
//		mysql_free_result(dbResult);
//		return;
//	}
//
//	//--- create new pathnode
//	sprintf(queryText, "INSERT INTO pathnodes (spawnid,contextid,posx,posy,posz,pindex)"
//		" VALUES (%d,%d,%f,%f,%f,%d)", 
//	tspawnid,tcontextid,tx,ty,tz,tpnodeindex);
//	//--- execute query
//	if( mysql_query(dbCon, queryText) )
//	{
//		//--- characterData->error = true;
//		printf("Error in query\n");
//		puts(queryText);
//		puts(mysql_error(dbCon));
//		if( cb )
//			((void (*)(void*,void*))cb)(param, job);
//		return;
//	}
//}

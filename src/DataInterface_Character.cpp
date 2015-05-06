#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"

void cb_DataInterface_Character_getCharacterPreviewInfo(MYSQL *dbCon, diJob_getCharacterPreviewInfo_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	if( job->slotIndex == -1 )
		wsprintf(queryText, "SELECT "
		"id,name,lastname,slotId,gender,raceId,classId,"
		"currentContextId,posX,posY,posZ,"
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
		" FROM characters WHERE userId=%I64u", job->userId);
	else
	{
		wsprintf(queryText, "SELECT "
			"id,name,lastname,slotId,gender,raceId,classId,"
			"currentContextId,posX,posY,posZ,"
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
			" FROM characters WHERE userId=%I64u AND slotId=%d", job->userId, job->slotIndex);
	}

	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		unsigned long long char_id;
		sint32 char_gender;
		sint32 char_slotIndex;
		sint32 char_rawSlotIndex;
		sint32 char_race;
		sint32 char_classId;
		sint32 char_currentContextId;
		double char_posX;
		double char_posY;
		double char_posZ;

		sscanf(dbRow[0], "%I64u", &char_id);
		sscanf(dbRow[3], "%d", &char_slotIndex);
		sscanf(dbRow[4], "%d", &char_gender);
		sscanf(dbRow[5], "%d", &char_race);
		sscanf(dbRow[6], "%d", &char_classId);
		sscanf(dbRow[7], "%d", &char_currentContextId);
		sscanf(dbRow[8], "%lf", &char_posX);
		sscanf(dbRow[9], "%lf", &char_posY);
		sscanf(dbRow[10], "%lf", &char_posZ);
		char_rawSlotIndex = char_slotIndex-1;
		job->outPreviewData[char_rawSlotIndex] = (di_characterPreview_t *)malloc(sizeof(di_characterPreview_t));
		sint32 rIdx = 11;
		for(sint32 i=0; i<21; i++)
		{
			sscanf(dbRow[rIdx+0], "%d", &job->outPreviewData[char_rawSlotIndex]->appearanceData[i].classId);
			sscanf(dbRow[rIdx+1], "%u", &job->outPreviewData[char_rawSlotIndex]->appearanceData[i].hue);
			rIdx += 2;
		}
		job->outPreviewData[char_rawSlotIndex]->characterID = char_id;
		strcpy(job->outPreviewData[char_rawSlotIndex]->unicodeName, dbRow[1]);
		strcpy(job->outPreviewData[char_rawSlotIndex]->unicodeFamily, dbRow[2]);
		job->outPreviewData[char_rawSlotIndex]->genderIsMale = char_gender==0;
		job->outPreviewData[char_rawSlotIndex]->raceID = char_race;
		job->outPreviewData[char_rawSlotIndex]->classID = char_classId;
		job->outPreviewData[char_rawSlotIndex]->slotIndex = char_slotIndex;
		job->outPreviewData[char_rawSlotIndex]->userID = job->userId;
		job->outPreviewData[char_rawSlotIndex]->currentContextId = char_currentContextId;
		job->outPreviewData[char_rawSlotIndex]->posX = (float)char_posX;
		job->outPreviewData[char_rawSlotIndex]->posY = (float)char_posY;
		job->outPreviewData[char_rawSlotIndex]->posZ = (float)char_posZ;
	}
	mysql_free_result(dbResult);
	// do callback
	((void (*)(void*,void*))cb)(param, job);
	// free data
	for(sint32 i=0; i<16; i++)
	{
		free(job->outPreviewData[i]);
	}
	DataInterface_freeJob(job);
}

void DataInterface_Character_getCharacterPreviewInfo(unsigned long long userID, uint32 slotIndex, void (*cb)(void *param, diJob_getCharacterPreviewInfo_t *jobData), void *param)
{	
	diJob_getCharacterPreviewInfo_t *job = (diJob_getCharacterPreviewInfo_t*)DataInterface_allocJob(sizeof(diJob_getCharacterPreviewInfo_t));
	job->userId = userID;
	job->slotIndex = slotIndex;
	for(sint32 i=0; i<16; i++)
		job->outPreviewData[i] = NULL;
	DataInterface_queueJob(job, cb_DataInterface_Character_getCharacterPreviewInfo, cb, param);
}

void cb_DataInterface_Character_getCharacterData(MYSQL *dbCon, diJob_characterData_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	wsprintf(queryText, "SELECT "
			"id,name,lastname,slotId,gender,raceId,classId,"
			"currentContextId,posX,posY,posZ,"
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
			" FROM characters WHERE userId=%I64u AND slotId=%d LIMIT 1", job->userId, job->slotIndex);

	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	job->outCharacterData = NULL;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		unsigned long long char_id;
		sint32 char_gender;
		sint32 char_slotIndex;
		sint32 char_rawSlotIndex;
		sint32 char_race;
		sint32 char_classId;
		sint32 char_currentContextId;
		double char_posX;
		double char_posY;
		double char_posZ;

		sscanf(dbRow[0], "%I64u", &char_id);
		sscanf(dbRow[3], "%d", &char_slotIndex);
		sscanf(dbRow[4], "%d", &char_gender);
		sscanf(dbRow[5], "%d", &char_race);
		sscanf(dbRow[6], "%d", &char_classId);
		sscanf(dbRow[7], "%d", &char_currentContextId);
		sscanf(dbRow[8], "%lf", &char_posX);
		sscanf(dbRow[9], "%lf", &char_posY);
		sscanf(dbRow[10], "%lf", &char_posZ);
		char_rawSlotIndex = char_slotIndex-1;
		job->outCharacterData = (di_characterData_t*)malloc(sizeof(di_characterData_t));
		sint32 rIdx = 11;
		for(sint32 i=0; i<21; i++)
		{
			sscanf(dbRow[rIdx+0], "%d", &job->outCharacterData->appearanceData[i].classId);
			sscanf(dbRow[rIdx+1], "%u", &job->outCharacterData->appearanceData[i].hue);
			rIdx += 2;
		}
		job->outCharacterData->characterID = char_id;
		strcpy(job->outCharacterData->unicodeName, dbRow[1]);
		strcpy(job->outCharacterData->unicodeFamily, dbRow[2]);
		job->outCharacterData->genderIsMale = char_gender==0;
		job->outCharacterData->raceID = char_race;
		job->outCharacterData->classID = char_classId;
		job->outCharacterData->slotIndex = char_slotIndex;
		job->outCharacterData->userID = job->userId;
		job->outCharacterData->currentContextId = char_currentContextId;
		job->outCharacterData->posX = (float)char_posX;
		job->outCharacterData->posY = (float)char_posY;
		job->outCharacterData->posZ = (float)char_posZ;
	}
	mysql_free_result(dbResult);
	// get mission data (TODO)
	job->outCharacterData->missionStateCount = 0;
	job->outCharacterData->missionStateData = NULL;
	// do callback
	((void (*)(void*,void*))cb)(param, job);
	// free data
	if( job->outCharacterData )
		free(job->outCharacterData);
	DataInterface_freeJob(job);
}

void DataInterface_Character_getCharacterData(unsigned long long userID, uint32 slotIndex, void (*cb)(void *param, diJob_characterData_t *jobData), void *param)
{	
	diJob_characterData_t *job = (diJob_characterData_t*)DataInterface_allocJob(sizeof(diJob_characterData_t));
	job->userId = userID;
	job->slotIndex = slotIndex;
	job->outCharacterData = NULL;
	DataInterface_queueJob(job, cb_DataInterface_Character_getCharacterData, cb, param);
}

void cb_DataInterface_Character_createCharacter(MYSQL *dbCon, di_characterLayout_t *characterData, void *cb, void *param)
{
	sint8 queryText[2048];
	// todo, validate name first ( so no sql injection is possible, use mysql_real_escape_string ? )
	// reset error codes
	characterData->error_nameAlreadyInUse = false;
	characterData->error = false;
	// check username first (maybe should lock table)
	sprintf(queryText, "SELECT id FROM characters WHERE name='%s'", characterData->unicodeName);
	if( mysql_query(dbCon, queryText) )
	{
		characterData->error = true;
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void (*)(void*,void*))cb)(param, characterData);
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	if( mysql_num_rows(dbResult) )
	{
		// name already in use
		characterData->error = true;
		characterData->error_nameAlreadyInUse = true;
		((void (*)(void*,void*))cb)(param, characterData);
		return;
	}
	mysql_free_result(dbResult);
	// try insert character	
	sprintf(queryText, "INSERT INTO characters ("
			"`name`,`lastname`,`slotId`,`gender`,`raceId`,`classId`,`userId`,"
			"`currentContextId`,`posX`,`posY`,`posZ`,`rotation`,"
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
			"('%s','%s',%d,%d,%d,%d,%I64u,"
			"%d,%f,%f,%f,0,"
			"%d,%u,%d,%u,%d,%u,%d,%u," // 1
			"%d,%u,%d,%u,%d,%u,%d,%u," // 5
			"%d,%u,%d,%u,%d,%u,%d,%u," // 9
			"%d,%u,%d,%u,%d,%u,%d,%u," // 13
			"%d,%u,%d,%u,%d,%u,%d,%u," // 17
			"%d,%u"					   // 21
			")", 
			characterData->unicodeName, characterData->unicodeFamily, characterData->slotIndex, characterData->genderIsMale?0:1, characterData->raceID, characterData->classId, characterData->userID,
			characterData->currentContextId,(float)characterData->posX,(float)characterData->posY,(float)characterData->posZ,
			characterData->appearanceData[0].classId,characterData->appearanceData[0].hue,
			characterData->appearanceData[1].classId,characterData->appearanceData[1].hue,
			characterData->appearanceData[2].classId,characterData->appearanceData[2].hue,
			characterData->appearanceData[3].classId,characterData->appearanceData[3].hue,
			characterData->appearanceData[4].classId,characterData->appearanceData[4].hue,
			characterData->appearanceData[5].classId,characterData->appearanceData[5].hue,
			characterData->appearanceData[6].classId,characterData->appearanceData[6].hue,
			characterData->appearanceData[7].classId,characterData->appearanceData[7].hue,
			characterData->appearanceData[8].classId,characterData->appearanceData[8].hue,
			characterData->appearanceData[9].classId,characterData->appearanceData[9].hue,
			characterData->appearanceData[10].classId,characterData->appearanceData[10].hue,
			characterData->appearanceData[11].classId,characterData->appearanceData[11].hue,
			characterData->appearanceData[12].classId,characterData->appearanceData[12].hue,
			characterData->appearanceData[13].classId,characterData->appearanceData[13].hue,
			characterData->appearanceData[14].classId,characterData->appearanceData[14].hue,
			characterData->appearanceData[15].classId,characterData->appearanceData[15].hue,
			characterData->appearanceData[16].classId,characterData->appearanceData[16].hue,
			characterData->appearanceData[17].classId,characterData->appearanceData[17].hue,
			characterData->appearanceData[18].classId,characterData->appearanceData[18].hue,
			characterData->appearanceData[19].classId,characterData->appearanceData[19].hue,
			characterData->appearanceData[20].classId,characterData->appearanceData[20].hue
			);
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		characterData->error = true;
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void (*)(void*,void*))cb)(param, characterData);
		return;
	}
	// do callback
	((void (*)(void*,void*))cb)(param, characterData);
	// dont free data as we do not allocate it here
}

void DataInterface_Character_createCharacter(di_characterLayout_t *characterData, void (*cb)(void *param, di_characterLayout_t *jobData), void *param)
{	
	DataInterface_queueJob(characterData, cb_DataInterface_Character_createCharacter, cb, param);
}

void cb_DataInterface_Character_deleteCharacter(MYSQL *dbCon, diJob_deleteCharacter_t *job, void *cb, void *param)
{
	sint8 queryText[512];
	wsprintf(queryText, "DELETE FROM characters WHERE userId=%I64u AND slotId=%d", job->userId, job->slotId);
	// execute query
	job->error = false;
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		job->error = true;
	}
	else
	{
		if( mysql_affected_rows(dbCon) == 0 ) // nothing deleted, set error
			job->error = true;
	}
	// do callback
	((void (*)(void*,void*))cb)(param, job);
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_deleteCharacter(unsigned long long userID, sint32 slotId, void (*cb)(void *param, diJob_deleteCharacter_t *jobData), void *param)
{	
	diJob_deleteCharacter_t *job = (diJob_deleteCharacter_t*)DataInterface_allocJob(sizeof(diJob_deleteCharacter_t));
	job->userId = userID;
	job->slotId = slotId;
	DataInterface_queueJob(job, cb_DataInterface_Character_deleteCharacter, cb, param);
}


// 

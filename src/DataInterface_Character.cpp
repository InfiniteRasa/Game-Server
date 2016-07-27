#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"
#include<time.h>

void cb_DataInterface_Character_getCharacterPreviewInfo(MYSQL *dbCon, diJob_getCharacterPreviewInfo_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	if (job->slotIndex == -1)
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
		"ad21_classId,ad21_hue,"
		"level,numLogins,totalTimePlayed,TIMESTAMPDIFF(MINUTE,timeSinceLastPlayed,NOW()) AS timeSinceLastPlayed,clonecredits,body,mind,spirit,experience"
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
			"ad21_classId,ad21_hue,"
			"level,numLogins,totalTimePlayed,TIMESTAMPDIFF(MINUTE,timeSinceLastPlayed,NOW()) AS timeSinceLastPlayed,clonecredits,body,mind,spirit,experience"
			" FROM characters WHERE userId=%I64u AND slotId=%d", job->userId, job->slotIndex);
	}

	// execute query
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query cb_DataInterface_Character_getCharacterPreviewInfo\n");
		while (1) Sleep(1000);
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	while ((dbRow = mysql_fetch_row(dbResult)))
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
		sint32 char_level;
		uint32 char_numLogins;
		uint32 char_totalTimePlayed;
		uint32 char_timeSinceLastPlayed;
		uint32 char_clonecredits;
		uint32 char_body;
		uint32 char_mind;
		uint32 char_spirit;
		uint32 char_experience;
		sscanf(dbRow[0], "%I64u", &char_id);
		sscanf(dbRow[3], "%d", &char_slotIndex);
		sscanf(dbRow[4], "%d", &char_gender);
		sscanf(dbRow[5], "%d", &char_race);
		sscanf(dbRow[6], "%d", &char_classId);
		sscanf(dbRow[7], "%d", &char_currentContextId);
		sscanf(dbRow[8], "%lf", &char_posX);
		sscanf(dbRow[9], "%lf", &char_posY);
		sscanf(dbRow[10], "%lf", &char_posZ);
		char_rawSlotIndex = char_slotIndex - 1;
		job->outPreviewData[char_rawSlotIndex] = (di_characterPreview_t *)malloc(sizeof(di_characterPreview_t));
		sint32 rIdx = 11;
		for (sint32 i = 0; i < 21; i++)
		{
			sscanf(dbRow[rIdx + 0], "%d", &job->outPreviewData[char_rawSlotIndex]->appearanceData[i].classId);
			sscanf(dbRow[rIdx + 1], "%u", &job->outPreviewData[char_rawSlotIndex]->appearanceData[i].hue);
			rIdx += 2;
		}
		job->outPreviewData[char_rawSlotIndex]->characterID = char_id;
		strcpy(job->outPreviewData[char_rawSlotIndex]->unicodeName, dbRow[1]);
		strcpy(job->outPreviewData[char_rawSlotIndex]->unicodeFamily, dbRow[2]);
		job->outPreviewData[char_rawSlotIndex]->genderIsMale = char_gender == 0;
		job->outPreviewData[char_rawSlotIndex]->raceID = char_race;
		job->outPreviewData[char_rawSlotIndex]->classID = char_classId;
		job->outPreviewData[char_rawSlotIndex]->slotIndex = char_slotIndex;
		job->outPreviewData[char_rawSlotIndex]->userID = job->userId;
		job->outPreviewData[char_rawSlotIndex]->currentContextId = char_currentContextId;
		job->outPreviewData[char_rawSlotIndex]->posX = (float)char_posX;
		job->outPreviewData[char_rawSlotIndex]->posY = (float)char_posY;
		job->outPreviewData[char_rawSlotIndex]->posZ = (float)char_posZ;
		sscanf(dbRow[53], "%d", &char_level);
		sscanf(dbRow[54], "%d", &char_numLogins);
		sscanf(dbRow[55], "%d", &char_totalTimePlayed);
		sscanf(dbRow[56], "%d", &char_timeSinceLastPlayed);
		sscanf(dbRow[57], "%d", &char_clonecredits);
		sscanf(dbRow[58], "%d", &char_body);
		sscanf(dbRow[59], "%d", &char_mind);
		sscanf(dbRow[60], "%d", &char_spirit);
		sscanf(dbRow[61], "%d", &char_experience);
		job->outPreviewData[char_rawSlotIndex]->level = char_level;
		job->outPreviewData[char_rawSlotIndex]->numLogins = char_numLogins;
		job->outPreviewData[char_rawSlotIndex]->totalTimePlayed = char_totalTimePlayed;
		job->outPreviewData[char_rawSlotIndex]->timeSinceLastPlayed = char_timeSinceLastPlayed;
		job->outPreviewData[char_rawSlotIndex]->clonecredits = char_clonecredits;
		job->outPreviewData[char_rawSlotIndex]->body = char_body;
		job->outPreviewData[char_rawSlotIndex]->mind = char_mind;
		job->outPreviewData[char_rawSlotIndex]->spirit = char_spirit;
		job->outPreviewData[char_rawSlotIndex]->experience = char_experience;
	}
	mysql_free_result(dbResult);
	// do callback
	((void(*)(void*, void*))cb)(param, job);
	// free data
	for (sint32 i = 0; i < 16; i++)
	{
		free(job->outPreviewData[i]);
	}
	DataInterface_freeJob(job);
}

void DataInterface_Character_getCharacterPreviewInfo(unsigned long long userID, uint32 slotIndex, void(*cb)(void *param, diJob_getCharacterPreviewInfo_t *jobData), void *param)
{
	diJob_getCharacterPreviewInfo_t *job = (diJob_getCharacterPreviewInfo_t*)DataInterface_allocJob(sizeof(diJob_getCharacterPreviewInfo_t));
	job->userId = userID;
	job->slotIndex = slotIndex;
	for (sint32 i = 0; i < 16; i++)
		job->outPreviewData[i] = NULL;
	DataInterface_queueJob(job, cb_DataInterface_Character_getCharacterPreviewInfo, cb, param);
}

void cb_DataInterface_Character_getCharacterData(MYSQL *dbCon, diJob_characterData_t *job, void *cb, void *param)
{
	sint8 queryText[1024];

	// get character data and stats
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
		"ad21_classId,ad21_hue,"
		"level,credits,prestige,experience,rotation,body,mind,spirit,logos,clonecredits"
		" FROM characters"
		" WHERE userId=%I64u AND slotId=%d LIMIT 1", job->userId, job->slotIndex);

	// execute query
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query cb_DataInterface_Character_getCharacterData\n");
		puts(queryText);
		puts(mysql_error(dbCon));
	}
	else
	{
		MYSQL_RES *dbResult = mysql_store_result(dbCon);
		MYSQL_ROW dbRow;
		job->outCharacterData = NULL;
		while ((dbRow = mysql_fetch_row(dbResult)))
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
			double char_rotation;
			sint32 char_level;
			uint32 char_credits;
			uint32 char_prestige;
			uint32 char_experience;
			uint32 char_body;
			uint32 char_mind;
			uint32 char_spirit;
			uint32 char_clonecredits;
			char char_logos[409];
			sscanf(dbRow[0], "%I64u", &char_id);
			sscanf(dbRow[3], "%d", &char_slotIndex);
			sscanf(dbRow[4], "%d", &char_gender);
			sscanf(dbRow[5], "%d", &char_race);
			sscanf(dbRow[6], "%d", &char_classId);
			sscanf(dbRow[7], "%d", &char_currentContextId);
			sscanf(dbRow[8], "%lf", &char_posX);
			sscanf(dbRow[9], "%lf", &char_posY);
			sscanf(dbRow[10], "%lf", &char_posZ);
			char_rawSlotIndex = char_slotIndex - 1;
			job->outCharacterData = (di_characterData_t*)malloc(sizeof(di_characterData_t));
			sint32 rIdx = 11;
			for (sint32 i = 0; i < 21; i++)
			{
				sscanf(dbRow[rIdx + 0], "%d", &job->outCharacterData->appearanceData[i].classId);
				sscanf(dbRow[rIdx + 1], "%u", &job->outCharacterData->appearanceData[i].hue);
				rIdx += 2;
			}
			job->outCharacterData->characterID = char_id;
			strcpy(job->outCharacterData->unicodeName, dbRow[1]);
			strcpy(job->outCharacterData->unicodeFamily, dbRow[2]);
			job->outCharacterData->genderIsMale = char_gender == 0;
			job->outCharacterData->raceID = char_race;
			job->outCharacterData->classID = char_classId;
			job->outCharacterData->slotIndex = char_slotIndex;
			job->outCharacterData->userID = job->userId;
			job->outCharacterData->currentContextId = char_currentContextId;
			job->outCharacterData->posX = (float)char_posX;
			job->outCharacterData->posY = (float)char_posY;
			job->outCharacterData->posZ = (float)char_posZ;
			sscanf(dbRow[53], "%d", &char_level);
			sscanf(dbRow[54], "%d", &char_credits);
			sscanf(dbRow[55], "%d", &char_prestige);
			sscanf(dbRow[56], "%d", &char_experience);
			sscanf(dbRow[57], "%lf", &char_rotation);
			sscanf(dbRow[58], "%d", &char_body);
			sscanf(dbRow[59], "%d", &char_mind);
			sscanf(dbRow[60], "%d", &char_spirit);
			sscanf(dbRow[61], "%s", &char_logos);
			sscanf(dbRow[62], "%d", &char_clonecredits);
			job->outCharacterData->level = char_level;
			job->outCharacterData->credits = char_credits;
			job->outCharacterData->prestige = char_prestige;
			job->outCharacterData->experience = char_experience;
			job->outCharacterData->rotation = (float)char_rotation;
			job->outCharacterData->body = char_body;
			job->outCharacterData->mind = char_mind;
			job->outCharacterData->spirit = char_spirit;
			job->outCharacterData->loginTime = time(NULL);
			job->outCharacterData->logos = (std::bitset<409>(char_logos));
			job->outCharacterData->clonecredits = char_clonecredits;
		}
		mysql_free_result(dbResult);
	}

	// get skills
	sprintf(queryText, "SELECT * FROM skills WHERE characterID=%I64u LIMIT 1", job->outCharacterData->characterID);

	// execute query
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
	}
	else
	{
		MYSQL_RES *dbResult = mysql_store_result(dbCon);
		MYSQL_ROW dbRow;
		while ((dbRow = mysql_fetch_row(dbResult)))
		{
			// get skill level
			for (sint32 i = 0; i < 73; i++)
				job->outCharacterData->skill[i] = atoi(dbRow[i]);
		}
		mysql_free_result(dbResult);
	}

	// get abilities
	sprintf(queryText, "SELECT * FROM abilities WHERE characterID=%I64u LIMIT 1", job->outCharacterData->characterID);

	// execute query
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
	}
	else
	{
		MYSQL_RES *dbResult = mysql_store_result(dbCon);
		MYSQL_ROW dbRow;
		while ((dbRow = mysql_fetch_row(dbResult)))
		{
			// get skill level
			for (sint32 i = 0; i < 50; i++)
			{
				job->outCharacterData->abilityDrawer[i / 2] = atoi(dbRow[i]);
				job->outCharacterData->abilityLvDrawer[i / 2] = atoi(dbRow[i + 1]);
				i++;
			}
			job->outCharacterData->currentAbilityDrawer = atoi(dbRow[50]);
		}
		mysql_free_result(dbResult);
	}

	// get mission data (TODO)
	job->outCharacterData->missionStateCount = 0;
	job->outCharacterData->missionStateData = NULL;
	// do callback
	((void(*)(void*, void*))cb)(param, job);
	// free data
	if (job->outCharacterData)
		free(job->outCharacterData);
	DataInterface_freeJob(job);
}

void DataInterface_Character_getCharacterData(unsigned long long userID, uint32 slotIndex, void(*cb)(void *param, diJob_characterData_t *jobData), void *param)
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
	if (mysql_query(dbCon, queryText))
	{
		characterData->error = true;
		printf("Error in query cb_DataInterface_Character_createCharacter\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void(*)(void*, void*))cb)(param, characterData);
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	if (mysql_num_rows(dbResult))
	{
		// name already in use
		characterData->error = true;
		characterData->error_nameAlreadyInUse = true;
		((void(*)(void*, void*))cb)(param, characterData);
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
		"`ad21_classId`,`ad21_hue`,`logos`"
		") VALUES"
		"('%s','%s',%d,%d,%d,%d,%I64u,"
		"%d,%f,%f,%f,0,"
		"%d,%u,%d,%u,%d,%u,%d,%u," // 1
		"%d,%u,%d,%u,%d,%u,%d,%u," // 5
		"%d,%u,%d,%u,%d,%u,%d,%u," // 9
		"%d,%u,%d,%u,%d,%u,%d,%u," // 13
		"%d,%u,%d,%u,%d,%u,%d,%u," // 17
		"%d,%u,'%s'"					   // 21
		")",
		characterData->unicodeName, characterData->unicodeFamily, characterData->slotIndex, characterData->genderIsMale ? 0 : 1, characterData->raceID, characterData->classId, characterData->userID,
		characterData->currentContextId, (float)characterData->posX, (float)characterData->posY, (float)characterData->posZ,
		characterData->appearanceData[0].classId, characterData->appearanceData[0].hue,
		characterData->appearanceData[1].classId, characterData->appearanceData[1].hue,
		characterData->appearanceData[2].classId, characterData->appearanceData[2].hue,
		characterData->appearanceData[3].classId, characterData->appearanceData[3].hue,
		characterData->appearanceData[4].classId, characterData->appearanceData[4].hue,
		characterData->appearanceData[5].classId, characterData->appearanceData[5].hue,
		characterData->appearanceData[6].classId, characterData->appearanceData[6].hue,
		characterData->appearanceData[7].classId, characterData->appearanceData[7].hue,
		characterData->appearanceData[8].classId, characterData->appearanceData[8].hue,
		characterData->appearanceData[9].classId, characterData->appearanceData[9].hue,
		characterData->appearanceData[10].classId, characterData->appearanceData[10].hue,
		characterData->appearanceData[11].classId, characterData->appearanceData[11].hue,
		characterData->appearanceData[12].classId, characterData->appearanceData[12].hue,
		characterData->appearanceData[13].classId, characterData->appearanceData[13].hue,
		characterData->appearanceData[14].classId, characterData->appearanceData[14].hue,
		characterData->appearanceData[15].classId, characterData->appearanceData[15].hue,
		characterData->appearanceData[16].classId, characterData->appearanceData[16].hue,
		characterData->appearanceData[17].classId, characterData->appearanceData[17].hue,
		characterData->appearanceData[18].classId, characterData->appearanceData[18].hue,
		characterData->appearanceData[19].classId, characterData->appearanceData[19].hue,
		characterData->appearanceData[20].classId, characterData->appearanceData[20].hue,
		std::bitset<409>().reset().to_string().c_str()
		);
	// execute query
	if (mysql_query(dbCon, queryText))
	{
		characterData->error = true;
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void(*)(void*, void*))cb)(param, characterData);
		return;
	}
	// create default entry in inventory table
	my_ulonglong characterID = mysql_insert_id(dbCon);
	sprintf(queryText, "INSERT INTO inventory "
		"(slot50item, slot50qty, slot250item, slot250qty, slot251item, slot251qty, slot252item, slot252qty, slot253item, slot253qty, slot265item, slot265qty, slot266item, slot266qty, slot267item, slot267qty, characterID) VALUES"
		"(28, 100, 17131, 1, 13126, 1, 13066, 1, 13096, 1, 13186, 1, 13156, 1, 17131, 1, %I64u)", characterID);
	// execute query
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query inventory\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void(*)(void*, void*))cb)(param, characterData);
		return;
	}
	// create default entry in skills table
	sprintf(queryText, "INSERT INTO skills (characterID) VALUES(%I64u)", characterID);
	// execute query
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query skills\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void(*)(void*, void*))cb)(param, characterData);
		return;
	}
	// create default entry in abilities table
	sprintf(queryText, "INSERT INTO abilities (characterID) VALUES(%I64u)", characterID);
	// execute query
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query abilities\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		((void(*)(void*, void*))cb)(param, characterData);
		return;
	}
	// do callback
	((void(*)(void*, void*))cb)(param, characterData);
	// dont free data as we do not allocate it here
}

void DataInterface_Character_createCharacter(di_characterLayout_t *characterData, void(*cb)(void *param, di_characterLayout_t *jobData), void *param)
{
	DataInterface_queueJob(characterData, cb_DataInterface_Character_createCharacter, cb, param);
}

void cb_DataInterface_Character_deleteCharacter(MYSQL *dbCon, diJob_deleteCharacter_t *job, void *cb, void *param)
{
	sint8 queryText[512];
	wsprintf(queryText, "DELETE characters, inventory, skills, abilities FROM characters INNER JOIN inventory ON characters.id = inventory.characterID INNER JOIN skills ON characters.id = skills.characterID INNER JOIN abilities ON characters.id = abilities.characterID WHERE userId=%I64u AND slotId=%d", job->userId, job->slotId);
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query 'DELETE character'\n");
		job->error = true;
	}
	else
	{
		if (mysql_affected_rows(dbCon) == 0) // nothing deleted, set error
			job->error = true;
	}
	// do callback
	((void(*)(void*, void*))cb)(param, job);
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_deleteCharacter(unsigned long long userID, sint32 slotId, void(*cb)(void *param, diJob_deleteCharacter_t *jobData), void *param)
{
	diJob_deleteCharacter_t *job = (diJob_deleteCharacter_t*)DataInterface_allocJob(sizeof(diJob_deleteCharacter_t));
	job->userId = userID;
	job->slotId = slotId;
	DataInterface_queueJob(job, cb_DataInterface_Character_deleteCharacter, cb, param);
}

void cb_DataInterface_Character_updateCharacter(MYSQL *dbCon, diJob_updateCharacter_t *job)
{
	sint8 queryText[1024];
	switch (job->stat)
	{
	case 1:
		sprintf(queryText, "UPDATE characters SET level=%d WHERE userId=%I64u AND slotId=%d", job->value, job->userId, job->slotId);
		break;
	case 2:
		sprintf(queryText, "UPDATE characters SET credits=%d WHERE userId=%I64u AND slotId=%d", job->value, job->userId, job->slotId);
		break;
	case 3:
		sprintf(queryText, "UPDATE characters SET prestige=%d WHERE userId=%I64u AND slotId=%d", job->value, job->userId, job->slotId);
		break;
	case 4:
		sprintf(queryText, "UPDATE characters SET experience=%d WHERE userId=%I64u AND slotId=%d", job->value, job->userId, job->slotId);
		break;
	case 5:
		sprintf(queryText, "UPDATE characters SET posX=%f, posY=%f, posZ=%f, rotation=%f, currentContextId =%d WHERE userId=%I64u AND slotId=%d", job->posX, job->posY, job->posZ, job->rotation, job->currentContextId, job->userId, job->slotId);
		break;
	case 6:
		sprintf(queryText, "UPDATE characters SET body=%d, mind=%d, spirit=%d WHERE userId=%I64u AND slotId=%d", job->body, job->mind, job->spirit, job->userId, job->slotId);
		break;
	case 7:
		sprintf(queryText, "UPDATE characters SET numLogins=numLogins+1, totalTimePlayed=totalTimePlayed+%d, timeSinceLastPlayed=NOW() WHERE userId=%I64u AND slotId=%d", job->value, job->userId, job->slotId);
		break;
	case 8:
		sprintf(queryText, "UPDATE characters SET logos='%s' WHERE userId=%I64u AND slotId=%d", job->logos, job->userId, job->slotId);
		break;
	case 9:
		sprintf(queryText, "UPDATE characters SET classID=%d WHERE userId=%I64u AND slotId=%d", job->value, job->userId, job->slotId);
		break;
	}
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query 'updateCharacter'\n");
		job->error = true;
	}
	else
	{
		if (mysql_affected_rows(dbCon) == 0) // nothing updated, set error
			job->error = true;
	}
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_updateCharacter(unsigned long long userID, sint32 slotId, uint32 stat, uint32 value)
{
	diJob_updateCharacter_t *job = (diJob_updateCharacter_t*)DataInterface_allocJob(sizeof(diJob_updateCharacter_t));
	job->userId = userID;
	job->slotId = slotId;
	job->stat = stat;
	job->value = value;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacter, NULL, NULL);
}

void DataInterface_Character_updateCharacter(unsigned long long userID, sint32 slotId, uint32 stat, float posX, float posY, float posZ, float rotation, sint32 contextId)
{
	diJob_updateCharacter_t *job = (diJob_updateCharacter_t*)DataInterface_allocJob(sizeof(diJob_updateCharacter_t));
	job->userId = userID;
	job->slotId = slotId;
	job->stat = stat;
	job->posX = posX;
	job->posY = posY;
	job->posZ = posZ;
	job->rotation = rotation;
	job->currentContextId = contextId;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacter, NULL, NULL);
}

void DataInterface_Character_updateCharacter(unsigned long long userID, sint32 slotId, uint32 stat, uint32 body, uint32 mind, uint32 spirit)
{
	diJob_updateCharacter_t *job = (diJob_updateCharacter_t*)DataInterface_allocJob(sizeof(diJob_updateCharacter_t));
	job->userId = userID;
	job->slotId = slotId;
	job->stat = stat;
	job->body = body;
	job->mind = mind;
	job->spirit = spirit;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacter, NULL, NULL);
}

void DataInterface_Character_updateCharacter(unsigned long long userID, sint32 slotId, uint32 stat, const char* logos)
{
	diJob_updateCharacter_t *job = (diJob_updateCharacter_t*)DataInterface_allocJob(sizeof(diJob_updateCharacter_t));
	job->userId = userID;
	job->slotId = slotId;
	job->stat = stat;
	strcpy(job->logos, logos);
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacter, NULL, NULL);
}

void cb_DataInterface_Character_getCharacterInventory(MYSQL *dbCon, diJob_getCharacterInventory_t *job, void(*cb)(void *param, diJob_getCharacterInventory_t *jobData), void *param)
{
	sint8 queryText[128];
	sprintf(queryText, "SELECT * FROM inventory WHERE characterID=%I64u LIMIT 1", job->characterID);
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query 'getCharacterInventory'\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		job->error = true;
	}
	else
	{
		MYSQL_RES *dbResult = mysql_store_result(dbCon);
		MYSQL_ROW dbRow;
		while ((dbRow = mysql_fetch_row(dbResult)))
		{
			// get item templateID and quantity
			for (sint32 i = 0; i < 544; i++)
			{
				*(job->item + (i / 2)) = atoi(dbRow[i]);
				*(job->qty + (i / 2)) = atoi(dbRow[i + 1]);
				i++;
			}
			// get ammo counts for weapon drawer
			for (sint32 i = 544; i < 549; i++)
			{
				*(job->qty + (i - 272)) = atoi(dbRow[i]);
				i++;
			}
		}
		mysql_free_result(dbResult);
	}
	// do callback
	((void(*)(void*, void*))cb)(param, job);
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_getCharacterInventory(unsigned long long characterID, uint32 * item, uint32 * qty, void(*cb)(void *param, diJob_getCharacterInventory_t *jobData), void *param)
{
	diJob_getCharacterInventory_t *job = (diJob_getCharacterInventory_t*)DataInterface_allocJob(sizeof(diJob_getCharacterInventory_t));
	job->characterID = characterID;
	job->item = item;
	job->qty = qty;
	DataInterface_queueJob(job, cb_DataInterface_Character_getCharacterInventory, cb, param);
}

void cb_DataInterface_Character_updateCharacterInventory(MYSQL *dbCon, diJob_updateCharacterInventory_t *job)
{
	sint8 queryText[128];
	sprintf(queryText, "UPDATE inventory SET slot%ditem=%d, slot%dqty=%d WHERE characterID=%I64u", job->slotIndex, job->templateId, job->slotIndex, job->qty, job->characterID);
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query 'cb_getCharacterInventory'\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		job->error = true;
	}
	else
	{
		if (mysql_affected_rows(dbCon) == 0) // nothing updated, set error
			job->error = true;
	}
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_updateCharacterInventory(unsigned long long characterID, sint32 slotIndex, uint32 templateId, uint32 qty)
{
	diJob_updateCharacterInventory_t *job = (diJob_updateCharacterInventory_t*)DataInterface_allocJob(sizeof(diJob_updateCharacterInventory_t));
	job->characterID = characterID;
	job->slotIndex = slotIndex;
	job->templateId = templateId;
	job->qty = qty;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacterInventory, NULL, NULL);
}

void cb_DataInterface_Character_updateCharacterAppearance(MYSQL *dbCon, diJob_updateCharacterAppearance_t *job)
{
	sint8 queryText[128];
	sprintf(queryText, "UPDATE characters SET ad%u_classId=%u, ad%u_hue=%u WHERE id=%I64u", job->index, job->classId, job->index, job->hue, job->characterID);
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query 'cb_updateCharacterAppearance'\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		job->error = true;
	}
	else
	{
		if (mysql_affected_rows(dbCon) == 0) // nothing updated, set error
			job->error = true;
	}
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_updateCharacterAppearance(unsigned long long characterID, uint32 index, uint32 classId, uint32 hue)
{
	diJob_updateCharacterAppearance_t *job = (diJob_updateCharacterAppearance_t*)DataInterface_allocJob(sizeof(diJob_updateCharacterAppearance_t));
	job->characterID = characterID;
	job->index = index;
	job->classId = classId;
	job->hue = hue;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacterAppearance, NULL, NULL);
}

void cb_DataInterface_Character_updateCharacterAmmo(MYSQL *dbCon, diJob_updateCharacterAmmo_t *job)
{
	sint8 queryText[128];
	sprintf(queryText, "UPDATE inventory SET slot%dammo=%d WHERE characterID=%I64u", job->slotIndex, job->ammo, job->characterID);
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query inventory\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		job->error = true;
	}
	else
	{
		if (mysql_affected_rows(dbCon) == 0) // nothing updated, set error
			job->error = true;
	}
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_updateCharacterAmmo(unsigned long long characterID, sint32 slotIndex, sint32 ammo)
{
	diJob_updateCharacterAmmo_t *job = (diJob_updateCharacterAmmo_t*)DataInterface_allocJob(sizeof(diJob_updateCharacterAmmo_t));
	job->characterID = characterID;
	job->slotIndex = slotIndex;
	job->ammo = ammo;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacterAmmo, NULL, NULL);
}

void cb_DataInterface_Character_updateCharacterSkills(MYSQL *dbCon, diJob_updateCharacterSkills_t *job)
{
	sint8 queryText[2048];
	sprintf(queryText, "UPDATE skills SET ");
	for (sint32 i = 0; i < 73; i++)
	{
		sprintf(queryText, "%s skill%d=%d,", queryText, i, *(job->level+i));
	}
	queryText[strlen(queryText) - 1] = '\0';
	sprintf(queryText, "%s WHERE characterID=%I64u;", queryText, job->characterID);
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		job->error = true;
	}
	else
	{
		if (mysql_affected_rows(dbCon) == 0) // nothing updated, set error
			job->error = true;
	}
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_updateCharacterSkills(unsigned long long characterID, uint8 * level)
{
	diJob_updateCharacterSkills_t *job = (diJob_updateCharacterSkills_t*)DataInterface_allocJob(sizeof(diJob_updateCharacterSkills_t));
	job->characterID = characterID;
	job->level = level;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacterSkills, NULL, NULL);
}

void cb_DataInterface_Character_updateCharacterAbility(MYSQL *dbCon, diJob_updateCharacterAbility_t *job)
{
	sint8 queryText[128];
	if (job->drawer != NULL)
		sprintf(queryText, "UPDATE abilities SET currentDrawer=%d WHERE characterID=%I64u", job->drawer, job->characterID);
	else
		sprintf(queryText, "UPDATE abilities SET ability%ditem=%d, ability%dlevel=%d WHERE characterID=%I64u", job->index, job->item, job->index, job->level, job->characterID);
	// execute query
	job->error = false;
	if (mysql_query(dbCon, queryText))
	{
		printf("Error in query\n");
		puts(queryText);
		puts(mysql_error(dbCon));
		job->error = true;
	}
	else
	{
		if (mysql_affected_rows(dbCon) == 0) // nothing updated, set error
			job->error = true;
	}
	// free data
	DataInterface_freeJob(job);
}

void DataInterface_Character_updateCharacterAbility(unsigned long long characterID, sint32 index, sint32 item, sint32 level, sint32 drawer)
{
	diJob_updateCharacterAbility_t *job = (diJob_updateCharacterAbility_t*)DataInterface_allocJob(sizeof(diJob_updateCharacterAbility_t));
	job->characterID = characterID;
	job->index = index;
	job->item = item;
	job->level = level;
	job->drawer = drawer;
	DataInterface_queueJob(job, cb_DataInterface_Character_updateCharacterAbility, NULL, NULL);
}


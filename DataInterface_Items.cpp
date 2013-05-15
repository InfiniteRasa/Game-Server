#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"


void cb_DataInterface_Item_getItemTemplates(MYSQL *dbCon, diJob_itemTemplate_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"itemTemplateId,classId,"
		"qualityId,`type`,hasSellableFlag,notTradeableFlag,hasCharacterUniqueFlag,hasAccountUniqueFlag,"
		"hasBoEFlag,boundToCharacterFlag,notPlaceableInLockBoxFlag,inventoryCategory,reqLevel"
		" FROM itemTemplate");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
	// allocate memory for item templates
	job->numberOfItemTemplates = rowCount;
	if( job->numberOfItemTemplates > 0 )
	{
		job->itemTemplateList = (diJob_itemTemplateEntry_t*)malloc(job->numberOfItemTemplates * sizeof(diJob_itemTemplateEntry_t));
		memset(job->itemTemplateList, 0, job->numberOfItemTemplates * sizeof(diJob_itemTemplateEntry_t));
	}
	// parse rows
	sint32 i = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		// "id,itemTemplateId,classId,"
		// "qualityId,`type`,hasSellableFlag,notTradeableFlag,hasCharacterUniqueFlag,hasAccountUniqueFlag,"
		// "hasAccountUniqueFlag,hasBoEFlag,boundToCharacterFlag,notPlaceableInLockBoxFlag,inventoryCategory"
		sint32 idx = 0;
		sint32 tmpValue = 0;
		// itemTemplateId
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].itemTemplateId = tmpValue;
		// classId
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].classId = tmpValue;
		// qualityId
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].qualityId = tmpValue;
		// type
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].type = tmpValue;
		// hasSellableFlag
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].hasSellableFlag = (tmpValue!=0);
		// notTradeableFlag
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].notTradeableFlag = (tmpValue!=0);
		// hasCharacterUniqueFlag
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].hasCharacterUniqueFlag = (tmpValue!=0);
		// hasAccountUniqueFlag
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].hasAccountUniqueFlag = (tmpValue!=0);
		// hasBoEFlag
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].hasBoEFlag = (tmpValue!=0);
		// boundToCharacterFlag
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].boundToCharacterFlag = (tmpValue!=0);
		// notPlaceableInLockBoxFlag
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].notPlaceableInLockBoxFlag = (tmpValue!=0);
		// inventoryCategory
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateList[i].inventoryCategory = tmpValue;
		// reqLevel
		if( dbRow[idx] )
		{
			sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		}
		else
		{
			tmpValue = 0;
			idx++;
		}
		job->itemTemplateList[i].reqLevel = tmpValue;
		// next
		i++;
	}
	// do callback param1: mapchannel param2: list of itemtemplate data
	((void (*)(void*,void*))cb)(param, job);
	mysql_free_result(dbResult);
	if( job->numberOfItemTemplates > 0 )
		free(job->itemTemplateList);
	DataInterface_freeJob(job);
}

/*
 * Reads data from the itemtemplate table
 */
void DataInterface_Item_getItemTemplates(void (*cb)(void *param, diJob_itemTemplate_t *jobData), void *param)
{	
	diJob_itemTemplate_t *job = (diJob_itemTemplate_t*)DataInterface_allocJob(sizeof(diJob_itemTemplate_t));
	DataInterface_queueJob(job, cb_DataInterface_Item_getItemTemplates, cb, param);
}

/*
 * Async callback for receiving equipment data from mysql
 */
void cb_DataInterface_Item_getEquipmentData(MYSQL *dbCon, diJob_itemTemplateEquipment_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"itemTemplateId,slotType,requiredSkillId,requiredSkillMinVal"
		" FROM itemTemplate_equipment");
	// todo: Maybe add appearance hue to table?
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
	// allocate memory for item templates
	job->numberOfTemplates = rowCount;
	if( job->numberOfTemplates > 0 )
	{
		job->itemTemplateEquipmentList = (diJob_itemTemplateEquipmentEntry_t*)malloc(job->numberOfTemplates * sizeof(diJob_itemTemplateEquipmentEntry_t));
		memset(job->itemTemplateEquipmentList, 0, job->numberOfTemplates * sizeof(diJob_itemTemplateEquipmentEntry_t));
	}
	// parse rows
	sint32 i = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		// "itemTemplateId,slotType,requiredSkillId,requiredSkillMinVal"
		sint32 idx = 0;
		sint32 tmpValue = 0;
		// itemTemplateId
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateEquipmentList[i].itemTemplateId = tmpValue;
		// slotType
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateEquipmentList[i].slotType = tmpValue;
		// required skill id (optional)
		if( dbRow[idx] == NULL )
		{
			job->itemTemplateEquipmentList[i].skillId = -1;
			idx++;
		}
		else
		{
			sscanf(dbRow[idx], "%d", &tmpValue); idx++;
			job->itemTemplateEquipmentList[i].skillId = tmpValue;
		}
		// required skill level (optional)
		if( dbRow[idx] == NULL )
		{
			job->itemTemplateEquipmentList[i].skillId = -1;
			idx++;
		}
		else
		{
			sscanf(dbRow[idx], "%d", &tmpValue); idx++;
			job->itemTemplateEquipmentList[i].skillMinVal = tmpValue;
		}
		// next
		i++;
	}
	// do callback param1: mapchannel param2: list of item equipment data
	((void (*)(void*,void*))cb)(param, job);
	mysql_free_result(dbResult);
	if( job->numberOfTemplates > 0 )
		free(job->itemTemplateEquipmentList);
	DataInterface_freeJob(job);
}

/*
 * Reads data from the itemtemplate_equipment table
 */
void DataInterface_Item_getItemEquipmentData(void (*cb)(void *param, diJob_itemTemplateEquipment_t *jobData), void *param)
{	
	diJob_itemTemplateEquipment_t *job = (diJob_itemTemplateEquipment_t*)DataInterface_allocJob(sizeof(diJob_itemTemplateEquipment_t));
	DataInterface_queueJob(job, cb_DataInterface_Item_getEquipmentData, cb, param);
}

/*
 * Async callback for receiving armor data from mysql
 */
void cb_DataInterface_Item_getArmorData(MYSQL *dbCon, diJob_itemTemplateArmor_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"itemTemplateId,armorValue,regenRate,damageAbsorbed"
		" FROM itemtemplate_armor");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
	// allocate memory for item templates
	job->numberOfTemplates = rowCount;
	if( job->numberOfTemplates > 0 )
	{
		job->itemTemplateArmorList = (diJob_itemTemplateArmorEntry_t*)malloc(job->numberOfTemplates * sizeof(diJob_itemTemplateArmorEntry_t));
		memset(job->itemTemplateArmorList, 0, job->numberOfTemplates * sizeof(diJob_itemTemplateArmorEntry_t));
	}
	// parse rows
	sint32 i = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		// "itemTemplateId,armorValue,regenRate,damageAbsorbed"
		sint32 idx = 0;
		sint32 tmpValue = 0;
		// itemTemplateId
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateArmorList[i].itemTemplateId = tmpValue;
		// armorValue
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateArmorList[i].armorValue = tmpValue;
		// regenRate
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateArmorList[i].regenRate = tmpValue;
		// damageAbsorbed
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateArmorList[i].damageAbsorbed = tmpValue;
		// next
		i++;
	}
	// do callback param1: mapchannel param2: list of item armor data
	((void (*)(void*,void*))cb)(param, job);
	mysql_free_result(dbResult);
	if( job->numberOfTemplates > 0 )
		free(job->itemTemplateArmorList);
	DataInterface_freeJob(job);
}

/*
 * Reads data from the itemtemplate_armor table
 */
void DataInterface_Item_getItemArmorData(void (*cb)(void *param, diJob_itemTemplateArmor_t *jobData), void *param)
{	
	diJob_itemTemplateArmor_t *job = (diJob_itemTemplateArmor_t*)DataInterface_allocJob(sizeof(diJob_itemTemplateArmor_t));
	DataInterface_queueJob(job, cb_DataInterface_Item_getArmorData, cb, param);
}


/*
 * Async callback for receiving weapon data from mysql
 */
void cb_DataInterface_Item_getWeaponData(MYSQL *dbCon, diJob_itemTemplateWeapon_t *job, void *cb, void *param)
{
	sint8 queryText[1024];
	sprintf(queryText, "SELECT "
		"itemTemplateId,clipSize,currentAmmo,aimRate,reloadTime,altActionId,altActionArg,"
		"aeType,aeRadius,recoilAmount,reuseOverride,coolRate,heatPerShot,toolType,ammoPerShot,"
		"minDamage,maxDamage,ammoClassId,damageType,windupTime,recoveryTime,refireTime,`range`,altMaxDamage,"
		"altDamageType,altRange,altAERadius,altAEType,attackType"
		" FROM itemtemplate_weapon");
	// execute query
	if( mysql_query(dbCon, queryText) )
	{
		printf("Error in query\n");
		while(1) Sleep(1000);	
	}
	MYSQL_RES *dbResult = mysql_store_result(dbCon);
	MYSQL_ROW dbRow;
	sint32 rowCount = (sint32)mysql_num_rows(dbResult);
	// allocate memory for item templates
	job->numberOfTemplates = rowCount;
	if( job->numberOfTemplates > 0 )
	{
		job->itemTemplateWeaponList = (diJob_itemTemplateWeaponEntry_t*)malloc(job->numberOfTemplates * sizeof(diJob_itemTemplateWeaponEntry_t));
		memset(job->itemTemplateWeaponList, 0, job->numberOfTemplates * sizeof(diJob_itemTemplateWeaponEntry_t));
	}
	// parse rows
	sint32 i = 0;
	while((dbRow = mysql_fetch_row(dbResult)))
	{
		// "itemTemplateId,clipSize,currentAmmo,aimRate,reloadTime,altActionId,altActionArg,"
		// "aeType,aeRadius,recoilAmount,reuseOverride,coolRate,heatPerShot,toolType,ammoPerShot,"
		// "minDamage,maxDamage,ammoClassId,damageType,windupTime,recoveryTime,refireTime,range,altMaxDamage,"
		// "altDamageType,altRange,altAERadius,altAEType,attackType"
		sint32 idx = 0;
		sint32 tmpValue = 0;
		float tmpValueF = 0.0f;
		// itemTemplateId
		sscanf(dbRow[idx], "%d", &tmpValue); idx++;
		job->itemTemplateWeaponList[i].itemTemplateId = tmpValue;
		// clipSize
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].clipSize = tmpValue;
		// currentAmmo
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].currentAmmo = tmpValue;
		// aimRate
		if( dbRow[idx] ) sscanf(dbRow[idx], "%f", &tmpValueF); else tmpValueF = 0.0f; idx++;
		job->itemTemplateWeaponList[i].aimRate = tmpValueF;
		// reloadTime
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].reloadTime = tmpValue;
		// altActionId
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].altActionId = tmpValue;
		// altActionArg
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].altActionArg = tmpValue;
		// aeType
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].aeType = tmpValue;
		// aeRadius
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].aeRadius = tmpValue;
		// recoilAmount
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].recoilAmount = tmpValue;
		// reuseOverride
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].reuseOverride = tmpValue;
		// coolRate
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].coolRate = tmpValue;
		// heatPerShot
		if( dbRow[idx] ) sscanf(dbRow[idx], "%f", &tmpValueF); else tmpValueF = 0.0f; idx++;
		job->itemTemplateWeaponList[i].heatPerShot = tmpValueF;
		// toolType
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].toolType = tmpValue;
		// ammoPerShot
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].ammoPerShot = tmpValue;
		// minDamage
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].minDamage = tmpValue;
		// maxDamage
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].maxDamage = tmpValue;
		// ammoClassId
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].ammoClassId = tmpValue;
		// damageType
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].damageType = tmpValue;
		// windupTime
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].windupTime = tmpValue;
		// recoveryTime
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].recoveryTime = tmpValue;
		// refireTime
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].refireTime = tmpValue;
		// range
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].range = tmpValue;
		// altMaxDamage
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].altMaxDamage = tmpValue;
		// altDamageType
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].altDamageType = tmpValue;
		// altRange
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].altRange = tmpValue;
		// altAERadius
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].altAERadius = tmpValue;
		// altAEType
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].altAEType = tmpValue;
		// attackType
		if( dbRow[idx] ) sscanf(dbRow[idx], "%d", &tmpValue); else tmpValue = 0; idx++;
		job->itemTemplateWeaponList[i].attackType = tmpValue;
		// next
		i++;
	}
	// do callback param1: mapchannel param2: list of item weapon data
	((void (*)(void*,void*))cb)(param, job);
	mysql_free_result(dbResult);
	if( job->numberOfTemplates > 0 )
		free(job->itemTemplateWeaponList);
	DataInterface_freeJob(job);
}

/*
 * Reads data from the itemtemplate_weapon table
 */
void DataInterface_Item_getItemWeaponData(void (*cb)(void *param, diJob_itemTemplateWeapon_t *jobData), void *param)
{	
	diJob_itemTemplateWeapon_t *job = (diJob_itemTemplateWeapon_t*)DataInterface_allocJob(sizeof(diJob_itemTemplateWeapon_t));
	DataInterface_queueJob(job, cb_DataInterface_Item_getWeaponData, cb, param);
}
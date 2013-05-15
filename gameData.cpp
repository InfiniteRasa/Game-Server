#include "Global.h"

//equipableClassEquipmentSlot.txt

hashTable_t ht_equipmentClassSlots;
hashTable_t ht_starterItemTemplateClassIds;


void _gameData_loadEquipmentClassSlots()
{
	hashTable_init(&ht_equipmentClassSlots, 128);
	file_t *file = fileMgr_open((sint8*)"gameData\\equipableClassEquipmentSlot.txt");
	if( file == NULL )
	{
		printf("Failed to load \"gameData\\equipableClassEquipmentSlot.txt\"\nServer halted\n");
		while( true ) ;
	}
	sint8 *line;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			sint32 classId;
			sint32 slotId;
			if( sscanf((char*)line, "(%d, %d)", &classId, &slotId) != 2 )
			{
				__debugbreak();
			}
			hashTable_set(&ht_equipmentClassSlots, classId, (void*)slotId);
		}
		free(line);
	}
	fileMgr_close(file);
}

void _gameData_loadStarterItemTemplateClassIds()
{
	hashTable_init(&ht_starterItemTemplateClassIds, 128);
	file_t *file = fileMgr_open((sint8*)"gameData\\starterItemTemplateClassIds.txt");
	if( file == NULL )
	{
		printf("Failed to load \"gameData\\starterItemTemplateClassIds.txt\"\nServer halted\n");
		while( true ) ;
	}
	sint8 *line;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			sint32 templateId;
			sint32 classId;
			if( sscanf((char*)line, "(%d, %d)", &templateId, &classId) != 2 )
			{
				__debugbreak();
			}
			hashTable_set(&ht_starterItemTemplateClassIds, templateId, (void*)classId);
		}
		free(line);
	}
	fileMgr_close(file);
}

sint32 mapInfoCount;
gameData_mapInfo_t *mapInfoArray;

void _gameData_loadMapInfo()
{
	// read number of maps
	mapInfoCount = 0;
	file_t *file = fileMgr_open((sint8*)"gameData\\mapInfo.txt");
	if( file == NULL )
	{
		printf("Failed to load \"gameData\\mapInfo.txt\"\nServer halted\n");
		while( true ) ;
	}
	sint8 *line;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			sint32 contextId;
			sint8 mapName[256];
			sint32 version;
			sint32 baseRegionId;
			if( sscanf((char*)line, "(%d, '%[^']', %d, %d)", &contextId, mapName, &version, &baseRegionId) == 4 )
				mapInfoCount++;
		}
		free(line);
	}
	fileMgr_close(file);
	// allocate maps
	mapInfoArray = (gameData_mapInfo_t*)malloc(sizeof(gameData_mapInfo_t)*mapInfoCount);
	// #(contextId, 'mapName', version, baseRegionId)
	file = fileMgr_open((sint8*)"gameData\\mapInfo.txt");
	sint32 i=0;
	while( line = fileMgr_readLine(file) )
	{
		if( line[0] == '(' )
		{
			sint32 contextId;
			sint8 mapName[256];
			sint32 version;
			sint32 baseRegionId;
			if( sscanf((char*)line, "(%d, '%[^']', %d, %d)", &contextId, mapName, &version, &baseRegionId) == 4 )
			{
				mapInfoArray[i].contextId = contextId;
				strcpy((char*)mapInfoArray[i].name, (char*)mapName);
				mapInfoArray[i].version = version;
				mapInfoArray[i].baseRegionId = baseRegionId;
				i++;
			}
		}
		free(line);
	}
	fileMgr_close(file);
}


sint32 gameData_getEquipmentClassIdSlot(sint32 classId)
{
	return (sint32)hashTable_get(&ht_equipmentClassSlots, classId);	
}

sint32 gameData_getStarterItemTemplateClassId(sint32 templateId)
{
	return (sint32)hashTable_get(&ht_starterItemTemplateClassIds, templateId);	
}

hashTable_t ht_itemTemplateId;
// hashTable_t ht_itemTemplateByName;

itemTemplate_t *gameData_getItemTemplateById(uint32 templateId)
{
	return (itemTemplate_t*)hashTable_get(&ht_itemTemplateId, templateId);	
}

//itemTemplate_t *gameData_getItemTemplateByName(sint8 *name)
//{
//	return (itemTemplate_t*)hashTable_get(&ht_itemTemplateByName, name);	
//}

bool _itemTemplatesLoaded = false;

/*
 * Called when the mysql server returns the item templates
 */
void _item_loadItemTemplates(void *param, diJob_itemTemplate_t *jobData)
{
	printf("Loading %d item templates from db...\n", jobData->numberOfItemTemplates);
	for(sint32 i=0; i<jobData->numberOfItemTemplates; i++)
	{
		itemTemplate_t* itemTemplate = (itemTemplate_t*)malloc(sizeof(itemTemplate_t));
		memset(itemTemplate, 0x00, sizeof(itemTemplate_t));
		itemTemplate->item.templateId = jobData->itemTemplateList[i].itemTemplateId;
		itemTemplate->item.classId = jobData->itemTemplateList[i].classId;
		itemTemplate->item.qualityId = jobData->itemTemplateList[i].qualityId;
		itemTemplate->item.type = jobData->itemTemplateList[i].type;
		itemTemplate->item.notTradable = jobData->itemTemplateList[i].notTradeableFlag;
		itemTemplate->item.hasSellableFlag = jobData->itemTemplateList[i].hasSellableFlag;
		itemTemplate->item.hasCharacterUniqueFlag = jobData->itemTemplateList[i].hasCharacterUniqueFlag;
		itemTemplate->item.hasAccountUniqueFlag = jobData->itemTemplateList[i].hasAccountUniqueFlag;
		itemTemplate->item.hasBoEFlag = jobData->itemTemplateList[i].hasBoEFlag;
		itemTemplate->item.boundToCharacter = jobData->itemTemplateList[i].boundToCharacterFlag;
		itemTemplate->item.notPlaceableInLockbox = jobData->itemTemplateList[i].notPlaceableInLockBoxFlag;
		itemTemplate->item.inventoryCategory = jobData->itemTemplateList[i].inventoryCategory;
		itemTemplate->item.reqLevel = jobData->itemTemplateList[i].reqLevel;
		// validate some of the values
		if( itemTemplate->item.inventoryCategory <= 0 )
		{
			printf("Warning: itemTemplate %d has invalid inventory category\n", jobData->itemTemplateList[i].itemTemplateId);
			// manually fix the inventory category
			itemTemplate->item.inventoryCategory = INVENTORY_CATEGORY_MISC;
		}
		// default values
		itemTemplate->item.currentHitPoints = 100;
		itemTemplate->item.maxHitPoints = 100;
		// reset equipment data
		itemTemplate->equipment.equiptmentSlotType = -1;
		itemTemplate->equipment.requiredSkillId = -1;
		itemTemplate->equipment.requiredSkillMinVal = -1;
		// create entry
		hashTable_set(&ht_itemTemplateId, jobData->itemTemplateList[i].itemTemplateId, itemTemplate);
	}
	// mark item templates as loaded
	_itemTemplatesLoaded = true;
}

/*
 * Called when the mysql server returns the item equipment data
 * The equipment data is additional info for equitable items
 * Note: The base item templates must already be loaded
 */
void _item_loadItemTemplatesEquipment(void *param, diJob_itemTemplateEquipment_t *jobData)
{
	printf("Loading %d equipment templates from db...\n", jobData->numberOfTemplates);
	for(sint32 i=0; i<jobData->numberOfTemplates; i++)
	{

		// find the already existing template
		itemTemplate_t* itemTemplate = (itemTemplate_t*)hashTable_get(&ht_itemTemplateId, jobData->itemTemplateEquipmentList[i].itemTemplateId);
		if( itemTemplate == NULL )
			continue; // equipment template has no item template
		// load additional equipment info
		if( itemTemplate->item.type != ITEMTYPE_WEAPON && itemTemplate->item.type != ITEMTYPE_ARMOR )
		{
			printf("Warning: itemTemplate %d has equipment data but is not a weapon/armor\n", jobData->itemTemplateEquipmentList[i].itemTemplateId);
		}
		itemTemplate->equipment.equiptmentSlotType = jobData->itemTemplateEquipmentList[i].slotType;
		itemTemplate->equipment.requiredSkillId = jobData->itemTemplateEquipmentList[i].skillId;
		itemTemplate->equipment.requiredSkillMinVal = jobData->itemTemplateEquipmentList[i].skillMinVal;
	}
	// mark item templates as loaded
	_itemTemplatesLoaded = true;
}


/*
 * Called when the mysql server returns the item armor data
 * The armor data is additional info for equipable items
 * Note: The base item templates must already be loaded
 */
void _item_loadItemTemplatesArmor(void *param, diJob_itemTemplateArmor_t *jobData)
{
	printf("Loading %d armor templates from db...\n", jobData->numberOfTemplates);
	for(sint32 i=0; i<jobData->numberOfTemplates; i++)
	{
		// find the already existing template
		itemTemplate_t* itemTemplate = (itemTemplate_t*)hashTable_get(&ht_itemTemplateId, jobData->itemTemplateArmorList[i].itemTemplateId);
		if( itemTemplate == NULL )
			continue; // equipment template has no item template
		// load additional equipment info
		if( itemTemplate->item.type != ITEMTYPE_ARMOR )
		{
			printf("Warning: itemTemplate %d has armor data but is not an armor\n", jobData->itemTemplateArmorList[i].itemTemplateId);
		}
		itemTemplate->armor.armorValue = jobData->itemTemplateArmorList[i].armorValue;
		itemTemplate->item.maxHitPoints = itemTemplate->armor.armorValue;
		itemTemplate->item.currentHitPoints = itemTemplate->armor.armorValue;
		itemTemplate->armor.regenRate = jobData->itemTemplateArmorList[i].regenRate;
		itemTemplate->armor.damageAbsorbed = jobData->itemTemplateArmorList[i].damageAbsorbed;
	}
	// mark item templates as loaded
	_itemTemplatesLoaded = true;
}

/*
 * Called when the mysql server returns the item weapon data
 * The weapon data is additional info for equipable items
 * Note: The base item templates must already be loaded
 */
void _item_loadItemTemplatesWeapon(void *param, diJob_itemTemplateWeapon_t *jobData)
{
	printf("Loading %d weapon templates from db...\n", jobData->numberOfTemplates);
	for(sint32 i=0; i<jobData->numberOfTemplates; i++)
	{
		// find the already existing template
		itemTemplate_t* itemTemplate = (itemTemplate_t*)hashTable_get(&ht_itemTemplateId, jobData->itemTemplateWeaponList[i].itemTemplateId);
		if( itemTemplate == NULL )
			continue; // equipment template has no item template
		// load additional equipment info
		if( itemTemplate->item.type != ITEMTYPE_WEAPON )
		{
			printf("Warning: itemTemplate %d has weapon data but is not an weapon\n", jobData->itemTemplateWeaponList[i].itemTemplateId);
		}
		itemTemplate->weapon.clipSize = jobData->itemTemplateWeaponList[i].clipSize;
		itemTemplate->weapon.currentAmmo = jobData->itemTemplateWeaponList[i].currentAmmo;
		itemTemplate->weapon.aimRate = jobData->itemTemplateWeaponList[i].aimRate;
		itemTemplate->weapon.reloadTime = jobData->itemTemplateWeaponList[i].reloadTime;
		itemTemplate->weapon.altActionId = jobData->itemTemplateWeaponList[i].altActionId;
		itemTemplate->weapon.altActionArg = jobData->itemTemplateWeaponList[i].altActionArg;
		itemTemplate->weapon.aeType = jobData->itemTemplateWeaponList[i].aeType;
		itemTemplate->weapon.aeRadius = jobData->itemTemplateWeaponList[i].aeRadius;
		itemTemplate->weapon.recoilAmount = jobData->itemTemplateWeaponList[i].recoilAmount;
		itemTemplate->weapon.reuseOverride = jobData->itemTemplateWeaponList[i].reuseOverride;
		itemTemplate->weapon.coolRate = jobData->itemTemplateWeaponList[i].coolRate;
		itemTemplate->weapon.heatPerShot = jobData->itemTemplateWeaponList[i].heatPerShot;
		itemTemplate->weapon.toolType = jobData->itemTemplateWeaponList[i].toolType;
		itemTemplate->weapon.ammoPerShot = jobData->itemTemplateWeaponList[i].ammoPerShot;
		itemTemplate->weapon.minDamage = jobData->itemTemplateWeaponList[i].minDamage;
		itemTemplate->weapon.maxDamage = jobData->itemTemplateWeaponList[i].maxDamage;
		itemTemplate->weapon.ammoClassId = jobData->itemTemplateWeaponList[i].ammoClassId;
		itemTemplate->weapon.damageType = jobData->itemTemplateWeaponList[i].damageType;
		itemTemplate->weapon.windupTime = jobData->itemTemplateWeaponList[i].windupTime;
		itemTemplate->weapon.recoveryTime = jobData->itemTemplateWeaponList[i].recoveryTime;
		itemTemplate->weapon.refireTime = jobData->itemTemplateWeaponList[i].refireTime;
		itemTemplate->weapon.range = jobData->itemTemplateWeaponList[i].range;
		itemTemplate->weapon.altMaxDamage = jobData->itemTemplateWeaponList[i].altMaxDamage;
		itemTemplate->weapon.altDamageType = jobData->itemTemplateWeaponList[i].altDamageType;
		itemTemplate->weapon.altRange = jobData->itemTemplateWeaponList[i].altRange;
		itemTemplate->weapon.altAERadius = jobData->itemTemplateWeaponList[i].altAERadius;
		itemTemplate->weapon.altAEType = jobData->itemTemplateWeaponList[i].altAEType;
		itemTemplate->weapon.attackType = jobData->itemTemplateWeaponList[i].attackType;
		// default values
		itemTemplate->weapon.isJammed = false;
	}
	// mark item templates as loaded
	_itemTemplatesLoaded = true;
}

void _gameData_loadItemTemplates()
{
	hashTable_init(&ht_itemTemplateId, 1024);
	// load item templates from db
	_itemTemplatesLoaded = false;
	DataInterface_Item_getItemTemplates(_item_loadItemTemplates, NULL);
	while( _itemTemplatesLoaded == false ) Sleep(100);
	// load additional equipment info data from db
	_itemTemplatesLoaded = false;
	DataInterface_Item_getItemEquipmentData(_item_loadItemTemplatesEquipment, NULL);
	while( _itemTemplatesLoaded == false ) Sleep(100);
	// load additional armor data
	_itemTemplatesLoaded = false;
	DataInterface_Item_getItemArmorData(_item_loadItemTemplatesArmor, NULL);
	while( _itemTemplatesLoaded == false ) Sleep(100);
	// load additional weapon data
	_itemTemplatesLoaded = false;
	DataInterface_Item_getItemWeaponData(_item_loadItemTemplatesWeapon, NULL);
	while( _itemTemplatesLoaded == false ) Sleep(100);
	// load additional misc data
	// todo
}




void gameData_load()
{
	_gameData_loadEquipmentClassSlots();
	_gameData_loadStarterItemTemplateClassIds();
	_gameData_loadMapInfo();
	_gameData_loadItemTemplates();
}
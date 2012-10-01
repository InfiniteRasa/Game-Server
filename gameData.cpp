#include "Global.h"

//equipableClassEquipmentSlot.txt

hashTable_t ht_equipmentClassSlots;
hashTable_t ht_starterItemTemplateClassIds;


void _gameData_loadEquipmentClassSlots()
{
	hashTable_init(&ht_equipmentClassSlots, 128);
	file_t *file = fileMgr_open((sint8*)"gameData\\equipableClassEquipmentSlot.txt");
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

hashTable_t ht_itemTemplateById;
hashTable_t ht_itemTemplateByName;

itemTemplate_t *gameData_getItemTemplateById(uint32 templateId)
{
	return (itemTemplate_t*)hashTable_get(&ht_itemTemplateById, templateId);	
}

itemTemplate_t *gameData_getItemTemplateByName(sint8 *name)
{
	return (itemTemplate_t*)hashTable_get(&ht_itemTemplateByName, name);	
}


void _gameData_loadItemTemplates()
{
	hashTable_init(&ht_itemTemplateById, 128);
	hashTable_init(&ht_itemTemplateByName, 128);
	//sData_t *sData_open(sint8 *path);
	//bool sData_nextCategory(sData_t *sData);
	//sint8 *sData_currentCategoryName(sData_t *sData);
	//sint8 *sData_findOption(sData_t *sData, sint8 *optionName);
	sData_t *it = sData_open((sint8*)"gameData\\ItemTemplates.txt");
	sint32 z = 0;
	while( sData_nextCategory(it) )
	{
		z++;
		if( z >= 128 )
		{
			printf("Too many item templates :(\n");
			Sleep(10000);
			ExitProcess(-1);
		}
		sint8 *catName = sData_currentCategoryName(it);
		printf("it: %s\n", catName);

		sint8 *s_ClassId = sData_findOption(it, (sint8*)"classId");
		sint8 *s_templateId = sData_findOption(it, (sint8*)"templateId");
		if( !s_ClassId || !s_templateId )
		{
			printf("ClassID or TemplateID missing at [%s]\n", catName);
			Sleep(10000);
			ExitProcess(-1);
		}

		itemTemplate_t *itemTemplate = (itemTemplate_t*)malloc(sizeof(itemTemplate_t));
		itemTemplate->classId = atoi((char*)s_ClassId);
		itemTemplate->templateId = atoi((char*)s_templateId);
		// get other states
		sint8 *s_type = sData_findOption(it, (sint8*)"type");
		if( s_type == NULL )
		{
			printf("'type' not set for [%s]\n", catName);
			Sleep(10000);
			ExitProcess(-1);
		}
		if( strcmp((char*)s_type, "WEAPON")==0 )
		{
			itemTemplate->type = ITEMTYPE_WEAPON;
		}
		else if( strcmp((char*)s_type, "ARMOR")==0 )
		{
			itemTemplate->type = ITEMTYPE_ARMOR;
		}
		else
		{
			printf("unknown type for [%s]\n", catName);
			Sleep(10000);
			ExitProcess(-1);
		}

		// Extra Info
		
		itemTemplate->currentHitPoints			= atoi((char*)sData_findOption(it, (sint8*)"currentHitPoints"));
		itemTemplate->maxHitPoints				= atoi((char*)sData_findOption(it, (sint8*)"maxHitPoints"));
		//s_info = sData_findOption(it, "modifiedBy");
		itemTemplate->hasSellableFlag			= (bool)atoi((char*)sData_findOption(it, (sint8*)"hasSellableFlag"));
		itemTemplate->hasCharacterUniqueFlag	= (bool)atoi((char*)sData_findOption(it, (sint8*)"hasCharacterUniqueFlag"));
		itemTemplate->hasAccountUniqueFlag		= (bool)atoi((char*)sData_findOption(it, (sint8*)"hasAccountUniqueFlag"));
		itemTemplate->hasBoEFlag				= (bool)atoi((char*)sData_findOption(it, (sint8*)"hasBoEFlag"));
		itemTemplate->qualityId					= atoi((char*)sData_findOption(it, (sint8*)"qualityId"));
		itemTemplate->boundToCharacter			= atoi((char*)sData_findOption(it, (sint8*)"boundToCharacter"));
		itemTemplate->notTradable				= atoi((char*)sData_findOption(it, (sint8*)"notTradable"));
		itemTemplate->notPlaceableInLockbox		= atoi((char*)sData_findOption(it, (sint8*)"notPlaceableInLockbox"));
		itemTemplate->inventoryCategory			= atoi((char*)sData_findOption(it, (sint8*)"inventoryCategory"));

		if (itemTemplate->type == ITEMTYPE_WEAPON)
		{
			itemTemplate->clipSize					= atoi((char*)sData_findOption(it, (sint8*)"clipSize"));
			itemTemplate->currentAmmo				= atoi((char*)sData_findOption(it, (sint8*)"currentAmmo"));
			itemTemplate->aimRate					= atof((char*)sData_findOption(it, (sint8*)"aimRate"));
			itemTemplate->reloadTime				= atoi((char*)sData_findOption(it, (sint8*)"reloadTime"));
			itemTemplate->altActionId				= atoi((char*)sData_findOption(it, (sint8*)"altActionId"));
			itemTemplate->altActionArg				= atoi((char*)sData_findOption(it, (sint8*)"altActionArg"));
			itemTemplate->aeType					= atoi((char*)sData_findOption(it, (sint8*)"aeType"));
			itemTemplate->aeRadius					= atoi((char*)sData_findOption(it, (sint8*)"aeRadius"));
			itemTemplate->recoilAmount				= atoi((char*)sData_findOption(it, (sint8*)"recoilAmount"));
			//itemTemplate->reuseOverride			= atoi(sData_findOption(it, "reuseOverride"));
			itemTemplate->coolRate					= atoi((char*)sData_findOption(it, (sint8*)"coolRate"));
			itemTemplate->heatPerShot				= atof((char*)sData_findOption(it, (sint8*)"heatPerShot"));
			itemTemplate->toolType					= atoi((char*)sData_findOption(it, (sint8*)"toolType"));
			itemTemplate->isJammed					= (bool)atoi((char*)sData_findOption(it, (sint8*)"isJammed"));
			itemTemplate->ammoPerShot				= atoi((char*)sData_findOption(it, (sint8*)"ammoPerShot"));
			itemTemplate->minDamage					= atoi((char*)sData_findOption(it, (sint8*)"MinDamage"));
			itemTemplate->maxDamage					= atoi((char*)sData_findOption(it, (sint8*)"MaxDamage"));
			itemTemplate->ammoClassId				= atoi((char*)sData_findOption(it, (sint8*)"AmmoClassId"));
			itemTemplate->damageType				= atoi((char*)sData_findOption(it, (sint8*)"DamageType"));
			itemTemplate->windupTime				= atoi((char*)sData_findOption(it, (sint8*)"WindupTime"));
			itemTemplate->recoveryTime				= atoi((char*)sData_findOption(it, (sint8*)"RecoveryTime"));
			itemTemplate->refireTime				= atoi((char*)sData_findOption(it, (sint8*)"RefireTime"));
			itemTemplate->range						= atoi((char*)sData_findOption(it, (sint8*)"Range"));
			itemTemplate->altMaxDamage				= atoi((char*)sData_findOption(it, (sint8*)"AltMaxDamage"));
			itemTemplate->altDamageType				= atoi((char*)sData_findOption(it, (sint8*)"AltDamageType"));
			itemTemplate->altRange					= atoi((char*)sData_findOption(it, (sint8*)"AltRange"));
			itemTemplate->altAERadius				= atoi((char*)sData_findOption(it, (sint8*)"AltAERadius"));
			itemTemplate->altAEType					= atoi((char*)sData_findOption(it, (sint8*)"AltAEType"));
			itemTemplate->attackType				= atoi((char*)sData_findOption(it, (sint8*)"AttackType"));
		}
		// Extra Info


		// set
		hashTable_set(&ht_itemTemplateById, itemTemplate->templateId, itemTemplate);
		hashTable_set(&ht_itemTemplateByName, catName, itemTemplate);
	}
	sData_close(it);
}




void gameData_load()
{
	_gameData_loadEquipmentClassSlots();
	_gameData_loadStarterItemTemplateClassIds();
	_gameData_loadMapInfo();
	_gameData_loadItemTemplates();
}
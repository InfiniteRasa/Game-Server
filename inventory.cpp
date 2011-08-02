#include "global.h"


/*
578
00000F81     5A - STORE_NAME          'RequestTooltipForItemTemplateId
*/

void item_recv_RequestTooltipForItemTemplateId(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	unsigned int itemTemplateId = pym_unpackInt(&pums);

	/*
	000008E2     64 - LOAD_CONST          579
	000008E5     5A - STORE_NAME          'ItemTemplateTooltipInfo'
	*/
	// itemTemplateId
	pyMarshalString_t pms;
	// create item entity
	pym_init(&pms);
	pym_tuple_begin(&pms);


	itemTemplate_t *itemTemplate = gameData_getItemTemplateById(itemTemplateId);
	if( itemTemplate == NULL )
		return; // todo: even answer on a unknown template, else the client will continue to spam us with requests
	/*
	itemTemplateId' (0E 02 00 00),
	0000A3E6                         STR: 'itemClassId' (1A 02 00 00),
	0000A3EB                         STR: 'info' (18 02 00 00)
	*/

	//pym_addNoneStruct(&pms);
	//pym_addInt(&pms, -1); // 15444
	pym_addInt(&pms, itemTemplateId); // templateId
	pym_addInt(&pms, itemTemplate->classId); // itemClass
	pym_dict_begin(&pms); // Info is a dict of { AugId : AugData }
	// ARMOR
	/*pym_addInt(&pms, 51);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 10);
	pym_tuple_end(&pms);*/
	// EQUIPABLE 4
	/*	pym_addInt(&pms, 4); 
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1);
	pym_tuple_end(&pms);*/
	// DECORATION 36

	// ITEM 6
	//pym_addInt(&pms, 6); 
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, 1); // tradeAble
	//pym_addInt(&pms, 2); // maxHps
	//pym_addInt(&pms, 3); // buybackPrice
	//pym_list_begin(&pms); // requirements (check File "python/client\ui\tooltipwindow.py", ptr0x743, in _FormatIndividualItemTooltip)
	//pym_tuple_begin(&pms);
	//	pym_addInt(&pms, 1); // id = 1, level
	//	pym_addInt(&pms, 1); // constant
	//pym_tuple_end(&pms);
	//pym_list_end(&pms); 
	//pym_list_begin(&pms); // moduleIds
	//	pym_addInt(&pms, 37);
	//pym_list_end(&pms); 

	////pym_addInt(&pms, 0); // moduleIds
	//pym_addNoneStruct(&pms); // raceIds
	//pym_tuple_end(&pms);


	if( itemTemplate->type == ITEMTYPE_WEAPON )
	{
		pym_addInt(&pms, 2); 
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 10);		// kWeaponIdx_MinDamage
		pym_addInt(&pms, 20);		// kWeaponIdx_MaxDamage
		pym_addInt(&pms, 3147);		// kWeaponIdx_AmmoClassId
		pym_addInt(&pms, 1);		// kWeaponIdx_ClipSize
		pym_addInt(&pms, 1);		// kWeaponIdx_AmmoPerShot
		pym_addInt(&pms, 1);		// kWeaponIdx_DamageType
		pym_addInt(&pms, 1);		// kWeaponIdx_WindupTime
		pym_addInt(&pms, 1);		// kWeaponIdx_RecoveryTime
		pym_addInt(&pms, 1);		// kWeaponIdx_RefireTime
		pym_addInt(&pms, 1);		// kWeaponIdx_ReloadTime
		pym_addInt(&pms, 10);		// kWeaponIdx_Range
		pym_addInt(&pms, 1);		// kWeaponIdx_AERadius
		pym_addInt(&pms, 1);		// kWeaponIdx_AEType
		pym_tuple_begin(&pms);		// kWeaponIdx_AltFire
		pym_addInt(&pms, 25);	// kWeaponAltIdx_MaxDamage
		pym_addInt(&pms, 1);	// kWeaponAltIdx_DamageType
		pym_addInt(&pms, 15);	// kWeaponAltIdx_Range
		pym_addInt(&pms, 1);	// kWeaponAltIdx_AERadius
		pym_addInt(&pms, 1);	// kWeaponAltIdx_AEType
		pym_tuple_end(&pms);
		pym_addInt(&pms, 1);		// kWeaponIdx_AttackType
		pym_addInt(&pms, 1);		// kWeaponIdx_ToolType


		pym_tuple_end(&pms);
	}

	/*
	0000074F     64 - LOAD_CONST          0
	00000752     5A - STORE_NAME          'kItemIdx_Tradable'
	00000755     64 - LOAD_CONST          1
	00000758     5A - STORE_NAME          'kItemIdx_MaxHPs'
	0000075B     64 - LOAD_CONST          2
	0000075E     5A - STORE_NAME          'kItemIdx_BuybackPrice'
	00000761     64 - LOAD_CONST          3
	00000764     5A - STORE_NAME          'kItemIdx_Requirements'
	00000767     64 - LOAD_CONST          4
	0000076A     5A - STORE_NAME          'kItemIdx_ModuleIds'
	0000076D     64 - LOAD_CONST          5
	00000770     5A - STORE_NAME          'kItemIdx_RaceIds'
	00000773     64 - LOAD_CONST          0
	00000776     5A - STORE_NAME          'kEquipableIdx_SkillInfo'
	00000779     64 - LOAD_CONST          1
	0000077C     5A - STORE_NAME          'kEquipableIdx_ResistList'
	0000077F     64 - LOAD_CONST          0
	00000782     5A - STORE_NAME          'kArmorIdx_RegenRate'
	00000785     64 - LOAD_CONST          0
	00000788     5A - STORE_NAME          'kWeaponIdx_MinDamage'
	0000078B     64 - LOAD_CONST          1
	0000078E     5A - STORE_NAME          'kWeaponIdx_MaxDamage'
	00000791     64 - LOAD_CONST          2
	00000794     5A - STORE_NAME          'kWeaponIdx_AmmoClassId'
	00000797     64 - LOAD_CONST          3
	0000079A     5A - STORE_NAME          'kWeaponIdx_ClipSize'
	0000079D     64 - LOAD_CONST          4
	000007A0     5A - STORE_NAME          'kWeaponIdx_AmmoPerShot'
	000007A3     64 - LOAD_CONST          5
	000007A6     5A - STORE_NAME          'kWeaponIdx_DamageType'
	000007A9     64 - LOAD_CONST          6
	000007AC     5A - STORE_NAME          'kWeaponIdx_WindupTime'
	000007AF     64 - LOAD_CONST          7
	000007B2     5A - STORE_NAME          'kWeaponIdx_RecoveryTime'
	000007B5     64 - LOAD_CONST          8
	000007B8     5A - STORE_NAME          'kWeaponIdx_RefireTime'
	000007BB     64 - LOAD_CONST          9
	000007BE     5A - STORE_NAME          'kWeaponIdx_ReloadTime'
	000007C1     64 - LOAD_CONST          10
	000007C4     5A - STORE_NAME          'kWeaponIdx_Range'
	000007C7     64 - LOAD_CONST          11
	000007CA     5A - STORE_NAME          'kWeaponIdx_AERadius'
	000007CD     64 - LOAD_CONST          12
	000007D0     5A - STORE_NAME          'kWeaponIdx_AEType'
	000007D3     64 - LOAD_CONST          13
	000007D6     5A - STORE_NAME          'kWeaponIdx_AltFire'
	000007D9     64 - LOAD_CONST          14
	000007DC     5A - STORE_NAME          'kWeaponIdx_AttackType'
	000007DF     64 - LOAD_CONST          15
	000007E2     5A - STORE_NAME          'kWeaponIdx_ToolType'
	000007E5     64 - LOAD_CONST          0
	000007E8     5A - STORE_NAME          'kWeaponAltIdx_MaxDamage'
	000007EB     64 - LOAD_CONST          1
	000007EE     5A - STORE_NAME          'kWeaponAltIdx_DamageType'
	000007F1     64 - LOAD_CONST          2
	000007F4     5A - STORE_NAME          'kWeaponAltIdx_Range'
	000007F7     64 - LOAD_CONST          3
	000007FA     5A - STORE_NAME          'kWeaponAltIdx_AERadius'
	000007FD     64 - LOAD_CONST          4
	00000800     5A - STORE_NAME          'kWeaponAltIdx_AEType'
	*/


	pym_dict_end(&pms);
	//pym_addUnicode(&pms, "tooltip info"); // info
	//pym_addInt(&pms, itemTemplateId); // itemTemplateId
	//pym_addInt(&pms, 15444); // itemClassId
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 12, 579, pym_getData(&pms), pym_getLen(&pms));
}

void inventory_removeItem(mapChannelClient_t *client, int inventoryType, int entityId)
{
	// removeItem: 88
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, inventoryType); // inventoryType
	pym_addInt(&pms, entityId); // entityID
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, 88, pym_getData(&pms), pym_getLen(&pms));
}

void inventory_addItem(mapChannelClient_t *client, int inventoryType, int entityId, int slotIndex)
{
	// addItem 85
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, inventoryType); // inventoryType
	pym_addInt(&pms, entityId); // entityID
	pym_addInt(&pms, slotIndex); // slotIndex
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, 85, pym_getData(&pms), pym_getLen(&pms));
}

item_t *item_create(itemTemplate_t *itemTemplate)//unsigned int classId, unsigned int templateId)
{
	item_t *item = (item_t*)malloc(sizeof(item_t));
	item->entityId = entityMgr_getFreeEntityIdForItem();
	item->itemTemplate = itemTemplate;
	item->locationEntityId = 0;
	item->locationSlotIndex = 0;
	// register item
	entityMgr_registerEntity(item->entityId, item);
	return item;
}

void item_setLocationHomeinventory(item_t *item, mapChannelClient_t *owner)
{
	item->locationEntityId = owner->clientEntityId;
	// find free slot
	int catIndex = 0;
	if( item->itemTemplate->type == ITEMTYPE_WEAPON )
		catIndex = 0;
	else if( item->itemTemplate->type == ITEMTYPE_ARMOR )
		catIndex = 50;
	else
		__debugbreak();
	for(int i=0; i<50; i++)
	{
		if( owner->inventory.slot[catIndex+i] == 0 )
		{
			item->locationSlotIndex = catIndex+i;
			break;
		}
	}
}

int item_findFreePlayerinventorySpace(mapChannelClient_t *owner, int itemType)
{
	if( itemType == ITEMTYPE_WEAPON || itemType == ITEMTYPE_ARMOR )
	{
		for(int i=0; i<50; i++)
		{
			if( owner->inventory.slot[i] == 0 )
			{
				return i;
			}
		}		
	}
	return -1;
}


void item_recv_PersonalInventoryMoveItem(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int fromSlot = pym_unpackInt(&pums);
	int toSlot = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// remove item
	if( fromSlot == toSlot )
		return;
	if( fromSlot < 0 || fromSlot >= 250 )
		return;
	if( toSlot < 0 || toSlot >= 250 )
		return;
	unsigned long long entityId = client->inventory.homeInventory[fromSlot];
	if( entityId == NULL )
		return;
	inventory_removeItem(client, 1, entityId);
	inventory_addItem(client, 1, entityId, toSlot);
	client->inventory.homeInventory[toSlot] = client->inventory.homeInventory[fromSlot];
	client->inventory.homeInventory[fromSlot] = 0;
	// send response
}

void item_recv_RequestEquipWeapon(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int srcSlot = pym_unpackInt(&pums);
	int srcInventory = pym_unpackInt(&pums);
	int dstSlot = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	if( srcInventory != 1 )
	{
		printf("Unsupported inventory\n");
		return;
	}
	if( srcSlot < 0 || srcSlot > 50 )
		return;
	if( dstSlot < 0 || dstSlot >= 5 )
		return;
	// equip item
	// todo: check entity type
	// swap item
	unsigned long long swapId = client->inventory.weaponDrawer[dstSlot];
	if( client->inventory.homeInventory[srcSlot] )
		inventory_removeItem(client, INVENTORY_PERSONAL, client->inventory.homeInventory[srcSlot]);
	if( client->inventory.weaponDrawer[dstSlot] )
		inventory_removeItem(client, INVENTORY_WEAPONDRAWERINVENTORY, client->inventory.weaponDrawer[dstSlot]);
	if( client->inventory.weaponDrawer[dstSlot] )
		inventory_addItem(client, INVENTORY_PERSONAL, client->inventory.weaponDrawer[dstSlot], srcSlot);
	if( client->inventory.homeInventory[srcSlot] )
		inventory_addItem(client, INVENTORY_WEAPONDRAWERINVENTORY, client->inventory.homeInventory[srcSlot], dstSlot);
	client->inventory.weaponDrawer[dstSlot] = client->inventory.homeInventory[srcSlot];
	client->inventory.homeInventory[srcSlot] = swapId;
	// update appearance if equipped on slot 0
	pyMarshalString_t pms;
	// update weapon drawer ( has no effect? )
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, dstSlot); // slotId
	//pym_addBool(&pms, false); // requested
	//pym_tuple_end(&pms);
	//netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, 574, pym_getData(&pms), pym_getLen(&pms));
	if( dstSlot == 0 && client->inventory.weaponDrawer[0] )
	{
		item_t *item = (item_t*)entityMgr_get(client->inventory.weaponDrawer[dstSlot]);
		if( !item )
			return;
		manifestation_setAppearanceItem(client->player, item->itemTemplate->classId, 0xFF808080);
		manifestation_updateAppearance(client);
	}
	if( dstSlot == client->inventory.activeWeaponDrawer )
	{
		inventory_notifyEquipmentUpdate(client);
	}
}

/*
	maybe move the weapon related entries to manifestation?
*/
void item_recv_RequestWeaponDraw(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	// TODO!
	//manifestation_updateWeaponReadyState(client); ?
	printf("TODO: "); puts(__FUNCTION__);
}

void item_recv_RequestWeaponStow(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	// TODO!
	//manifestation_updateWeaponReadyState(client); ?
	printf("TODO: "); puts(__FUNCTION__);
}

void item_recv_RequestWeaponReload(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	// TODO!
	printf("TODO: "); puts(__FUNCTION__);
}

void item_sendItemDataToClient(mapChannelClient_t *client, item_t *item)
{
	pyMarshalString_t pms;
	// create item entity
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, item->entityId); // entityID
	pym_addInt(&pms, item->itemTemplate->classId); // classID
	pym_addNoneStruct(&pms); // entityData (dunno)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	// send item info
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 100); // 'currentHitPoints'
	pym_addInt(&pms, 100); // 'maxHitPoints'
	pym_addUnicode(&pms, "Any name"); // 'crafterName'
	pym_addInt(&pms, item->itemTemplate->templateId);//15276); // 'itemTemplateId'
	//pym_addNoneStruct(&pms);
	pym_addBool(&pms, true); // 'hasSellableFlag'
	pym_addBool(&pms, false); // 'hasCharacterUniqueFlag'
	pym_addBool(&pms, false); // 'hasAccountUniqueFlag'
	pym_addBool(&pms, false); // 'hasBoEFlag'
	// 'classModuleIds' (classLootModuleIds)
	pym_list_begin(&pms);
	pym_list_end(&pms);
	// 'lootModuleIds'
	pym_list_begin(&pms);
	pym_list_end(&pms);
	pym_addInt(&pms, 2); // 'qualityId'
	/*
	QualitiIds:
	MISSION = 1
	NORMAL = 2
	UNCOMMON = 3
	RARE = 4
	EPIC = 5
	LEGENDARY = 6
	JUNK = 7
	*/
	pym_addInt(&pms, 0); // 'boundToCharacter'
	pym_addInt(&pms, 0); // 'notTradable'
	pym_addInt(&pms, 0); // 'notPlaceableInLockbox'
	if( item->itemTemplate->type == ITEMTYPE_WEAPON )
		pym_addInt(&pms, 1); // 'inventoryCategory' (Weapon/Armor)
	else if( item->itemTemplate->type == ITEMTYPE_ARMOR )
		pym_addInt(&pms, 1); // 'inventoryCategory' (Weapon/Armor)
	else
		__debugbreak();
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 469, pym_getData(&pms), pym_getLen(&pms)); // ItemInfo

	if( item->itemTemplate->type == ITEMTYPE_WEAPON )
	{
		// send weapon data
		pym_init(&pms);
		pym_tuple_begin(&pms);	
		//pym_addUnicode(&pms, "testName"); // '_Weapon__weaponName'
		//pym_addInt(&pms, 1); // '_Weapon__clipSize'
		//pym_addInt(&pms, 10); // '_Weapon__currentAmmo'
		//pym_addInt(&pms, 1); // '_Weapon__aimRate'
		//pym_addInt(&pms, 1); // '_Weapon__reloadTimeMs'
		//pym_addInt(&pms, 2); // '_Weapon__aeRadius'
		////pym_addInt(&pms, 3); // '_Weapon__altActionId' (level)
		////pym_addInt(&pms, 1); // '_Weapon__altActionArg' (type)
		//pym_addInt(&pms, 174);
		//pym_addInt(&pms, 1);
		//pym_addInt(&pms, 1); // '_Weapon__recoilAmount'
		//pym_addInt(&pms, 13); // '_Weapon__reuseOverride'
		//pym_addInt(&pms, 1); // '_Weapon__coolRate'
		//pym_addInt(&pms, 1); // '_Weapon__heatPerShot'
		//pym_addInt(&pms, 1); // '_Weapon__toolType'
		//pym_addBool(&pms, false); // '_Weapon__jammed'
		//pym_addInt(&pms, 0); // '_Weapon__ammoPerShot'
		pym_addUnicode(&pms, "testName"); // weaponName
		pym_addInt(&pms, 20); // clipSize
		pym_addInt(&pms, 1337); // currentAmmo
		pym_addFloat(&pms, 1.0f);//pym_addInt(&pms, 1); // aimRate
		pym_addInt(&pms, 1); // reloadTime
		pym_addInt(&pms, 1); // altActionId
		pym_addInt(&pms, 133); // altActionArg
		pym_addInt(&pms, 2);//pym_addNoneStruct(&pms);//pym_addInt(&pms, 1); // aeType
		pym_addNoneStruct(&pms);//pym_addInt(&pms, 100); // aeRadius
		pym_addInt(&pms, 1); // recoilAmount
		pym_addNoneStruct(&pms);//pym_addInt(&pms, 1); // reuseOverride
		pym_addInt(&pms, 1); // coolRate
		pym_addInt(&pms, 1); // heatPerShot
		pym_addInt(&pms, 8); // toolType ( PISTOL = 8 )
		pym_addBool(&pms, 0); // isJammed
		pym_addInt(&pms, 1); // ammoPerShot

		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 237, pym_getData(&pms), pym_getLen(&pms)); //WeaponInfo

		// send jammed state
		//pym_init(&pms);
		//pym_tuple_begin(&pms);	
		//pym_addBool(&pms, false); // isJammed
		//pym_tuple_end(&pms);
		//netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 585, pym_getData(&pms), pym_getLen(&pms)); //WeaponInfo
	}
}

void item_sendInfo(item_t *item)
{
	if( item->entityId == 0 )
		return; // no location
	// todo: check if entityId is mapChannelClient
	mapChannelClient_t *client = (mapChannelClient_t*)entityMgr_get(item->locationEntityId);
	if( client == NULL )
		return;
	if( item->locationSlotIndex >= 0 || item->locationSlotIndex < 250 )
	{
		client->inventory.homeInventory[item->locationSlotIndex] = item->entityId;
		item_sendItemDataToClient(client, item);
		inventory_addItem(client, 1, item->entityId, item->locationSlotIndex);
	}
}

/*
 * Rather troublesome function, used to tell the client new equipment data. Is needed despite the fact that he should
 * already know from other packets.
 * Important for 'StartAutoFire' to work. (i.e. GetWeapon() will return null or the wrong item if not set correctly)
 */
void inventory_notifyEquipmentUpdate(mapChannelClient_t *client)
{
	/*
	equipmentInfo
	*/
	pyMarshalString_t pms;
	// tell client that the equipment state has changed
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_list_begin(&pms); // pairs of (slotId, entityId)
	// slotIds are the same as for appearance info
	if( client->inventory.weaponDrawer[client->inventory.activeWeaponDrawer] ) // is a weapon equipped?
	{
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 13); // slotIndex for weapon (see GetWeapon())
		pym_addInt(&pms, client->inventory.weaponDrawer[client->inventory.activeWeaponDrawer]); // entityID
		pym_tuple_end(&pms);
	}
	// more equipment todo?
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, METHODID_EQUIPMENTINFO, pym_getData(&pms), pym_getLen(&pms));
}



item_t* item_createFromTemplate(char *name)
{
	itemTemplate_t *itemTemplate = gameData_getItemTemplateByName(name);
	if( !itemTemplate )
		return NULL;
	item_t *item = item_create(itemTemplate);
	return item;
}


void inventory_initForClient(mapChannelClient_t *client)
{
	pyMarshalString_t pms;
	// create home inventory
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, INVENTORY_PERSONAL); // inventoryType
	pym_list_begin(&pms);
	/* {itemId, slotIndex} list */
	pym_list_end(&pms);
	pym_addInt(&pms, 250); // maxSize (50 per category)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, 86, pym_getData(&pms), pym_getLen(&pms));
	// create weapon drawer
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, INVENTORY_WEAPONDRAWERINVENTORY); // inventoryType
	pym_list_begin(&pms);
	/* {itemId, slotIndex} list */
	pym_list_end(&pms);
	pym_addInt(&pms, 5); // maxSize
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, 86, pym_getData(&pms), pym_getLen(&pms));
	// test
	item_t *testA;

	for(int i=0; i<30; i++)
	{
		if (i==0)
		{
			testA = item_createFromTemplate("Weapon_Avatar_Pistol_Physical_UNC_01_to_04");
			if( testA )
			{
				item_setLocationHomeinventory(testA, client);
				item_sendInfo(testA);
			}
		}

		testA = item_createFromTemplate("Weapon_Avatar_Pistol_Physical_UNC_01_to_04");
		if( testA )
		{
			item_setLocationHomeinventory(testA, client);
			item_sendInfo(testA);
		}
	}
}

/*
moduleInfo structure: (send as response for RequestModuleInfo)
00000142     7D - STORE_FAST          'effectId'
00000145     7D - STORE_FAST          'setLevel'
00000148     7D - STORE_FAST          'flatValue'
0000014B     7D - STORE_FAST          'linearValue'
0000014E     7D - STORE_FAST          'expValue'
00000151     7D - STORE_FAST          'arg1'
00000154     7D - STORE_FAST          'arg2'
00000157     7D - STORE_FAST          'arg3'
0000015A     7D - STORE_FAST          'arg4'
*/
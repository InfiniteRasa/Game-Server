#include "global.h"

#define ARMOR		 51
#define DECORATION	 36
#define EQUIPABLE	 4
#define ITEM		 6
#define WEAPON		 2

/*
578
00000F81     5A - STORE_NAME          'RequestTooltipForItemTemplateId
*/

void item_recv_RequestTooltipForItemTemplateId(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) ) { return; }

	unsigned int itemTemplateId = pym_unpackInt(&pums);
	itemTemplate_t *itemTemplate = gameData_getItemTemplateById(itemTemplateId);
	if( itemTemplate == NULL )
		return; // todo: even answer on a unknown template, else the client will continue to spam us with requests

	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, itemTemplateId);			// templateId
	pym_addInt(&pms, itemTemplate->classId);	// itemClass
	pym_dict_begin(&pms); // Info is a dict of { AugId : AugData }

	if( itemTemplate->type == ITEMTYPE_ARMOR )
	{
		//printf("RequestTooltipForItemTemplateId for Armor\n");
	}
	if( itemTemplate->type == ITEMTYPE_WEAPON )
	{
		printf("RequestTooltipForItemTemplateId for Weapon\n");
		// Item start
		pym_addInt(&pms, ITEM);
		pym_tuple_begin(&pms);
			pym_addBool(&pms, !itemTemplate->notTradable);		// kItemIdx_Tradable		= 0
			pym_addInt(&pms, itemTemplate->maxHitPoints);		// kItemIdx_MaxHPs			= 1
			pym_addInt(&pms, 1000);								// kItemIdx_BuybackPrice	= 2
			pym_addNoneStruct(&pms);							// kItemIdx_Requirements	= 3
			//pym_tuple_begin(&pms);
				pym_addNoneStruct(&pms);						// kItemIdx_ModuleIds		= 4
			//pym_tuple_end(&pms);
			pym_addNoneStruct(&pms);							// kItemIdx_RaceIds			= 5
		pym_tuple_end(&pms);
		//Item end
		// Equipable start
		pym_addInt(&pms, EQUIPABLE);
		pym_tuple_begin(&pms);
			// kEquipableIdx_SkillInfo = 0 (SkillId, MinSkillLvl)
			pym_tuple_begin(&pms);
				pym_addNoneStruct(&pms); // Skill ID
				pym_addNoneStruct(&pms); // Skill Level
			pym_tuple_end(&pms);
			// kEquipableIdx_ResistList = 1 (damageType, resistValue)
			pym_tuple_begin(&pms);
			pym_tuple_end(&pms);
		pym_tuple_end(&pms);
		// Equipable end
		// Weapon start
		pym_addInt(&pms, WEAPON);
		pym_tuple_begin(&pms);
			pym_addInt(&pms, itemTemplate->minDamage);			//kWeaponIdx_MinDamage		= 0
			pym_addInt(&pms, itemTemplate->maxDamage);			//kWeaponIdx_MaxDamage		= 1
			pym_addInt(&pms, itemTemplate->ammoClassId);		//kWeaponIdx_AmmoClassId	= 2 // standard grade cartridge
			pym_addInt(&pms, itemTemplate->clipSize);			//kWeaponIdx_ClipSize		= 3
			pym_addInt(&pms, itemTemplate->ammoPerShot);		//kWeaponIdx_AmmoPerShot	= 4
			pym_addInt(&pms, itemTemplate->damageType);			//kWeaponIdx_DamageType		= 5 // normal
			pym_addInt(&pms, itemTemplate->windupTime);			//kWeaponIdx_WindupTime		= 6 // autofire
			pym_addInt(&pms, itemTemplate->recoveryTime);		//kWeaponIdx_RecoveryTime   = 7
			pym_addInt(&pms, itemTemplate->refireTime);			//kWeaponIdx_RefireTime		= 8
			pym_addInt(&pms, itemTemplate->reloadTime);			//kWeaponIdx_ReloadTime		= 9
			pym_addInt(&pms, itemTemplate->range);				//kWeaponIdx_Range			= 10
			pym_addInt(&pms, itemTemplate->aeRadius);			//kWeaponIdx_AERadius		= 11
			pym_addInt(&pms, itemTemplate->aeType);				//kWeaponIdx_AEType			= 12
			//kWeaponIdx_AltFire		= 13
			pym_tuple_begin(&pms);		
			if (itemTemplate->altActionId != 0)
			{
				pym_addInt(&pms, itemTemplate->altMaxDamage);	// kWeaponAltIdx_MaxDamage	= 0
				pym_addInt(&pms, itemTemplate->altDamageType);	// kWeaponAltIdx_DamageType = 1
				pym_addInt(&pms, itemTemplate->altRange);		// kWeaponAltIdx_Range		= 2
				pym_addInt(&pms, itemTemplate->altAERadius);	// kWeaponAltIdx_AERadius	= 3
				pym_addInt(&pms, itemTemplate->altAEType);		// kWeaponAltIdx_AEType		= 4
			} else { pym_addNoneStruct(&pms); }
			pym_tuple_end(&pms);
			pym_addInt(&pms, itemTemplate->attackType);			//kWeaponIdx_AttackType		= 14 // ranged
			pym_addInt(&pms, itemTemplate->toolType);			//kWeaponIdx_ToolType		= 15 // rifle
		pym_tuple_end(&pms);
		// Weapon end
	}

	pym_dict_end(&pms);
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
		catIndex = 0; // 50
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

void item_setLocationEquippedinventory(item_t *item, mapChannelClient_t *owner)
{
	item->locationEntityId = owner->clientEntityId;
	item->locationSlotIndex = gameData_getEquipmentClassIdSlot(item->itemTemplate->classId);
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

void item_recv_RequestEquipArmor(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen)
{
	printf("RequestEquipArmor pl0x\n");
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int srcSlot = pym_unpackInt(&pums); // Source Slot
	int srcInventory = pym_unpackInt(&pums); // Source Inventory
	int dstSlot = pym_unpackInt(&pums); // Destination Slot
	if( pums.unpackErrorEncountered )
		return;
	if( srcInventory != 1 )
	{
		printf("Unsupported inventory\n");
		return;
	}
	printf("Src: %i ", srcSlot);
	printf("Dst: %i ", dstSlot);
	printf("Inv: %i\n", srcInventory);
	if( srcSlot < 0 || srcSlot > 50 )
		return;
	//if( dstSlot < 0 || dstSlot >= 5 )
	//	return;
	unsigned long long swapId = client->inventory.equippedInventory[dstSlot];
	if( client->inventory.homeInventory[srcSlot] )
		inventory_removeItem(client, INVENTORY_PERSONAL, client->inventory.homeInventory[srcSlot]);
	if( client->inventory.equippedInventory[dstSlot] )
		inventory_removeItem(client, INVENTORY_EQUIPPEDINVENTORY, client->inventory.equippedInventory[dstSlot]);
	if( client->inventory.equippedInventory[dstSlot] )
		inventory_addItem(client, INVENTORY_PERSONAL, client->inventory.equippedInventory[dstSlot], srcSlot);
	if( client->inventory.homeInventory[srcSlot] )
		inventory_addItem(client, INVENTORY_EQUIPPEDINVENTORY, client->inventory.homeInventory[srcSlot], dstSlot);
	client->inventory.equippedInventory[dstSlot] = client->inventory.homeInventory[srcSlot];
	client->inventory.homeInventory[srcSlot] = swapId;

	pyMarshalString_t pms;
	item_t *item = (item_t*)entityMgr_get(client->inventory.equippedInventory[dstSlot]);
	if( !item )
		return;
	manifestation_setAppearanceItem(client->player, item->itemTemplate->classId, 0xFF808080);
	manifestation_updateAppearance(client);
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
	pym_addInt(&pms, item->entityId);									// entityID
	pym_addInt(&pms, item->itemTemplate->classId);						// classID
	pym_addNoneStruct(&pms);											// entityData (dunno)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	// send item info
	pym_init(&pms);
	pym_tuple_begin(&pms);
		pym_addInt(&pms, item->itemTemplate->currentHitPoints);			// 'currentHitPoints'
		pym_addInt(&pms, item->itemTemplate->maxHitPoints);				// 'maxHitPoints'
		pym_addUnicode(&pms, "Richard Garriott");						// Modified By
		pym_addInt(&pms, item->itemTemplate->templateId);				// 'itemTemplateId'
		pym_addBool(&pms, item->itemTemplate->hasSellableFlag);			// 'hasSellableFlag'
		pym_addBool(&pms, item->itemTemplate->hasCharacterUniqueFlag);	// 'hasCharacterUniqueFlag'
		pym_addBool(&pms, item->itemTemplate->hasAccountUniqueFlag);	// 'hasAccountUniqueFlag'
		pym_addBool(&pms, item->itemTemplate->hasBoEFlag);				// 'hasBoEFlag'
		// 'classModuleIds' (classLootModuleIds)
		pym_list_begin(&pms);
		pym_list_end(&pms);
		// 'lootModuleIds'
		pym_list_begin(&pms);
		pym_list_end(&pms);
		pym_addInt(&pms, item->itemTemplate->qualityId);				// 'qualityId'
		/*	QualitiIds:
			MISSION = 1		NORMAL = 2		UNCOMMON = 3	
			RARE = 4		EPIC = 5		LEGENDARY = 6
			JUNK = 7	*/
		pym_addInt(&pms, item->itemTemplate->boundToCharacter);			// 'boundToCharacter'
		pym_addInt(&pms, item->itemTemplate->notTradable);				// 'notTradable'
		pym_addInt(&pms, item->itemTemplate->notPlaceableInLockbox);	// 'notPlaceableInLockbox'
		pym_addInt(&pms, item->itemTemplate->inventoryCategory);		// 'inventoryCategory'
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 469, pym_getData(&pms), pym_getLen(&pms)); // ItemInfo

	if( item->itemTemplate->type == ITEMTYPE_WEAPON )
	{
		// send weapon data
		pym_init(&pms);
		pym_tuple_begin(&pms);	
			pym_addNoneStruct(&pms);//pym_addUnicode(&pms, "testName"); // weaponName - not used
			pym_addInt(&pms, item->itemTemplate->clipSize);				// clipSize
			pym_addInt(&pms, item->itemTemplate->currentAmmo);			// currentAmmo
			pym_addFloat(&pms, item->itemTemplate->aimRate);			// aimRate
			pym_addInt(&pms, item->itemTemplate->reloadTime);			// reloadTime
			pym_addInt(&pms, item->itemTemplate->altActionId);			// altActionId
			pym_addInt(&pms, item->itemTemplate->altActionArg);			// altActionArg
			pym_addInt(&pms, item->itemTemplate->aeType);				// aeType
			pym_addInt(&pms, item->itemTemplate->aeRadius);				//pym_addInt(&pms, 100); // aeRadius
			pym_addInt(&pms, item->itemTemplate->recoilAmount);			// recoilAmount
			pym_addNoneStruct(&pms);									// reuseOverride
			pym_addInt(&pms, item->itemTemplate->coolRate);				// coolRate
			pym_addFloat(&pms, item->itemTemplate->heatPerShot);		// heatPerShot
			pym_addInt(&pms, item->itemTemplate->toolType);				// toolType ( PISTOL = 8 )
			pym_addBool(&pms, item->itemTemplate->isJammed);			// isJammed
			pym_addInt(&pms, item->itemTemplate->ammoPerShot);			// ammoPerShot
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 237, pym_getData(&pms), pym_getLen(&pms)); //WeaponInfo

		// send jammed state
		//pym_init(&pms);
		//pym_tuple_begin(&pms);	
		//pym_addBool(&pms, false); // isJammed
		//pym_tuple_end(&pms);
		//netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 585, pym_getData(&pms), pym_getLen(&pms)); //WeaponInfo
	}
	if( item->itemTemplate->type == ITEMTYPE_ARMOR )
	{
		// Recv_ArmorInfo
		pym_init(&pms);
		pym_tuple_begin(&pms);	
		pym_addInt(&pms, item->itemTemplate->currentHitPoints); // currentHitPoints
		pym_addInt(&pms, item->itemTemplate->maxHitPoints); // maxHitPoints
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 406, pym_getData(&pms), pym_getLen(&pms)); //ArmorInfo
	}
}

void item_sendEquippedInfo(item_t *item, mapChannelClient_t *owner)
{
	item_setLocationEquippedinventory(item, owner);
	if( item->entityId == 0 )
		return; 

	mapChannelClient_t *client = (mapChannelClient_t*)entityMgr_get(item->locationEntityId);
	if( client == NULL )
		return;
	if( item->locationSlotIndex >= 0 || item->locationSlotIndex < 16 )
	{
		client->inventory.equippedInventory[item->locationSlotIndex] = item->entityId;
		item_sendItemDataToClient(client, item);
		inventory_addItem(client, 8, item->entityId, item->locationSlotIndex);
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
	// create equipped inventory
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, INVENTORY_EQUIPPEDINVENTORY); // inventoryType
	pym_list_begin(&pms);
	/* {itemId, slotIndex} list */
	pym_list_end(&pms);
	pym_addInt(&pms, 5); // maxSize
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, 86, pym_getData(&pms), pym_getLen(&pms));
	// test
	item_t *testA;
	item_t *testB;

	testA = item_createFromTemplate("Armor_T2_Reflective_V01_CMN_Helmet_05_to_08");
	if( testA )
	{
		item_sendEquippedInfo(testA, client);
	}
	testA = item_createFromTemplate("Armor_T2_Reflective_V01_CMN_Vest_05_to_08");
	if( testA )
	{
		item_sendEquippedInfo(testA, client);
	}
	testA = item_createFromTemplate("Armor_T2_Reflective_V01_CMN_Gloves_05_to_08");
	if( testA )
	{
		item_sendEquippedInfo(testA, client);
	}
	testA = item_createFromTemplate("Armor_T2_Reflective_V01_CMN_Legs_05_to_08");
	if( testA )
	{
		item_sendEquippedInfo(testA, client);
	}
	testA = item_createFromTemplate("Armor_T2_Reflective_V01_CMN_Boots_05_to_08");
	if( testA )
	{
		item_sendEquippedInfo(testA, client);
	}
	testB = item_createFromTemplate("Weapon_Avatar_MachineGun_Physical_ELT_08_to_12");
	if( testB )
	{
		item_setLocationHomeinventory(testB, client);
		item_sendInfo(testB);
	}
	testB = item_createFromTemplate("Weapon_Avatar_Pistol_Physical_UNC_01_to_04");
	if( testB )
	{
		item_setLocationHomeinventory(testB, client);
		item_sendInfo(testB);
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
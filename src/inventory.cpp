#include "global.h"

#define AUGMENTATION_ARMOR		 51
#define AUGMENTATION_DECORATION	 36
#define AUGMENTATION_EQUIPABLE	 4
#define AUGMENTATION_ITEM		 6
#define AUGMENTATION_WEAPON		 2


item_t* inventory_CurrentWeapon(mapChannelClient_t *client)
{
	item_t *item = (item_t*)entityMgr_get(client->inventory.weaponDrawer[client->inventory.activeWeaponDrawer]);
	if( !item ) { return NULL; }
	return item;
}

/*
578
00000F81     5A - STORE_NAME          'RequestTooltipForItemTemplateId
*/
void item_recv_RequestTooltipForItemTemplateId(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) ) { return; }

	uint32 itemTemplateId = pym_unpackInt(&pums);
	itemTemplate_t *itemTemplate = gameData_getItemTemplateById(itemTemplateId);
	if( itemTemplate == NULL )
		return; // todo: even answer on a unknown template, else the client will continue to spam us with requests

	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, itemTemplateId);				// templateId
	pym_addInt(&pms, itemTemplate->item.classId);	// itemClass
	pym_dict_begin(&pms); // Info is a dict of { AugId : AugData }

	// we always send the item augmentation
	// Item start
	pym_addInt(&pms, AUGMENTATION_ITEM);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, !itemTemplate->item.notTradable);		// kItemIdx_Tradable		= 0
	pym_addInt(&pms, itemTemplate->item.maxHitPoints);		// kItemIdx_MaxHPs			= 1
	pym_addInt(&pms, itemTemplate->item.sellPrice);			// kItemIdx_BuybackPrice	= 2
	// kItemIdx_Requirements	= 3
	if( itemTemplate->item.reqLevel > 0 )
	{
		pym_list_begin(&pms);	
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 1); // REQ_XP_LEVEL
		pym_addInt(&pms, itemTemplate->item.reqLevel);		
		pym_tuple_end(&pms);
		pym_list_end(&pms);	
	}
	else
		pym_addNoneStruct(&pms); // no requirements					
	// Item requirements are min/max level or attributes, for req. equipment skills use the AUGMENTATION_EQUIPABLE skill info 
	//pym_tuple_begin(&pms);
	pym_addNoneStruct(&pms);								// kItemIdx_ModuleIds		= 4
	//pym_tuple_end(&pms);
	pym_addNoneStruct(&pms);								// kItemIdx_RaceIds			= 5
	pym_tuple_end(&pms);
	// item end
	// we always send the equipable augmentation if type is weapon or armor
	if( itemTemplate->item.type == ITEMTYPE_WEAPON || itemTemplate->item.type == ITEMTYPE_ARMOR )
	{
		// Equipable start
		pym_addInt(&pms, AUGMENTATION_EQUIPABLE);
		pym_tuple_begin(&pms);
		// kEquipableIdx_SkillInfo = 0 (SkillId, MinSkillLvl)
		pym_tuple_begin(&pms);
		if( itemTemplate->equipment.requiredSkillId <= 0 || itemTemplate->equipment.requiredSkillMinVal <= 0 )
		{
			pym_addNoneStruct(&pms); // Skill ID
			pym_addNoneStruct(&pms); // Skill Level
		}
		else
		{
			pym_addInt(&pms, itemTemplate->equipment.requiredSkillId);
			pym_addInt(&pms, itemTemplate->equipment.requiredSkillMinVal);
		}
		pym_tuple_end(&pms);
		// kEquipableIdx_ResistList = 1 (damageType, resistValue)
		pym_tuple_begin(&pms);
		pym_tuple_end(&pms);
		pym_tuple_end(&pms);
		// Equipable end
	}
	// armor specific augmentation data
	if( itemTemplate->item.type == ITEMTYPE_ARMOR )
	{
		pym_addInt(&pms, AUGMENTATION_ARMOR);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, itemTemplate->armor.regenRate); // regen rate
		pym_tuple_end(&pms);
	}
	// weapon specific augmentation data
	if( itemTemplate->item.type == ITEMTYPE_WEAPON )
	{
		// Weapon start
		pym_addInt(&pms, AUGMENTATION_WEAPON);
		pym_tuple_begin(&pms);
			pym_addInt(&pms, itemTemplate->weapon.minDamage);			//kWeaponIdx_MinDamage		= 0
			pym_addInt(&pms, itemTemplate->weapon.maxDamage);			//kWeaponIdx_MaxDamage		= 1
			pym_addInt(&pms, itemTemplate->weapon.ammoClassId);			//kWeaponIdx_AmmoClassId	= 2 // standard grade cartridge
			pym_addInt(&pms, itemTemplate->weapon.clipSize);			//kWeaponIdx_ClipSize		= 3
			pym_addInt(&pms, itemTemplate->weapon.ammoPerShot);			//kWeaponIdx_AmmoPerShot	= 4
			pym_addInt(&pms, itemTemplate->weapon.damageType);			//kWeaponIdx_DamageType		= 5 // normal
			pym_addInt(&pms, itemTemplate->weapon.windupTime);			//kWeaponIdx_WindupTime		= 6 // autofire
			pym_addInt(&pms, itemTemplate->weapon.recoveryTime);		//kWeaponIdx_RecoveryTime   = 7
			pym_addInt(&pms, itemTemplate->weapon.refireTime);			//kWeaponIdx_RefireTime		= 8
			pym_addInt(&pms, itemTemplate->weapon.reloadTime);			//kWeaponIdx_ReloadTime		= 9
			pym_addInt(&pms, itemTemplate->weapon.range);				//kWeaponIdx_Range			= 10
			pym_addInt(&pms, itemTemplate->weapon.aeRadius);			//kWeaponIdx_AERadius		= 11
			//pym_addInt(&pms, itemTemplate->aeType);					//kWeaponIdx_AEType			= 12
			if (itemTemplate->weapon.aeType == 0)
				pym_addNoneStruct(&pms);
			else
				pym_addInt(&pms, itemTemplate->weapon.aeType);			       //kWeaponIdx_AEType      = 12

			//kWeaponIdx_AltFire		= 13
			pym_tuple_begin(&pms);		
			if (itemTemplate->weapon.altActionId != 0)
			{
				pym_addInt(&pms, itemTemplate->weapon.altMaxDamage);	// kWeaponAltIdx_MaxDamage	= 0
				pym_addInt(&pms, itemTemplate->weapon.altDamageType);	// kWeaponAltIdx_DamageType = 1
				pym_addInt(&pms, itemTemplate->weapon.altRange);		// kWeaponAltIdx_Range		= 2
				pym_addInt(&pms, itemTemplate->weapon.altAERadius);		// kWeaponAltIdx_AERadius	= 3
				pym_addInt(&pms, itemTemplate->weapon.altAEType);// kWeaponAltIdx_AEType		= 4
			} else { pym_addNoneStruct(&pms); }
			pym_tuple_end(&pms);
			pym_addInt(&pms, itemTemplate->weapon.attackType);			//kWeaponIdx_AttackType		= 14 // ranged
			pym_addInt(&pms, itemTemplate->weapon.toolType);			//kWeaponIdx_ToolType		= 15 // rifle
		pym_tuple_end(&pms);
		// Weapon end
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 12, 579, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Removes the item from the slot and also sends an update to the client
 */
void inventory_removeItemBySlot(mapChannelClient_t *client, sint32 inventoryType, sint32 slotIndex)
{
	// get entityId from slot
	sint64 entityId = 0;
	if( inventoryType == INVENTORY_PERSONAL )
	{
		entityId = client->inventory.personalInventory[slotIndex];
		client->inventory.personalInventory[slotIndex] = 0; // update slot
	}
	else if( inventoryType == INVENTORY_EQUIPPEDINVENTORY )
	{
		entityId = client->inventory.equippedInventory[slotIndex];
		client->inventory.equippedInventory[slotIndex] = 0; // update slot
	}
	else if( inventoryType == INVENTORY_WEAPONDRAWERINVENTORY )
	{
		entityId = client->inventory.weaponDrawer[slotIndex];
		client->inventory.weaponDrawer[slotIndex] = 0; // update slot
	}
	if( entityId == 0 )
	{
		printf("inventory_removeItemBySlot: Invalid inventoryType(%d)/slotIndex(%d)\n", inventoryType, slotIndex);
		return;
	}
	// send inventoryRemoveItem
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, inventoryType); // inventoryType
	pym_addInt(&pms, entityId); // entityID
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, METHODID_INVENTORYREMOVEITEM, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Adds the item to the inventory and sends an update to the client
 */
void inventory_addItemBySlot(mapChannelClient_t *client, sint32 inventoryType, sint64 entityId, sint32 slotIndex)
{
	item_t* itemEntity = (item_t*)entityMgr_get(entityId);
	if( itemEntity == NULL )
		return;
	// set entityId in slot
	if( inventoryType == INVENTORY_PERSONAL )
	{
		client->inventory.personalInventory[slotIndex] = entityId; // update slot
		itemEntity->locationSlotIndex = slotIndex + 0;
	}
	else if( inventoryType == INVENTORY_EQUIPPEDINVENTORY )
	{
		client->inventory.equippedInventory[slotIndex] = entityId; // update slot
		itemEntity->locationSlotIndex = slotIndex + 50*5;
	}
	else if( inventoryType == INVENTORY_WEAPONDRAWERINVENTORY )
	{
		client->inventory.weaponDrawer[slotIndex] = entityId; // update slot
		itemEntity->locationSlotIndex = slotIndex + 50*5 + 17;
	}
	else
	{
		printf("inventory_addItemBySlot: Invalid inventoryType(%d)\n", inventoryType);
		return;
	}
	// update item location
	itemEntity->locationEntityId = client->player->actor->entityId;
	// send inventoryAddItem
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, inventoryType); // inventoryType
	pym_addInt(&pms, entityId); // entityID
	pym_addInt(&pms, slotIndex); // slotIndex
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, METHODID_INVENTORYADDITEM, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Creates an item entity from a item template
 */
item_t *item_create(itemTemplate_t *itemTemplate, sint32 stacksize)
{
	if( itemTemplate == NULL )
		return NULL; // "no-template items" must not exist
	item_t *item = (item_t*)malloc(sizeof(item_t));
	RtlZeroMemory(item, sizeof(item_t));
	item->entityId = entityMgr_getFreeEntityIdForItem();
	item->itemTemplate = itemTemplate;
	item->locationEntityId = 0;
	item->locationSlotIndex = 0;
	item->stacksize = stacksize;
	// register item
	entityMgr_registerEntity(item->entityId, item);
	return item;
}

/*
 * Makes a 1:1 copy of the item with a different entityId
 * The new item will have a stacksize of newStacksize
 */
item_t* item_duplicate(item_t* item, sint32 newStacksize)
{
	item_t* newItem = item_createFromTemplateId(item->itemTemplate->item.templateId, newStacksize);
	newItem->stacksize = newStacksize;
	// todo: Once we support stuff like item modifiers/modules we need to duplicate them in this function too
	return newItem;
}

void item_setLocationEquippedinventory(item_t *item, mapChannelClient_t *owner)
{
	item->locationEntityId = owner->clientEntityId;
	item->locationSlotIndex = gameData_getEquipmentClassIdSlot(item->itemTemplate->item.classId);
}

/*
 * Tries to find an empty slot in the player's inventory
 * Return -1 if no free slot was found, returns the slot index on success
 */
sint32 item_findFreePlayerinventorySpace(mapChannelClient_t *owner, sint32 itemType)
{
	if( itemType == ITEMTYPE_WEAPON || itemType == ITEMTYPE_ARMOR )
	{
		for(sint32 i=0; i<50; i++)
		{
			if( owner->inventory.slot[i] == 0 )
			{
				return i;
			}
		}		
	}
	return -1;
}

/*
 * Tries to add/merge the item into the players inventory and send the packets
 * Note previously to this call, the player should already know about the item (use item_sendItemCreation())
 * If it fails, the return value is NULL (inventory full)
 * If it succeeds, it returns the item or the item it was merged with
 */
item_t* inventory_addItemToInventory(mapChannelClient_t *client, item_t* item)
{
	pyMarshalString_t pms;
	// get item category offset
	sint32 itemCategoryOffset = (item->itemTemplate->item.inventoryCategory-1);
	if( itemCategoryOffset < 0 || itemCategoryOffset >= 5 )
	{
		printf("inventory_addItemToInventory: The item inventory category(%d) is invalid\n", itemCategoryOffset);
		return NULL;
	}
	itemCategoryOffset *= 50;
	bool stackSizeChanged = false;
	// see if we can merge the item into an already existing item
	for(sint32 i=0; i<50; i++)
	{
		if( client->inventory.personalInventory[itemCategoryOffset+i] != 0 )
		{
			// get item
			item_t* slotItem = (item_t*)entityMgr_get(client->inventory.personalInventory[itemCategoryOffset+i]);
			// same item template?
			if( slotItem->itemTemplate->item.templateId != item->itemTemplate->item.templateId )
				continue;
			// calculate how many items we can add to the stack
			sint32 stackAdd = slotItem->itemTemplate->item.stacksize - slotItem->stacksize;
			if( stackAdd == 0 )
				continue;
			// add item to empty slot
			sint32 stackMove = min(stackAdd, item->stacksize);
			slotItem->stacksize += stackMove;
			item->stacksize -= stackMove;
			// notify client of changed stack count
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_addInt(&pms,slotItem->stacksize);
			pym_tuple_end(&pms);
			netMgr_pythonAddMethodCallRaw(client->cgm, slotItem->entityId, SetStackCount, pym_getData(&pms), pym_getLen(&pms));
			// also remember that the source item changed stack count too
			stackSizeChanged = true;
			// do we still need to continue?
			if( item->stacksize == 0 )
			{
				// destroy the item
				item_sendItemDestruction(client, item);
				item_free(item);
				// return the 'new' item instead
				return slotItem;
			}
		}
	}
	// does the item has a different stack countsize?
	if( stackSizeChanged )
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms,item->stacksize);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, SetStackCount, pym_getData(&pms), pym_getLen(&pms));
	}
	// find a free slot
	for(sint32 i=0; i<50; i++)
	{
		if( client->inventory.personalInventory[itemCategoryOffset+i] == 0 )
		{
			// add item to empty slot
			inventory_addItemBySlot(client, INVENTORY_PERSONAL, item->entityId, itemCategoryOffset+i);
			return item;
		}
	}
	return NULL;
}


void item_recv_PersonalInventoryMoveItem(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 fromSlot = pym_unpackInt(&pums);
	sint32 toSlot = pym_unpackInt(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// remove item
	if( fromSlot == toSlot )
		return;
	if( fromSlot < 0 || fromSlot >= 250 )
		return;
	if( toSlot < 0 || toSlot >= 250 )
		return;
	unsigned long long entityId = client->inventory.personalInventory[fromSlot];
	if( entityId == NULL )
		return;
	inventory_removeItemBySlot(client, INVENTORY_PERSONAL, fromSlot);
	inventory_addItemBySlot(client, INVENTORY_PERSONAL, entityId, toSlot);
}

void item_recv_RequestEquipArmor(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 srcSlot = pym_unpackInt(&pums); // Source Slot
	sint32 srcInventory = pym_unpackInt(&pums); // Source Inventory
	sint32 dstSlot = pym_unpackInt(&pums); // Destination Slot
	if( pums.unpackErrorEncountered )
		return;
	if( srcInventory != 1 )
	{
		printf("Unsupported inventory\n");
		return;
	}
	if( srcSlot < 0 || srcSlot > 50 )
		return;
	if( dstSlot < 0 || dstSlot > 17 )
		return;
	sint64 entityId_equippedItem = 0;
	sint64 entityId_inventoryItem = 0;
	entityId_equippedItem = client->inventory.equippedInventory[dstSlot]; // the old equipped item (can be none)
	entityId_inventoryItem = client->inventory.personalInventory[srcSlot]; // the new equipped item (can be none)
	// can we equip the item?
	item_t *itemToEquip = NULL;
	if( entityId_inventoryItem )
	{
		itemToEquip = (item_t*)entityMgr_get(entityId_inventoryItem);
		// min level criteria met?
		if( itemToEquip && client->player->actor->stats.level < itemToEquip->itemTemplate->item.reqLevel )
		{
			// level too low, cannot equip item
			return;
		}
	}	
	// swap items on the client and server
	if( client->inventory.personalInventory[srcSlot] )
		inventory_removeItemBySlot(client, INVENTORY_PERSONAL, srcSlot);
	if( client->inventory.equippedInventory[dstSlot] )
		inventory_removeItemBySlot(client, INVENTORY_EQUIPPEDINVENTORY, dstSlot);
	if( entityId_equippedItem )
		inventory_addItemBySlot(client, INVENTORY_PERSONAL, entityId_equippedItem, srcSlot);
	if( entityId_inventoryItem )
		inventory_addItemBySlot(client, INVENTORY_EQUIPPEDINVENTORY, entityId_inventoryItem, dstSlot);
	// update appearance
	pyMarshalString_t pms;
	
	if( !itemToEquip )
	{
		// remove item graphic if dequipped
		item_t* prevEquippedItem = (item_t*)entityMgr_get(entityId_equippedItem);
		manifestation_removeAppearanceItem(client->player, prevEquippedItem->itemTemplate->item.classId);
	}
	else
	{
		manifestation_setAppearanceItem(client->player, itemToEquip->itemTemplate->item.classId, 0xFF808080);
	}
	manifestation_updateAppearance(client);
	// update armor
	manifestation_updateStatsValues(client, false);
	manifestation_buildAttributeInfoPacket(client, &pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(client, client->player->actor->entityId, METHODID_ATTRIBUTEINFO, pym_getData(&pms), pym_getLen(&pms));
}

void item_recv_RequestEquipWeapon(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 srcSlot = pym_unpackInt(&pums);
	sint32 srcInventory = pym_unpackInt(&pums);
	sint32 dstSlot = pym_unpackInt(&pums);
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
	sint64 entityId_equippedItem = 0;
	sint64 entityId_inventoryItem = 0;
	entityId_equippedItem = client->inventory.weaponDrawer[dstSlot]; // the old equipped item (can be none)
	entityId_inventoryItem = client->inventory.personalInventory[srcSlot]; // the new equipped item (can be none)
	// can we equip the item?
	item_t *itemToEquip = NULL;
	if( entityId_inventoryItem )
	{
		itemToEquip = (item_t*)entityMgr_get(entityId_inventoryItem);
		// min level criteria met?
		if( itemToEquip && client->player->actor->stats.level < itemToEquip->itemTemplate->item.reqLevel )
		{
			// level too low, cannot equip item
			return;
		}
	}	
	// swap items on the client and server
	if( client->inventory.personalInventory[srcSlot] )
		inventory_removeItemBySlot(client, INVENTORY_PERSONAL, srcSlot);
	if( client->inventory.weaponDrawer[dstSlot] )
		inventory_removeItemBySlot(client, INVENTORY_WEAPONDRAWERINVENTORY, dstSlot);
	if( entityId_equippedItem )
		inventory_addItemBySlot(client, INVENTORY_PERSONAL, entityId_equippedItem, srcSlot);
	if( entityId_inventoryItem )
		inventory_addItemBySlot(client, INVENTORY_WEAPONDRAWERINVENTORY, entityId_inventoryItem, dstSlot);
	// update appearance
	if( dstSlot == client->inventory.activeWeaponDrawer )
	{
		inventory_notifyEquipmentUpdate(client);
		//pyMarshalString_t pms;
		if( !itemToEquip )
		{
			// remove item graphic if dequipped
			item_t* prevEquippedItem = (item_t*)entityMgr_get(entityId_equippedItem);
			manifestation_removeAppearanceItem(client->player, prevEquippedItem->itemTemplate->item.classId);
		}
		else
		{
			manifestation_setAppearanceItem(client->player, itemToEquip->itemTemplate->item.classId, 0xFF808080);
		}
		manifestation_updateAppearance(client);
	}	
}

/*
	maybe move the weapon related entries to manifestation? // i agree
*/
void item_recv_RequestArmWeapon(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen)
{
	// RequestArmWeapon : 507
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint32 slot = pym_unpackInt(&pums);
	cm->inventory.activeWeaponDrawer = (sint8)slot;
	// 574 Recv_WeaponDrawerSlot(self, slotNum, bRequested = True):
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, slot);
	pym_addBool(&pms, true);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, cm->player->actor->entityId, 574, pym_getData(&pms), pym_getLen(&pms));
	//tell client to change weapon appearance //---@ 4ae9585968 Disastorm
 	inventory_notifyEquipmentUpdate(cm);
	item_t *item = (item_t*)entityMgr_get(cm->inventory.weaponDrawer[cm->inventory.activeWeaponDrawer]);
	if( !item )
  		return;
	manifestation_setAppearanceItem(cm->player, item->itemTemplate->item.classId, 0xFF808080);
	manifestation_updateAppearance(cm);
	// update ammo info
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, item->weaponData.ammoCount);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, item->entityId, WeaponAmmoInfo, pym_getData(&pms), pym_getLen(&pms));
}
 
void item_recv_RequestWeaponDraw(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	// TODO!
	//manifestation_updateWeaponReadyState(client); ?
	printf("TODO: "); puts(__FUNCTION__);
	// weapon ready
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, true);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, client->player->actor, client->player->actor->entityId, 575, pym_getData(&pms), pym_getLen(&pms));
}

void item_recv_RequestWeaponStow(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	// TODO!
	//manifestation_updateWeaponReadyState(client); ?
	printf("TODO: "); puts(__FUNCTION__);
	// weapon ready
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, false);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(client->mapChannel, client->player->actor, client->player->actor->entityId, 575, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Will reduce the stack count of the passed item by the given count
 * If the stack count reaches zero, the item is deleted and removed from the inventory
 */
void inventory_reduceStackCount(mapChannelClient_t *client, item_t* item, sint32 stackDecreaseCount)
{
	pyMarshalString_t pms;
	if( item->locationEntityId != client->player->actor->entityId )
		return; // item is not on this client's inventory
	sint32 newStackCount = (sint32)item->stacksize - stackDecreaseCount;
	if( newStackCount <= 0 )
	{
		// destroy item
		inventory_removeItemBySlot(client, INVENTORY_PERSONAL, item->locationSlotIndex); // todo: locationSlotIndex is the slot in the inventory union that spans ALL inventory types? 
		item_sendItemDestruction(client, item); // make sure the client forgets about the item entity
		item_free(item);
	}
	else
	{
		// update stack count
		// item->locationSlotIndex
		item->stacksize = newStackCount;
		// set stackcount
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms,item->stacksize ); // stacksize
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, SetStackCount, pym_getData(&pms), pym_getLen(&pms));
	}
}

/*
 * Called when the reload action finishes
 */
void _cb_item_recv_RequestWeaponReload_actionUpdate(mapChannel_t* mapChannel, actor_t* actor, sint32 newActionState)
{
	pyMarshalString_t pms;
	if( newActionState == ACTOR_ACTION_STATE_COMPLETE )
	{
		mapChannelClient_t* client = actor->owner;
		// get the weapon item
		item_t *item = (item_t*)entityMgr_get(client->inventory.weaponDrawer[client->inventory.activeWeaponDrawer]);
		if( !item )
			return;
		// find and eat a piece of ammunition
		sint32 ammoClassId = item->itemTemplate->weapon.ammoClassId;
		bool foundAmmo = false;
		sint32 foundAmmoAmount = 0;
		for(sint32 i=0; i<50; i++)
		{
			if( client->inventory.personalInventory[INVENTORY_SLOTOFFSET_CATEGORY_CONSUMABLE+i] == 0 )
				continue;
			item_t *itemAmmo = (item_t*)entityMgr_get(client->inventory.personalInventory[INVENTORY_SLOTOFFSET_CATEGORY_CONSUMABLE+i]);
			if( !itemAmmo )
				return;
			if( itemAmmo->itemTemplate->item.classId == ammoClassId )
			{
				// consume ammo
				sint32 ammoToGrab = min(item->itemTemplate->weapon.clipSize-foundAmmoAmount, itemAmmo->stacksize);
				foundAmmoAmount += ammoToGrab;
				inventory_reduceStackCount(client, itemAmmo, ammoToGrab);
				foundAmmo = true;
				if( foundAmmoAmount == item->itemTemplate->weapon.clipSize )
					break;
			}
		}
		if( foundAmmo == false )
			return; // no ammo found -> Todo: Tell the client?
		// update the ammo count
		item->weaponData.ammoCount = foundAmmoAmount;
		// send new ammo count -> Not needed, it is done as part of the PerformRecovery packet
		// send action complete packet
		pym_init(&pms);
		pym_tuple_begin(&pms);  							// Packet Start
		pym_addInt(&pms, actor->currentAction.actionId);	// Action ID
		pym_addInt(&pms, actor->currentAction.actionArgId);	// Arg ID
		pym_addInt(&pms, item->weaponData.ammoCount);		// new ammo count
		pym_tuple_end(&pms); 								// Packet End
		netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, METHODID_PERFORMRECOVERY, pym_getData(&pms), pym_getLen(&pms));
		/*pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, item->weaponData.ammoCount);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, WeaponAmmoInfo, pym_getData(&pms), pym_getLen(&pms));*/
	}
	else
		__debugbreak(); // reload interruption is still todo
}

void item_recv_RequestWeaponReload(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen, bool tellSelf)
{
	pyMarshalString_t pms;
	// get the weapon item
	item_t *item = (item_t*)entityMgr_get(client->inventory.weaponDrawer[client->inventory.activeWeaponDrawer]);
	if( !item )
		return;
	// find and eat a piece of ammunition
	sint32 ammoClassId = item->itemTemplate->weapon.ammoClassId;
	bool foundAmmo = false;
	for(sint32 i=0; i<50; i++)
	{
		if( client->inventory.personalInventory[INVENTORY_SLOTOFFSET_CATEGORY_CONSUMABLE+i] == 0 )
			continue;
		item_t *itemAmmo = (item_t*)entityMgr_get(client->inventory.personalInventory[INVENTORY_SLOTOFFSET_CATEGORY_CONSUMABLE+i]);
		if( !itemAmmo )
			return;
		if( itemAmmo->itemTemplate->item.classId == ammoClassId )
		{
			// consume one piece of ammo
			foundAmmo = true;
			break;
		}
	}
	if( foundAmmo == false )
		return; // no ammo found -> Todo: Tell the client?
	// send action start
	sint32 reloadActionId = 134;
	sint32 reloadActionArgId = 1;// todo: Use correct argId depending on weapon type
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, reloadActionId); // actionId -> WEAPON_RELOAD
	pym_addInt(&pms, reloadActionArgId); // actionArgId
	pym_tuple_end(&pms);
	if( tellSelf )
	{
		// if item_recv_RequestWeaponReload() is not called by client, then the server issued the weapon reload request
		// tell all clients about the action start
		netMgr_cellDomain_pythonAddMethodCallRaw(client, client->player->actor->entityId, PerformWindup, pym_getData(&pms), pym_getLen(&pms));
	}
	else
	{
		// if the client sent the request, tell everyone but the client to start the reload action
		// In this case, the action is created on the client without actually having to trigger it via Recv_PerformWindup
		netMgr_cellDomain_pythonAddMethodCallRawIgnoreSelf(client, client->player->actor->entityId, PerformWindup, pym_getData(&pms), pym_getLen(&pms));
	}
	// start reload action
	actor_startActionOnEntity(client->mapChannel, client->player->actor, 0, reloadActionId, reloadActionArgId, 1500, 0, _cb_item_recv_RequestWeaponReload_actionUpdate);
}

/*
 * Send item entity destruction to client
 */
void item_sendItemDestruction(mapChannelClient_t *client, item_t *item)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, item->entityId); // entityID
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, DestroyPhysicalEntity, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Destroys the item instance and frees the entityId
 * Before calling this method, make sure no inventory and no vendor references this item
 * You should also call _sendItemDestruction() to clients that know about this item before calling this method
 */
void item_free(item_t* item)
{
	entityMgr_unregisterEntity(item->entityId);
	free(item);
}

/*
 * Send item entity creation and item data to client
 */
void item_sendItemDataToClient(mapChannelClient_t *client, item_t *item)
{
	pyMarshalString_t pms;
	// create item entity
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, item->entityId);									// entityID
	pym_addInt(&pms, item->itemTemplate->item.classId);					// classID
	pym_addNoneStruct(&pms);											// entityData (dunno)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	// send item info
	pym_init(&pms);
	pym_tuple_begin(&pms);
		pym_addInt(&pms, item->itemTemplate->item.currentHitPoints);			// 'currentHitPoints' --> Displayed as "Armor: x" in case of armor
		pym_addInt(&pms, item->itemTemplate->item.maxHitPoints);				// 'maxHitPoints'
		//pym_addUnicode(&pms, "Richard Garriott");								// Modified By
		pym_addNoneStruct(&pms);
		pym_addInt(&pms, item->itemTemplate->item.templateId);					// 'itemTemplateId'
		pym_addBool(&pms, item->itemTemplate->item.hasSellableFlag);			// 'hasSellableFlag'
		pym_addBool(&pms, item->itemTemplate->item.hasCharacterUniqueFlag);		// 'hasCharacterUniqueFlag'
		pym_addBool(&pms, item->itemTemplate->item.hasAccountUniqueFlag);		// 'hasAccountUniqueFlag'
		pym_addBool(&pms, item->itemTemplate->item.hasBoEFlag);					// 'hasBoEFlag'
		// 'classModuleIds' (classLootModuleIds)
		pym_list_begin(&pms);
		pym_list_end(&pms);
		// 'lootModuleIds'
		pym_list_begin(&pms);
		pym_list_end(&pms);
		pym_addInt(&pms, item->itemTemplate->item.qualityId);				// 'qualityId'
		/*	QualitiIds:
			MISSION = 1		NORMAL = 2		UNCOMMON = 3	
			RARE = 4		EPIC = 5		LEGENDARY = 6
			JUNK = 7	*/
		pym_addInt(&pms, item->itemTemplate->item.boundToCharacter);		// 'boundToCharacter'
		pym_addInt(&pms, item->itemTemplate->item.notTradable);				// 'notTradable'
		pym_addInt(&pms, item->itemTemplate->item.notPlaceableInLockbox);	// 'notPlaceableInLockbox'
		pym_addInt(&pms, item->itemTemplate->item.inventoryCategory);		// 'inventoryCategory'
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 469, pym_getData(&pms), pym_getLen(&pms)); // ItemInfo

	if( item->itemTemplate->item.type == ITEMTYPE_WEAPON )
	{
		// send weapon data
		pym_init(&pms);
		pym_tuple_begin(&pms);	
			pym_addNoneStruct(&pms);//pym_addUnicode(&pms, "testName"); // weaponName - not used
			pym_addInt(&pms, item->itemTemplate->weapon.clipSize);				// clipSize
			pym_addInt(&pms, item->itemTemplate->weapon.currentAmmo);			// currentAmmo
			pym_addFloat(&pms, item->itemTemplate->weapon.aimRate);			// aimRate
			pym_addInt(&pms, item->itemTemplate->weapon.reloadTime);			// reloadTime
			pym_addInt(&pms, item->itemTemplate->weapon.altActionId);			// altActionId
			pym_addInt(&pms, item->itemTemplate->weapon.altActionArg);			// altActionArg
			pym_addInt(&pms, item->itemTemplate->weapon.aeType);				// aeType
			pym_addInt(&pms, item->itemTemplate->weapon.aeRadius);				//pym_addInt(&pms, 100); // aeRadius
			pym_addInt(&pms, item->itemTemplate->weapon.recoilAmount);			// recoilAmount
			pym_addNoneStruct(&pms);									// reuseOverride
			pym_addInt(&pms, item->itemTemplate->weapon.coolRate);				// coolRate
			pym_addFloat(&pms, item->itemTemplate->weapon.heatPerShot);		// heatPerShot
			pym_addInt(&pms, item->itemTemplate->weapon.toolType);				// toolType ( PISTOL = 8 )
			pym_addBool(&pms, item->itemTemplate->weapon.isJammed);			// isJammed
			pym_addInt(&pms, item->itemTemplate->weapon.ammoPerShot);			// ammoPerShot
			pym_addString(&pms, "");									// cameraProfile
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 237, pym_getData(&pms), pym_getLen(&pms)); //WeaponInfo

		// weapon ammo info
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, item->weaponData.ammoCount);
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, WeaponAmmoInfo, pym_getData(&pms), pym_getLen(&pms));

		// send jammed state
		//pym_init(&pms);
		//pym_tuple_begin(&pms);	
		//pym_addBool(&pms, false); // isJammed
		//pym_tuple_end(&pms);
		//netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 585, pym_getData(&pms), pym_getLen(&pms)); //WeaponInfo
	}
	if( item->itemTemplate->item.type == ITEMTYPE_ARMOR )
	{
		// Recv_ArmorInfo <-- doesnt do anything really
		// we dont need to send this, it is just an empty method on the client which has no effect
		//pym_init(&pms);
		//pym_tuple_begin(&pms);	
		//pym_addInt(&pms, item->itemTemplate->item.currentHitPoints+2); // currentHitPoints
		//pym_addInt(&pms, item->itemTemplate->item.maxHitPoints+3); // maxHitPoints
		//pym_tuple_end(&pms);
		//netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, 406, pym_getData(&pms), pym_getLen(&pms)); //ArmorInfo
	}
	// set stackcount
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, item->stacksize); // stacksize
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, item->entityId, SetStackCount, pym_getData(&pms), pym_getLen(&pms));
}

///*
// * Used to send the client item info + item add when logging in
// */
//void item_sendEquippedInfo(item_t *item, mapChannelClient_t *owner)
//{
//	item_setLocationEquippedinventory(item, owner);
//	if( item->entityId == 0 )
//		return; 
//	mapChannelClient_t *client = (mapChannelClient_t*)entityMgr_get(item->locationEntityId);
//	if( client == NULL )
//		return;
//	if( item->locationSlotIndex >= 0 || item->locationSlotIndex < 16 )
//	{
//		client->inventory.equippedInventory[item->locationSlotIndex] = item->entityId;
//		item_sendItemDataToClient(client, item);
//		inventory_addItemBySlot(client, INVENTORY_EQUIPPEDINVENTORY, item->entityId, item->locationSlotIndex);
//	}
//}
//

/*
 * Sends the item entity creation and info the the client
 */
void item_sendItemCreation(mapChannelClient_t* client, item_t *item)
{
	item_sendItemDataToClient(client, item);
	//if( item->entityId == 0 )
	//	return; // no location
	//if( client == NULL )
	//	return;
	//if( item->locationSlotIndex >= 0 || item->locationSlotIndex < 250 )
	//{
	//	inventory_addItemBySlot(client, INVENTORY_PERSONAL, item->entityId, item->locationSlotIndex);
	//}
	//else
	//	__debugbreak(); // todo
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

/*
 * Creates an item from a item templateId
 */
item_t* item_createFromTemplateId(uint32 itemTemplateId, sint32 stacksize)
{
	itemTemplate_t *itemTemplate = gameData_getItemTemplateById(itemTemplateId);
	if( !itemTemplate )
		return NULL;
	item_t *item = item_create(itemTemplate, stacksize);
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
	//for(sint32 i=0; i<250; i++)
	//{
	//	if( client->inventory.personalInventory[i] != 0 )
	//	{
	//		pym_tuple_begin(&pms);
	//		pym_addInt(&pms, 0+i); // key -> slot
	//		pym_addInt(&pms, client->inventory.personalInventory[i]); // value -> entityId of item
	//		pym_tuple_end(&pms);
	//	}
	//}
	// note: Actually, we dont need to send any items for the personal inventory, as it has no effect
	pym_list_end(&pms);
	pym_addInt(&pms, 250); // maxSize (50 per category)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, METHODID_INVENTORYCREATE, pym_getData(&pms), pym_getLen(&pms));
	// create weapon drawer
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, INVENTORY_WEAPONDRAWERINVENTORY); // inventoryType
	pym_list_begin(&pms);
	/* {itemId, slotIndex} list */
	pym_list_end(&pms);
	pym_addInt(&pms, 5); // maxSize
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, METHODID_INVENTORYCREATE, pym_getData(&pms), pym_getLen(&pms));
	// create equipped inventory
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, INVENTORY_EQUIPPEDINVENTORY); // inventoryType
	pym_list_begin(&pms);
	/* {itemId, slotIndex} list */
	pym_list_end(&pms);
	pym_addInt(&pms, 5); // maxSize
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 9, METHODID_INVENTORYCREATE, pym_getData(&pms), pym_getLen(&pms));

	// test items
	// (item templates + properties should be read from the db and created dynamically)
	item_t* testItem = item_createFromTemplateId(127973, 1);
	// we manually update the location of the item (to avoid sending data yet)
	testItem->locationEntityId = client->clientEntityId;
	client->inventory.personalInventory[3] = testItem->entityId;

	// item 2
	testItem = item_createFromTemplateId(127982, 1);
	client->inventory.personalInventory[4] = testItem->entityId;
	// item 3
	testItem = item_createFromTemplateId(45857, 1);
	client->inventory.personalInventory[0] = testItem->entityId;
	// item 4
	testItem = item_createFromTemplateId(17131, 1);
	client->inventory.personalInventory[1] = testItem->entityId;
	// item 5
	testItem = item_createFromTemplateId(17384, 1);
	client->inventory.personalInventory[15] = testItem->entityId;
	// item 6 (cartridges)
	testItem = item_createFromTemplateId(45031, 40);
	client->inventory.personalInventory[50+0] = testItem->entityId;
	// weapon drawer
	testItem = item_createFromTemplateId(17131, 1);
	client->inventory.weaponDrawer[0] = testItem->entityId;


	// send inventory data - personal inventory
	for(sint32 i=0; i<250; i++)
	{
		if( client->inventory.slot[i] == 0 )
			continue;
		// item in slot present
		// get item handle
		item_t* slotItem = (item_t*)entityMgr_get(client->inventory.slot[i]);
		item_sendItemCreation(client, slotItem);
		// make the item appear on the client
		inventory_addItemBySlot(client, INVENTORY_PERSONAL, client->inventory.slot[i], i);
	}
	// send inventory data - weapon drawer
	for(sint32 i=0; i<5; i++)
	{
		if( client->inventory.weaponDrawer[i] == 0 )
			continue;
		// item in slot present
		// get item handle
		item_t* slotItem = (item_t*)entityMgr_get(client->inventory.weaponDrawer[i]);
		item_sendItemCreation(client, slotItem);
		// make the item appear on the client
		inventory_addItemBySlot(client, INVENTORY_WEAPONDRAWERINVENTORY, client->inventory.weaponDrawer[i], i);
		// update appearance
		manifestation_setAppearanceItem(client->player, slotItem->itemTemplate->item.classId, 0xFF808080);
	}
	manifestation_updateAppearance(client);
	inventory_notifyEquipmentUpdate(client);
	// send inventory data - equipment
	
	
	// update appearance
	// manifestation_setAppearanceItem





	// test
	/*item_t *testA;
	item_t *testB;

	testA = item_createFromTemplate("Armor_T1_MotorAssist_V09_RAR_Boots_01_to_02");
	if (testA)
	{
		item_sendEquippedInfo(testA, client);
		manifestation_setAppearanceItem(client->player, testA->itemTemplate->item.classId, 0xFF808080);
	}
	testA = item_createFromTemplate("Armor_T1_MotorAssist_V09_RAR_Gloves_01_to_02");
	if (testA)
	{
		item_sendEquippedInfo(testA, client);
		manifestation_setAppearanceItem(client->player, testA->itemTemplate->item.classId, 0xFF808080);
	}
	testA = item_createFromTemplate("Armor_T1_MotorAssist_V09_RAR_Legs_01_to_02");
	if (testA)
	{
		item_sendEquippedInfo(testA, client);
		manifestation_setAppearanceItem(client->player, testA->itemTemplate->item.classId, 0xFF808080);
	}
	testA = item_createFromTemplate("Armor_T1_MotorAssist_V09_RAR_Vest_01_to_02");
	if (testA)
	{
		item_sendEquippedInfo(testA, client);
		manifestation_setAppearanceItem(client->player, testA->itemTemplate->item.classId, 0xFF808080);
	}
	testA = item_createFromTemplate("Armor_T1_MotorAssist_V09_RAR_Helmet_01_to_02");
	if (testA)
	{
		item_sendEquippedInfo(testA, client);
		manifestation_setAppearanceItem(client->player, testA->itemTemplate->item.classId, 0xFF808080);
	}
	
	manifestation_updateAppearance(client);

	testB = item_createFromTemplate("Weapon_Avatar_Rifle_Physical_UNC_01_to_05");
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
	testB = item_createFromTemplate("Weapon_Avatar_MachineGun_Physical_ELT_08_to_12");
	if( testB )
	{
		item_setLocationHomeinventory(testB, client);
		item_sendInfo(testB);
	}*/
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
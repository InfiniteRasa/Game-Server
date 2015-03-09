#include"global.h"

extern dynObjectType_t dynObjectType_lootdispenser;

typedef struct  
{
	uint8 state;
	bool alreadyInUse;
	sint64 userEntityId;
	sint64 lootOwnerEntityId;
	sint64 attachedToEnityId;
	std::vector<item_t*> list_itemLoot;
	sint32 lootCredits;
	bool fullyLooted; // set to true once all items are picked up (TODO: Looting of single items)
	//uint32 updateCounter; // used to detect if players leave the lootdispenser
}lootdispenser_t;

void lootdispenser_createLoot(mapChannel_t *mapChannel, dynObject_t* dynObject, lootdispenser_t* lootdispenserData, creature_t* creature, mapChannelClient_t* creatureKiller)
{
	creatureType_t* creatureType = creature->type;
	for(sint32 i=0; i<creatureType->lootTableSize; i++)
	{
		// roll chance
		float rolledChance = ((float)rand()) / ((float)(RAND_MAX+1)); // 0.0 to 0.999...
		// item not rolled?
		if( rolledChance >= creatureType->lootTable[i].chance )
			continue;
		// generate item
		sint32 stackRangeMod = (creatureType->lootTable[i].stacksizeMax - creatureType->lootTable[i].stacksizeMin) + 1;
		sint32 stacksize = creatureType->lootTable[i].stacksizeMin+(rand()%stackRangeMod);
		item_t* lootItem = item_createFromTemplateId(creatureType->lootTable[i].itemTemplateId, stacksize);
		if( lootItem == NULL )
		{
			printf("lootdispenser_createLoot: Failed to generate item loot for itemTemplateId %d and creatureTypeId %d\n", creatureType->lootTable[i].itemTemplateId, creatureType->typeId);
			continue;
		}
		// add item to loot
		lootdispenserData->list_itemLoot.push_back(lootItem);
	}
	// calculate credits
	sint32 credits = creature->actor.stats.level * 5;
	sint32 creditRange = creature->actor.stats.level * 2;
	credits += ((rand()%(creditRange*2+1))-creditRange);
	lootdispenserData->lootCredits = credits;
}

dynObject_t* lootdispenser_create(mapChannel_t *mapChannel, creature_t* creature, mapChannelClient_t* creatureKiller)
{
	dynObject_t *dynObject = _dynamicObject_create(10000035, &dynObjectType_lootdispenser);
	if( !dynObject )
		return NULL;
	dynObject->stateId = 0;
	dynamicObject_setPosition(dynObject, creature->actor.posX, creature->actor.posY, creature->actor.posZ);
	// setup lootdispenser specific data
	lootdispenser_t* objData = (lootdispenser_t*)malloc(sizeof(lootdispenser_t));
	memset(objData, 0x00, sizeof(lootdispenser_t));
	new(objData) lootdispenser_t();
	objData->attachedToEnityId = creature->actor.entityId;
	objData->lootOwnerEntityId = creatureKiller->player->actor->entityId;
	dynObject->objectData = objData;
	// calculate and generate loot
	lootdispenser_createLoot(mapChannel, dynObject, objData, creature, creatureKiller);
	// set rotation
	float randomRotY = 0.0f;
	float randomRotX = 0.0f;
	float randomRotZ = 0.0f;
	dynamicObject_setRotation(dynObject, randomRotY, randomRotX, randomRotZ);
	cellMgr_addToWorld(mapChannel, dynObject);
	// return object
	return dynObject;
}

void lootdispenser_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	lootdispenser_t* objData = (lootdispenser_t*)dynObject->objectData;
	//delete(objData->list_itemLoot);
	//todo
	//printf("lootdispenser_destroy: Todo\n");
}

#define LOOT_QUALITY_MISSION	1
#define LOOT_QUALITY_NORMAL		2
#define LOOT_QUALITY_UNCOMMON	3
#define LOOT_QUALITY_RARE		4
#define LOOT_QUALITY_EPIC		5
#define LOOT_QUALITY_LEGENDARY	6
#define LOOT_QUALITY_JUNK		7

void lootdispenser_appearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	lootdispenser_t* objData = (lootdispenser_t*)dynObject->objectData;
	// check if the loot player is in the list (we only want to send the data to the lootdispenser 'owner')
	// todo: Add support for group loot
	mapChannelClient_t* lootOwner = NULL;
	for(sint32 i=0; i<playerCount; i++)
	{
		if( playerList[i]->player->actor->entityId == objData->lootOwnerEntityId )
		{
			lootOwner = playerList[i];
			break;
		}
	}
	if( lootOwner == NULL )
		return; // not a loot owner, cant loot so we dont need to send loot info
	// sent item data of loot
	if( objData->list_itemLoot.empty() != true )
	{
		item_t **itemList = &objData->list_itemLoot[0];
		sint32 itemCount = objData->list_itemLoot.size();
		for(sint32 f=0; f<itemCount; f++)
		{
			item_t* lootItem = itemList[f];
			item_sendItemCreation(lootOwner, lootItem);
		}
	}
	// sent data only to loot player
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, objData->attachedToEnityId);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(lootOwner->cgm, dynObject->entityId, AttachInfo, pym_getData(&pms), pym_getLen(&pms));
	// example loot item
	// lootdispenserData->list_itemLoot
	// Recv_LootInfo
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	if( objData->list_itemLoot.empty() != true )
	{
		item_t **itemList = &objData->list_itemLoot[0];
		sint32 itemCount = objData->list_itemLoot.size();
		for(sint32 f=0; f<itemCount; f++)
		{
			item_t* lootItem = itemList[f];
			// (itemTemplateId, itemQuantity, actorId, partyId,)
			pym_addInt(&pms, lootItem->entityId);
			pym_tuple_begin(&pms);
			pym_addInt(&pms, lootItem->itemTemplate->item.templateId);
			pym_addInt(&pms, 1);
			pym_addInt(&pms, objData->lootOwnerEntityId);
			pym_addNoneStruct(&pms);
			pym_tuple_end(&pms);
			// item_sendItemCreation(playerCount)
			//cellMgr_removeCreatureFromWorld(mapChannel, creatureList[f]);
			// delete creature entity
			//creature_destroy(creatureList[f]);
		}
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(lootOwner->cgm, dynObject->entityId, LootInfo, pym_getData(&pms), pym_getLen(&pms));
	// set loot quality
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, LOOT_QUALITY_NORMAL);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(lootOwner->cgm, dynObject->entityId, OverallQuality, pym_getData(&pms), pym_getLen(&pms));
	// send can loot items
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, true); // isLootable
	// canLootItem dict
	pym_dict_begin(&pms);
	if( objData->list_itemLoot.empty() != true )
	{
		item_t **itemList = &objData->list_itemLoot[0];
		sint32 itemCount = objData->list_itemLoot.size();
		for(sint32 f=0; f<itemCount; f++)
		{
			item_t* lootItem = itemList[f];
			// (itemTemplateId, itemQuantity, actorId, partyId,)
			pym_addInt(&pms, lootItem->entityId);
			pym_tuple_begin(&pms);
			pym_addBool(&pms, true);
			pym_tuple_end(&pms);
			// item_sendItemCreation(playerCount)
			//cellMgr_removeCreatureFromWorld(mapChannel, creatureList[f]);
			// delete creature entity
			//creature_destroy(creatureList[f]);
		}
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(lootOwner->cgm, dynObject->entityId, CanLootItems, pym_getData(&pms), pym_getLen(&pms));
}

void lootdispenser_disappearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// called before the object is removed from player sight
}

/*
 * Called when the player runs over the corpse to pickup all items
 */
void lootdispenser_recv_RequestLootAllFromCorpse(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	sint64 lootdispenserEntityId = pym_unpackLongLong(&pums);
	bool autoLootOnly = pym_unpackBool(&pums);
	if( pums.unpackErrorEncountered )
		return;
	// get lootdispenser
	dynObject_t* dynObject = (dynObject_t*)entityMgr_get(lootdispenserEntityId);
	if( dynObject == NULL || entityMgr_getEntityType(lootdispenserEntityId) != ENTITYTYPE_OBJECT )
		return;
	if( dynObject->type != &dynObjectType_lootdispenser )
		return;
	lootdispenser_t* objData = (lootdispenser_t*)dynObject->objectData;
	// is it the correct player?
	if( objData->lootOwnerEntityId != client->player->actor->entityId )
		return;
	// was the creature already looted?
	if( objData->fullyLooted == true )
		return; // creature already fully looted
	objData->fullyLooted = true; // mark as fully looted (todo: We should actually check if all items could be looted and we didn't have a full inventory)
	// send updates
	// ActorGotLoot
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, objData->attachedToEnityId); // actor
	// looted items list
	pym_list_begin(&pms);
	if( objData->list_itemLoot.empty() != true )
	{
		item_t **itemList = &objData->list_itemLoot[0];
		sint32 itemCount = objData->list_itemLoot.size();
		for(sint32 f=0; f<itemCount; f++)
		{
			item_t* lootItem = itemList[f];
			pym_addInt(&pms, lootItem->entityId);
		}
	}
	pym_list_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, ActorGotLoot, pym_getData(&pms), pym_getLen(&pms));
	// TakenItems
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	if( objData->list_itemLoot.empty() != true )
	{
		item_t **itemList = &objData->list_itemLoot[0];
		sint32 itemCount = objData->list_itemLoot.size();
		for(sint32 f=0; f<itemCount; f++)
		{
			item_t* lootItem = itemList[f];
			pym_addInt(&pms, lootItem->entityId);
			pym_addBool(&pms, true);
		}
	}
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, TakenInfo, pym_getData(&pms), pym_getLen(&pms));
	// tell player the object is no more lootable (is this the correct method?)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, false); // isLootable
	pym_dict_begin(&pms);	// canLootItem dict
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, dynObject->entityId, CanLootItems, pym_getData(&pms), pym_getLen(&pms));
	// tell player the about the loot
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, objData->attachedToEnityId);
	pym_list_begin(&pms);
	if( objData->list_itemLoot.empty() != true )
	{
		item_t **itemList = &objData->list_itemLoot[0];
		sint32 itemCount = objData->list_itemLoot.size();
		for(sint32 f=0; f<itemCount; f++)
		{
			item_t* lootItem = itemList[f];
			pym_tuple_begin(&pms);
			pym_addInt(&pms, lootItem->itemTemplate->item.classId);
			pym_addInt(&pms, lootItem->stacksize);
			pym_addInt(&pms, lootItem->entityId);
			pym_tuple_end(&pms);
		}	
	}
	pym_list_end(&pms);
	pym_addInt(&pms, objData->lootCredits);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(client->cgm, 5, GotLoot, pym_getData(&pms), pym_getLen(&pms));
	// give player the actual loot
	if( objData->list_itemLoot.empty() != true )
	{
		item_t **itemList = &objData->list_itemLoot[0];
		sint32 itemCount = objData->list_itemLoot.size();
		for(sint32 f=0; f<itemCount; f++)
		{
			item_t* lootItem = itemList[f];
			item_t* newItemPtr = inventory_addItemToInventory(client, lootItem);
			if( newItemPtr )
				itemList[f] = newItemPtr; // we aware that inventory_addItemToInventory() can invalidate the old item
			if( newItemPtr == NULL )
			{
				// failed to add item to inventory, likely the inventory is full
				item_sendItemDestruction(client, lootItem);
				item_free(lootItem);
			}
		}
	}
	// update credits if necessary
	if( objData->lootCredits > 0 )
	{
		client->player->credits += objData->lootCredits;
		DataInterface_Character_updateCharacter(client->tempCharacterData->userID, client->tempCharacterData->slotIndex, UPDATE_CREDITS, client->player->credits);
		// inform owner
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 1); // creditType (1 -> fund)	
		pym_addInt(&pms, client->player->credits); // new amount
		pym_addInt(&pms, objData->lootCredits); // delta
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(client->cgm, client->player->actor->entityId, METHODID_UPDATECREDITS, pym_getData(&pms), pym_getLen(&pms));
	}
}

void lootdispenser_useObject(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	printf("lootdispenser use callback\n");
	// Lootdispenser are not directly a useable object but the manually inherit from it. Therefore lootdispenser can likely never be used the normal way?
}

void lootdispenser_interruptUse(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	// not used
}

bool lootdispenser_periodicCallback(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	return false;
	//pyMarshalString_t pms;
	//if( timerID == 1 )
	//{
	//	// timerID 1 indicates that the object was successfully used
	//	// inform clients and update state
	//	lootdispenser_t* objData = (lootdispenser_t*)dynObject->objectData;
	//	if( objData->alreadyInUse )
	//	{
	//		objData->alreadyInUse = false;
	//		// send performRecovery (complete action)
	//		pym_init(&pms);
	//		pym_tuple_begin(&pms);  		// Packet Start
	//		pym_addInt(&pms, 80);			// Action ID
	//		pym_addInt(&pms, 6);			// Arg ID
	//		pym_tuple_end(&pms); 			// Packet End
	//		netMgr_pythonAddMethodCallRaw(mapChannel, objData->userEntityId, METHODID_PERFORMRECOVERY, pym_getData(&pms), pym_getLen(&pms));
	//		// update lootdispenser point state
	//		pym_init(&pms);
	//		pym_tuple_begin(&pms);
	//		pym_addNoneStruct(&pms);	// actorId
	//		pym_addInt(&pms, USE_ID_STATE_ACTIVE);	// curStateId
	//		pym_addInt(&pms, 10000);	// windupTime (the time required to convert the control point)
	//		pym_tuple_begin(&pms);	// args
	//		pym_tuple_end(&pms);
	//		pym_tuple_end(&pms);
	//		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, dynObject, dynObject->entityId, METHODID_USE, pym_getData(&pms), pym_getLen(&pms));
	//		// remove user
	//		objData->userEntityId = 0;
	//	}
	//	return false; // immediately unregister timer (one-shot)
	//}

	//return true;
}

dynObjectType_t dynObjectType_lootdispenser = 
{
	lootdispenser_destroy, // destroy
	lootdispenser_appearForPlayers, // appearForPlayers
	lootdispenser_disappearForPlayers, // disappearForPlayer
	lootdispenser_periodicCallback, // periodicCallback
	lootdispenser_useObject, // useObject
	lootdispenser_interruptUse
};
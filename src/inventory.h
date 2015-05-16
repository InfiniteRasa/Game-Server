
typedef union
{
	struct 
	{
		// entry in inventory -> entityId
		unsigned long long personalInventory[50*5]; // 250 slots
		unsigned long long equippedInventory[17]; // armor
		unsigned long long weaponDrawer[5];
	};
	struct  
	{
		unsigned long long slot[250+17+5];
		sint8 activeWeaponDrawer;
	};
}inventory_t;

typedef struct  
{
	unsigned long long entityId;
	//uint32 entityClassId;
	//uint32 itemTemplateId;
	// location info
	unsigned long long locationEntityId;
	sint32 locationSlotIndex;
	// template
	itemTemplate_t *itemTemplate;
	// item instance specific
	sint32 stacksize;
	// item type specific
	union
	{
		// we use a union to save some space
		struct  
		{
			sint32 ammoCount; // amount of remaining ammo
		}weaponData;
	};
}item_t;

void inventory_initForClient(mapChannelClient_t *client);
void _inventory_initForClient(void *param, diJob_getCharacterInventory_t *jobData);
item_t* inventory_CurrentWeapon(mapChannelClient_t *client);

void inventory_notifyEquipmentUpdate(mapChannelClient_t *client);

void inventory_removeItemBySlot(mapChannelClient_t *client, sint32 inventoryType, sint32 slotIndex);
void inventory_addItemBySlot(mapChannelClient_t *client, sint32 inventoryType, sint64 entityId, sint32 slotIndex);
item_t* inventory_addItemToInventory(mapChannelClient_t *client, item_t* item);

item_t* item_createFromTemplateId(uint32 itemTemplateId, sint32 stacksize);
item_t* item_duplicate(item_t* item, sint32 newStacksize);
void item_sendItemCreation(mapChannelClient_t* client, item_t *item);
void item_sendItemDestruction(mapChannelClient_t *client, item_t *item);
void item_free(item_t* item);

void item_recv_RequestTooltipForItemTemplateId(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void item_recv_PersonalInventoryDestroyItem(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void item_recv_PersonalInventoryMoveItem(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestEquipArmor(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestEquipWeapon(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestArmWeapon(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestWeaponDraw(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestWeaponStow(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestWeaponReload(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen, bool tellSelf);

#define INVENTORY_SLOTOFFSET_PLAYER			0
#define INVENTORY_SLOTOFFSET_EQUIPPED		250
#define INVENTORY_SLOTOFFSET_WEAPONDRAWER	266


#define INVENTORY_PERSONAL				1
#define INVENTORY_HOMEINVENTORY			2 // (lockbox)
#define INVENTORY_HIDDENINVENTORY		3
#define INVENTORY_TRADEINVENTORY		4
#define INVENTORY_PLAYERVENDORINVENTORY	5
#define INVENTORY_GAMECONTEXTINVENTORY	6
#define INVENTORY_OVERFLOWINVENTORY		7
#define INVENTORY_EQUIPPEDINVENTORY		8
#define INVENTORY_WEAPONDRAWERINVENTORY	9
#define INVENTORY_BUYBACKINVENTORY		10
#define INVENTORY_AUCTIONINVENTORY		11
#define INVENTORY_INBOXINVENTORY		12
#define INVENTORY_OUTBOXINVENTORY		13
#define INVENTORY_WAGERINVENTORY		14
#define INVENTORY_CLANINVENTORY			15

#define INVENTORY_CATEGORY_EQUIPMENT	1
#define INVENTORY_CATEGORY_CONSUMABLE	2
#define INVENTORY_CATEGORY_CRAFTING		3
#define INVENTORY_CATEGORY_MISSION		4
#define INVENTORY_CATEGORY_MISC			5

#define INVENTORY_SLOTOFFSET_CATEGORY_CONSUMABLE	(50)

// equipment data slot type (not to be confused with real inventory slot index)
#define EQUIPMENT_HELMET				1
#define EQUIPMENT_SHOES					2
#define EQUIPMENT_GLOVES				3
#define EQUIPMENT_WEAPON				13
#define EQUIPMENT_HAIR					14
#define EQUIPMENT_TORSO					15
#define EQUIPMENT_LEGS					16
#define EQUIPMENT_FACE					17
#define EQUIPMENT_WING					18
#define EQUIPMENT_EYEWEAR				19
#define EQUIPMENT_BEARD					20
#define EQUIPMENT_MASK					21
// there are a few more, like MECH_WEAPON_x, do we need these?
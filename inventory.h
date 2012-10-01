

typedef union
{
	struct 
	{
		unsigned long long homeInventory[50*5]; // 250 slots
		unsigned long long equippedInventory[17]; // armor
		unsigned long long weaponDrawer[5];
	};
	struct  
	{
		unsigned long long slot[250+16+5];
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
}item_t;

void inventory_initForClient(mapChannelClient_t *client);
item_t* inventory_CurrentWeapon(mapChannelClient_t *client);

void inventory_notifyEquipmentUpdate(mapChannelClient_t *client);

void item_recv_RequestTooltipForItemTemplateId(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void item_recv_PersonalInventoryMoveItem(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestEquipArmor(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestEquipWeapon(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestArmWeapon(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestWeaponDraw(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestWeaponStow(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void item_recv_RequestWeaponReload(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);

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
void gameData_load();

sint32 gameData_getEquipmentClassIdSlot(sint32 classId);
sint32 gameData_getStarterItemTemplateClassId(sint32 templateId);

typedef struct  
{
	sint32 contextId;
	sint8 name[64];
	sint32 version;
	sint32 baseRegionId;
}gameData_mapInfo_t;

extern sint32 mapInfoCount;
extern gameData_mapInfo_t *mapInfoArray;


#define ITEMTYPE_WEAPON	1
#define ITEMTYPE_ARMOR	2

#define UPDATE_LEVEL 1
#define UPDATE_CREDITS 2
#define UPDATE_PRESTIGE 3
#define UPDATE_EXPERIENCE 4
#define UPDATE_POSITION 5
#define UPDATE_ABILITY 6
#define UPDATE_LOGIN 7
#define UPDATE_LOGOS 8
#define UPDATE_CLASS 9

//kItemIdx_Tradable = 0
//kItemIdx_MaxHPs = 1
//kItemIdx_BuybackPrice = 2
//kItemIdx_Requirements = 3
//kItemIdx_ModuleIds = 4
//kItemIdx_RaceIds = 5
//kEquipableIdx_SkillInfo = 0
//kEquipableIdx_ResistList = 1
//kArmorIdx_RegenRate = 0
//kWeaponIdx_MinDamage = 0
//kWeaponIdx_MaxDamage = 1
//kWeaponIdx_AmmoClassId = 2
//kWeaponIdx_ClipSize = 3
//kWeaponIdx_AmmoPerShot = 4
//kWeaponIdx_DamageType = 5
//kWeaponIdx_WindupTime = 6
//kWeaponIdx_RecoveryTime = 7
//kWeaponIdx_RefireTime = 8
//kWeaponIdx_ReloadTime = 9
//kWeaponIdx_Range = 10
//kWeaponIdx_AERadius = 11
//kWeaponIdx_AEType = 12
//kWeaponIdx_AltFire = 13
//kWeaponIdx_AttackType = 14
//kWeaponIdx_ToolType = 15
//kWeaponAltIdx_MaxDamage = 0
//kWeaponAltIdx_DamageType = 1
//kWeaponAltIdx_Range = 2
//kWeaponAltIdx_AERadius = 3
//kWeaponAltIdx_AEType = 4

typedef struct  
{	
	// item augmentation and general item info
	struct  
	{
		// note: The item system is coded in a way so only one variation of an item can exist per templateId
		//       although we can still have variable modifiers per item instance
		// client side template info
		uint32	classId;
		uint32	templateId;
		sint8	type;
		// general stuff
		uint32	currentHitPoints;
		uint32	maxHitPoints;
		// other
		sint8*	modifiedBy;
		bool	notTradable;
		bool	hasSellableFlag;
		bool	hasCharacterUniqueFlag;
		bool	hasAccountUniqueFlag;
		bool	hasBoEFlag;
		uint8	qualityId;
		bool	boundToCharacter;
		bool	notPlaceableInLockbox;
		uint8	inventoryCategory;
		sint32	reqLevel; // required level
		sint32	buyPrice; // price in credits to buy the item from a vendor
		sint32	sellPrice; // price in credits to sell the item to a vendor
		sint32	stacksize; // max stacksize
	}item;
	// equipment data
	struct  
	{
		sint16 equiptmentSlotType;
		sint32 requiredSkillId; // <=0 if not used 
		sint16 requiredSkillMinVal; // level of skill required, <= 0 if not used
	}equipment;
	// armor specific
	struct  
	{
		sint32 damageAbsorbed; // damage absorbed
		sint32 regenRate; // regen rate
		sint32 armorValue; // body armor
	}armor;
	// weapon specific
	struct  
	{
		uint32	clipSize;
		uint32	currentAmmo;
		float	aimRate;
		uint32	reloadTime;
		sint32	altActionId;
		sint32	altActionArg;
		sint8	aeType;
		uint32	aeRadius;
		uint32	recoilAmount;
		sint8	reuseOverride;
		uint32	coolRate;
		float	heatPerShot;
		sint8	toolType;
		bool	isJammed;
		sint8	ammoPerShot;
		uint32	minDamage;
		uint32	maxDamage;
		uint32	ammoClassId;
		sint8	damageType;
		sint32	windupTime;
		uint32	recoveryTime;
		uint32	refireTime;
		uint32	range;
		uint32	altMaxDamage;
		sint8	altDamageType;
		uint32	altRange;
		uint32	altAERadius;
		sint8	altAEType;
		sint8	attackType;
	}weapon;
	
	
	
	
}itemTemplate_t;

itemTemplate_t *gameData_getItemTemplateById(uint32 templateId);

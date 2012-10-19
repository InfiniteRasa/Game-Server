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


typedef struct  
{
	uint32	classId;
	uint32	templateId;
	sint8	type;
	uint32	currentHitPoints;
	uint32	maxHitPoints;
	sint8*	modifiedBy;
	bool	hasSellableFlag;
	bool	hasCharacterUniqueFlag;
	bool	hasAccountUniqueFlag;
	bool	hasBoEFlag;
	uint8	qualityId;
	bool	boundToCharacter;
	bool	notTradable;
	bool	notPlaceableInLockbox;
	uint8	inventoryCategory;
	uint32	clipSize;
	uint32	currentAmmo;
	float	aimRate;
	uint32	reloadTime;
	sint8	altActionId;
	sint8	altActionArg;
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
}itemTemplate_t;

itemTemplate_t *gameData_getItemTemplateById(uint32 templateId);
itemTemplate_t *gameData_getItemTemplateByName(sint8 *name);

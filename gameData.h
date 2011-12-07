void gameData_load();

int gameData_getEquipmentClassIdSlot(int classId);
int gameData_getStarterItemTemplateClassId(int templateId);

typedef struct  
{
	int contextId;
	char name[64];
	int version;
	int baseRegionId;
}gameData_mapInfo_t;

extern int mapInfoCount;
extern gameData_mapInfo_t *mapInfoArray;


#define ITEMTYPE_WEAPON	1
#define ITEMTYPE_ARMOR	2


typedef struct  
{
	unsigned int	classId;
	unsigned int	templateId;
	char			type;
	unsigned int	currentHitPoints;
	unsigned int	maxHitPoints;
	char*			modifiedBy;
	bool			hasSellableFlag;
	bool			hasCharacterUniqueFlag;
	bool			hasAccountUniqueFlag;
	bool			hasBoEFlag;
	char			qualityId;
	char			boundToCharacter;
	char			notTradable;
	char			notPlaceableInLockbox;
	char			inventoryCategory;
	unsigned int	clipSize;
	unsigned int	currentAmmo;
	float			aimRate;
	unsigned int	reloadTime;
	char			altActionId;
	char			altActionArg;
	char			aeType;
	unsigned int	aeRadius;
	unsigned int	recoilAmount;
	char			reuseOverride;
	unsigned int	coolRate;
	float			heatPerShot;
	char			toolType;
	bool			isJammed;
	char			ammoPerShot;
	unsigned int	minDamage;
	unsigned int	maxDamage;
	unsigned int	ammoClassId;
	char			damageType;
	int				windupTime;
	unsigned int	recoveryTime;
	unsigned int	refireTime;
	unsigned int	range;
	unsigned int	altMaxDamage;
	char			altDamageType;
	unsigned int	altRange;
	unsigned int	altAERadius;
	char			altAEType;
	char			attackType;
}itemTemplate_t;

itemTemplate_t *gameData_getItemTemplateById(unsigned int templateId);
itemTemplate_t *gameData_getItemTemplateByName(char *name);

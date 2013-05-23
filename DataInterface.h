#include "framework.h"
#include <mysql/mysql.h>
#include "INIParser.h"

#define WORKER_QUEUELENGTH	128 // jobs at max
#define WORKER_THREADS		1

typedef struct
{
	sint8 Accountname[15];
	uint32 ID;
	union
	{
		struct
		{
			uint32 UID1;
			uint32 UID2;
		};
	unsigned long long uid;
	};
}authSessionInfo_t;

typedef struct  
{
	void (*workCallback)(MYSQL *dbCon, void *job, void *cb, void *param);
	void *doneCallBack;
	void *param; // cgm
	void *jobData;
}DataInterfaceWorkerQueueEntry_t;

typedef struct  
{
	MYSQL *dbCon;
	DataInterfaceWorkerQueueEntry_t jobQueue[WORKER_QUEUELENGTH];
	sint32 queueWriteIndex;
	sint32 queueReadIndex;
}DataInterfaceWorkerThread_t;


/* character */
#define SWAPSET_SIZE	21
#define CHARACTER_FIRSTNAMELIMIT	64
#define NPC_NAMELIMIT				56

typedef struct  
{
	unsigned long long characterID;
	sint8 unicodeName[CHARACTER_FIRSTNAMELIMIT];
	sint8 unicodeFamily[CHARACTER_FIRSTNAMELIMIT];
	bool genderIsMale;
	sint8 raceID; // 1 to 4
	sint8 slotIndex; // 0 to 15
	sint32 classID;
	unsigned long long userID;
	float posX;
	float posY;
	float posZ;
	sint32 currentContextId;
	struct  
	{
		sint32 classId;		// entityClassId
		uint32 hue;	// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
}di_characterPreview_t;

typedef struct  
{
	sint8 unicodeName[CHARACTER_FIRSTNAMELIMIT];
	sint8 unicodeFamily[CHARACTER_FIRSTNAMELIMIT];
	bool genderIsMale;
	sint8 raceID; // 1 to 4
	sint8 slotIndex; // 0 to 15
	sint32 classId; // character class
	unsigned long long userID;
	float posX;
	float posY;
	float posZ;
	sint32 currentContextId;
	struct  
	{
		sint32 classId;		// entityClassId
		uint32 hue;	// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
	// output
	bool error; // set to true by the sint32erface if the character could not be created
	bool error_nameAlreadyInUse; // set to true if the name is already used
}di_characterLayout_t;

typedef struct {
	uint32 missionId;
	uint32 missionState;
}di_CharacterMissionData;

typedef struct  
{
	unsigned long long characterID;
	sint8 unicodeName[CHARACTER_FIRSTNAMELIMIT];
	sint8 unicodeFamily[CHARACTER_FIRSTNAMELIMIT];
	bool genderIsMale;
	sint8 raceID; // 1 to 4
	sint8 slotIndex; // 0 to 15
	sint32 classID;
	unsigned long long userID;
	float posX;
	float posY;
	float posZ;
	sint32 currentContextId;
	struct  
	{
		sint32 classId;		// entityClassId
		uint32 hue;	// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
	// stats shit

	// inventory shit

	// mission data
	uint32 missionStateCount;
	di_CharacterMissionData *missionStateData;
	
}di_characterData_t;

/* npc */

typedef struct  
{
	unsigned long long npcID;
	uint32 creatureTypeId;
	sint32 npcPackageId;
	struct  
	{
		sint32 classId;	// entityClassId
		uint32 hue;		// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
}di_npcData_t;

/* vendor */

typedef struct  
{
	uint32 itemTemplateId;
	uint32 stacksize;
	sint32 sequence;
}di_vendorItemData_t;

typedef struct  
{
	uint32 creatureTypeId;
	sint32 vendorPackageId;
	// list of items
	uint32 numberOfItems;
	di_vendorItemData_t* itemList;
}di_vendorData_t;

/* creature type */

typedef struct  
{	
	sint32 id;
	sint8 name[70];
	sint32 nameId;
	sint32 classId;
	sint16 faction;
	float walkspeed;
	float runspeed;
	sint32 hitpoints;
	sint32 missile[8]; // up to 8 different abilities/attacks
	float wanderDistance;
}diJob_creatureType_t;

/* missile */

typedef struct  
{	
	sint32 id;
	sint8 description[50];
	sint32 actionId; // action
	sint32 actionArgId; // subaction
	float rangeMin;
	float rangeMax;
	sint32 recoverTime; // cooldown time after use
	sint32 recoverTimeGlobal; // min cooldown of all available actions
	sint32 windupTime; // windup animation time
	sint32 minDamage; // min damage, not all missile actions use this
	sint32 maxDamage; // max damage, not all missile actions use this
}diJob_creatureAction_t;

/* spawnpool */

typedef struct  
{	
	sint32 id;
	sint32 mode; // 0 -> automatic spawn, 1 -> base/outpost(CP) spawn, 2 -> scripted spawn (must be triggered)
	sint8 animType; // 0 -> no animation, 1 -> bane dropship
	sint32 respawnTime; // only for automatic spawn, how long it takes to respawn creatures after all are killed
	float posX;
	float posY;
	float posZ;
	sint32 contextId;
	struct  
	{
		sint32 creatureType;
		sint32 countMin;
		sint32 countMax;
	}spawnSlot[6];
}diJob_spawnpool_t;

typedef struct
{
	sint32 mode;
	sint8 animType;
	sint32 respawnTime;
	float posX;
	float posY;
	float posZ;
	sint32 contextId;
	struct  
	{
		sint32 creatureType;
		sint32 countMin;
		sint32 countMax;
	}spawnSlot[6];
}diData_spawnEntry_t;

/* TEST:entitydata */

typedef struct  
{
	unsigned long long npcID;
	sint8 unicodeName[NPC_NAMELIMIT];
	sint32 entityClassID;
	float posX;
	float posY;
	float posZ;
	float rotation;
	sint32 currentContextId;
	struct  
	{
		sint32 classId;		// entityClassId
		uint32 hue;			// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
}di_entityDataW_t;

/*TEST entity queue job*/
typedef struct  
{
	di_entityDataW_t *entityData;
}diJob_updateEntityW_t;

/* TEST 2*/

typedef struct baseBehavior_baseNode
{
	float x;
	float y;
	float z;
	sint32 contextid;
	sint32 pindex; //-- postition of pathnode
}cbaseNode;

typedef struct  
{
	
	sint32 spawntype;
	float posX;
	float posY;
	float posZ;
	sint32 pathindex;
	sint32 currentContextId;
	
}di_pathNodeDataW2_t;

typedef struct  
{
	di_pathNodeDataW2_t *pnodedata;
}diJob_updatePathNodeW2_t;

typedef struct
{
  uint32 id;
  uint32 contextid;
  uint32 type;   //--- twostate teleporter, waypoint, wormhole,etc
  uint32 nameId; // used for name lookup
  //uint32 modelid; //---appereance (now integrated into type)
  sint8 label[50];
  float sx; //---current location
  float sy;
  float sz;
  float dx; //---destination
  float dy;
  float dz;
  float bx; //---activation area
  float bz;
}di_teleporterData;


typedef struct
{
    uint32 mapContextId;
	uint32 scount;
	di_teleporterData *tdata;
}diJob_teleporterData;

/* item templates */

typedef struct  
{	
	sint32 itemTemplateId;
	sint32 classId;
	sint32 qualityId;
	sint32 type;
	sint32 reqLevel; // min level required
	bool hasSellableFlag;
	bool notTradeableFlag;
	bool hasCharacterUniqueFlag;
	bool hasAccountUniqueFlag;
	bool hasBoEFlag;
	bool boundToCharacterFlag;
	bool notPlaceableInLockBoxFlag;
	sint32 inventoryCategory;
	sint32 buyPrice;
	sint32 sellPrice;
	sint32 stacksize;
}diJob_itemTemplateEntry_t;

typedef struct  
{	
	sint32 numberOfItemTemplates;
	diJob_itemTemplateEntry_t* itemTemplateList;
}diJob_itemTemplate_t;

typedef struct  
{	
	sint32 itemTemplateId;
	sint32 slotType;
	sint32 skillId; // -1 if not used
	sint32 skillMinVal;
	// todo: Add appearance hue to this table?
}diJob_itemTemplateEquipmentEntry_t;

typedef struct  
{	
	sint32 numberOfTemplates;
	diJob_itemTemplateEquipmentEntry_t* itemTemplateEquipmentList;
}diJob_itemTemplateEquipment_t;

typedef struct  
{	
	sint32 itemTemplateId;
	sint32 damageAbsorbed; // damage absorbed
	sint32 regenRate; // regen rate
	sint32 armorValue; // body armor
}diJob_itemTemplateArmorEntry_t;

typedef struct  
{	
	sint32 numberOfTemplates;
	diJob_itemTemplateArmorEntry_t* itemTemplateArmorList;
}diJob_itemTemplateArmor_t;

typedef struct  
{
	sint32  itemTemplateId;
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
	// bool	isJammed; -> Doesn't make much sense to have weapons that are "jammed" by default?
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
}diJob_itemTemplateWeaponEntry_t;

typedef struct  
{	
	sint32 numberOfTemplates;
	diJob_itemTemplateWeaponEntry_t* itemTemplateWeaponList;
}diJob_itemTemplateWeapon_t;

/* mission */

typedef struct  
{
	sint32 missionId;
	unsigned long long dispenserNPC;
	unsigned long long collectorNPC;
	// more todo
}di_missionData_t;

typedef struct  
{
	uint32 id;
	uint32 missionId;
	uint16 command;
	uint16 state;
	uint8 flags;
	sint32 value1;
	sint32 value2;
	sint32 value3;
}di_missionScriptLine_t;

typedef struct  
{
	uint32 scriptLineCount;
	di_missionScriptLine_t* scriptLines;
}di_missionScript_t;

/* job data blocks */

typedef struct  
{
	unsigned long long userId;
	uint32 slotIndex;
	// output
	di_characterPreview_t *outPreviewData[16];
}diJob_getCharacterPreviewInfo_t;

typedef struct  
{
	unsigned long long userId;
	uint32 slotIndex;
	// output
	di_characterData_t *outCharacterData;
}diJob_characterData_t;

typedef struct  
{
	unsigned long long userId;
	uint32 slotId;
	// output
	bool error;
}diJob_deleteCharacter_t;

typedef struct  
{
	// output
	sint32 outNpcCount;
	di_npcData_t *outNpcList;
}diJob_npcListData_t;

typedef struct  
{
	// output
	sint32 outVendorCount;
	di_vendorData_t *outVendorList;
}diJob_vendorListData_t;

typedef struct  
{
	di_npcData_t *npcData;
}diJob_updateNPC_t;

typedef struct  
{
	// output
	sint32 outState;
	unsigned long long outEntityId;
}diJob_getLastNPCEntityID_t;

typedef struct  
{
	// output
	sint32 outMissionCount;
	di_missionData_t *outMissionList;
}diJob_missionListData_t;

typedef struct  
{
	float pos[3];
	//sint32 nodeIndex;
}diData_pathNodes_t;

typedef struct  
{
	sint32 pathId;
	sint32 contextId;
	sint32 mode;
	sint32 numberOfNodes;
	sint32 spawnpool; // the spawnpool that will use this path, 0 if not used
	float nodeOffsetRandomization; // how far away can creatures pass a path node
	diData_pathNodes_t* pathnodes;
	bool _isFinished; // only used by the data interface methods
}diData_path_t;

typedef struct  
{
	// when writing a single path
	diData_path_t* pathData;
	// when reading all paths from the db
	diData_path_t* pathList;
	sint32 pathCount;
}diJob_path_t;

/* general */
void DataInterface_init();

void* DataInterface_allocJob(sint32 size);
void DataInterface_freeJob(void *job);

void DataInterface_queueJob(void *jobData, void *workCallback, void *doneCallback, void *param);

/* character */
void DataInterface_Character_getCharacterPreviewInfo(unsigned long long userID, uint32 slotIndex, void (*cb)(void *param, diJob_getCharacterPreviewInfo_t *jobData), void *param);
void DataInterface_Character_getCharacterData(unsigned long long userID, uint32 slotIndex, void (*cb)(void *param, diJob_characterData_t *jobData), void *param);
void DataInterface_Character_createCharacter(di_characterLayout_t *characterData, void (*cb)(void *param, di_characterLayout_t *jobData), void *param);
void DataInterface_Character_deleteCharacter(unsigned long long userID, sint32 slotId, void (*cb)(void *param, diJob_deleteCharacter_t *jobData), void *param);

/* npc */
void DataInterface_NPC_getNPCList(void (*cb)(void *param, diJob_npcListData_t *jobData), void *param);

/* vendor */ 

void DataInterface_Vendor_getVendorList(void (*cb)(void *param, diJob_vendorListData_t *jobData), void *param);

//void DataInterface_NPC_updateNPC(di_npcData_t *npcData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param);

/* creatureType */
void DataInterface_Creature_getCreatureTypeList(void (*cb)(void *param, diJob_creatureType_t *jobData), void *param);

/* path nodes */
void DataInterface_Creature_getPathList(void (*cb)(void *param, diJob_path_t *jobData), void *param);
void DataInterface_Creature_savePath(diData_path_t* pathData);

/* spawn system */
void DataInterface_SpawnSystem_getSpawnPoolList(void (*cb)(void *param, diJob_spawnpool_t *jobData), void *param);
sint32 DataInterface_SpawnSystem_addSpawnPoint(diData_spawnEntry_t* spawnEntry);

/* missile */
void DataInterface_Creature_getCreatureActionList(void (*cb)(void *param, diJob_creatureAction_t *jobData), void *param);

/* mission */
void DataInterface_Mission_getMissionList(void (*cb)(void *param, diJob_missionListData_t *jobData), void *param);
void DataInterface_Mission_getMissionScriptData(void (*cb)(void *param, di_missionScript_t *jobData), void *param);

/* item template */
void DataInterface_Item_getItemTemplates(void (*cb)(void *param, diJob_itemTemplate_t *jobData), void *param);
void DataInterface_Item_getItemEquipmentData(void (*cb)(void *param, diJob_itemTemplateEquipment_t *jobData), void *param);
void DataInterface_Item_getItemArmorData(void (*cb)(void *param, diJob_itemTemplateArmor_t *jobData), void *param);
void DataInterface_Item_getItemWeaponData(void (*cb)(void *param, diJob_itemTemplateWeapon_t *jobData), void *param);

/* extra */
void DataInterface_registerServerForAuth();
uint32 DataInterface_IPtoHex();
uint32 DataInterface_getMyIP();
sint32 DataInterface_QuerySession(uint32 ID1, uint32 ID2, authSessionInfo_t *asiOut);

//void DataInterface_Entity_updateEntityW(di_entityDataW_t *entityData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param);

void DataInterface_teleporter_getList(uint32 mapContextId, void (*cb)(void *param, diJob_teleporterData *jobData), void *param);
void DataInterface_teleporter_updateList( di_teleporterData *objectData, void (*cb)(void *param, diJob_teleporterData *jobData), void *param);
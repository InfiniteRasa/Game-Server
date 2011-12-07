#include<mysql.h>

#define WORKER_QUEUELENGTH	128 // jobs at max
#define WORKER_THREADS		1

typedef struct
{
	char Accountname[15];
	unsigned int ID;
	union
	{
		struct
		{
			unsigned int UID1;
			unsigned int UID2;
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
}dataInterfaceWorkerQueueEntry_t;

typedef struct  
{
	MYSQL *dbCon;
	dataInterfaceWorkerQueueEntry_t jobQueue[WORKER_QUEUELENGTH];
	int queueWriteIndex;
	int queueReadIndex;
}dataInterfaceWorkerThread_t;


/* character */
#define SWAPSET_SIZE	21
#define CHARACTER_FIRSTNAMELIMIT	64
#define NPC_NAMELIMIT				56

typedef struct  
{
	unsigned long long characterID;
	char unicodeName[CHARACTER_FIRSTNAMELIMIT];
	char unicodeFamily[CHARACTER_FIRSTNAMELIMIT];
	bool genderIsMale;
	char raceID; // 1 to 4
	char slotIndex; // 0 to 15
	int classID;
	unsigned long long userID;
	float posX;
	float posY;
	float posZ;
	int currentContextId;
	struct  
	{
		int classId;		// entityClassId
		unsigned int hue;	// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
}di_characterPreview_t;

typedef struct  
{
	char unicodeName[CHARACTER_FIRSTNAMELIMIT];
	char unicodeFamily[CHARACTER_FIRSTNAMELIMIT];
	bool genderIsMale;
	char raceID; // 1 to 4
	char slotIndex; // 0 to 15
	int classId; // character class
	unsigned long long userID;
	float posX;
	float posY;
	float posZ;
	int currentContextId;
	struct  
	{
		int classId;		// entityClassId
		unsigned int hue;	// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
	// output
	bool error; // set to true by the interface if the character could not be created
	bool error_nameAlreadyInUse; // set to true if the name is already used
}di_characterLayout_t;

typedef struct {
	unsigned int missionId;
	unsigned int missionState;
}di_CharacterMissionData;

typedef struct  
{
	unsigned long long characterID;
	char unicodeName[CHARACTER_FIRSTNAMELIMIT];
	char unicodeFamily[CHARACTER_FIRSTNAMELIMIT];
	bool genderIsMale;
	char raceID; // 1 to 4
	char slotIndex; // 0 to 15
	int classID;
	unsigned long long userID;
	float posX;
	float posY;
	float posZ;
	int currentContextId;
	struct  
	{
		int classId;		// entityClassId
		unsigned int hue;	// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
	// stats shit

	// inventory shit

	// mission data
	unsigned int missionStateCount;
	di_CharacterMissionData *missionStateData;
	
}di_characterData_t;

/* npc */

typedef struct  
{
	unsigned long long npcID;
	char unicodeName[NPC_NAMELIMIT];
	int entityClassID;
	float posX;
	float posY;
	float posZ;
	int currentContextId;
	struct  
	{
		int classId;		// entityClassId
		unsigned int hue;	// 0xAABBGGRR
	}appearanceData[SWAPSET_SIZE];
}di_npcData_t;

/* TEST:entitydata */

typedef struct  
{
	unsigned long long npcID;
	char unicodeName[NPC_NAMELIMIT];
	int entityClassID;
	float posX;
	float posY;
	float posZ;
	float rotation;
	int currentContextId;
	struct  
	{
		int classId;		// entityClassId
		unsigned int hue;	// 0xAABBGGRR
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
	int contextid;
	int pindex; //-- postition of pathnode
}cbaseNode;

typedef struct  
{
	
	int spawntype;
	float posX;
	float posY;
	float posZ;
	int currentContextId;
	baseBehavior_baseNode *pathnodes;
	int nodeCount;
	
}di_spawnDataW2_t;

typedef struct  
{	
	int spawntype; 
	int maxcreatures; //---max creatures per cell
	char label[50]; //--- short description
	char creatures[70]; //---class ids tokens
	int faction;
    di_spawnDataW2_t *locationlist;
	int spawnlocCount;
	int anim_type; //---spawnanimation: drophip,beam,none
	int activeSpawnCount; //--maximun simultaneously delivered spawnpoints
	int locktime;//---time to be passed before location can used again
	int attackspeed; //--attackspeed
	int attackaction; //---attack animation
	float velocity; //---movement speed
	int attackstyle; //---range or melee
	int actionid;
	int dmg_melee;
	int dmg_range;
	int hitpoints;
}di_spawnTypeW2_t;

typedef struct  
{
	unsigned int mapContextId;
	di_spawnTypeW2_t *spawnType;
	int scount; //number of spawntypes in list
}diJob_spawnTypeW2_t;

/*TEST entity queue job*/
typedef struct  
{
	di_spawnDataW2_t *spawnData;
}diJob_updateSpawnW2_t;

typedef struct  
{
	
	int spawntype;
	float posX;
	float posY;
	float posZ;
	int pathindex;
	int currentContextId;
	
}di_pathNodeDataW2_t;

typedef struct  
{
	di_pathNodeDataW2_t *pnodedata;
}diJob_updatePathNodeW2_t;

typedef struct
{
  unsigned int id;
  unsigned int contextid;
  unsigned int type;   //--- twostate teleporter, waypoint, wormhole,etc
  unsigned int modelid; //---appereance
  char label[50];
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
    unsigned int mapContextId;
	unsigned int scount;
	di_teleporterData *tdata;
}diJob_teleporterData;

/* mission */

typedef struct  
{
	int missionId;
	unsigned long long dispenserNPC;
	unsigned long long collectorNPC;
	// more todo
}di_missionData_t;

/* job data blocks */

typedef struct  
{
	unsigned long long userId;
	unsigned int slotIndex;
	// output
	di_characterPreview_t *outPreviewData[16];
}diJob_getCharacterPreviewInfo_t;

typedef struct  
{
	unsigned long long userId;
	unsigned int slotIndex;
	// output
	di_characterData_t *outCharacterData;
}diJob_characterData_t;

typedef struct  
{
	unsigned long long userId;
	unsigned int slotId;
	// output
	bool error;
}diJob_deleteCharacter_t;

typedef struct  
{
	unsigned int mapContextId;
	// output
	int outNpcCount;
	di_npcData_t *outNpcList;
}diJob_npcListData_t;

typedef struct  
{
	di_npcData_t *npcData;
}diJob_updateNPC_t;

typedef struct  
{
	// output
	int outState;
	unsigned long long outEntityId;
}diJob_getLastNPCEntityID_t;

typedef struct  
{
	// output
	int outMissionCount;
	di_missionData_t *outMissionList;
}diJob_missionListData_t;


/* general */
void dataInterface_init();

void* dataInterface_allocJob(int size);
void dataInterface_freeJob(void *job);

void dataInterface_queueJob(void *jobData, void *workCallback, void *doneCallback, void *param);

/* character */
void dataInterface_Character_getCharacterPreviewInfo(unsigned long long userID, unsigned int slotIndex, void (*cb)(void *param, diJob_getCharacterPreviewInfo_t *jobData), void *param);
void dataInterface_Character_getCharacterData(unsigned long long userID, unsigned int slotIndex, void (*cb)(void *param, diJob_characterData_t *jobData), void *param);
void dataInterface_Character_createCharacter(di_characterLayout_t *characterData, void (*cb)(void *param, di_characterLayout_t *jobData), void *param);
void dataInterface_Character_deleteCharacter(unsigned long long userID, int slotId, void (*cb)(void *param, diJob_deleteCharacter_t *jobData), void *param);

/* npc */
void dataInterface_NPC_getNPCList(unsigned int mapContextId, void (*cb)(void *param, diJob_npcListData_t *jobData), void *param);
void dataInterface_NPC_updateNPC(di_npcData_t *npcData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param);
unsigned long long dataInterface_NPC_getLastNPCEntityID();

/* mission */
void dataInterface_Mission_getMissionList(void (*cb)(void *param, diJob_missionListData_t *jobData), void *param);

/* extra */
void dataInterface_registerServerForAuth();
unsigned int dataInterface_getMyIP();
int dataInterface_QuerySession(unsigned int ID1, unsigned int ID2, authSessionInfo_t *asiOut);

void dataInterface_Entity_updateEntityW(di_entityDataW_t *entityData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param);
void dataInterface_Spawnpool_updateSpawnW2(di_spawnDataW2_t *entityData, void (*cb)(void *param, diJob_updateNPC_t *jobData), void *param);

void dataInterface_Spawn_getSpawnpool(unsigned int mapContextId, void (*cb)(void *param, diJob_spawnTypeW2_t *jobData), void *param);
void dataInterface_PathNode_setPathnode(di_pathNodeDataW2_t *pnodedata, void (*cb)(void *param, diJob_spawnTypeW2_t *jobData), void *param);
void dataInterface_teleporter_getList(unsigned int mapContextId, void (*cb)(void *param, diJob_teleporterData *jobData), void *param);
void dataInterface_teleporter_updateList( di_teleporterData *objectData, void (*cb)(void *param, diJob_teleporterData *jobData), void *param);

/* creature type */

typedef struct _creatureType_t 
{
	// actor info
	int nameId; // note that it is also possible to overwrite the name with actor Recv_SetName
	int entityClassId;
	// stats info
	int maxHealth;
	int RangeMissile;
	int MeleeMissile;
}creatureType_t;

creatureType_t*		creatureType_createCreatureType(int entityClassId, int nameId);
void				creatureType_registerCreatureType(creatureType_t *creatureType, char *name);
void				creatureType_setMaxHealth(creatureType_t *creatureType, int maxHealth);

/* creature */

typedef struct baseBehavior_baseNode
{
	float x;
	float y;
	float z;
}cbaseNode;

typedef struct _creature_t
{
	creatureType_t *type;		// the creature 'layout'
	actor_t			actor;		// the base actor object
	// stats
	int currentHealth;
	int attackspeed;
	int lastattack;
	int lastresttime;
	float velocity;
	float rotspeed;
	float range; //attackrange
	int attack_habbit; //meelee or range fighter 
	int agression; // hunting timer for enemys
	int lastagression;
	int faction; // hostile /friendly
	int wanderstate; 
	float wx,wy,wz; 
    float wander_dist; // -- wander boundaries 
    baseBehavior_baseNode homePos;  //--- spawn location (used for wander)
    baseBehavior_baseNode *pathnodes; //--entity patrol nodes
	// origin
	spawnPool_t *spawnPool; // the spawnpool that initiated the creation of this creature
	// behavior controller
	behaviorState_t controller;
}creature_t;

//creature_t*			creature_createCreature(mapChannel_t *mapChannel, int classId, char *name);
//creature_t*			creature_createCreature(mapChannel_t *mapChannel, unsigned long long entityId, int classId, char *name);
creature_t*			creature_createCreature(mapChannel_t *mapChannel, char *typeName, spawnPool_t *spawnPool);
creature_t*			creature_createCreature(mapChannel_t *mapChannel, creatureType_t *creatureType, spawnPool_t *spawnPool,int faction);
creatureType_t*		creatureType_findType(char *typeName);
void				creature_setLocation(creature_t *creature, float x, float y, float z, float rX, float rY);


// cellMgr related
void creature_cellIntroduceCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, int playerCount);
void creature_cellIntroduceCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, int creatureCount);
void creature_cellDiscardCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, int playerCount);
void creature_cellDiscardCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, int creatureCount);

// global init
void creature_init();
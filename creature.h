
/* creature type */
typedef struct _creatureType_t 
{
	// actor info
	sint32 nameId; // note that it is also possible to overwrite the name with actor Recv_SetName
	sint32 entityClassId;
	// stats info
	sint32 maxHealth;
	sint32 RangeMissile;
	sint32 MeleeMissile;
}creatureType_t;

creatureType_t*		creatureType_createCreatureType(sint32 entityClassId, sint32 nameId);
void				creatureType_registerCreatureType(creatureType_t *creatureType, sint8 *name);
void				creatureType_setMaxHealth(creatureType_t *creatureType, sint32 maxHealth);

/* creature */

typedef struct _creature_t
{
	creatureType_t *type;		// the creature 'layout'
	actor_t			actor;		// the base actor object
	// stats
	sint32 currentHealth;
	sint32 attackspeed;
	sint32 lastattack;
	sint32 lastresttime;
	float velocity;
	sint32 rottime; //rotation speed
	float range; //attackrange
	sint32 attack_habbit; //meelee or range fighter 
	sint32 agression; // hunting timer for enemys
	sint32 lastagression;
	sint32 faction; // hostile /friendly
	sint32 wanderstate;
	sint32 movestate;
	float wx,wy,wz; 
    float wander_dist; // -- wander boundaries 
    baseBehavior_baseNode homePos;  //--- spawn location (used for wander)
    baseBehavior_baseNode *pathnodes; //--entity patrol nodes
	sint32 **aggrotable; //stores enemydamage
	sint32 aggrocount; 
	// origin
	spawnPool_t *spawnPool; // the spawnpool that initiated the creation of this creature
	// behavior controller
	behaviorState_t controller;
}creature_t;

//creature_t*			creature_createCreature(mapChannel_t *mapChannel, sint32 classId, sint8 *name);
//creature_t*			creature_createCreature(mapChannel_t *mapChannel, unsigned long long entityId, sint32 classId, sint8 *name);
creature_t*			creature_createCreature(mapChannel_t *mapChannel, sint8 *typeName, spawnPool_t *spawnPool);
creature_t*			creature_createCreature(mapChannel_t *mapChannel, creatureType_t *creatureType, spawnPool_t *spawnPool,sint32 faction);
creatureType_t*		creatureType_findType(sint8 *typeName);
void				creature_setLocation(creature_t *creature, float x, float y, float z, float rX, float rY);


// cellMgr related
void creature_cellintroduceCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, sint32 playerCount);
void creature_cellIntroduceCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, sint32 creatureCount);
void creature_cellDiscardCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, sint32 playerCount);
void creature_cellDiscardCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, sint32 creatureCount);

// global init
void creature_init();
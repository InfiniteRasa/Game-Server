
/* creature missile */
typedef struct  
{
	sint32 id;
	sint32 actionId; // action
	sint32 actionArgId; // subaction
	float rangeMin;
	float rangeMax;
	sint32 recoverTime; // cooldown time after use
	sint32 recoverTimeGlobal; // cooldown of all available actions (if lower than active recover timer, will have no effect)
	sint32 windupTime; // windup animation time
	sint32 minDamage; // min damage, not all missile actions use this
	sint32 maxDamage; // max damage, not all missile actions use this
}creatureMissile_t; // someday we should rename this to creatureAction, creatureAbility or creatureAttack

/* creature type */
typedef struct _creatureType_t 
{
	sint32 typeId; // db id (used by mission system)
	// actor info
	actorAppearanceData_t appearanceData[21];
	sint32 nameId; // if 0, use name field instead.
	sint32 entityClassId;
	char name[70];
	sint32 faction;
	// stats info
	float walkspeed;
	float runspeed;
	sint32 maxHealth;
	//sint32 RangeMissile;
	//sint32 MeleeMissile;
	creatureMissile_t* actions[8]; // creature available
	// todo fields
	sint32 aggressionTime;
	float wander_dist; // -- wander boundaries 
	// aggro info
	float aggroRange; // how far away the creature can detect enemies, usually 24.0f but can be increased by having high-range attacks
	// npc data (only if creature is a NPC)
	npcData_t* npcData; 
	// vendor data (only if creature is a vendor, creatures do not need to be a NPC to be a vendor serverside)
	vendorData_t* vendorData;
}creatureType_t;

creatureType_t*		creatureType_createCreatureType(sint32 entityClassId, sint32 nameId);
void				creatureType_registerCreatureType(creatureType_t *creatureType, sint8 *name);
void				creatureType_setMaxHealth(creatureType_t *creatureType, sint32 maxHealth);

#define CREATURE_LOCATION_UPDATE_TIME	(1500) // update cell location every 1500ms

/* creature */
typedef struct _creature_t
{
	creatureType_t *type;		// the creature 'layout'
	actor_t			actor;		// the base actor object
	sint32			updatePositionCounter; // decreases, when it hits 0 and the cell position changed, call creature_updateCellLocation() 
	// stats
	//sint32 currentHealth; --> this is already stored in creature.actor.currentHealth
	//sint32 attackspeed;
	sint32 lastattack;
	sint32 lastresttime;
	//float velocity;
	//sint32 rottime; //rotation speed
	//float range; //attackrange
	//sint32 attack_habbit; //meelee or range fighter 
	//sint32 agression; // hunting timer for enemys
	sint32 lastagression;
	//sint32 wanderstate;
	//sint32 movestate;
	//float wx,wy,wz; // target destination (can be far away)
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
creature_t* creature_createCreature(mapChannel_t *mapChannel, creatureType_t *creatureType, spawnPool_t *spawnPool);
creatureType_t*		creatureType_findType(sint32 typeId);
void				creature_setLocation(creature_t *creature, float x, float y, float z, float rX, float rY);

void creature_destroy(creature_t *creature);

void creature_handleCreatureKill(mapChannel_t* mapChannel, creature_t *creature, actor_t* killedBy);

// cellMgr related
void creature_cellintroduceCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, sint32 playerCount);
void creature_cellIntroduceCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, sint32 creatureCount);
void creature_cellDiscardCreatureToClients(mapChannel_t *mapChannel, creature_t *creature, mapChannelClient_t **playerList, sint32 playerCount);
void creature_cellDiscardCreaturesToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, creature_t **creatureList, sint32 creatureCount);
void creature_cellUpdateLocation(mapChannel_t *mapChannel, creature_t* creature, sint32 newLocX, sint32 newLocZ);

// global init
void creature_init();
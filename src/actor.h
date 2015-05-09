#include"gameEffects.h"

typedef struct  
{
	uint32 classId;
	uint32 hue;
}actorAppearanceData_t;

/*
 * Central structure used to store any kind of NPC/creature/player attribute
 * Note that even creatures have attributes like body, mind and spirit since they inherit from the actor class.
 */
typedef struct  
{
	sint32 level;
	// regen rate
	sint32 regenRateCurrentMax;
	sint32 regenRateNormalMax; // regen rate without bonus
	float regenHealthPerSecond; // health regen per second
	// health
	sint32 healthCurrent;
	sint32 healthCurrentMax;
	sint32 healthNormalMax;
	// body
	sint32 bodyCurrent;
	sint32 bodyCurrentMax;
	sint32 bodyNormalMax;
	// mind
	sint32 mindCurrent;
	sint32 mindCurrentMax;
	sint32 mindNormalMax;
	// spirit
	sint32 spiritCurrent;
	sint32 spiritCurrentMax;
	sint32 spiritNormalMax;
	// chi/power
	sint32 chiCurrent;
	sint32 chiCurrentMax;
	sint32 chiNormalMax;
	// armor 
	sint32 armorCurrent;
	sint32 armorCurrentMax;
	sint32 armorNormalMax;
	// armor regen
	sint32 armorRegenCurrent;
	// todo: Regeneration rates
}actorStats_t;

/*
	BODY = 1
	MIND = 2
	SPIRIT = 3
	HEALTH = 4
	CHI = 5
	POWER = 6
	AWARE = 7
	ARMOR = 8
	SPEED = 9
	REGEN = 10
*/



typedef struct  
{
	sint32 actionId;
	sint32 actionArgId;
	sint64 targetEntityId;
	void(*actorActionUpdateCallback)(mapChannel_t* mapChannel, actor_t* actor, sint32 newActionState);
	// todo: For now, objects use a separate action handling code - eventually it should be merged into this one
}actorCurrentAction_t;

#define ACTOR_ACTION_STATE_COMPLETE			(1)
#define ACTOR_ACTION_STATE_INTERRUPTED		(2)


typedef struct _actor_t
{
	sint32 entityId;
	sint32 entityClassId;
	sint8 name[64];
	sint8 family[64];
	actorStats_t stats;
	// actorAppearanceData_t appearanceData[21]; // should move this to manifestation and npc structure? Is this used by creatures?
	float posX;
	float posY;
	float posZ;
	float rotation;
	//sint32 attackstyle;
	//sint32 actionid;
	bool isRunning;
	bool inCombatMode;
	mapCellLocation_t cellLocation;
	gameEffect_t *activeEffects;
	sint8 state;
	// action data
	actorCurrentAction_t currentAction;
	// sometimes we only have access to the actor, the owner variable allows us to access the client anyway (only if actor is a player manifestation)
	mapChannelClient_t* owner;
}actor_t;

#define ACTOR_STATE_ALIVE	0
#define ACTOR_STATE_DEAD	1


// actor action handling
bool actor_hasActiveAction(actor_t* actor);
void actor_completeCurrentAction(mapChannel_t* mapChannel, actor_t* actor); // should not be called manually
void actor_startActionOnEntity(mapChannel_t* mapChannel, actor_t* actor, sint64 targetEntityId, sint32 actionId, sint32 actionArgId, sint32 windupTime, sint32 recoverTime, void(*actorActionUpdateCallback)(mapChannel_t* mapChannel, actor_t* actor, sint32 newActionState));

#define WINDUP_MANUAL_ACTION	(-1) // pass to actor_startActionOnEntity to disable the automatic action completion
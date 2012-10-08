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
	// todo: Regeneration rates
}actorStats_t;

typedef struct _actor_t
{
	sint32 entityId;
	sint32 entityClassId;
	sint8 name[64];
	sint8 family[64];
	actorStats_t stats;
	actorAppearanceData_t appearanceData[21]; // should move this to manifestation and npc structure? (Because not used by monsters)
	float posX;
	float posY;
	float posZ;
	float rotation;
	sint32 attackstyle;
	sint32 actionid;
	bool isRunning;
	bool inCombatMode;
	mapCellLocation_t cellLocation;
	gameEffect_t *activeEffects;
	sint8 state;
	// sometimes we only have access to the actor, the owner variable allows us to access the client anyway (if owned by a player)
	mapChannelClient_t* owner;
}actor_t;

#define ACTOR_STATE_ALIVE	0
#define ACTOR_STATE_DEAD	1



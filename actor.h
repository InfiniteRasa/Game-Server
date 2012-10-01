#include"gameEffects.h"

typedef struct  
{
	uint32 classId;
	uint32 hue;
}actorAppearanceData_t;

typedef struct  
{
	sint32 level;
	sint32 healthCurrent;
	sint32 healthMax;
	sint32 healthBonus;

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
}actor_t;

#define ACTOR_STATE_ALIVE	0
#define ACTOR_STATE_DEAD	1



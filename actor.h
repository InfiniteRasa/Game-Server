#include"gameEffects.h"

typedef struct  
{
	unsigned int classId;
	unsigned int hue;
}actorAppearanceData_t;

typedef struct  
{
	int level;
	int healthCurrent;
	int healthMax;
	int healthBonus;

}actorStats_t;

typedef struct _actor_t
{
	int entityId;
	int entityClassId;
	char name[64];
	char family[64];
	actorStats_t stats;
	actorAppearanceData_t appearanceData[21]; // should move this to manifestation and npc structure? (Because not used by monsters)
	float posX;
	float posY;
	float posZ;
	float rotation;
	bool isRunning;
	bool inCombatMode;
	mapCellLocation_t cellLocation;
	gameEffect_t *activeEffects;
	char state;
}actor_t;

#define ACTOR_STATE_ALIVE	0
#define ACTOR_STATE_DEAD	1



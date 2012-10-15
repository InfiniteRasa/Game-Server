#if(_MSC_VER>=1600)
 #define _HAS_ITERATOR_DEBUGGING 0	
#endif
#include <vector>

typedef struct _missile_t
{
	//sint32 type;
	sint32 damageA;
	sint32 damageB;
	sint32 actionId;
	sint32 argId;
	unsigned long long targetEntityId; // the entityId of the destination (it is possible that the object does no more exist on arrival)
	actor_t* source;
	sint32 triggerTime; // amount of milliseconds left before the missile is triggered, is decreased on every tick
	//_missile_t *previous;
	//_missile_t *next;
}missile_t;

/*
 *	Missiles are, of course, map bound.
 *	So the structure below is included in the mapchannel
 */
typedef struct  
{
	std::vector<missile_t> list;
}missileInfo_t;

void missile_initForMapchannel(mapChannel_t *mapChannel);
void missile_check(mapChannel_t *mapChannel, sint32 passedTime);

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, sint32 damage, sint32 actionId, sint32 actionArgId);

//#define MISSILE_PISTOL      1
//#define MISSILE_RIFLE      2
//#define MISSILE_NETGUN      10
//#define MISSILE_THRAX_PISTOL  3
//#define MISSILE_THRAX_RIFLE    4
//#define MELEE_THRAX        5 
//#define MELEE_BOARGAR      6
//#define THRAX_KICK        7
//#define MISSILE_LIGHTNING    8
//#define MELEE_PISTOL      9
//#define MISSILE_HUNTER_PULSEGUN 11
//#define MELEE_HUNTER 12
//#define MELEE_AMOEBOID 13
//#define MISSILE_AFSMECH_MG 14
//#define MELEE_RIFLE 15
//#define MISSILE_GRENADE 16
//#define MELEE_MACHINEGUN 17
//#define MISSILE_MACHINEGUN 18
//#define MELEE_SHOTGUN 19
//#define MISSILE_SHOTGUN 20
//#define MISSILE_PROPELLANT_ICE 21
//#define MISSILE_SHOTGUN_V3 22
//#define MISSILE_MACHINEGUN_V3 23
//#define MISSILE_STALKER 24

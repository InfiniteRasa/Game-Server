#if(_MSC_VER>=1600)
#define _HAS_ITERATOR_DEBUGGING 0
#endif
#include <vector>

typedef struct _missile_t
{
	int type;
	int damageA;
	int damageB;
	int actionId;
	int argId;
	unsigned long long targetEntityId; // the entityId of the destination (it is possible that the object does no more exist on arrival)
	actor_t* source;
	int triggerTime; // amount of milliseconds left before the missile is triggered, is decreased on every tick
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
void missile_check(mapChannel_t *mapChannel, int passedTime);

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, int type, int damage);
void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, int type, int damage, float maxRange);

#define MISSILE_PISTOL			1
#define MISSILE_RIFLE			2
#define MISSILE_THRAX_PISTOL	3
#define MISSILE_THRAX_RIFLE		4
#define MELEE_THRAX				5 
#define MELEE_BOARGAR			6
#define THRAX_KICK				7
#define MISSILE_LIGHTNING		8
#define MELEE_PISTOL			9
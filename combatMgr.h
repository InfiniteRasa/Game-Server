
typedef struct _missile_t
{
	int type;
	int damageA;
	int damageB;
	unsigned long long targetEntityId; // the entityId of the destination (it is possible that the object does no more exist on arrival)
	actor_t* source;
	int triggerTime; // amount of milliseconds left before the missile is triggered, is decreased on every tick
	_missile_t *previous;
	_missile_t *next;
}missile_t;

/*
 *	Missiles are, of course, map bound.
 *	So the structure below is included in the mapchannel
 */
typedef struct  
{
	missile_t *firstMissile;
}missileInfo_t;

void missile_initForMapchannel(mapChannel_t *mapChannel);
void missile_check(mapChannel_t *mapChannel, int passedTime);

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, int type, int damage);

#define MISSILE_PISTOL	1
#define MISSILE_LIGHTNING 2
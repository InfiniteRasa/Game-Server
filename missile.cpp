#include"Global.h"

void missile_initForMapchannel(mapChannel_t *mapChannel)
{
	mapChannel->missileInfo.firstMissile = NULL;
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, int type, int damage)
{
	missile_t *missile = (missile_t*)malloc(sizeof(missile_t));
	memset(missile, 0x00, sizeof(missile_t));
	missile->type = type;
	missile->damageA = damage;
	missile->targetEntityId = targetEntityId;
	// get distance between actors
	actor_t *targetActor = NULL;
	int targetType = entityMgr_getEntityType(targetEntityId);
	void *entity = entityMgr_get(targetEntityId);
	if( entity == NULL )
	{
		// entity does not exist
		free(missile);
		return;
	}
	if( targetType == ENTITYTYPE_NPC )
	{
		puts("Can't shoot NPCs yet");
		free(missile);
		return;
	}
	else if( targetType == ENTITYTYPE_CREATURE )
	{
		creature_t *creature = (creature_t*)entity;
		targetActor = &creature->actor;
	}
	else
	{
		puts("Can't shoot that object");
		free(missile);
		return;
	}
	// calculate missile time
	float dx = targetActor->posX - origin->posX;
	float dy = targetActor->posY - origin->posY;
	float dz = targetActor->posZ - origin->posZ;
	float distance = sqrt(dx*dx+dy*dy+dz*dz);
	if( type == MISSILE_PISTOL )
	{
		missile->triggerTime = (int)(distance*0.5f);
	}
	else
	{
		puts("Unknown missile type");
		free(missile);
		return;
	}
	// append to list
	if( mapChannel->missileInfo.firstMissile == NULL )
	{
		mapChannel->missileInfo.firstMissile = missile;
	}
	else
	{
		mapChannel->missileInfo.firstMissile->previous = missile;
		missile->next = mapChannel->missileInfo.firstMissile;
		mapChannel->missileInfo.firstMissile = missile;
	}
}

void _missile_trigger(mapChannel_t *mapChannel, missile_t *missile)
{
	pyMarshalString_t pms;
	// do work
	int targetType = entityMgr_getEntityType(missile->targetEntityId);
	void *entity = entityMgr_get(missile->targetEntityId);
	if( entity == NULL ) // check if entity still exists
		return;
	// do handling of the different types
	if( targetType == ENTITYTYPE_CREATURE )
	{
		creature_t *creature = (creature_t*)entity;
		if( creature->actor.state == ACTOR_STATE_DEAD )
			return;
		creature->currentHealth -= missile->damageA;
		if( creature->currentHealth <= 0 )
		{
			creature->actor.state = ACTOR_STATE_DEAD;
			// dead!
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_list_begin(&pms);
			pym_addInt(&pms, 5); // dead
			pym_list_end(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, 206, pym_getData(&pms), pym_getLen(&pms));
			// fix health
			creature->currentHealth = 0;
			// tell spawnpool if set
			if( creature->spawnPool )
			{
				spawnPool_decreaseAliveCreatureCount(mapChannel, creature->spawnPool);
				spawnPool_increaseDeadCreatureCount(mapChannel, creature->spawnPool);
			}
		}
		// send recovery
		//Recv_PerformRecovery 125
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, 102); // actionId
		pym_addInt(&pms, 1); // actionArg
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, 125, pym_getData(&pms), pym_getLen(&pms));

		// TODO: Should we use AnnounceDamage?
		// TODO: Also do use Recv_MadeDead for killing on load
		// update health (Recv_UpdateHealth 380)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, creature->currentHealth); // current
		pym_addInt(&pms, creature->type->maxHealth); // currentMax
		pym_addInt(&pms, 0); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, 380, pym_getData(&pms), pym_getLen(&pms));
	}
	else
		puts("No damage handling for that type yet");
}


void missile_check(mapChannel_t *mapChannel, int passedTime)
{
	missile_t *missile = mapChannel->missileInfo.firstMissile;
	while( missile )
	{
		missile->triggerTime -= passedTime;
		if( missile->triggerTime <= 0 )
		{
			// do missile action
			_missile_trigger(mapChannel, missile);
			// remove missile
			missile_t *nextMissile = missile->next;
			if(missile->previous == NULL )
			{
				// is first
				if( missile->next )
					missile->next->previous = NULL;
				mapChannel->missileInfo.firstMissile = NULL;
			}
			else
			{
				// is somewhere between
				if( missile->next )
					missile->next->previous = missile->previous;
				missile->previous->next = missile->next;
			}
			free(missile);
			missile = nextMissile;
			continue;
		}
		// next
		missile = missile->next;
	}
}
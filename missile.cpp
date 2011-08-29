#include"Global.h"

void missile_initForMapchannel(mapChannel_t *mapChannel)
{
	
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, int type, int damage)
{
	printf("launching missile\n");
	missile_t missile;
	missile.type = type;
	missile.damageA = damage;
	missile.targetEntityId = targetEntityId;
	missile.source = origin;
	// get distance between actors
	actor_t *targetActor = NULL;
	int targetType = entityMgr_getEntityType(targetEntityId);
	void *entity = entityMgr_get(targetEntityId);
	if( entity == NULL )
	{
		printf("the missile target doesnt exist\n");
		// entity does not exist
		return;
	}
	switch( targetType )
	{
	case ENTITYTYPE_NPC:
		printf("Can't shoot NPCs yet\n");
		return;
	case ENTITYTYPE_CREATURE:
		printf("target is a creature\n");
		{ creature_t *creature = (creature_t*)entity;
		targetActor = &creature->actor; }
		break;
		case ENTITYTYPE_CLIENT:
         printf("target is a player\n");
            { 
				mapChannelClient_t *player = (mapChannelClient_t*)entity;            
				targetActor = player->player->actor; 
			}
            break;
	default:
		printf("Can't shoot that object\n");
		return;
	};

	// calculate missile time
	float dx = targetActor->posX - origin->posX;
	float dy = targetActor->posY - origin->posY;
	float dz = targetActor->posZ - origin->posZ;
	float distance = sqrt(dx*dx+dy*dy+dz*dz);
	switch( type )
	{
	case MISSILE_PISTOL:
		missile.triggerTime = (int)(distance*0.5f);
		break;
	case MISSILE_LIGHTNING:
		missile.triggerTime = (int)(distance*0.5f);
		break;
	default:
		printf("Unknown missile type\n");
		return;
	};

	// append to list
	printf("Añadiendo misil a la lista\n");
	mapChannel->missileInfo.list.push_back(missile);
}

void _missile_trigger(mapChannel_t *mapChannel, missile_t *missile)
{
	printf("missile exploding\n");
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
	
		gameEffect_attachW2(mapChannel, &creature->actor, 102, 1,500);										
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
		/*
		// TODO: Should we use AnnounceDamage? (399)
		// (None, ((4101, (None, 0, 0, 0, 0, 10, 0, 0, 0, 0, None, None)),))
		// def Recv_AnnounceDamage(self, effectTarget, damageData):
		// for (targetId, rawInfo,) in damageData:
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addNoneStruct(&pms); // effectTarget (not used)
		pym_tuple_begin(&pms); // damageData start
		pym_tuple_begin(&pms); // targetId, rawInfo 1 start
		pym_addInt(&pms, creature->actor.entityId); // targetId
		pym_tuple_begin(&pms); // rawInfo start
		pym_addNoneStruct(&pms); //self.damageType = none
		pym_addInt(&pms, 0); //self.reflected = 0
		pym_addInt(&pms, 0); //self.filtered = 0
		pym_addInt(&pms, 0); //self.absorbed = 0
		pym_addInt(&pms, 0); //self.resisted = 0
		pym_addInt(&pms, missile->damageA); //self.finalAmt = missile->damageA
		pym_addInt(&pms, 0); //self.isCrit = 0
		pym_addInt(&pms, 0); //self.deathBlow = 0
		pym_addInt(&pms, 0); //self.coverModifier = 0
		pym_addInt(&pms, 0); //self.wasImmune = 0
		pym_addNoneStruct(&pms); //targetEffectIds // 131
		pym_addNoneStruct(&pms); //sourceEffectIds
		pym_tuple_end(&pms); // rawInfo end
		pym_tuple_end(&pms); // targetId, rawInfo 1 end
		pym_tuple_end(&pms); // damageData end
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->activeEffects->effectId, 399, pym_getData(&pms), pym_getLen(&pms));
		*/
		// TODO: Also do use Recv_MadeDead for killing on load
		// update health (Recv_UpdateHealth 380)
		printf("updating creature health\n");
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, creature->currentHealth); // current
		pym_addInt(&pms, creature->type->maxHealth); // currentMax
		pym_addInt(&pms, 0); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, 380, pym_getData(&pms), pym_getLen(&pms));
	}
	else if( targetType == ENTITYTYPE_CLIENT )
	{
		mapChannelClient_t *player = (mapChannelClient_t*)entity;
		if( player->player->actor->state == ACTOR_STATE_DEAD )
			return;
        //direct decrease health
		
		pym_init(&pms);
		pym_tuple_begin(&pms);
	
		gameEffect_attachW2(mapChannel, player->player->actor, 102, 1,500);										
		player->player->actor->stats.healthCurrent -= missile->damageA;
		 //send death notification when health <= zero
		if( player->player->actor->stats.healthCurrent <= 0 )
		{
			player->player->actor->state = ACTOR_STATE_DEAD;
			// dead!
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_list_begin(&pms);
			pym_addInt(&pms, 5); // dead
			pym_list_end(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, 
				                                    player->player->actor, 
													player->player->actor->entityId, 
													206, 
													pym_getData(&pms), pym_getLen(&pms));
			// fix health
			player->player->actor->stats.healthCurrent = 0;
			
		}

		// if still alive update health,play anims
		// update health (Recv_UpdateHealth 380) or 285
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, player->player->actor->stats.healthCurrent); // current
		pym_addInt(&pms, player->player->actor->stats.healthMax); // currentMax
		pym_addInt(&pms, 0); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel,
												 player->player->actor, 
			                                     player->player->actor->entityId, 380, 
												 pym_getData(&pms), pym_getLen(&pms));

	}
	else
		puts("No damage handling for that type yet");
}


void missile_check(mapChannel_t *mapChannel, int passedTime)
{
	std::vector<missile_t>* missileList = &mapChannel->missileInfo.list;
	if (missileList->size() > 0)
	{
		printf("Size of missile list: %i\n", missileList->size());
	}
	int removedMissiles = 0;
	for (int i = 0; i < missileList->size(); i++) 
	{
		printf("checking missile\n");
		missile_t* missile = &missileList->at(i);
		missile->triggerTime -= passedTime;

		if( missile->triggerTime <= 0 )
		{
			// do missile action
			_missile_trigger(mapChannel, missile);
			// remove missile
			missileList->erase(missileList->begin() + i - removedMissiles);
			++removedMissiles;
		}
	}
}
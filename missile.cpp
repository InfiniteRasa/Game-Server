#include"Global.h"

void missile_initForMapchannel(mapChannel_t *mapChannel)
{
	
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, int type, int damage)
{
	missile_launch(mapChannel, origin, targetEntityId, type, damage, -1);
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, int type, int damage, float maxRange)
{
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
		{ creature_t *creature = (creature_t*)entity;
		targetActor = &creature->actor; }
		break;
		case ENTITYTYPE_CLIENT:
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

	//check range
	if(maxRange >= 0){
		if(distance > maxRange){
			return;
		}
	}

	switch( type )
	{
	case MISSILE_PISTOL:
		missile.triggerTime = (int)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 133;
		break;
	case MELEE_PISTOL:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 4;
		break;
	case MISSILE_RIFLE:
		missile.triggerTime = (int)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 134;
		break;
	case MISSILE_THRAX_PISTOL:
		missile.triggerTime = (int)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 1;
		break;
	case MISSILE_THRAX_RIFLE:
		missile.triggerTime = (int)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 79;
		break;
	case MELEE_THRAX:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 46;
		break;
	//case THRAX_KICK:
	//	missile.triggerTime = 0;
	//	missile.actionId = 397;
	//	missile.argId = 1;
	//	break;
	case MELEE_BOARGAR:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 10;
		break;
	case MISSILE_LIGHTNING:
		missile.triggerTime = (int)(distance*0.5f);
		missile.actionId = 194;
		missile.argId = 1;
		break;
	default:
		printf("Unknown missile type %i\n", type);
		return;
	};

	// append to list
	mapChannel->missileInfo.list.push_back(missile);
}
/*
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
	
		gameEffect_attach(mapChannel, &creature->actor, 102, 1,500);										
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
	else if( targetType == ENTITYTYPE_CLIENT )
	{
		mapChannelClient_t *player = (mapChannelClient_t*)entity;
		if( player->player->actor->state == ACTOR_STATE_DEAD )
			return;
        //direct decrease health
		
		pym_init(&pms);
		pym_tuple_begin(&pms);
	
		gameEffect_attach(mapChannel, player->player->actor, 102, 1,500);										
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
*/
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

		pym_init(&pms);
		pym_tuple_begin(&pms);  						// Packet Start
		pym_addInt(&pms, missile->actionId);			// Action ID // 1 Weapon attack
		pym_addInt(&pms, missile->argId);				// Arg ID // 133 pistol physical not crouched
		pym_list_begin(&pms); 							// Hits Start
			pym_addInt(&pms, creature->actor.entityId);	// Each hit creature
		pym_list_end(&pms); 							// Hits End
		pym_list_begin(&pms); 							// Misses Start
		pym_list_end(&pms); 							// Misses End
		pym_list_begin(&pms); 							// Misses Data Start
		pym_list_end(&pms); 							// Misses Data End
		pym_list_begin(&pms); 							// Hits Data Start
			pym_tuple_begin(&pms); 						// Each Hit tuple start
				pym_addInt(&pms, creature->actor.entityId); // Creature entity ID
				pym_tuple_begin(&pms); 						// rawInfo start
						pym_addInt(&pms, 1); 				//self.damageType = normal
						pym_addInt(&pms, 0); 					//self.reflected = 0
						pym_addInt(&pms, 0); 					//self.filtered = 0
						pym_addInt(&pms, 0); 					//self.absorbed = 0
						pym_addInt(&pms, 0); 					//self.resisted = 0
						pym_addInt(&pms, missile->damageA); 	//self.finalAmt = missile->damageA
						pym_addInt(&pms, 0); 					//self.isCrit = 0
						pym_addInt(&pms, 0); 					//self.deathBlow = 0
						pym_addInt(&pms, 0); 					//self.coverModifier = 0
						pym_addInt(&pms, 0); 					//self.wasImmune = 0
 						//targetEffectIds // 131
						pym_list_begin(&pms);
						pym_list_end(&pms);
						//sourceEffectIds
						pym_list_begin(&pms);
						pym_list_end(&pms);
				pym_tuple_end(&pms); 						// rawInfo end
				pym_addNoneStruct(&pms);  					// OnHitData
			pym_tuple_end(&pms); 						// Each Hit tuple start
		pym_list_end(&pms); 							// Hits Data End
		pym_tuple_end(&pms); 							// Packet End
		// 311
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, 125, pym_getData(&pms), pym_getLen(&pms));
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
	else if( targetType == ENTITYTYPE_CLIENT )
	{
		mapChannelClient_t *client = (mapChannelClient_t*)entity;
		if( client->player->actor->state == ACTOR_STATE_DEAD )
			return;
        //direct decrease health
		
		pym_init(&pms);
		pym_tuple_begin(&pms);
	
		gameEffect_attach(mapChannel, client->player->actor, 102, 1,500);										
		client->player->actor->stats.healthCurrent -= missile->damageA;
		 //send death notification when health <= zero
		if( client->player->actor->stats.healthCurrent <= 0 )
		{
			client->player->actor->state = ACTOR_STATE_DEAD;
			// dead!
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_list_begin(&pms);
			pym_addInt(&pms, 5); // dead
			pym_list_end(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, 
				                                    client->player->actor, 
													client->player->actor->entityId, 
													206, 
													pym_getData(&pms), pym_getLen(&pms));
			// fix health
			client->player->actor->stats.healthCurrent = 0;
			
		}

		pym_init(&pms);
		pym_tuple_begin(&pms);  						// Packet Start
		pym_addInt(&pms, missile->actionId);			// Action ID // 1 Weapon attack
		pym_addInt(&pms, missile->argId);				// Arg ID // 133 pistol physical not crouched
		pym_list_begin(&pms); 							// Hits Start
			pym_addInt(&pms, client->player->actor->entityId);	// Each hit creature
		pym_list_end(&pms); 							// Hits End
		pym_list_begin(&pms); 							// Misses Start
		pym_list_end(&pms); 							// Misses End
		pym_list_begin(&pms); 							// Misses Data Start
		pym_list_end(&pms); 							// Misses Data End
		pym_list_begin(&pms); 							// Hits Data Start
			pym_tuple_begin(&pms); 						// Each Hit tuple start
				pym_addInt(&pms, client->player->actor->entityId); // Creature entity ID
				pym_tuple_begin(&pms); 						// rawInfo start
						pym_addInt(&pms, 1); 				//self.damageType = normal
						pym_addInt(&pms, 0); 					//self.reflected = 0
						pym_addInt(&pms, 0); 					//self.filtered = 0
						pym_addInt(&pms, 0); 					//self.absorbed = 0
						pym_addInt(&pms, 0); 					//self.resisted = 0
						pym_addInt(&pms, missile->damageA); 	//self.finalAmt = missile->damageA
						pym_addInt(&pms, 0); 					//self.isCrit = 0
						pym_addInt(&pms, 0); 					//self.deathBlow = 0
						pym_addInt(&pms, 0); 					//self.coverModifier = 0
						pym_addInt(&pms, 0); 					//self.wasImmune = 0
 						//targetEffectIds // 131
						pym_list_begin(&pms);
						pym_list_end(&pms);
						//sourceEffectIds
						pym_list_begin(&pms);
						pym_list_end(&pms);
				pym_tuple_end(&pms); 						// rawInfo end
				pym_addNoneStruct(&pms);  					// OnHitData
			pym_tuple_end(&pms); 						// Each Hit tuple start
		pym_list_end(&pms); 							// Hits Data End
		pym_tuple_end(&pms); 							// Packet End
		// 311
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, 125, pym_getData(&pms), pym_getLen(&pms));

		// update health (Recv_UpdateHealth 380) or 285
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.healthCurrent); // current
		pym_addInt(&pms, client->player->actor->stats.healthMax); // currentMax
		pym_addInt(&pms, 0); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel,
												 client->player->actor, 
			                                     client->player->actor->entityId, 380, 
												 pym_getData(&pms), pym_getLen(&pms));
	}
	else
		puts("No damage handling for that type yet");
}

void missile_check(mapChannel_t *mapChannel, int passedTime)
{
	std::vector<missile_t>* missileList = &mapChannel->missileInfo.list;
	int removedMissiles = 0;
	int totalMissiles = missileList->size();
	int i = 0;

	while(i < totalMissiles)
	{
		missile_t* missile = &missileList->at(i);
		missile->triggerTime -= passedTime;

		if( missile->triggerTime <= 0 )
		{
			// do missile action
			_missile_trigger(mapChannel, missile);
			// remove missile
			missileList->erase(missileList->begin() + i);
			++removedMissiles;
		}
		i -= removedMissiles;
		if( i < 0 ) { i = 0; }
		totalMissiles -= removedMissiles;
	}
}
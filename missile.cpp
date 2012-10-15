#include"Global.h"

void missile_initForMapchannel(mapChannel_t *mapChannel)
{
	
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, sint32 damage, sint32 actionId, sint32 actionArgId)
{
	missile_t missile;
	//missile.type = type;
	missile.damageA = damage;
	missile.targetEntityId = targetEntityId;
	missile.source = origin;
	// get distance between actors
	actor_t *targetActor = NULL;
	sint32 targetType = entityMgr_getEntityType(targetEntityId);
	void *entity = entityMgr_get(targetEntityId);
	if( entity == NULL )
	{
		printf("the missile target doesnt exist: %u\n",targetEntityId);
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
	//check range Disastorm @88450b338c
	//if(maxRange >= 0){
	//	if(distance > maxRange){
	//		return;	
	//	}
	//}
	missile.triggerTime = (sint32)(distance*0.5f);
	missile.actionId = actionId;
	missile.argId = actionArgId;
	// send windup
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, missile.actionId); // actionId
	pym_addInt(&pms, missile.argId); // actionArgId (subaction)
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, origin, origin->entityId, 126, pym_getData(&pms), pym_getLen(&pms));
	// append to list
	mapChannel->missileInfo.list.push_back(missile);
}

/*
 * Updates health, handles death and sends packets (creature)
 */
void missile_doDamage(mapChannel_t *mapChannel, creature_t* creature, sint32 damage, actor_t* damageBy)
{
	pyMarshalString_t pms;
	if( creature->actor.state == ACTOR_STATE_DEAD )
		return;	
	creature->actor.stats.healthCurrent -= damage;
	if( creature->actor.stats.healthCurrent <= 0 )
	{
		// fix health
		creature->actor.stats.healthCurrent = 0;
		creature_handleCreatureKill(mapChannel, creature, damageBy);
	}
	else
	{
		// shooting at wandering creatures makes them ANGRY
		if( creature->controller.currentAction == BEHAVIOR_ACTION_WANDER )
		{
			if( damageBy->owner )
				controller_setActionFighting(creature, damageBy->owner->clientEntityId); // player
			else
				controller_setActionFighting(creature, damageBy->entityId); // creature
		}
	}
	// update health (Recv_UpdateHealth 380)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, creature->actor.stats.healthCurrent); // current
	pym_addInt(&pms, creature->type->maxHealth); // currentMax
	pym_addInt(&pms, 0); // refreshAmount
	pym_addInt(&pms, 0); // whoId
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, METHODID_UPDATEHEALTH, pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Updates health, handles death and sends packets (player)
 */
void missile_doDamage(mapChannel_t *mapChannel, mapChannelClient_t* client, sint32 damage, actor_t* damageBy)
{
	pyMarshalString_t pms;
	if( client->player->actor->state == ACTOR_STATE_DEAD )
		return;

	pym_init(&pms);
	pym_tuple_begin(&pms);
	//gameEffect_attach(mapChannel, client->player->actor, 102, 1,500);	// the blood splat animation, we should use creature_action and weapon data to store hit fx info?							
	client->player->actor->stats.healthCurrent -= damage;
	// send death notification when health <= zero
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
<<<<<<< HEAD
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, METHODID_UPDATEHEALTH, pym_getData(&pms), pym_getLen(&pms));
	}
	else if( targetType == ENTITYTYPE_CLIENT )
	{
		mapChannelClient_t *client = (mapChannelClient_t*)entity;
		if( client->player->actor->state == ACTOR_STATE_DEAD )
			return;
        
		//direct decrease health
		gameEffect_attach(mapChannel, client->player->actor, 102, 1,500);										
		client->player->actor->stats.healthCurrent -= missile->damageA;
		 //send death notification when health <= zero
		if( client->player->actor->stats.healthCurrent <= 0 )
		{
			client->player->actor->state = ACTOR_STATE_DEAD;

			// Recv_ActorKilled
			pym_init(&pms);
			pym_tuple_begin(&pms);
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, 
				                                    client->player->actor, 
													client->player->actor->entityId, 
													776, 
													pym_getData(&pms), pym_getLen(&pms));

			// fix health
			client->player->actor->stats.healthCurrent = 0;

			//Recv_PlayerDead(sourceId, graveyardList, canRevive) -> None   
			// Notification that we are dying, and that 'sourceId' killed us.
			pym_init(&pms);
			pym_tuple_begin(&pms);
				pym_addInt(&pms, 0); // sourceId <- killer
				pym_tuple_begin(&pms); // graveyardList						
					pym_dict_begin(&pms);// begin graveyard 1 => cf PYTHON::/shared/graveyardinfo.py
						pym_dict_addKey(&pms,"Id");
							pym_addInt(&pms, 2); // id 
						pym_dict_addKey(&pms,"pos");
							pym_tuple_begin(&pms);//begin pos
								pym_addInt(&pms, -218); //'-58.4531'
								pym_addInt(&pms, 100);
								pym_addInt(&pms, -58);
							pym_tuple_end(&pms); // end pos
						pym_dict_addKey(&pms,"isSafe");
							pym_addInt(&pms, 0); //isSafe = False => not given
						pym_dict_addKey(&pms,"name");
							pym_addString(&pms,"Outpost Hospital"); // name = none => not given
					pym_dict_end(&pms); // end graveyard 1
				pym_tuple_end(&pms);
				pym_addBool(&pms, true); // canRevive
			pym_tuple_end(&pms);
			netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, 
				                                    client->player->actor, 
													client->player->actor->entityId, 
													METHODID_PLAYERDEAD, 
													pym_getData(&pms), pym_getLen(&pms));
			
		}
=======
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, 
			client->player->actor, 
			client->player->actor->entityId, 
			METHODID_STATECHANGE, 
			pym_getData(&pms), pym_getLen(&pms));

		// fix health
		client->player->actor->stats.healthCurrent = 0;
		// send method id 595 "PlayerDead"
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addBool(&pms, true); // canRevive
		pym_tuple_begin(&pms); // graveyardList						
		pym_dict_begin(&pms);// begin graveyard 1 => cf PYTHON::/shared/graveyardinfo.py
		pym_dict_addKey(&pms,"Id");
		pym_addInt(&pms, 2); // id 
		pym_dict_addKey(&pms,"pos");
		pym_tuple_begin(&pms);//begin pos
		pym_addInt(&pms, -218); //'-58.4531'
		pym_addInt(&pms, 100);
		pym_addInt(&pms, -58);
		pym_tuple_end(&pms); // end pos
		pym_dict_addKey(&pms,"isSafe");
		pym_addInt(&pms, 0); //isSafe = False => not given
		pym_dict_addKey(&pms,"name");
		pym_addString(&pms,"Outpost Hospital"); // name = none => not given
		pym_dict_end(&pms); // end graveyard 1
		pym_tuple_end(&pms);
		pym_addInt(&pms, client->player->actor->entityId); // ActorKilled
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, 
			client->player->actor, 
			client->player->actor->entityId, 
			METHODID_PLAYERDEAD, 
			pym_getData(&pms), pym_getLen(&pms));
	}
	// update health (Recv_UpdateHealth 380) or 285
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, client->player->actor->stats.healthCurrent); // current
	pym_addInt(&pms, client->player->actor->stats.healthCurrentMax); // currentMax
	pym_addFloat(&pms, client->player->actor->stats.regenHealthPerSecond); // refreshAmount
	pym_addInt(&pms, 0); // whoId
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel,
		client->player->actor, 
		client->player->actor->entityId, METHODID_UPDATEHEALTH, 
		pym_getData(&pms), pym_getLen(&pms));
}

/*
 * Handles all weapon use actions
 */
void missile_ActionRecoveryHandler_WeaponAttack(mapChannel_t *mapChannel, missile_t *missile)
{
	// todo: Some weapons can hit multiple targets
	pyMarshalString_t pms;
	sint32 targetType = entityMgr_getEntityType(missile->targetEntityId);
	void *entity = entityMgr_get(missile->targetEntityId);
	if( entity == NULL ) // check if entity still exists
		return;
	sint32 damage = missile->damageA;
	/* Execute action */
	pym_init(&pms);
	pym_tuple_begin(&pms);  						// Packet Start
	pym_addInt(&pms, missile->actionId);			// Action ID // 1 Weapon attack
	pym_addInt(&pms, missile->argId);				// Arg ID // 133 pistol physical not crouched
	pym_list_begin(&pms); 							// Hits Start
		pym_addLong(&pms, missile->targetEntityId);// Each hit creature	(ktb: Must be a long for some reason?)
	pym_list_end(&pms); 							// Hits End
	pym_list_begin(&pms); 							// Misses Start
	pym_list_end(&pms); 							// Misses End
	pym_list_begin(&pms); 							// Misses Data Start
	pym_list_end(&pms); 							// Misses Data End
	pym_list_begin(&pms); 							// Hits Data Start
		pym_tuple_begin(&pms); 						// Each Hit tuple start
			pym_addInt(&pms, missile->targetEntityId); // Creature entity ID
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
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, METHODID_PERFORMRECOVERY, pym_getData(&pms), pym_getLen(&pms));
	
	if( targetType == ENTITYTYPE_CREATURE )
		missile_doDamage(mapChannel, (creature_t*)entity, damage, missile->source);
	else if( targetType == ENTITYTYPE_CLIENT )
		missile_doDamage(mapChannel, (mapChannelClient_t*)entity, damage, missile->source);
	else
		printf("Unsupported entity type for missile_doDamage()\n");
}
>>>>>>> Second part of combat update

/*
 * Handles all weapon melee actions
 */
void missile_ActionRecoveryHandler_WeaponMelee(mapChannel_t *mapChannel, missile_t *missile)
{
	// forward to weapon attack for now, until there is better handling for melee weapons
	missile_ActionRecoveryHandler_WeaponAttack(mapChannel, missile);
}

/*
 * Handles all thrax kick actions
 */
void missile_ActionRecoveryHandler_ThraxKick(mapChannel_t *mapChannel, missile_t *missile)
{
	pyMarshalString_t pms;
	sint32 targetType = entityMgr_getEntityType(missile->targetEntityId);
	void *entity = entityMgr_get(missile->targetEntityId);
	if( entity == NULL ) // check if entity still exists
		return;
	sint32 damage = missile->damageA;
	/* Execute action */
	pym_init(&pms);
	pym_tuple_begin(&pms);  						// Packet Start
	pym_addInt(&pms, missile->actionId);			// Action ID // 1 Weapon attack
	pym_addInt(&pms, missile->argId);				// Arg ID // 133 pistol physical not crouched
	pym_list_begin(&pms); 							// Hits Start
		pym_addLong(&pms, missile->targetEntityId);// Each hit creature
	pym_list_end(&pms); 							// Hits End
	pym_list_begin(&pms); 							// Misses Start
	pym_list_end(&pms); 							// Misses End
	pym_list_begin(&pms); 							// Misses Data Start
	pym_list_end(&pms); 							// Misses Data End
	pym_list_begin(&pms); 							// Hits Data Start
		pym_tuple_begin(&pms); 						// Each Hit tuple start
			//pym_addInt(&pms, missile->targetEntityId); // thrax kick actions dont need this field for some reason?
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
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, METHODID_PERFORMRECOVERY, pym_getData(&pms), pym_getLen(&pms));
	if( targetType == ENTITYTYPE_CREATURE )
		missile_doDamage(mapChannel, (creature_t*)entity, damage, missile->source);
	else if( targetType == ENTITYTYPE_CLIENT )
	{
		missile_doDamage(mapChannel, (mapChannelClient_t*)entity, damage, missile->source);
		//todo: Make knockback effect work
		//gameEffect_attach(mapChannel, ((mapChannelClient_t*)entity)->player->actor, 10000082, 10000082, 1, 5000);
	}
	else
		printf("Unsupported entity type for missile_doDamage()\n");
}

void _missile_trigger(mapChannel_t *mapChannel, missile_t *missile)
{
	if( missile->actionId == 1 )
		missile_ActionRecoveryHandler_WeaponAttack(mapChannel, missile);
	else if( missile->actionId == 174 )
		missile_ActionRecoveryHandler_WeaponMelee(mapChannel, missile);
	else if( missile->actionId == 397 )
		missile_ActionRecoveryHandler_ThraxKick(mapChannel, missile);
	else
		printf("unsupported missile actionId (animation will freeze?)\n");
}

void missile_check(mapChannel_t *mapChannel, sint32 passedTime)
{
	std::vector<missile_t>* missileList = &mapChannel->missileInfo.list;
	sint32 removedMissiles = 0;
	sint32 totalMissiles = missileList->size();
	sint32 i = 0;

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
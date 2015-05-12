#include"Global.h"

void missile_trigger(mapChannel_t *mapChannel, missile_t *missile);

void missile_initForMapchannel(mapChannel_t *mapChannel)
{
	
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, sint32 damage, sint32 actionId, sint32 actionArgId)
{
	missile_t missile;
	//missile.type = type;
	missile.damageA = damage;
	missile.source = origin;
	// get distance between actors
	actor_t *targetActor = NULL;
	float triggerTime = 0; // time between windup and recovery
	if( targetEntityId )
	{
		// target on entity
		sint32 targetType = entityMgr_getEntityType(targetEntityId);
		void *entity = entityMgr_get(targetEntityId);
		if( entity == NULL )
		{
			printf("the missile target doesnt exist: %u\n", targetEntityId);
			// entity does not exist
			return;
		}
		switch( targetType )
		{
		case ENTITYTYPE_CREATURE:
			{ 
				creature_t *creature = (creature_t*)entity;
				targetActor = &creature->actor;
				missile.targetEntityId = targetEntityId;
			}
			break;
		case ENTITYTYPE_CLIENT:
			{ 
				mapChannelClient_t *player = (mapChannelClient_t*)entity;            
				targetActor = player->player->actor; 
				missile.targetEntityId = targetEntityId;
			}
			break;
		default:
			printf("Can't shoot that object\n");
			return;
		};
		if( targetActor->state == ACTOR_STATE_DEAD )
			return; // actor is dead, cannot be shot at
		float dx = targetActor->posX - origin->posX;
		float dy = targetActor->posY - origin->posY;
		float dz = targetActor->posZ - origin->posZ;
		float distance = sqrt(dx*dx+dy*dy+dz*dz);
		triggerTime = (sint32)(distance * 0.5f);
		// calculate missile time
		// todo (is this actually needed, since we have windup + recovery time anyway?)
	}
	else
	{
		// has no target -> Shoot towards looking angle
		targetActor = 0;
		triggerTime = 0;
	}

	// is the missile/action an ability that need needs to use Recv_PerformAbility?
	bool isAbility = false;
	if( actionId == 194 ) // recruit lighting ability
		isAbility = true;
	//check range Disastorm @88450b338c
	//if(maxRange >= 0){
	//	if(distance > maxRange){
	//		return;	
	//	}
	//}
	missile.targetActor = targetActor;
	missile.triggerTime = (sint32)triggerTime;
	missile.actionId = actionId;
	missile.argId = actionArgId;
	missile.isAbility = isAbility;
	// send windup and append to queue (only for non-abilities)
	if( isAbility == false )
	{
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, missile.actionId); // actionId
		pym_addInt(&pms, missile.argId); // actionArgId (subaction)
		if( missile.targetActor && missile.targetActor->entityId )
			pym_addInt(&pms, missile.targetActor->entityId); // all weapon attacks have a target?
		else
			pym_addNoneStruct(&pms); // no target
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, origin, origin->entityId, PerformWindup, pym_getData(&pms), pym_getLen(&pms));
		// append to list
		mapChannel->missileInfo.list.push_back(missile);
	}
	else
	{
		// abilities get applied directly without delay
		missile_trigger(mapChannel, &missile);
	}
}

/*
 * Updates health, handles death and sends packets (creature)
 */
void missile_doDamage(mapChannel_t *mapChannel, creature_t* creature, sint32 damage, actor_t* damageBy)
{
	pyMarshalString_t pms;
	if( creature->actor.state == ACTOR_STATE_DEAD )
		return;
	// decrease armor first
	sint32 armorDecrease = min(damage, creature->actor.stats.armorCurrent);
	creature->actor.stats.armorCurrent -= armorDecrease;
	// decrease health (if armor is depleted)
	creature->actor.stats.healthCurrent -= (damage-armorDecrease);
	if( creature->actor.stats.healthCurrent <= 0 )
	{
		// fix health
		creature->actor.stats.healthCurrent = 0;
		creature_handleCreatureKill(mapChannel, creature, damageBy);
	}
	else
	{
		// shooting at wandering creatures makes them ANGRY
		if( creature->controller.currentAction == BEHAVIOR_ACTION_WANDER || creature->controller.currentAction == BEHAVIOR_ACTION_FOLLOWINGPATH )
		{
			if( damageBy->owner )
				controller_setActionFighting(creature, damageBy->owner->clientEntityId); // player
			else
				controller_setActionFighting(creature, damageBy->entityId); // creature
		}
	}
	// update health (Recv_UpdateHealth 380)
	if( armorDecrease != damage )
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, creature->actor.stats.healthCurrent); // current
		pym_addInt(&pms, creature->type->maxHealth); // currentMax
		pym_addInt(&pms, 0); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, UpdateHealth, pym_getData(&pms), pym_getLen(&pms));
	}
	// update armor (Recv_UpdateArmor 380)
	if( armorDecrease > 0 )
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, creature->actor.stats.armorCurrent); // current
		pym_addInt(&pms, creature->actor.stats.armorCurrentMax); // currentMax
		pym_addInt(&pms, 0); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, &creature->actor, creature->actor.entityId, UpdateArmor, pym_getData(&pms), pym_getLen(&pms));
	}
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
	// decrease armor first
	sint32 armorDecrease = min(damage, client->player->actor->stats.armorCurrent);
	client->player->actor->stats.armorCurrent -= armorDecrease;
	// decrease health (if armor is depleted)
	client->player->actor->stats.healthCurrent -= (damage-armorDecrease);
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
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, client->player->actor, client->player->actor->entityId, StateChange, pym_getData(&pms), pym_getLen(&pms));
		//direct decrease health
		//gameEffect_attach(mapChannel, client->player->actor, 102, 1,500);										
		//client->player->actor->stats.healthCurrent -= missile->damageA;
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
					PlayerDead, 
					pym_getData(&pms), pym_getLen(&pms));
		}
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
			PlayerDead, 
			pym_getData(&pms), pym_getLen(&pms));
	}
	if( armorDecrease != damage )
	{
		// update health (even when dead)
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.healthCurrent); // current
		pym_addInt(&pms, client->player->actor->stats.healthCurrentMax); // currentMax
		pym_addFloat(&pms, client->player->actor->stats.regenHealthPerSecond); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel,
			client->player->actor, 
			client->player->actor->entityId, UpdateHealth, 
			pym_getData(&pms), pym_getLen(&pms));
	}
	if( armorDecrease > 0 )
	{
		// update armor
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, client->player->actor->stats.armorCurrent); // current
		pym_addInt(&pms, client->player->actor->stats.armorCurrentMax); // currentMax
		//pym_addFloat(&pms, client->player->actor->stats.regenHealthPerSecond); // refreshAmount
		pym_addFloat(&pms, client->player->actor->stats.armorRegenCurrent);
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel,
			client->player->actor, 
			client->player->actor->entityId, UpdateArmor, 
			pym_getData(&pms), pym_getLen(&pms));
	}
}

/*
 * Handles all weapon use actions
 */
void missile_ActionRecoveryHandler_WeaponAttack(mapChannel_t *mapChannel, missile_t *missile)
{
	// todo: Some weapons can hit multiple targets
	pyMarshalString_t pms;
	sint32 targetType = 0;
	void *entity = NULL;
	if( missile->targetActor )
	{
		targetType = entityMgr_getEntityType(missile->targetEntityId);
		entity = entityMgr_get(missile->targetEntityId);
	}
	sint32 damage = missile->damageA;
	/* Execute action */
	pym_init(&pms);
	pym_tuple_begin(&pms);  						// Packet Start
	pym_addInt(&pms, missile->actionId);			// Action ID // 1 Weapon attack
	pym_addInt(&pms, missile->argId);				// Arg ID // 133 pistol physical not crouched
	pym_list_begin(&pms); 							// Hits Start
		if( entity )
			pym_addLong(&pms, missile->targetActor->entityId);//pym_addLong(&pms, missile->targetEntityId);// Each hit creature	(ktb: Must be a long for some reason?)
	pym_list_end(&pms); 							// Hits End
	pym_list_begin(&pms); 							// Misses Start
	pym_list_end(&pms); 							// Misses End
	pym_list_begin(&pms); 							// Misses Data Start
	pym_list_end(&pms); 							// Misses Data End
	pym_list_begin(&pms); 							// Hits Data Start
	if( entity )
	{
		pym_tuple_begin(&pms); 						// Each Hit tuple start
			pym_addInt(&pms, missile->targetActor->entityId); // Creature entity ID
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
		pym_tuple_end(&pms); 						// Each Hit tuple end
	}
	pym_list_end(&pms); 							// Hits Data End
	pym_tuple_end(&pms); 							// Packet End
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, PerformRecovery, pym_getData(&pms), pym_getLen(&pms));
	
	if( targetType == 0 )
	{
		// todo
	}
	else if( targetType == ENTITYTYPE_CREATURE )
		missile_doDamage(mapChannel, (creature_t*)entity, damage, missile->source);
	else if( targetType == ENTITYTYPE_CLIENT )
		missile_doDamage(mapChannel, (mapChannelClient_t*)entity, damage, missile->source);
	else
		printf("Unsupported entity type for missile_doDamage()\n");
}

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
		pym_addLong(&pms, missile->targetActor->entityId);// Each hit creature
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
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, PerformRecovery, pym_getData(&pms), pym_getLen(&pms));
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


/*
 * Handles use of the recruit lighting ability
 */
void missile_ActionHandler_Lighting(mapChannel_t *mapChannel, missile_t *missile)
{
	// Despite being an ability, most abilities dont use Recv_PerformObjectAbility
	pyMarshalString_t pms;
	sint32 targetType = 0;
	void *entity = NULL;
	if( missile->targetActor )
	{
		targetType = entityMgr_getEntityType(missile->targetEntityId);
		entity = entityMgr_get(missile->targetEntityId);
	}
	sint32 damage = missile->damageA;
	/* Execute action */
	pym_init(&pms);
	pym_tuple_begin(&pms);  						// Packet Start
	pym_addInt(&pms, missile->actionId);			// Action ID // 1 Weapon attack
	pym_addInt(&pms, missile->argId);				// Arg ID // 133 pistol physical not crouched
	pym_list_begin(&pms); 							// Hits Start
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, 0); // hit index
	pym_addInt(&pms, missile->targetActor->entityId);
	//pym_tuple_end(&pms);
		//if( entity )
		//	pym_addLong(&pms, missile->targetActor->entityId);//pym_addLong(&pms, missile->targetEntityId);// Each hit creature	(ktb: Must be a long for some reason?)
	pym_list_end(&pms); 							// Hits End
	pym_list_begin(&pms); 							// Misses Start
	pym_list_end(&pms); 							// Misses End
	pym_list_begin(&pms); 							// Misses Data Start
	pym_list_end(&pms); 							// Misses Data End
	pym_list_begin(&pms); 							// Hits Data Start
	if( entity )
	{
		pym_tuple_begin(&pms); 						// Each Hit tuple start
			//pym_addInt(&pms, missile->targetActor->entityId); // Creature entity ID
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
			pym_tuple_begin(&pms);  					// OnHitData
			pym_tuple_begin(&pms);						// OnHitData - arcData
			pym_tuple_end(&pms);
			pym_tuple_end(&pms);
		pym_tuple_end(&pms); 						// Each Hit tuple end
	}
	pym_list_end(&pms); 							// Hits Data End
	pym_tuple_end(&pms); 							// Packet End
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, PerformRecovery, pym_getData(&pms), pym_getLen(&pms));
	if( targetType == 0 )
	{
		// todo
	}
	else if( targetType == ENTITYTYPE_CREATURE )
		missile_doDamage(mapChannel, (creature_t*)entity, damage, missile->source);
	else if( targetType == ENTITYTYPE_CLIENT )
		missile_doDamage(mapChannel, (mapChannelClient_t*)entity, damage, missile->source);
	else
		printf("Unsupported entity type for missile_doDamage()\n");
}

void missile_trigger(mapChannel_t *mapChannel, missile_t *missile)
{
	if( missile->actionId == 1 )
		missile_ActionRecoveryHandler_WeaponAttack(mapChannel, missile);
	else if( missile->actionId == 174 )
		missile_ActionRecoveryHandler_WeaponMelee(mapChannel, missile);
	else if( missile->actionId == 194 )
		missile_ActionHandler_Lighting(mapChannel, missile);
	else if( missile->actionId == 397 )
		missile_ActionRecoveryHandler_ThraxKick(mapChannel, missile);
	else
	{
		printf("unsupported missile actionId %d - using default: WeaponAttack\n", missile->actionId);
		missile_ActionRecoveryHandler_WeaponAttack(mapChannel, missile);
	}
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
			missile_trigger(mapChannel, missile);
			// remove missile
			missileList->erase(missileList->begin() + i);
			++removedMissiles;
		}
		i -= removedMissiles;
		if( i < 0 ) { i = 0; }
		totalMissiles -= removedMissiles;
	}
}


//
//void _testCreatureBirthAction(mapChannelClient_t* cm)
//{
//	pyMarshalString_t pms;
//	pym_init(&pms);
//	pym_tuple_begin(&pms);
//	pym_addInt(&pms, 155); // actionId
//	pym_addInt(&pms, 3762); // actionArgId (subaction)
//	pym_tuple_end(&pms);
//	netMgr_cellDomain_pythonAddMethodCallRaw(cm, cm->player->actor->entityId, PerformWindup, pym_getData(&pms), pym_getLen(&pms));
//
//	return;
//	//printf("continue\n");
//	//Sleep(12000);
//	/* Execute action */
//	pym_init(&pms);
//	pym_tuple_begin(&pms);  						// Packet Start
//	pym_addInt(&pms, 155);			// Action ID // 1 Weapon attack
//	pym_addInt(&pms, 3762);				// Arg ID // 133 pistol physical not crouched
//	//pym_list_begin(&pms); 							// Hits Start
//	//pym_addNoneStruct(&pms);
//	////pym_addLong(&pms, missile->targetActor->entityId);// Each hit creature
//	//pym_list_end(&pms); 							// Hits End
//	//pym_list_begin(&pms); 							// Misses Start
//	//pym_list_end(&pms); 							// Misses End
//	//pym_list_begin(&pms); 							// Misses Data Start
//	//pym_list_end(&pms); 							// Misses Data End
//	//pym_list_begin(&pms); 							// Hits Data Start
//	//pym_tuple_begin(&pms); 						// Each Hit tuple start
//	////pym_addInt(&pms, missile->targetEntityId); // thrax kick actions dont need this field for some reason?
//	////pym_tuple_begin(&pms); 						// rawInfo start
//	////pym_addInt(&pms, 1); 				//self.damageType = normal
//	////pym_addInt(&pms, 0); 					//self.reflected = 0
//	////pym_addInt(&pms, 0); 					//self.filtered = 0
//	////pym_addInt(&pms, 0); 					//self.absorbed = 0
//	////pym_addInt(&pms, 0); 					//self.resisted = 0
//	////pym_addInt(&pms, missile->damageA); 	//self.finalAmt = missile->damageA
//	////pym_addInt(&pms, 0); 					//self.isCrit = 0
//	////pym_addInt(&pms, 0); 					//self.deathBlow = 0
//	////pym_addInt(&pms, 0); 					//self.coverModifier = 0
//	////pym_addInt(&pms, 0); 					//self.wasImmune = 0
//	//////targetEffectIds // 131
//	////pym_list_begin(&pms);
//	////pym_list_end(&pms);
//	//////sourceEffectIds
//	////pym_list_begin(&pms);
//	////pym_list_end(&pms);
//	////pym_tuple_end(&pms); 						// rawInfo end
//	//pym_addNoneStruct(&pms);
//	//pym_addNoneStruct(&pms);  					// OnHitData
//	//pym_tuple_end(&pms); 						// Each Hit tuple start
//	//pym_list_end(&pms); 							// Hits Data End
//	pym_tuple_end(&pms); 							// Packet End
//	netMgr_cellDomain_pythonAddMethodCallRaw(cm, cm->player->actor->entityId, PerformRecovery, pym_getData(&pms), pym_getLen(&pms));
//}

/*
 * Called when a player tries to fire a shoot from the currently armed weapon
 */
void missile_playerTryFireWeapon(mapChannelClient_t* cm)
{
	pyMarshalString_t pms;
	// get used weapon
	item_t* item = inventory_CurrentWeapon(cm);
	if( item == NULL )
		return; // no weapon armed but player tries to shoot?
	if( item->itemTemplate->item.type != ITEMTYPE_WEAPON )
		return; // item is not a weapon (it shouldn't be possible to get non-weapons on the weapondrawer, but we check anyway)
	// has ammo?
	if( item->weaponData.ammoCount < item->itemTemplate->weapon.ammoPerShot )
		return; // not enough ammo for a single shot
	// decrease ammo
	item->weaponData.ammoCount -= item->itemTemplate->weapon.ammoPerShot;
	// update ammo in database
	DataInterface_Character_updateCharacterAmmo(cm->tempCharacterData->characterID, item->locationSlotIndex, item->weaponData.ammoCount);
	// weapon ammo info
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, item->weaponData.ammoCount);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, item->entityId, WeaponAmmoInfo, pym_getData(&pms), pym_getLen(&pms));
	// calculate damage
	sint32 damageRange = item->itemTemplate->weapon.maxDamage-item->itemTemplate->weapon.minDamage;
	damageRange = max(damageRange, 1); // to avoid division by zero in the next line
	sint32 damage = item->itemTemplate->weapon.minDamage+(rand()%damageRange);
	// for now we just ignore no-target attacks
	//if( cm->player->targetEntityId == 0 )
	//	return;
	// launch correct missile type depending on weapon type
	if( item->itemTemplate->weapon.altActionId == 1 )
	{
		// weapon range attacks
		if( item->itemTemplate->weapon.altActionArg == 133 ) // physical pistol
			missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, damage, item->itemTemplate->weapon.altActionId, item->itemTemplate->weapon.altActionArg); // pistol
		else
			printf("missile_playerTryFireWeapon: Unsupported weapon altActionArg (action %d/%d)\n", item->itemTemplate->weapon.altActionId, item->itemTemplate->weapon.altActionArg);
	}
	else
		printf("missile_playerTryFireWeapon: Unsupported weapon altActionId (action %d/%d)\n", item->itemTemplate->weapon.altActionId, item->itemTemplate->weapon.altActionArg);


	// melee stuff :O
	//if(inventory_CurrentWeapon(cm)->itemTemplate->item.classId == 27220)
	//	missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 5); // rifle
	//else if(inventory_CurrentWeapon(cm)->itemTemplate->item.classId == 27320)
	//	missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 6); // shotgun
	//else if(inventory_CurrentWeapon(cm)->itemTemplate->item.classId == 28066)
	//	missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 3); // machinegun
	//else
	//	missile_launch(cm->mapChannel, cm->player->actor, cm->player->targetEntityId, 20, 174, 4); // pistol
}
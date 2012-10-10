#include"Global.h"

extern sint32 gridL1;
extern  sint32 gridL2;
extern sint32 gridCount;
extern sint32** entityPosGrid;

void missile_initForMapchannel(mapChannel_t *mapChannel)
{
	
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, sint32 type, sint32 damage)
{
  missile_launch(mapChannel, origin, targetEntityId, type, damage, -1);
}

void missile_launch(mapChannel_t *mapChannel, actor_t *origin, unsigned long long targetEntityId, sint32 type, sint32 damage, float maxRange)
{
	missile_t missile;
	missile.type = type;
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
	if(maxRange >= 0){
		if(distance > maxRange){
			return;	
		}
	}

	switch( type )
	{
	case MISSILE_PISTOL:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 133;
	break;
	case MELEE_PISTOL:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 4;
	break;
	case MELEE_RIFLE:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 5;
	break;
	case MELEE_MACHINEGUN:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 3;
	break;
	case MISSILE_MACHINEGUN:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 149;
		missile.argId = 1;
	break;
	case MISSILE_MACHINEGUN_V3:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 149;
		missile.argId = 7;
	break;
	case MISSILE_RIFLE:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 134;
	break;
	case MISSILE_THRAX_PISTOL:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 1;
	break;
	case MISSILE_GRENADE:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 6;
	break;
	case MISSILE_PROPELLANT_ICE:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 3;
	break;
	case MISSILE_THRAX_RIFLE:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 79;
	break;
		case MELEE_THRAX:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 46;
	break;
	case THRAX_KICK:
		 missile.triggerTime = 0;
		 missile.actionId = 397;
		 missile.argId = 1;
	break;
	case MELEE_BOARGAR:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 10;
	break;
	case MISSILE_LIGHTNING:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 194;
		missile.argId = 1;
	break;
	case MISSILE_HUNTER_PULSEGUN:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 244;
	break;
	case MELEE_HUNTER:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 33;
	break;
	case MELEE_AMOEBOID:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 431;
	break;
	case MISSILE_AFSMECH_MG:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 293;
		missile.argId = 1;
	break;
	case MISSILE_SHOTGUN:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 121;
	break;
	case MISSILE_SHOTGUN_V3:
		missile.triggerTime = (sint32)(distance*0.5f);
		missile.actionId = 1;
		missile.argId = 287;
	break;
	case MELEE_SHOTGUN:
		missile.triggerTime = 0;
		missile.actionId = 174;
		missile.argId = 6;
	break;
	case MISSILE_STALKER:
		missile.triggerTime = (sint32)(distance*0.5f);;
		missile.actionId = 1;
		missile.argId = 78;
	break;
	default:
		printf("Unknown missile type\n");
		return;
	};

	// append to list
	mapChannel->missileInfo.list.push_back(missile);
}

void _missile_trigger(mapChannel_t *mapChannel, missile_t *missile)
{
	pyMarshalString_t pms;
	// do work
	sint32 targetType = entityMgr_getEntityType(missile->targetEntityId);
	void *entity = entityMgr_get(missile->targetEntityId);
	if( entity == NULL ) // check if entity still exists
		return;
	// do handling of the different types
	if( targetType == ENTITYTYPE_CREATURE )
	{
		creature_t *creature = (creature_t*)entity;
		if( creature->actor.state == ACTOR_STATE_DEAD )
			return;
										
		creature->actor.stats.healthCurrent -= missile->damageA;
		if( creature->actor.stats.healthCurrent <= 0 )
		{
			// fix health
			creature->actor.stats.healthCurrent = 0;
			creature_handleCreatureKill(mapChannel, creature, missile->source);
		}

		pym_init(&pms);
		pym_tuple_begin(&pms);  						// Packet Start
		pym_addInt(&pms, missile->actionId);			// Action ID // 1 Weapon attack
		pym_addInt(&pms, missile->argId);				// Arg ID // 133 pistol physical not crouched
		pym_list_begin(&pms); 							// Hits Start
			pym_addLong(&pms, creature->actor.entityId);// Each hit creature	(ktb: Must be a long for some reason?)
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
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, missile->source, missile->source->entityId, METHODID_PERFORMRECOVERY, pym_getData(&pms), pym_getLen(&pms));
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
		pym_addInt(&pms, client->player->actor->stats.healthCurrentMax); // currentMax
		pym_addFloat(&pms, client->player->actor->stats.regenHealthPerSecond); // refreshAmount
		pym_addInt(&pms, 0); // whoId
		pym_tuple_end(&pms);
		netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel,
												 client->player->actor, 
			                                     client->player->actor->entityId, METHODID_UPDATEHEALTH, 
												 pym_getData(&pms), pym_getLen(&pms));
	}
	else
		puts("No damage handling for that type yet");
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
#include"Global.h"

// update the walk/runspeed of the actor
void _gameEffect_updateMovementMod(mapChannel_t *mapChannel, actor_t *actor)
{
	float movementMod = 1.0f; 
	// check for sprint
	gameEffect_t *gameeffect = actor->activeEffects;
	while( gameeffect )
	{
		if( gameeffect->typeId == EFFECT_TYPE_SPRINT )
		{
			// apply sprint bonus
			movementMod += 0.10f;
			movementMod += (float)gameeffect->effectLevel * 0.10f;
			break;
		}
		// next
		gameeffect = gameeffect->next;
	}
	// todo: other modificators?
	// Recv_MovementModChange 580
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addFloat(&pms, movementMod); //speed
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, actor, actor->entityId, 580, pym_getData(&pms), pym_getLen(&pms));	
}



void _gameEffect_addToList(actor_t *actor, gameEffect_t *gameEffect)
{
	// fast and simple prepend list 
	gameEffect->next = actor->activeEffects;
	if( gameEffect->next )
		gameEffect->next->previous = gameEffect;
	gameEffect->previous = NULL;
	actor->activeEffects = gameEffect;
}

void _gameEffect_removeFromList(actor_t *actor, gameEffect_t *gameEffect)
{
	// fast and simple prepend list 
	if( gameEffect->previous == NULL )
	{
		// is first element
		if( gameEffect->next )
			gameEffect->next->previous = NULL;
		actor->activeEffects = gameEffect->next;
	}
	else
	{
		// is not first element
		if( gameEffect->next )
			gameEffect->next->previous = gameEffect->previous;
		gameEffect->previous->next = gameEffect->next;
	}
}

/*
 * Attach game effect sprint
 */
void gameEffect_attachSprint(mapChannel_t *mapChannel, actor_t *actor, sint32 effectLevel, sint32 duration)
{
	mapChannel->currentEffectID++; // generate new effectId
	sint32 effectId = mapChannel->currentEffectID;
	// effectId -> The id used to identify the effect when sending/receiving effect related data (similiar to entityId, just for effects)
	// typeId -> The id used to lookup the effect class and animation
	// level -> The sub id of the effect, some effects have multiple levels (especially the ones linked with player abilities)
	// create effect struct
	gameEffect_t *gameEffect = (gameEffect_t*)malloc(sizeof(gameEffect_t));
	// setup struct
	gameEffect->aliveTime = duration; // 5 seconds (test)
	gameEffect->time = 0; // reset timer
	gameEffect->typeId = EFFECT_TYPE_SPRINT;
	gameEffect->effectId = effectId;
	gameEffect->effectLevel = effectLevel;
	// add to list
	_gameEffect_addToList(actor, gameEffect);
	// inform clients
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, (sint32)gameEffect->typeId);	//typeId
	pym_addInt(&pms, (sint32)gameEffect->effectId);	//effectId
	pym_addInt(&pms, (sint32)gameEffect->effectLevel);	//level
	pym_addInt(&pms, (sint32)0);//sourceId
	//pym_addNoneStruct(&pms);
	pym_addBool(&pms, (sint32)true);//announce
	pym_dict_begin(&pms);//tooltipDict
	// keys:
	// 'duration'
	// 'damageType'
	// 'attrId'
	// 'isActive'
	// 'isBuff'
	// 'isDebuff'
	// 'isNegativeEffect'
	pym_dict_end(&pms);
	// args (variable)
	pym_tuple_begin(&pms);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, actor, actor->entityId, 74, pym_getData(&pms), pym_getLen(&pms));
	// do ability specific work
	_gameEffect_updateMovementMod(mapChannel, actor);
}

///*
//	Attaches a GameEffect (buffs, etc.) to an entityId
//*/
//void gameEffect_attach(mapChannel_t *mapChannel, uint64 entityId, sint32 typeId, sint32 effectId, sint32 effectLevel, sint32 duration)
//{
//	sint32 entityType = entityMgr_getEntityType(entityId);
//	void *entity = entityMgr_get(entityId);
//	if( entity == NULL ) { return; }
//	if (entityType == ENTITYTYPE_CREATURE)
//	{
//		gameEffect_attach(mapChannel, &((creature_t*)entity)->actor, typeId, effectId, effectLevel, duration);
//	}
//	else if (entityType == ENTITYTYPE_CLIENT)
//	{
//		gameEffect_attach(mapChannel, ((mapChannelClient_t*)entity)->player->actor, typeId, effectId, effectLevel, duration);
//	}
//	else
//	{
//		printf("Could not attach game effect");
//		return; 
//	}
//}

void gameEffect_dettach(mapChannel_t *mapChannel, actor_t *actor, gameEffect_t *gameEffect)
{
	// inform clients (Recv_GameEffectDetached 75)
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, gameEffect->effectId); //effectId
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, actor, actor->entityId, 75, pym_getData(&pms), pym_getLen(&pms));
	// remove from list
	_gameEffect_removeFromList(actor, gameEffect);
	// do ability specific work
	if( gameEffect->typeId == EFFECT_TYPE_SPRINT )
		_gameEffect_updateMovementMod(mapChannel, actor);
	// more todo..
	// free effect
	free(gameEffect);
}

/*
	Periodically called for all clients in a mapContext
	Updates and applys effects of abilities or items
*/
void gameEffect_checkForPlayers(mapChannelClient_t **clients, sint32 count, sint32 passedTime)
{
	for(sint32 c=0; c<count; c++)
	{
		mapChannelClient_t *client = clients[c];
		if( client->player == NULL )
			continue;
		actor_t *actor = client->player->actor;
		gameEffect_t *gameEffect = client->player->actor->activeEffects;
		while( gameEffect )
		{
			// mapChannelClient_t

			// add time
			gameEffect->time += passedTime;
			// stop effect if too old
			if( gameEffect->time >= gameEffect->aliveTime )
			{
				gameEffect_t *nextGameEffect = gameEffect->next;
				gameEffect_dettach(client->mapChannel, actor, gameEffect);
				gameEffect = nextGameEffect;
				continue;
			}
			// next
			gameEffect = gameEffect->next;
		}
	}
}
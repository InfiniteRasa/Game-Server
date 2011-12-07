#include"Global.h"

// update the walk/runspeed of the actor
void _gameEffect_updateMovementMod(mapChannel_t *mapChannel, actor_t *actor)
{
	float movementMod = 1.0f; 
	// check for sprint
	gameEffect_t *gameeffect = actor->activeEffects;
	while( gameeffect )
	{
		if( gameeffect->effectId == EFFECTID_SPRINT )
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
	Attaches a GameEffect (buffs, etc.) to a actor entity.
*/
void gameEffect_attach(mapChannel_t *mapChannel, unsigned int entityId, int effectId, int effectLevel, int duration)
{
	int entityType = entityMgr_getEntityType(entityId);
	void *entity = entityMgr_get(entityId);
	if( entity == NULL ) { return; }
	creature_t *creature = NULL;
	if (entityType == ENTITYTYPE_CREATURE)
	{ creature = (creature_t*)entity; }
	else { return; }
	gameEffect_attach(mapChannel, &creature->actor, effectId, effectLevel, duration);
}

void gameEffect_attach(mapChannel_t *mapChannel, actor_t *actor, int effectId, int effectLevel, int duration)
{
	// check if a effect with the same ID already exists
	gameEffect_t *link = actor->activeEffects;
	while( link )
	{
		if( link->effectId == effectId )
			return; // already exists
		// next
		link = link->next;
	}
	// create effect struct
	gameEffect_t *gameEffect = (gameEffect_t*)malloc(sizeof(gameEffect_t));
	// setup struct
	gameEffect->aliveTime = duration; // 5 seconds (test)
	gameEffect->time = 0; // reset timer
	gameEffect->effectId = effectId;
	gameEffect->effectLevel = effectLevel;
	// add to list
	_gameEffect_addToList(actor, gameEffect);
	// inform clients
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, (int)gameEffect->effectId);	//typeId
	pym_addInt(&pms, (int)gameEffect->effectId);	//effectId
	pym_addInt(&pms, (int)gameEffect->effectLevel);	//level
	pym_addInt(&pms, (int)0);//sourceId
	pym_addBool(&pms, (int)true);//announce
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
	//args ( variable)
	pym_dict_begin(&pms);
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel, actor, actor->entityId, 74, pym_getData(&pms), pym_getLen(&pms));
	// do ability specific work
	if( effectId == EFFECTID_SPRINT )
		_gameEffect_updateMovementMod(mapChannel, actor);
	// more todo..
}

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
	if( gameEffect->effectId == EFFECTID_SPRINT )
		_gameEffect_updateMovementMod(mapChannel, actor);
	// more todo..
	// free effect
	free(gameEffect);
}

/*
	Periodically called for all clients in a mapContext
	Updates and applys effects of abilities or items
*/
void gameEffect_checkForPlayers(mapChannelClient_t **clients, int count, int passedTime)
{
	for(int c=0; c<count; c++)
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
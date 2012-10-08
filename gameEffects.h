typedef struct _gameEffect_t
{
	sint32 effectId;
	sint32 effectLevel;
	sint32 aliveTime;
	sint32 time;
	// list
	_gameEffect_t *previous;
	_gameEffect_t *next;
}gameEffect_t;

#define EFFECTID_SPRINT	247

void gameEffect_attach(mapChannel_t *mapChannel, uint32 entityId, sint32 effectId, sint32 effectLevel, sint32 duration = 5000);
void gameEffect_attach(mapChannel_t *mapChannel, actor_t *actor, sint32 effectId, sint32 effectLevel, sint32 duration = 5000);
void gameEffect_dettach(mapChannel_t *mapChannel, actor_t *actor, gameEffect_t *gameEffect);
void gameEffect_checkForPlayers(mapChannelClient_t **clients, sint32 count, sint32 passedTime);
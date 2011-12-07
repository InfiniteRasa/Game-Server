typedef struct _gameEffect_t
{
	int effectId;
	int effectLevel;
	int aliveTime;
	int time;
	// list
	_gameEffect_t *previous;
	_gameEffect_t *next;
}gameEffect_t;

#define EFFECTID_SPRINT	247

void gameEffect_attach(mapChannel_t *mapChannel, unsigned int entityId, int effectId, int effectLevel, int duration = 5000);
void gameEffect_attach(mapChannel_t *mapChannel, actor_t *actor, int effectId, int effectLevel, int duration = 5000);
void gameEffect_dettach(mapChannel_t *mapChannel, actor_t *actor, gameEffect_t *gameEffect);
void gameEffect_checkForPlayers(mapChannelClient_t **clients, int count, int passedTime);
typedef struct _gameEffect_t
{
	sint32 typeId; // effect class
	sint32 effectId; // effect id
	sint32 effectLevel;
	sint32 aliveTime;
	sint32 time;
	// list
	_gameEffect_t *previous;
	_gameEffect_t *next;
}gameEffect_t;

//#define EFFECTID_SPRINT	247

#define EFFECT_TYPE_SPRINT		247
#define EFFECT_TYPE_KNOCKBACK	8

#define EFFECT_ID_SPRINT		247

void gameEffect_attachSprint(mapChannel_t *mapChannel, actor_t *actor, sint32 effectLevel, sint32 duration);


void gameEffect_dettach(mapChannel_t *mapChannel, actor_t *actor, gameEffect_t *gameEffect);
void gameEffect_checkForPlayers(mapChannelClient_t **clients, sint32 count, sint32 passedTime);
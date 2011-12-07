
typedef struct  
{
	int currentAction;
	int faction; // 'team'
	// combat info
	long long targetEntityId;
}behaviorState_t;


#define BEHAVIOR_ACTION_IDLE				0
#define BEHAVIOR_ACTION_FOLLOWINGPATH		1
#define BEHAVIOR_ACTION_FIGHTING			2
#define BEHAVIOR_ACTION_WANDER			    3  
#define BEHAVIOR_ACTION_PATROL              4	

void controller_initForMapChannel(mapChannel_t *mapChannel);
void controller_mapChannelThink(mapChannel_t *mapChannel);
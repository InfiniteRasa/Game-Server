
#define PATH_LENGTH_LIMIT	72

typedef struct  
{
	uint32 deadTime; // amount of time that has passed since the actor died
	sint32 currentAction;
	sint32 faction; // 'team'
	// combat info
	uint32 timerPathUpdateLock; // avoids path-update-spamming for permanently moving units
	//long long targetEntityId;
	// path info
	float path[3*PATH_LENGTH_LIMIT]; // calculate path nodes
	// maybe we can optimize this to not waste as much memory?
	sint32 pathIndex; // the path node we are currently at
	sint32 pathLength; // how many nodes the current path has, 0 means no active path
	//sint32 path;
	struct  
	{
		float lockedTargetPosition[3]; // the creature position we are pathing to
		uint64 targetEntityId;
	}actionFighting;
	struct  
	{
		sint8 state;
		float wanderDestination[3];
	}actionWander;
	uint32 actionLockTime[8];
}behaviorState_t;


#define BEHAVIOR_ACTION_IDLE				0
#define BEHAVIOR_ACTION_FOLLOWINGPATH		1
#define BEHAVIOR_ACTION_FIGHTING			2
#define BEHAVIOR_ACTION_WANDER			    3  
#define BEHAVIOR_ACTION_PATROL              4	

void controller_initForMapChannel(mapChannel_t *mapChannel);
void controller_mapChannelThink(mapChannel_t *mapChannel);

void controller_setActionFighting(creature_t *creature, uint64 targetEntityId);
#include"global.h"

/*
 * Returns true if the actor is currently executing an action
 */
bool actor_hasActiveAction(actor_t* actor)
{
	if( actor->currentAction.actionId == 0 )
		return false;
	return true;
}

/*
 * Completes the action
 */
void actor_completeCurrentAction(mapChannel_t* mapChannel, actor_t* actor)
{
	if( actor->currentAction.actionId == 0 )
		return; // no active action
	if( actor->currentAction.actorActionUpdateCallback )
		actor->currentAction.actorActionUpdateCallback(mapChannel, actor, ACTOR_ACTION_STATE_COMPLETE);
	actor->currentAction.actionId = 0;
}


bool _cb_actor_actionCompletedTimer(mapChannel_t* mapChannel, void* param, sint32 timePassed)
{
	actor_t* actor = (actor_t*)param;
	actor_completeCurrentAction(mapChannel, actor);
	// return false to delete the timer immediately
	return false;
}

/*
 * Starts a new action on a given entityId
 */
void actor_startActionOnEntity(mapChannel_t* mapChannel, actor_t* actor, sint64 targetEntityId, sint32 actionId, sint32 actionArgId, sint32 windupTime, sint32 recoverTime, void(*actorActionUpdateCallback)(mapChannel_t* mapChannel, actor_t* actor, sint32 newActionState))
{
	if( actor->currentAction.actionId != 0 )
		return;
	// update current action data
	actor->currentAction.actionId = actionId;
	actor->currentAction.actionArgId = actionArgId;
	actor->currentAction.targetEntityId = targetEntityId;
	actor->currentAction.actorActionUpdateCallback = actorActionUpdateCallback;
	if( windupTime >= 0 )
		mapChannel_registerTimer(mapChannel, windupTime, actor, _cb_actor_actionCompletedTimer);
	else if( windupTime == 0 )
		_cb_actor_actionCompletedTimer(mapChannel, actor, 0);
}

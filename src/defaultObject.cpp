#include"global.h"

void defaultObject_create(mapChannel_t *mapChannel, float x, float y, float z, sint32 classId)
{
	dynObject_t *dynObject = _dynamicObject_create(classId, 0);//OBJECTTYPE_BANE_DROPSHIP);
	if( !dynObject )
		return;
	dynObject->objectData = NULL;
	dynObject->stateId = 0;
	float randomX = 2.0f - (float)(rand()%100) * 0.04f;
	float randomY = 0.5f - (float)(rand()%100) * 0.01f;
	float randomZ = 2.0f - (float)(rand()%100) * 0.04f;
	dynamicObject_setPosition(dynObject, x + randomX, y + randomY, z + randomZ);
	// randomize rotation
	float randomRotY = (float)(rand()%640) * 0.01f;
	float randomRotX = 0.0f;
	float randomRotZ = 0.0f;
	dynamicObject_setRotation(dynObject, randomRotY, randomRotX, randomRotZ);
	cellMgr_addToWorld(mapChannel, dynObject);
}

void defaultObject_destroy(mapChannel_t *mapChannel, dynObject_t *dynObject)
{
	// called shortly before free()
}

void defaultObject_appearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// called whenever the object appears in player sight		
}

void defaultObject_disappearForPlayers(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t **playerList, sint32 playerCount)
{
	// called before the object is removed from player sight
}

bool defaultObject_periodicCallback(mapChannel_t *mapChannel, dynObject_t *dynObject, uint8 timerID, sint32 timePassed)
{
	// called whenever any registered periodic timer fires
	return true;
}

void defaultObject_useObject(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{
	// called whenever any client sends a UseRequest
}

void defaultObject_interruptUse(mapChannel_t *mapChannel, dynObject_t *dynObject, mapChannelClient_t* client, sint32 actionID, sint32 actionArg)
{

}

dynObjectType_t dynObjectType_defaultObject = 
{
	defaultObject_destroy, // destroy
	defaultObject_appearForPlayers, // appearForPlayers
	defaultObject_disappearForPlayers, // disappearForPlayer
	defaultObject_periodicCallback, // periodicCallback
	defaultObject_useObject, // useObject
	defaultObject_interruptUse
};
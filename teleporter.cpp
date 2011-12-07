
#include "global.h"

void _cb_teleporter_initForMapChannel(void *param, diJob_teleporterData *jobData)
{
	mapChannel_t *mapChannel = (mapChannel_t*)param;
	
	for(int i=0; i<jobData->scount; i++)
	{
		di_teleporterData *teleporterList = jobData->tdata+i;
		
		/*dynamicObject_developer_createCustom(mapChannel,teleporterList->modelid,
														teleporterList->sx,
														teleporterList->sy,
														teleporterList->sz);*/
		dynObject_t *dynObject = _dynamicObject_create(teleporterList->modelid, 
			                                           teleporterList->type); 
		if( !dynObject )
			return;
		dynamicObject_setPosition(dynObject, teleporterList->sx, teleporterList->sy, teleporterList->sz);
		//dynObject->stateId = 1;
		cellMgr_addToWorld(mapChannel, dynObject);
	}

	mapChannel->loadState = 1; // loading is done
}


void teleporter_initForMapChannel(mapChannel_t *mapChannel)
{
	mapChannel->loadState = 0;
	dataInterface_teleporter_getList(mapChannel->mapInfo->contextId,_cb_teleporter_initForMapChannel, mapChannel);
	while( mapChannel->loadState == 0 ) Sleep(100);
}
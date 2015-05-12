#include "global.h"
//
//bool teleporter_process_waypoint(mapChannel_t *mapChannel, dynObject_t *dynObject, sint32 timePassed)
//{
//
//
//	return true;
//}

void _cb_teleporter_initForMapChannel(void *param, diJob_teleporterData *jobData)
{
	mapChannel_t *mapChannel = (mapChannel_t*)param;
	
	for(sint32 i=0; i<jobData->scount; i++)
	{
		di_teleporterData *teleporterList = jobData->tdata+i;
		
		/*dynamicObject_developer_createCustom(mapChannel,teleporterList->modelid,
														teleporterList->sx,
														teleporterList->sy,
														teleporterList->sz);*/
		dynObject_t *dynObject = NULL;
		//_dynamicObject_create(teleporterList->modelid, teleporterList->type); 
		if (mapChannel->mapInfo->contextId == teleporterList->contextId)
		{
			if (teleporterList->type == OBJECTTYPE_WAYPOINT)
			{
				dynObject = waypoint_create(mapChannel, teleporterList->sx, teleporterList->sy, teleporterList->sz, 0.0f, teleporterList->id, teleporterList->nameId, teleporterList->contextId);
			}
			else if (teleporterList->type == OBJECTTYPE_BASEWORMHOLE)
			{
				dynObject = wormhole_create(mapChannel, teleporterList->sx, teleporterList->sy, teleporterList->sz, 0.0f, teleporterList->id, teleporterList->nameId, teleporterList->contextId);
				printf("wormhole created \n");
			}
			if (!dynObject)
			{
				printf("Unknown teleporter type(%d) in db\n", teleporterList->type);
				continue;
			}
		}
	}

	mapChannel->loadState = 1; // loading is done
}


void teleporter_initForMapChannel(mapChannel_t *mapChannel)
{
	mapChannel->loadState = 0;
	DataInterface_teleporter_getList(mapChannel->mapInfo->contextId,_cb_teleporter_initForMapChannel, mapChannel);
	while( mapChannel->loadState == 0 ) Sleep(100);
}
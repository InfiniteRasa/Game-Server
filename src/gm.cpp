#include"global.h"

/*
 * Inits the data to create path
 * Triggered by entering .createpath
 */
void gm_startPathCreation(mapChannelClient_t *cm)
{
	if( cm->player->gmData == NULL )
		return; // not a gm
	if( cm->player->gmData->pathCreation )
	{
		// path creation is already active
		communicator_systemMessage(cm, "Please complete the last path with .finishpath first, or use .discardpath to delete the path");
		return;
	}
	cm->player->gmData->pathCreation = new gmPathCreation_t();
	communicator_systemMessage(cm, "Starting path creation...");
}

/*
 * Adds a new node to the path
 * Triggered by entering .addnode
 */
void gm_addPathNode(mapChannelClient_t *cm)
{
	if( cm->player->gmData == NULL )
		return; // not a gm
	if( cm->player->gmData->pathCreation == NULL )
	{
		// path creation is not active
		communicator_systemMessage(cm, "Please start a path with .createpath first");
		return;
	}
	// init node and add it to the list
	gmPathNode_t gmPathNode;
	gmPathNode.x = cm->player->actor->posX;
	gmPathNode.y = cm->player->actor->posY;
	gmPathNode.z = cm->player->actor->posZ;
	// create an object only visible for the gm
	gmPathNode.entityId_nodeDebugObject = 20000000 + rand()*6500 + rand(); // random entity (and we hope it is free)
	// create object entity
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, gmPathNode.entityId_nodeDebugObject); // entityID
	pym_addInt(&pms, 26582); // classID
	pym_addNoneStruct(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	// set position
	pym_init(&pms);
	pym_tuple_begin(&pms);
	// position
	pym_tuple_begin(&pms);
	pym_addFloat(&pms, cm->player->actor->posX);		// x
	pym_addFloat(&pms, cm->player->actor->posY+0.1f);	// y
	pym_addFloat(&pms, cm->player->actor->posZ);		// z
	pym_tuple_end(&pms); 
	// rotation quaternion
	pym_tuple_begin(&pms);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 0.0f);
	pym_addFloat(&pms, 1.0f);
	pym_tuple_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cm->cgm, gmPathNode.entityId_nodeDebugObject, 243, pym_getData(&pms), pym_getLen(&pms));	
	communicator_systemMessage(cm, "Created object");
	// add to node list
	cm->player->gmData->pathCreation->pathNodes.push_back(gmPathNode);
	// tell user we created the path
	char textMsg[256];
	sint32 pathNodeIndex = cm->player->gmData->pathCreation->pathNodes.size();
	sprintf(textMsg, "Path node with index %d added (%f %f %f)\n", pathNodeIndex, cm->player->actor->posX, cm->player->actor->posY, cm->player->actor->posZ);
	communicator_systemMessage(cm, textMsg);
}

/*
 * Deletes the currently generated path
 * Triggered by entering .discardpath
 */
void gm_discardPath(mapChannelClient_t *cm)
{
	if( cm->player->gmData == NULL )
		return; // not a gm
	if( cm->player->gmData->pathCreation == NULL )
	{
		// path creation is not active
		communicator_systemMessage(cm, "No path active");
		return;
	}
	// delete all entities
	gmPathNode_t *pathNodeList = &cm->player->gmData->pathCreation->pathNodes[0];
	sint32 pathNodeCount = cm->player->gmData->pathCreation->pathNodes.size();
	for(sint32 f=0; f<pathNodeCount; f++)
	{
		// send client destroy packet for each debug entity (node object)
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, pathNodeList[f].entityId_nodeDebugObject); // entityID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, 5, METHODID_DESTROYPHYSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	}
	// delete path
	delete cm->player->gmData->pathCreation;
	cm->player->gmData->pathCreation = NULL;
	communicator_systemMessage(cm, "Active path discarded");
}

/*
 * Analyzes, sets path mode and saves the path
 * Triggered by entering .finishpath
 */
void gm_finishPath(mapChannelClient_t *cm, bool attachToLastSpawnpool)
{
	if( cm->player->gmData == NULL )
		return; // not a gm
	if( cm->player->gmData->pathCreation == NULL )
	{
		// path creation is not active
		communicator_systemMessage(cm, "No path active");
		return;
	}
	// if attaching to spawnpool, test if there is a spawnpool
	if( cm->player->gmData->lastCreatedSpawnpool == 0 )
	{
		// no spawnpool
		communicator_systemMessage(cm, "You have not created a spawnpool in this session");
		return;
	}
	// delete all entities
	gmPathNode_t *pathNodeList = &cm->player->gmData->pathCreation->pathNodes[0];
	sint32 pathNodeCount = cm->player->gmData->pathCreation->pathNodes.size();
	for(sint32 f=0; f<pathNodeCount; f++)
	{
		// send client destroy packet for each debug entity (node object)
		pyMarshalString_t pms;
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, pathNodeList[f].entityId_nodeDebugObject); // entityID
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cm->cgm, 5, METHODID_DESTROYPHYSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	}
	// analyze path to get mode (cyclic or one-shot)
	float startLoc[3];
	float endLoc[3];
	startLoc[0] = pathNodeList[0].x;
	startLoc[1] = pathNodeList[0].y;
	startLoc[2] = pathNodeList[0].z;
	endLoc[0] = pathNodeList[pathNodeCount-1].x;
	endLoc[1] = pathNodeList[pathNodeCount-1].y;
	endLoc[2] = pathNodeList[pathNodeCount-1].z;
	float distX = startLoc[0] - endLoc[0];
	float distY = startLoc[1] - endLoc[1];
	float distZ = startLoc[2] - endLoc[2];
	float dist = sqrt(distX*distX+distY*distY+distZ*distZ);
	bool modeIsCyclic = false;
	if( dist < 9.0f )
		modeIsCyclic = true;
	// save path
	diData_path_t pathData = {0};
	pathData.contextId = cm->mapChannel->mapInfo->contextId;
	pathData.mode = modeIsCyclic?1:0;
	pathData.pathId = 0;
	pathData.numberOfNodes = pathNodeCount;
	pathData.pathnodes = (diData_pathNodes_t*)malloc(sizeof(diData_pathNodes_t)*pathNodeCount);
	for(sint32 f=0; f<pathNodeCount; f++)
	{
		pathData.pathnodes[f].pos[0] = pathNodeList[f].x;
		pathData.pathnodes[f].pos[1] = pathNodeList[f].y;
		pathData.pathnodes[f].pos[2] = pathNodeList[f].z;
	}
	if( attachToLastSpawnpool )
		pathData.spawnpool = cm->player->gmData->lastCreatedSpawnpool;
	DataInterface_Creature_savePath(&pathData);
	free(pathData.pathnodes);
	sint32 pathId = pathData.pathId;
	// delete path
	delete cm->player->gmData->pathCreation;
	cm->player->gmData->pathCreation = NULL;
	char msgText[256];
	char* pathMode = modeIsCyclic?"cyclic":"one-shot";
	sprintf(msgText, "Path saved. pathId: %d Mode: %s Nodes: %d Spawnpool: %d%s", pathId, pathMode, pathNodeCount, pathData.spawnpool, (pathData.spawnpool==0)?"(none)":"");
	communicator_systemMessage(cm, msgText);
}

/*
 * Called to parse gm commands (to keep communicator.cpp a bit more clean)
 * Returns true if the message should be blocked and not written to the chat
 */
bool gm_parseGmCommands(mapChannelClient_t *cm, char *textMsg)
{
	if( memcmp(textMsg, ".createpath", 11) == 0 )
	{
		gm_startPathCreation(cm);
		// we also immediately create our first path node
		gm_addPathNode(cm);
		return true;
	}
	else if( memcmp(textMsg, ".addnode", 8) == 0 )
	{
		gm_addPathNode(cm);
		return true;
	}
	else if( memcmp(textMsg, ".discardpath", 12) == 0 )
	{
		gm_discardPath(cm);
		return true;
	}
	else if( memcmp(textMsg, ".finishpath", 11) == 0 )
	{
		gm_finishPath(cm, false);
		return true;
	}
	else if( memcmp(textMsg, ".spawnpoolpath", 14) == 0 )
	{
		gm_finishPath(cm, true);
		return true;
	}
	return false; // do not suppress message
}
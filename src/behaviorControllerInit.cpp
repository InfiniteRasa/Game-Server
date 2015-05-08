#include"global.h"


// big global AI class 
struct 
{
	aiPath_t* pathList;
	sint32 numberOfPaths;
}aiEnv;

bool _controllerDataLoaded;
void _controller_init_getPathList(void *param, diJob_path_t *jobData)
{
	aiEnv.numberOfPaths = jobData->pathCount;
	aiEnv.pathList = (aiPath_t*)malloc(sizeof(aiPath_t) * jobData->pathCount);
	for(sint32 i=0; i<jobData->pathCount; i++)
	{
		aiEnv.pathList[i].pathId = jobData->pathList[i].pathId;
		aiEnv.pathList[i].mode = jobData->pathList[i].mode;
		aiEnv.pathList[i].spawnpool = jobData->pathList[i].spawnpool;
		aiEnv.pathList[i].nodeOffsetRandomization = jobData->pathList[i].nodeOffsetRandomization;
		aiEnv.pathList[i].numberOfPathNodes = jobData->pathList[i].numberOfNodes;
		aiEnv.pathList[i].pathNodeList = (aiPathNode_t*)malloc(sizeof(aiPathNode_t)*aiEnv.pathList[i].numberOfPathNodes);
		for(sint32 f=0; f<aiEnv.pathList[i].numberOfPathNodes; f++)
		{
			aiEnv.pathList[i].pathNodeList[f].pos[0] = jobData->pathList[i].pathnodes[f].pos[0];
			aiEnv.pathList[i].pathNodeList[f].pos[1] = jobData->pathList[i].pathnodes[f].pos[1];
			aiEnv.pathList[i].pathNodeList[f].pos[2] = jobData->pathList[i].pathnodes[f].pos[2];
		}
		// is the path connected to a spawnpool?

	}
	printf("AI: Loaded %d paths\n", jobData->pathCount);
	_controllerDataLoaded = true;
}

/*
 * Scans for attached paths for the spawnpool
 */
void controller_attachPathsToSpawnpool(spawnPool_t* spawnpool)
{
	// we count the attached paths first
	sint32 numberOfAttachedPaths = 0;
	for(sint32 i=0; i<aiEnv.numberOfPaths; i++)
	{
		if( aiEnv.pathList[i].spawnpool == spawnpool->id )
		{
			numberOfAttachedPaths++;
		}
	}
	// no path?
	if( numberOfAttachedPaths == 0 )
		return;
	// init list
	spawnpool->pathCount = numberOfAttachedPaths;
	spawnpool->pathList = (aiPath_t**)malloc(sizeof(aiPath_t*) * numberOfAttachedPaths);
	// add paths to list
	numberOfAttachedPaths = 0;
	for(sint32 i=0; i<aiEnv.numberOfPaths; i++)
	{
		if( aiEnv.pathList[i].spawnpool == spawnpool->id )
		{
			spawnpool->pathList[numberOfAttachedPaths] = &aiEnv.pathList[i];
			numberOfAttachedPaths++;
		}
	}
}


/*
 * Called on server boot, after all other data has been read from the database
 */
void controller_init()
{
	_controllerDataLoaded = true;
	DataInterface_Creature_getPathList(_controller_init_getPathList, NULL);
	while( _controllerDataLoaded == false ) Sleep(10);
}
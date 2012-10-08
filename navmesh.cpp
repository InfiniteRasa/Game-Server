#include "global.h"

typedef struct  
{
	sint32 tileX;
	sint32 tileY;
	float tileWidth;
	float tileHeight;
	sint32 maxPoly;
	sint32 maxTiles;
	float orig[3];
}tr_navmesh_meta_t;

uint32 gameMap_generateSimpleMapNameHash(char* mapname)
{
	uint32 hash = 0xE9CB876F;
	while( *mapname )
	{
		uint32 temp = (uint32)*mapname;
		hash ^= temp;
		hash = (hash<<29) | (hash>>3);
		mapname++;
	}
	return hash;
}

void navmesh_initForMapChannel(mapChannel_t *mapChannel)
{
	uint32 mapLoadId = gameMap_generateSimpleMapNameHash(mapChannel->mapInfo->name);
	// load detour navmesh tiles
	char navmeshFilePath[512];
	// load meta data first
	sprintf(navmeshFilePath, "navmesh/map%08X.nmt", mapLoadId);
	file_t* navMetaFile = fileMgr_open(navmeshFilePath);
	if( navMetaFile == NULL )
	{
		printf("Failed to load NavMesh meta file\n");
		return;
	}
	tr_navmesh_meta_t meta = {0};
	fileMgr_readData(navMetaFile, &meta, sizeof(tr_navmesh_meta_t));
	fileMgr_close(navMetaFile);
	dtNavMeshParams  params = {0};
	params.maxPolys = meta.maxPoly;
	params.maxTiles = meta.maxTiles;
	params.orig[0] = meta.orig[0];
	params.orig[1] = meta.orig[1];
	params.orig[2] = meta.orig[2];
	params.tileWidth = meta.tileWidth;
	params.tileHeight = meta.tileHeight;
	dtNavMesh* navMesh = new dtNavMesh();
	navMesh->init(&params);
	// name format: map[%08X:mapcontext_id]_[%d:tileX]_[%d:tileY].nav
	sint32 tilesX = 100; // we expect 100 tiles along the x axis
	sint32 tilesY = 100; // we expect 100 tiles along the z axis
	sint32 loadedTileCount = 0; // number of tiles actually loaded
	sprintf(navmeshFilePath, "navmesh/map%08X.nav", mapLoadId);
	file_t* navTileFile = fileMgr_open(navmeshFilePath);
	if( navTileFile == NULL )
	{
		printf("Failed to load NavMesh data file\n");
		return;
	}
	if( navTileFile )
	{
		for(sint32 i=0; i<1024*1024; i++)
		{
			uint32 navMeshDataBlockSize = fileMgr_readU32(navTileFile);
			sint32 tx = fileMgr_readS32(navTileFile);
			sint32 ty = fileMgr_readS32(navTileFile);
			if( navMeshDataBlockSize == 0 && tx == 0 && ty == 0 )
				break;
			// map data into memory	
			sint32 navTileSize = navMeshDataBlockSize;
			uint8* navTileData = (uint8*)malloc(navTileSize);
			fileMgr_readData(navTileFile, navTileData, navTileSize);
			if( navMesh->addTile(navTileData, navTileSize, DT_TILE_FREE_DATA, 0, 0) == 0 )
			{
				printf("Failed to load NavMesh tile %d/%d\n", tx, ty);
			}
			loadedTileCount++;
		}
		fileMgr_close(navTileFile);
	}
	else
	{
		printf("Failed to load navmesh data file\n");
	}
	mapChannel->navMesh = navMesh;
	dtNavMeshQuery* navMeshQuery = new dtNavMeshQuery();
	navMeshQuery->init(navMesh, 0x8000);
	mapChannel->navMeshQuery = navMeshQuery;
	printf("Navmesh loaded\n");
}

#define MAX_PATH_LENGTH				PATH_LENGTH_LIMIT
#define MAX_POLY					PATH_LENGTH_LIMIT
#define INVALID_POLYREF				0
#define SMOOTH_PATH_STEP_SIZE		4.0f
#define SMOOTH_PATH_SLOP			0.1f

uint32 fixupCorridor(dtPolyRef* path, uint32 npath, uint32 maxPath,
								 const dtPolyRef* visited, uint32 nvisited)
{
	sint32 furthestPath = -1;
	sint32 furthestVisited = -1;

	// Find furthest common polygon.
	for (sint32 i = npath-1; i >= 0; --i)
	{
		bool found = false;
		for (sint32 j = nvisited-1; j >= 0; --j)
		{
			if (path[i] == visited[j])
			{
				furthestPath = i;
				furthestVisited = j;
				found = true;
			}
		}
		if (found)
			break;
	}

	// If no intersection found just return current path.
	if (furthestPath == -1 || furthestVisited == -1)
		return npath;

	// Concatenate paths.

	// Adjust beginning of the buffer to include the visited.
	uint32 req = nvisited - furthestVisited;
	uint32 orig = uint32(furthestPath+1) < npath ? furthestPath+1 : npath;
	uint32 size = npath-orig > 0 ? npath-orig : 0;
	if (req+size > maxPath)
		size = maxPath-req;

	if (size)
		memmove(path+req, path+orig, size*sizeof(dtPolyRef));

	// Store visited
	for (uint32 i = 0; i < req; ++i)
		path[i] = visited[(nvisited-1)-i];

	return req+size;
}

bool inRangeYZX(const float* v1, const float* v2, float r, float h)
{
	float dx = v2[0] - v1[0];
	float dy = v2[1] - v1[1]; // elevation
	float dz = v2[2] - v1[2];
	return (dx*dx + dz*dz) < r*r && fabsf(dy) < h;
}

bool getSteerTarget(mapChannel_t *mapChannel, const float* startPos, const float* endPos,
								float minTargetDist, const dtPolyRef* path, uint32 pathSize,
								float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef)
{
	// Find steer target.
	static const uint32 MAX_STEER_POINTS = 3;
	float steerPath[MAX_STEER_POINTS*3];
	unsigned char steerPathFlags[MAX_STEER_POINTS];
	dtPolyRef steerPathPolys[MAX_STEER_POINTS];
	uint32 nsteerPath = 0;
	dtStatus dtResult = mapChannel->navMeshQuery->findStraightPath(startPos, endPos, path, pathSize,
		steerPath, steerPathFlags, steerPathPolys, (int*)&nsteerPath, MAX_STEER_POINTS);
	if (!nsteerPath || (dtResult&DT_SUCCESS) == 0 )
		return false;

	// Find vertex far enough to steer to.
	uint32 ns = 0;
	while (ns < nsteerPath)
	{
		// Stop at Off-Mesh link or when point is further than slop away.
		if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
			!inRangeYZX(&steerPath[ns*3], startPos, minTargetDist, 1000.0f))
			break;
		ns++;
	}
	// Failed to find good point to steer to.
	if (ns >= nsteerPath)
		return false;

	dtVcopy(steerPos, &steerPath[ns*3]);
	steerPos[1] = startPos[1];  // keep Z value
	steerPosFlag = steerPathFlags[ns];
	steerPosRef = steerPathPolys[ns];

	return true;
}

dtStatus findSmoothPath(mapChannel_t *mapChannel, const float* startPos, const float* endPos, dtQueryFilter* m_filter,
									const dtPolyRef* polyPath, uint32 polyPathSize,
									float* smoothPath, int* smoothPathSize, uint32 maxSmoothPathSize)
{
	*smoothPathSize = 0;
	uint32 nsmoothPath = 0;

	dtPolyRef polys[MAX_PATH_LENGTH];
	memcpy(polys, polyPath, sizeof(dtPolyRef)*polyPathSize);
	uint32 npolys = polyPathSize;

	float iterPos[3], targetPos[3];
	if(DT_SUCCESS != mapChannel->navMeshQuery->closestPointOnPolyBoundary(polys[0], startPos, iterPos))
		return DT_FAILURE;

	if(DT_SUCCESS != mapChannel->navMeshQuery->closestPointOnPolyBoundary(polys[npolys-1], endPos, targetPos))
		return DT_FAILURE;

	dtVcopy(&smoothPath[nsmoothPath*3], iterPos);
	nsmoothPath++;

	// Move towards target a small advancement at a time until target reached or
	// when ran out of memory to store the path.
	while (npolys && nsmoothPath < maxSmoothPathSize)
	{
		// Find location to steer towards.
		float steerPos[3];
		unsigned char steerPosFlag;
		dtPolyRef steerPosRef = INVALID_POLYREF;

		if (!getSteerTarget(mapChannel, iterPos, targetPos, SMOOTH_PATH_SLOP, polys, npolys, steerPos, steerPosFlag, steerPosRef))
			break;

		bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END);
		bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION);

		// Find movement delta.
		float delta[3];
		dtVsub(delta, steerPos, iterPos);
		float len = dtSqrt(dtVdot(delta,delta));
		// If the steer target is end of path or off-mesh link, do not move past the location.
		if ((endOfPath || offMeshConnection) && len < SMOOTH_PATH_STEP_SIZE)
			len = 1.0f;
		else
			len = SMOOTH_PATH_STEP_SIZE / len;

		float moveTgt[3];
		dtVmad(moveTgt, iterPos, delta, len);

		// Move
		float result[3];
		const static uint32 MAX_VISIT_POLY = 16;
		dtPolyRef visited[MAX_VISIT_POLY];

		uint32 nvisited = 0;
		mapChannel->navMeshQuery->moveAlongSurface(polys[0], iterPos, moveTgt, m_filter, result, visited, (int*)&nvisited, MAX_VISIT_POLY);
		npolys = fixupCorridor(polys, npolys, MAX_PATH_LENGTH, visited, nvisited);

		mapChannel->navMeshQuery->getPolyHeight(polys[0], result, &result[1]);
		result[1] += 0.5f;
		dtVcopy(iterPos, result);

		// Handle end of path and off-mesh links when close enough.
		if (endOfPath && inRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 1.0f))
		{
			// Reached end of path.
			dtVcopy(iterPos, targetPos);
			if (nsmoothPath < maxSmoothPathSize)
			{
				dtVcopy(&smoothPath[nsmoothPath*3], iterPos);
				nsmoothPath++;
			}
			break;
		}
		else if (offMeshConnection && inRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 1.0f))
		{
			// Advance the path up to and over the off-mesh connection.
			dtPolyRef prevRef = INVALID_POLYREF;
			dtPolyRef polyRef = polys[0];
			uint32 npos = 0;
			while (npos < npolys && polyRef != steerPosRef)
			{
				prevRef = polyRef;
				polyRef = polys[npos];
				npos++;
			}

			for (uint32 i = npos; i < npolys; ++i)
				polys[i-npos] = polys[i];

			npolys -= npos;

			// Handle the connection.
			float startPos[3], endPos[3];
			if (DT_SUCCESS == mapChannel->navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos))
			{
				if (nsmoothPath < maxSmoothPathSize)
				{
					dtVcopy(&smoothPath[nsmoothPath*3], startPos);
					nsmoothPath++;
				}
				// Move position at the other side of the off-mesh link.
				dtVcopy(iterPos, endPos);

				mapChannel->navMeshQuery->getPolyHeight(polys[0], iterPos, &iterPos[1]);
				iterPos[1] += 0.5f;
			}
		}

		// Store results.
		if (nsmoothPath < maxSmoothPathSize)
		{
			dtVcopy(&smoothPath[nsmoothPath*3], iterPos);
			nsmoothPath++;
		}
	}

	*smoothPathSize = nsmoothPath;

	// this is most likely a loop
	return nsmoothPath < MAX_PATH_LENGTH ? DT_SUCCESS : DT_FAILURE;
}

sint32 navmesh_getPath(mapChannel_t *mapChannel, float start[3], float end[3], float* path, bool appendEndPoint)
{
	// if no navmesh available just path to the destination directly
	if( !mapChannel->navMeshQuery )
	{
		path[0] = end[0];
		path[1] = end[1];
		path[2] = end[2];
		return 1;
	}
	//mapChannel->navMeshQuery->getPolyHeight()
	//float spos[3] = { start.x, start.y, start.z };
	//float epos[3] = { end.x, end.y, end.z };
	dtQueryFilter filter;
	filter.setIncludeFlags(0xffffff);
	filter.setExcludeFlags(0);
	float extents[3] = { 16.0f, 32.0f, 16.0f };
	dtPolyRef startRef;
	dtPolyRef endRef;
	float nfs[3] = { 0, 0, 0 };
	float nfe[3] = { 0, 0, 0 };
	mapChannel->navMeshQuery->findNearestPoly(start, extents, &filter, &startRef, nfs);
	mapChannel->navMeshQuery->findNearestPoly(end, extents, &filter, &endRef, nfe);
	if (!startRef || !endRef)
		return 0;
	dtPolyRef  polys[MAX_POLY];
	int   nbPolys = 0;
	float   strPath[MAX_POLY*3];
	int   nbStrPath = 0;
	unsigned char strPathFlags[MAX_POLY];
	dtPolyRef  strPathPolys[MAX_POLY];

	sint32 pathNodes = 0;

	mapChannel->navMeshQuery->findPath(startRef, endRef, start, end, &filter, polys, &nbPolys, PATH_LENGTH_LIMIT);
	if(nbPolys)
	{
		//mapChannel->navMeshQuery->findStraightPath(start, end, polys, nbPolys, strPath, strPathFlags, strPathPolys, &nbStrPath, PATH_LENGTH_LIMIT);
		findSmoothPath(mapChannel, start, end, &filter, polys, nbPolys, strPath, &nbStrPath, PATH_LENGTH_LIMIT);
		for( int i = 0; i < nbStrPath; i++)
		{
			//Vector3 point(strPath[i*3+0], strPath[i*3+1], strPath[i*3+2]);
			//path.push_back(point);
			path[pathNodes*3+0] = strPath[i*3+0];
			path[pathNodes*3+1] = strPath[i*3+1];
			path[pathNodes*3+2] = strPath[i*3+2];
			pathNodes++;
		}
		//path.back().x = end.x;
		//path.back().y = end.y;
		//path.back().z = end.z;
		if( appendEndPoint && (pathNodes+1)<PATH_LENGTH_LIMIT )
		{
			// makes sure we reach the destination, but can result in NPCs going through walls or sinking into ground
			path[pathNodes*3+0] = end[0];
			path[pathNodes*3+1] = end[1];
			path[pathNodes*3+2] = end[2];
			pathNodes++;
 		}
	}
	return pathNodes;
}
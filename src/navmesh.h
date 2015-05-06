void navmesh_initForMapChannel(mapChannel_t *mapChannel);
sint32 navmesh_getPath(mapChannel_t *mapChannel, float start[3], float end[3], float* path, bool appendEndPoint);
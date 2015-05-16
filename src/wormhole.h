dynObject_t* wormhole_create(mapChannel_t *mapChannel, float x, float y, float z, float orientation, uint32 waypointID, uint32 nameId, uint32 contextId);
void wormhole_recv_SelectWormhole(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);

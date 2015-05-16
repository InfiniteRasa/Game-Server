void netMgr_pythonAddMethodCallRaw(clientGamemain_t *cgm, uint32 EntityID, uint32 MethodID, uint8 *pyObjString, sint32 pyObjLen);

// mapChannel broadcast
void netMgr_pythonAddMethodCallRaw(mapChannel_t *broadCastChannel, uint32 EntityID, uint32 MethodID, uint8 *pyObjString, sint32 pyObjLen);

void netMgr_testOpc(clientGamemain_t *cgm);
void netMgr_entityMovementTest(clientGamemain_t *cgm, uint8 *pyObjString, sint32 pyObjLen);

typedef struct  
{
	unsigned long long entityId;
	uint32 posX24b;
	uint32 posY24b;
	uint32 posZ24b;
	uint16 velocity;
	uint8 flag;
	uint16 viewX;
	uint16 viewY;
}netCompressedMovement_t;



void netMgr_pythonAddMethodCallRaw(mapChannelClient_t **clientList, sint32 clientCount, uint32 entityId, uint32 MethodID, uint8 *pyObjString, sint32 pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannelClient_t *aggregator, uint32 EntityID, uint32 MethodID, uint8 *pyObjString, sint32 pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel_t *mapChannel, actor_t *aggregator, uint32 EntityID, uint32 MethodID, uint8 *pyObjString, sint32 pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRawIgnoreSelf(mapChannelClient_t *aggregator, uint32 EntityID, uint32 MethodID, uint8 *pyObjString, sint32 pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel_t *mapChannel, dynObject_t *aggregator, uint32 EntityID, uint32 MethodID, uint8 *pyObjString, sint32 pyObjLen);
void netMgr_cellDomain_sendEntityMovement(mapChannelClient_t *aggregator, netCompressedMovement_t *movement, bool skipOwner);
void netMgr_cellDomain_sendEntityMovement(mapChannel_t *mapChannel, actor_t *aggregator, netCompressedMovement_t *movement);
//void netMgr_broadcastEntityMovement(mapChannel_t *broadCastChannel, netCompressedMovement_t *movement, bool skipOwner);
void netMgr_sendEntityMovement(clientGamemain_t *cgm, netCompressedMovement_t *movement);
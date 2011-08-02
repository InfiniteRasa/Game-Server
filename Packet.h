void netMgr_pythonAddMethodCallRaw(clientGamemain_t *cgm, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen);

// mapChannel broadcast
void netMgr_pythonAddMethodCallRaw(mapChannel_t *broadCastChannel, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen);

void netMgr_testOpc(clientGamemain_t *cgm);
void netMgr_entityMovementTest(clientGamemain_t *cgm, unsigned char *pyObjString, int pyObjLen);

typedef struct  
{
	unsigned long long entityId;
	unsigned int posX24b;
	unsigned int posY24b;
	unsigned int posZ24b;
	unsigned short velocity;
	unsigned char flag;
	unsigned short viewX;
	unsigned short viewY;
}netCompressedMovement_t;



void netMgr_pythonAddMethodCallRaw(mapChannelClient_t **clientList, int clientCount, unsigned int entityId, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannelClient_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel_t *mapChannel, actor_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRawIgnoreSelf(mapChannelClient_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen);
void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel_t *mapChannel, dynObject_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen);
void netMgr_cellDomain_sendEntityMovement(mapChannelClient_t *aggregator, netCompressedMovement_t *movement, bool skipOwner);
void netMgr_cellDomain_sendEntityMovement(mapChannel_t *mapChannel, actor_t *aggregator, netCompressedMovement_t *movement);
//void netMgr_broadcastEntityMovement(mapChannel_t *broadCastChannel, netCompressedMovement_t *movement, bool skipOwner);
void netMgr_sendEntityMovement(clientGamemain_t *cgm, netCompressedMovement_t *movement);
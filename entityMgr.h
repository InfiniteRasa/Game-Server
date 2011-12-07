void entityMgr_init();

#define ENTITYTYPE_PLAYER	0
#define ENTITYTYPE_CLIENT	1
#define ENTITYTYPE_ITEM		2
#define ENTITYTYPE_OBJECT	3
#define ENTITYTYPE_NPC		4
#define ENTITYTYPE_CREATURE	5

unsigned long long entityMgr_getFreeEntityIdForClient();
unsigned long long entityMgr_getFreeEntityIdForPlayer();
unsigned long long entityMgr_getFreeEntityIdForItem();
unsigned long long entityMgr_getFreeEntityIdForObject();
unsigned long long entityMgr_getFreeEntityIdForNPC();
unsigned long long entityMgr_getFreeEntityIdForCreature();

unsigned char entityMgr_getEntityType(unsigned long long entityId);

void entityMgr_registerEntity(unsigned long long entityId, void *entity);
void entityMgr_unregisterEntity(unsigned long long entityId);
void *entityMgr_get(unsigned long long entityId);
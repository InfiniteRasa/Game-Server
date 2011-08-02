
#define CELL_SIZE	25.0f //32.0f
#define CELL_VIEWRANGE 2 // 2 cells in evers direction

#define CELL_BIAS	32768.0f


typedef struct  
{
	int cellX;
	int cellZ;
	HashTable_uint32Iterable_t ht_playerList; // players currently in this cell
	HashTable_uint32Iterable_t ht_playerNotifyList; // players that currently see this cell and should be informed about updates
	HashTable_uint32Iterable_t ht_objectList; // dynamic gameobjects that are in the cell
	HashTable_uint32Iterable_t ht_npcList; // dynamic npcs that are in the cell
	HashTable_uint32Iterable_t ht_creatureList; // dynamic creatures that are in the cell
}mapCell_t;

typedef struct  
{
	int x;
	int z;
}mapCellLocation_t;

typedef struct  
{
	HashTable_uint32_t ht_cells;
	int loadedCellCount;
	int loadedCellLimit;
	mapCell_t **loadedCellList;
	unsigned int time_updateVisibility;
}mapCellInfo_t;


bool cellMgr_initForMapChannel(mapChannel_t *mapChannel);
// add/remove client from the world
void cellMgr_addToWorld( mapChannelClient_t *client );
void cellMgr_removeFromWorld( mapChannelClient_t *client );
// add/remove object from the world
void cellMgr_addToWorld(mapChannel_t *mapChannel, dynObject_t *dynObject);
void cellMgr_removeFromWorld(mapChannel_t *mapChannel, dynObject_t *dynObject);
// add/remove npc from the world
void cellMgr_addToWorld(mapChannel_t *mapChannel, npc_t *npc);

// add/remove creature from the world
void cellMgr_addToWorld(mapChannel_t *mapChannel, creature_t *creature);

// misc
npc_t *cellMgr_findNPCinViewOf(mapChannelClient_t *aggregator, unsigned long long npcEntityId);

mapCell_t* cellMgr_tryGetCell(mapChannel_t *mapChannel, int x, int z);

mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannelClient_t *aggregator, int *oCount);
mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, actor_t *aggregator, int *oCount);
mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, dynObject_t *aggregator, int *oCount);

void cellMgr_doWork( mapChannel_t *mapChannel );
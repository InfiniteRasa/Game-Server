
#define CELL_SIZE	25.0f //32.0f
#define CELL_VIEWRANGE 2 // 2 cells in evers direction

#define CELL_BIAS	32768.0f

/*
 * A map cell trigger area is an box shaped area on the map with callbacks for various player events
 * Currently supported callbacks are:
 * Player enters area
 * Player leaves area
 * Periodic callback for all players within the area
 * Main uses:
 * enter waypoints, keep track of players in a certain region
 */
typedef struct  
{
	float minBoundX;
	float minBoundY;
	float minBoundZ;
	float maxBoundX;
	float maxBoundY;
	float maxBoundZ;
}mapCellTriggerArea_t;

typedef struct  
{
	sint32 cellX;
	sint32 cellZ;
	std::vector<mapChannelClient_t*> ht_playerList; // players currently in this cell
	std::vector<mapChannelClient_t*> ht_playerNotifyList; // players that currently see this cell and should be informed about updates
	std::vector<dynObject_t*> ht_objectList; // dynamic gameobjects that are in the cell
	std::vector<npc_t*> ht_npcList; // dynamic npcs that are in the cell
	std::vector<creature_t*> ht_creatureList; // dynamic creatures that are in the cell
}mapCell_t;

typedef struct  
{
	sint32 x;
	sint32 z;
}mapCellLocation_t;

typedef struct  
{
	hashTable_t ht_cells;
	sint32 loadedCellCount;
	sint32 loadedCellLimit;
	mapCell_t **loadedCellList;
	uint32 time_updateVisibility;
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
void cellMgr_removeCreatureFromWorld( mapChannel_t *mapChannel, creature_t *creat );

// misc
npc_t *cellMgr_findNPCinViewOf(mapChannelClient_t *aggregator, unsigned long long npcEntityId);

mapCell_t* cellMgr_tryGetCell(mapChannel_t *mapChannel, sint32 x, sint32 z);
mapCell_t* cellMgr_getCell(mapChannel_t *mapChannel, sint32 x, sint32 z);

mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannelClient_t *aggregator, sint32 *oCount);
mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, actor_t *aggregator, sint32 *oCount);
mapChannelClient_t **cellMgr_getNotifiedPlayers( mapChannel_t *mapChannel, dynObject_t *aggregator, sint32 *oCount);

void cellMgr_doWork( mapChannel_t *mapChannel );
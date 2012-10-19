#define USE_STATE_NULL						97  // doesnt work
#define USE_CPOINT_STATE_FACTION_A_OWNED	176
#define USE_CPOINT_STATE_FACTION_B_OWNED	179
#define USE_CPOINT_STATE_UNCLAIMED			181

#define USE_CPOINT_STATE_FACTION_B_CLAIMING 177 // unused
#define USE_CPOINT_STATE_FACTION_A_CLAIMING 180 // unused


dynObject_t* controlpoint_create(mapChannel_t *mapChannel, float x, float y, float z, float orientation);
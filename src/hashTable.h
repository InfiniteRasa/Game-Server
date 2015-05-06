
typedef struct  
{
	sint32 itemIndex;
}_HashTable_uint32Iterable_entry_t;

typedef struct
{
	_HashTable_uint32Iterable_entry_t *entrys;
	uint32 *itemKeyArray;
	void **itemValueArray;
	uint32 size;
	uint32 count;
}hashTable_t;

void hashTable_init(hashTable_t *hashTable, sint32 itemLimit);
void hashTable_destroy(hashTable_t *hashTable);
void hashTable_clear(hashTable_t *hashTable);
bool hashTable_set(hashTable_t *hashTable, uint32 key, void *item);
void *hashTable_get(hashTable_t *hashTable, uint32 key);

void** hashTable_getValueArray(hashTable_t *hashTable);
uint32* hashTable_getKeyArray(hashTable_t *hashTable);
uint32 hashTable_getCount(hashTable_t *hashTable);

bool hashTable_set(hashTable_t *hashTable, sint8 *key, void *item);
void *hashTable_get(hashTable_t *hashTable, sint8 *key);

typedef struct
{
	_HashTable_uint32Iterable_entry_t *entrys;
	uint32 *itemKeyArray;
	void **itemValueArray;
	uint32 size;
	uint32 count;
	TMutex criticalSection;
}hashTableSynced_t;

void hashTable_init(hashTableSynced_t *hashTable, sint32 itemLimit);
void hashTable_destroy(hashTableSynced_t *hashTable);
void hashTable_clear(hashTableSynced_t *hashTable);
bool hashTable_set(hashTableSynced_t *hashTable, uint32 key, void *item);
void *hashTable_get(hashTableSynced_t *hashTable, uint32 key);

void** hashTable_getValueArray(hashTableSynced_t *hashTable);
uint32* hashTable_getKeyArray(hashTableSynced_t *hashTable);
uint32 hashTable_getCount(hashTableSynced_t *hashTable);

bool hashTable_set(hashTableSynced_t *hashTable, sint8 *key, void *item);
void *hashTable_get(hashTableSynced_t *hashTable, sint8 *key);

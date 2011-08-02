
typedef struct  
{
	DWORD hashB;
	DWORD hashC;
	void *item;
}_HashTable_string_entry_t;

typedef struct
{
	_HashTable_string_entry_t *entrys;
	DWORD size;
}HashTable_string_t;


void hashTable_init(HashTable_string_t *hashTable, int itemLimit);
bool hashTable_set(HashTable_string_t *hashTable, char *key, void *item);
void *hashTable_get(HashTable_string_t *hashTable, char *key);

typedef struct
{
	_HashTable_string_entry_t *entrys;
	DWORD size;
	CRITICAL_SECTION cs;
}HashTable_stringSynced_t;

void hashTable_init(HashTable_stringSynced_t *hashTable, int itemLimit);
bool hashTable_set(HashTable_stringSynced_t *hashTable, char *key, void *item);
void *hashTable_get(HashTable_stringSynced_t *hashTable, char *key);


typedef struct  
{
	DWORD originalValue;
	void *item;
}_HashTable_uint32_entry_t;

typedef struct
{
	_HashTable_uint32_entry_t *entrys;
	DWORD size;
	DWORD count;
}HashTable_uint32_t;


void hashTable_init(HashTable_uint32_t *hashTable, int itemLimit);
void hashTable_destroy(HashTable_uint32_t *hashTable);
bool hashTable_set(HashTable_uint32_t *hashTable, unsigned int key, void *item);
void *hashTable_get(HashTable_uint32_t *hashTable, unsigned int key);


typedef struct  
{
	int itemIndex;
}_HashTable_uint32Iterable_entry_t;

typedef struct
{
	_HashTable_uint32Iterable_entry_t *entrys;
	unsigned int *itemKeyArray;
	void **itemValueArray;
	DWORD size;
	DWORD count;
}HashTable_uint32Iterable_t;

void hashTable_init(HashTable_uint32Iterable_t *hashTable, int itemLimit);
void hashTable_destroy(HashTable_uint32Iterable_t *hashTable);
bool hashTable_set(HashTable_uint32Iterable_t *hashTable, unsigned int key, void *item);
void *hashTable_get(HashTable_uint32Iterable_t *hashTable, unsigned int key);

void** hashTable_getValueArray(HashTable_uint32Iterable_t *hashTable);
unsigned int* hashTable_getKeyArray(HashTable_uint32Iterable_t *hashTable);
unsigned int hashTable_getCount(HashTable_uint32Iterable_t *hashTable);
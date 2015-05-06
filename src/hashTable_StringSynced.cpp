#include<Windows.h>
#include"hashTable.h"
//#include"memMgr.h"

#ifndef memMgr_alloc
#define memMgr_alloc(x,y) malloc(y)
#define memMgr_free(x,y) free(y)
#endif

void hashTable_init(HashTable_stringSynced_t *hashTable, int itemLimit)
{
	InitializeCriticalSection(&hashTable->cs);
	hashTable->size = itemLimit;
	hashTable->entrys = (_HashTable_string_entry_t*)memMgr_alloc(NULL, sizeof(_HashTable_string_entry_t)*itemLimit);
	// reset all items
	for(int i=0; i<itemLimit; i++)
	{
		hashTable->entrys[i].hashB = 0xFFFFFFFF;
		hashTable->entrys[i].hashC = 0xFFFFFFFF;
		hashTable->entrys[i].item = NULL;
	}
}

void _HashTable_genHashByString(char *key, DWORD *hash); // already defined by the original string HashTable

bool hashTable_set(HashTable_stringSynced_t *hashTable, char *key, void *item)
{
	DWORD hash[3];
	_HashTable_genHashByString(key, hash);
	EnterCriticalSection(&hashTable->cs);
	// get entry
	int index = hash[0]%hashTable->size;
	for(int i=0; i<hashTable->size; i++) 
	{
		if( hashTable->entrys[index].hashB == 0xFFFFFFFF && hashTable->entrys[index].hashC == 0xFFFFFFFF )
		{
			// set entry
			hashTable->entrys[index].hashB = hash[1];
			hashTable->entrys[index].hashC = hash[2];
			hashTable->entrys[index].item = item;
			LeaveCriticalSection(&hashTable->cs);
			return true;
		}
		else if( hashTable->entrys[index].hashB == hash[1] && hashTable->entrys[index].hashC == hash[2] )
		{
			// update entry
			hashTable->entrys[index].item = item;
			LeaveCriticalSection(&hashTable->cs);
			return true;
		}	
		index++;
		index%=hashTable->size;
	}
	LeaveCriticalSection(&hashTable->cs);
	return false;
}

void *hashTable_get(HashTable_stringSynced_t *hashTable, char *key)
{
	DWORD hash[3];
	_HashTable_genHashByString(key, hash);
	EnterCriticalSection(&hashTable->cs);
	// get entry
	int index = hash[0]%hashTable->size;
	for(int i=0; i<hashTable->size; i++) 
	{
		if( hashTable->entrys[index].hashB == hash[1] && hashTable->entrys[index].hashC == hash[2] )
		{
			return hashTable->entrys[index].item;
		}
		else if( hashTable->entrys[index].hashB == 0xFFFFFFFF )
		{
			return NULL; // empty entry marks end of search
		}
		index++;
		index %= hashTable->size;
	}
	LeaveCriticalSection(&hashTable->cs);
	return NULL;
}
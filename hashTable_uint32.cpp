#include<Windows.h>
#include"hashTable.h"

#ifndef memMgr_alloc
#define memMgr_alloc(x,y) malloc(y)
#define memMgr_free(x,y) free(y)
#endif
// uint32 tables are growable
void hashTable_init(HashTable_uint32_t *hashTable, int itemLimit)
{
	hashTable->size = itemLimit;
	hashTable->count = 0;
	hashTable->entrys = (_HashTable_uint32_entry_t*)memMgr_alloc(NULL, sizeof(_HashTable_uint32_entry_t)*itemLimit);
	// reset all items
	for(int i=0; i<itemLimit; i++)
	{
		hashTable->entrys[i].originalValue = 0xFFFFFFFF;
		hashTable->entrys[i].item = NULL;
	}
}

void hashTable_destroy(HashTable_uint32_t *hashTable)
{
	memMgr_free(NULL, (void*)hashTable->entrys);
}

void hashTable_enlarge(HashTable_uint32_t *hashTable)
{
	// create new hashTable with double size
	HashTable_uint32_t hashTable2;
	hashTable_init(&hashTable2, hashTable->size*2);
	// move entrys from old table to new table
	for(unsigned int i=0; i<hashTable->size; i++)
	{
		if( hashTable->entrys[i].item )
		{
			hashTable_set(&hashTable2, hashTable->entrys[i].originalValue, hashTable->entrys[i].item); 
		}
	}
	// destroy old table
	hashTable_destroy(hashTable);
	// set new table
	hashTable->count = hashTable2.count;
	hashTable->size = hashTable2.size;
	hashTable->entrys = hashTable2.entrys;
}

bool hashTable_set(HashTable_uint32_t *hashTable, unsigned int key, void *item)
{
	//DWORD hash[3];
	//_HashTable_genHashByString(key, hash);
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	int index = hashA%hashTable->size;
	for(int i=0; i<hashTable->size; i++) 
	{
		if( hashTable->entrys[index].item == NULL )
		{
			// set entry
			hashTable->entrys[index].originalValue = key;
			hashTable->entrys[index].item = item;
			// update table size
			if( item )
			{
				hashTable->count++;
				if( hashTable->count >= (hashTable->size*3/4) )
				{
					// enlarge table
					hashTable_enlarge(hashTable);
				}
			}
			return true;
		}
		else if( hashTable->entrys[index].originalValue == key && hashTable->entrys[index].item )
		{
			// update entry
			if( hashTable->entrys[index].item && item == NULL )
				hashTable->count--;
			hashTable->entrys[index].item = item;
			return true;
		}	
		index++;
		index%=hashTable->size;
	}
	// no free entry
	hashTable_enlarge(hashTable);
	return hashTable_set(hashTable, key, item);
}


void *hashTable_get(HashTable_uint32_t *hashTable, unsigned int key)
{
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	int index = hashA%hashTable->size;
	for(int i=0; i<hashTable->size; i++) 
	{
		if( hashTable->entrys[index].originalValue == key && hashTable->entrys[index].item )
		{
			return hashTable->entrys[index].item;
		}
		else if( !hashTable->entrys[index].item )
		{
			return NULL; // empty entry marks end of search
		}
		index++;
		index %= hashTable->size;
	}
	return NULL;
}
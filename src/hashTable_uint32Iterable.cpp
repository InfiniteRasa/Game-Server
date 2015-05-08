#include<Windows.h>
#include"hashTable.h"

#ifndef memMgr_alloc
#define memMgr_alloc(x,y) malloc(y)
#define memMgr_free(x,y) free(y)
#endif

#define MAXSCAN_LENGTH	32 // maximal number of filled hashentrys in a row

// uint32 tables are growable
void hashTable_init(HashTable_uint32Iterable_t *hashTable, int itemLimit)
{
	hashTable->size = itemLimit;
	hashTable->count = 0;
	hashTable->entrys = (_HashTable_uint32Iterable_entry_t*)memMgr_alloc(NULL, sizeof(_HashTable_uint32Iterable_entry_t)*itemLimit);
	hashTable->itemKeyArray = (unsigned int*)memMgr_alloc(NULL, sizeof(unsigned int)*itemLimit);
	hashTable->itemValueArray = (void**)memMgr_alloc(NULL, sizeof(void*)*itemLimit);
	// reset all items
	for(int i=0; i<itemLimit; i++)
	{
		hashTable->entrys[i].itemIndex = 0; // 1-indexed
		hashTable->itemKeyArray[i] = 0;
		hashTable->itemValueArray[i] = 0;
		// hashTable->entrys[i].originalValue = 0xFFFFFFFF;
		// hashTable->entrys[i].item = NULL;
	}
}

void hashTable_destroy(HashTable_uint32Iterable_t *hashTable)
{
	memMgr_free(NULL, (void*)hashTable->itemKeyArray);
	memMgr_free(NULL, (void*)hashTable->itemValueArray);
	memMgr_free(NULL, (void*)hashTable->entrys);
}

void hashTable_enlarge(HashTable_uint32Iterable_t *hashTable)
{
	// create new hashTable with double size
	HashTable_uint32Iterable_t hashTable2;
	hashTable_init(&hashTable2, hashTable->size*2);
	// move entrys from old table to new table
	for(unsigned int i=0; i<hashTable->size; i++)
	{
		if( hashTable->entrys[i].itemIndex )
		{
			int index = hashTable->entrys[i].itemIndex - 1;
			if( hashTable->entrys[i].itemIndex == -1 ) // deleted entry
				continue;
			hashTable_set(&hashTable2, hashTable->itemKeyArray[index], (void*)hashTable->itemValueArray[index]);//hashTable->entrys[i].item); 
		}
	}
	// destroy old table
	hashTable_destroy(hashTable);
	// set new table
	hashTable->count = hashTable2.count;
	hashTable->size = hashTable2.size;
	hashTable->entrys = hashTable2.entrys;
	hashTable->itemKeyArray = hashTable2.itemKeyArray;
	hashTable->itemValueArray = hashTable2.itemValueArray;
}

void _hashTable_updateReference(HashTable_uint32Iterable_t *hashTable, unsigned int key, int oldReference, int newReference)
{
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	int index = hashA%hashTable->size;
	for(int i=0; i<hashTable->size; i++) 
	{
		int ridx = hashTable->entrys[index].itemIndex;
		if( ridx == 0 )
		{
			return;
		}
		else if( ridx == oldReference )
		{
			// update entry
			hashTable->entrys[index].itemIndex = newReference;
			return;
		}	
		index++;
		index%=hashTable->size;
	}
}

bool hashTable_set(HashTable_uint32Iterable_t *hashTable, unsigned int key, void *item)
{
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	int index = hashA%hashTable->size;
	for(int i=0; i<MAXSCAN_LENGTH; i++) 
	{
		int ridx = hashTable->entrys[index].itemIndex;
		if( ridx == 0 )
		{
			// set entry if valid
			if( item )
			{
				hashTable->entrys[index].itemIndex = hashTable->count+1;
				hashTable->itemKeyArray[hashTable->count] = key;
				hashTable->itemValueArray[hashTable->count] = item;
				hashTable->count++;
				if( hashTable->count >= (hashTable->size*3/4) )
				{
					// enlarge table
					hashTable_enlarge(hashTable);
				}
			}
			else return false;
			return true;
		}
		else if( hashTable->itemKeyArray[ridx-1] == key )
		{
			// update entry
			if( hashTable->itemValueArray[ridx-1] && item == NULL )
			{
				// remove entry
				if( hashTable->count != ridx ) // is not the last entry
				{
					// move last entry to current
					hashTable->itemKeyArray[ridx-1] = hashTable->itemKeyArray[hashTable->count-1];
					hashTable->itemValueArray[ridx-1] = hashTable->itemValueArray[hashTable->count-1];
					// update id
					_hashTable_updateReference(hashTable, hashTable->itemKeyArray[hashTable->count-1], hashTable->count, ridx);
				}
				hashTable->entrys[index].itemIndex = -1; // mark as deleted
				hashTable->count--;
			}
			return true;
		}	
		index++;
		index%=hashTable->size;
	}
	// no free entry
	hashTable_enlarge(hashTable);
	return hashTable_set(hashTable, key, item);
}



void *hashTable_get(HashTable_uint32Iterable_t *hashTable, unsigned int key)
{
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	if( hashTable->size == 0 )
		return NULL; // hashTable not initialized or empty
	int index = hashA%hashTable->size;
	for(int i=0; i<MAXSCAN_LENGTH; i++) 
	{
		int ridx = hashTable->entrys[index].itemIndex;
		if( !ridx )
			return NULL;
		if( ridx != -1 )
		{
			if( hashTable->itemKeyArray[ridx-1] == key )
			{
				return hashTable->itemValueArray[ridx-1];
			}
		}
		index++;
		index %= hashTable->size;
	}
	return NULL;
}

void** hashTable_getValueArray(HashTable_uint32Iterable_t *hashTable)
{
	return hashTable->itemValueArray;
}

unsigned int* hashTable_getKeyArray(HashTable_uint32Iterable_t *hashTable)
{
	return hashTable->itemKeyArray;
}

unsigned int hashTable_getCount(HashTable_uint32Iterable_t *hashTable)
{
	return hashTable->count;
}
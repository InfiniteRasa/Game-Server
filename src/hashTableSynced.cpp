#include"framework.h"

#define MAXSCAN_LENGTH	32 // maximal number of filled hashentrys in a row

// uint32 tables are growable
void hashTable_init(hashTableSynced_t *hashTable, sint32 itemLimit)
{
	hashTable->size = itemLimit;
	hashTable->count = 0;
	hashTable->entrys = (_HashTable_uint32Iterable_entry_t*)malloc(sizeof(_HashTable_uint32Iterable_entry_t)*itemLimit);
	hashTable->itemKeyArray = (uint32*)malloc(sizeof(uint32)*itemLimit);
	hashTable->itemValueArray = (void**)malloc(sizeof(void*)*itemLimit);
	// reset all items
	for(sint32 i=0; i<itemLimit; i++)
	{
		hashTable->entrys[i].itemIndex = 0; // 1-indexed
		hashTable->itemKeyArray[i] = 0;
		hashTable->itemValueArray[i] = 0;
		// hashTable->entrys[i].originalValue = 0xFFFFFFFF;
		// hashTable->entrys[i].item = NULL;
	}
	Thread::InitMutex(&hashTable->criticalSection);
}

void hashTable_destroy(hashTableSynced_t *hashTable)
{
	free((void*)hashTable->itemKeyArray);
	free((void*)hashTable->itemValueArray);
	free((void*)hashTable->entrys);
}

void hashTable_clear(hashTableSynced_t *hashTable)
{
	Thread::LockMutex(&hashTable->criticalSection);
	hashTable->count = 0;
	for(uint32 i=0; i<hashTable->size; i++)
	{
		hashTable->entrys[i].itemIndex = 0;
		hashTable->itemKeyArray[i] = 0;
		hashTable->itemValueArray[i] = 0;
	}
	Thread::UnlockMutex(&hashTable->criticalSection);
}

void hashTable_enlarge(hashTableSynced_t *hashTable)
{
	// create new hashTable with double size
	hashTableSynced_t hashTable2;
	hashTable_init(&hashTable2, hashTable->size*2);
	// move entrys from old table to new table
	for(uint32 i=0; i<hashTable->size; i++)
	{
		if( hashTable->entrys[i].itemIndex )
		{
			sint32 index = hashTable->entrys[i].itemIndex - 1;
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

void _hashTable_updateReference(hashTableSynced_t *hashTable, uint32 key, sint32 oldReference, sint32 newReference)
{
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	sint32 index = hashA%hashTable->size;
	for(DWORD i=0; i<hashTable->size; i++) 
	{
		sint32 ridx = hashTable->entrys[index].itemIndex;
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

bool hashTable_set(hashTableSynced_t *hashTable, uint32 key, void *item)
{
	Thread::LockMutex(&hashTable->criticalSection);
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	sint32 index = hashA%hashTable->size;
	for(sint32 i=0; i<MAXSCAN_LENGTH; i++) 
	{
		sint32 ridx = hashTable->entrys[index].itemIndex;
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
			else
			{
				Thread::UnlockMutex(&hashTable->criticalSection);
				return false;
			}
			Thread::UnlockMutex(&hashTable->criticalSection);
			return true;
		}
		else if( hashTable->itemKeyArray[ridx-1] == key )
		{
			// update entry
			//if( item )
			//	__debugbreak();
			sint32 itemIndex = ridx-1;
			hashTable->itemValueArray[itemIndex] = item;

			//if( hashTable->itemValueArray[ridx-1] && item == NULL )
			//{
			//	// remove entry
			//	if( hashTable->count != ridx ) // is not the last entry
			//	{
			//		// move last entry to current
			//		hashTable->itemKeyArray[ridx-1] = hashTable->itemKeyArray[hashTable->count-1];
			//		hashTable->itemValueArray[ridx-1] = hashTable->itemValueArray[hashTable->count-1];
			//		// update id
			//		_hashTable_updateReference(hashTable, hashTable->itemKeyArray[hashTable->count-1], hashTable->count, ridx);
			//	}
			//	hashTable->entrys[index].itemIndex = -1; // mark as deleted
			//	hashTable->count--;
			//}
			Thread::UnlockMutex(&hashTable->criticalSection);
			return true;
		}	
		index++;
		index%=hashTable->size;
	}
	// no free entry
	hashTable_enlarge(hashTable);
#ifdef USE_PTHREADS
	Thread::UnlockMutex(&hashTable->criticalSection); // pthread specific
#endif
	bool result = hashTable_set(hashTable, key, item);
	Thread::UnlockMutex(&hashTable->criticalSection);
	return result;
}

bool hashTable_set(hashTableSynced_t *hashTable, sint8 *key, void *item)
{
	uint32 keyValue = 0xF94A9E53;
	uint32 keyB = 0x2D48C92E;
	while( *key )
	{
		keyValue	+=	(uint32)*key;
		keyB		^=	((uint32)*key)*7;
		keyValue	=	(keyValue<<3)|(keyValue>>29);
		keyB		=	(keyB<<7)|(keyB>>25);
		keyValue	+=	keyB;
		key++;
	}
	return hashTable_set(hashTable, keyValue, item);
}

void *hashTable_get(hashTableSynced_t *hashTable, uint32 key)
{
	Thread::LockMutex(&hashTable->criticalSection);
	DWORD hashA = key + key*3 + key*7 + key*11;
	// get entry
	if( hashTable->size == 0 )
	{
		Thread::UnlockMutex(&hashTable->criticalSection);
		return NULL; // hashTable not initialized or empty
	}
	sint32 index = hashA%hashTable->size;
	for(sint32 i=0; i<MAXSCAN_LENGTH; i++) 
	{
		sint32 ridx = hashTable->entrys[index].itemIndex;
		if( !ridx )
		{
			Thread::UnlockMutex(&hashTable->criticalSection);
			return NULL;
		}
		if( ridx != -1 )
		{
			if( hashTable->itemKeyArray[ridx-1] == key )
			{
				Thread::UnlockMutex(&hashTable->criticalSection);
				return hashTable->itemValueArray[ridx-1];
			}
		}
		index++;
		index %= hashTable->size;
	}
	Thread::UnlockMutex(&hashTable->criticalSection);
	return NULL;
}

void *hashTable_get(hashTableSynced_t *hashTable, sint8 *key)
{
	uint32 keyValue = 0xF94A9E53;
	uint32 keyB = 0x2D48C92E;
	while( *key )
	{
		keyValue	+=	(uint32)*key;
		keyB		^=	((uint32)*key)*7;
		keyValue	=	(keyValue<<3)|(keyValue>>29);
		keyB		=	(keyB<<7)|(keyB>>25);
		keyValue	+=	keyB;
		key++;
	}
	return hashTable_get(hashTable, keyValue);
}

void** hashTable_getValueArray(hashTableSynced_t *hashTable)
{
	return hashTable->itemValueArray;
}

uint32* hashTable_getKeyArray(hashTableSynced_t *hashTable)
{
	return hashTable->itemKeyArray;
}

uint32 hashTable_getCount(hashTableSynced_t *hashTable)
{
	return hashTable->count;
}
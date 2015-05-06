#include<Windows.h>
#include"hashTable.h"
//#include"memMgr.h"

#ifndef memMgr_alloc
#define memMgr_alloc(x,y) malloc(y)
#define memMgr_free(x,y) free(y)
#endif

void hashTable_init(HashTable_string_t *hashTable, int itemLimit)
{
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

void _HashTable_genHashByString(char *key, DWORD *hash)
{
	DWORD hashA='HASH',hashB=0x8823FF,hashC=0xB284819F;
	while( *key )
	{
		hashA += *key;
		hashB ^= (*key)*1011;
		hashA += hashB;
		hashB = (hashB>>3)|(hashB<<29);
		hashC += hashA;
		hashC += hashB;
		key++;
	}
	// 4 extra rounds
	hashA += hashC;
	hashC += hashB;
	hashC ^= hashA;
	hashB += hashA;

	hash[0] = hashA;
	hash[1] = hashB;
	hash[2] = hashC;
}

bool hashTable_set(HashTable_string_t *hashTable, char *key, void *item)
{
	DWORD hash[3];
	_HashTable_genHashByString(key, hash);
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
			return true;
		}
		else if( hashTable->entrys[index].hashB == hash[1] && hashTable->entrys[index].hashC == hash[2] )
		{
			// update entry
			hashTable->entrys[index].item = item;
			return true;
		}	
		index++;
		index%=hashTable->size;
	}
	return false;
}

void *hashTable_get(HashTable_string_t *hashTable, char *key)
{
	DWORD hash[3];
	_HashTable_genHashByString(key, hash);
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
	return NULL;
}
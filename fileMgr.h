typedef struct  
{
	HANDLE hFile;
}file_t;

file_t *fileMgr_open(sint8 *name);
file_t *fileMgr_create(sint8 *name);
void fileMgr_close(file_t *file);

void fileMgr_writeS8(file_t *file, sint8 value);
void fileMgr_writeS16(file_t *file, sint16 value);
void fileMgr_writeS32(file_t *file, sint32 value);
void fileMgr_writeU8(file_t *file, uint8 value);
void fileMgr_writeU16(file_t *file, uint16 value);
void fileMgr_writeU32(file_t *file, uint32 value);
void fileMgr_writeFloat(file_t *file, float value);
void fileMgr_writeData(file_t *file, void *data, sint32 len);

sint8 fileMgr_readS8(file_t *file);
sint16 fileMgr_readS16(file_t *file);
sint32 fileMgr_readS32(file_t *file);
uint8 fileMgr_readU8(file_t *file);
uint16 fileMgr_readU16(file_t *file);
uint32 fileMgr_readU32(file_t *file);
float fileMgr_readFloat(file_t *file);
void fileMgr_readData(file_t *file, void *data, sint32 len);

sint8 *fileMgr_readLine(file_t *file);

void fileMgr_setSeek(file_t *file, uint32 seek);
uint32 fileMgr_getSeek(file_t *file);

// fileMgr extensions
#include "sData.h"
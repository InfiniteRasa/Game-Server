typedef struct  
{
	HANDLE hFile;

}file_t;

file_t *fileMgr_open(char *name);
file_t *fileMgr_create(char *name);
void fileMgr_close(file_t *file);

void fileMgr_writeS8(file_t *file, char value);
void fileMgr_writeS16(file_t *file, short value);
void fileMgr_writeS32(file_t *file, int value);
void fileMgr_writeU8(file_t *file, unsigned char value);
void fileMgr_writeU16(file_t *file, unsigned short value);
void fileMgr_writeU32(file_t *file, unsigned int value);
void fileMgr_writeFloat(file_t *file, float value);
void fileMgr_writeData(file_t *file, void *data, int len);

char fileMgr_readS8(file_t *file);
short fileMgr_readS16(file_t *file);
int fileMgr_readS32(file_t *file);
unsigned char fileMgr_readU8(file_t *file);
unsigned short fileMgr_readU16(file_t *file);
unsigned int fileMgr_readU32(file_t *file);
float fileMgr_readFloat(file_t *file);
void fileMgr_readData(file_t *file, void *data, int len);

char *fileMgr_readLine(file_t *file);

void fileMgr_setSeek(file_t *file, unsigned int seek);
unsigned int fileMgr_getSeek(file_t *file);

// fileMgr extensions
#include "sData.h"
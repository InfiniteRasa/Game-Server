#include<Windows.h>
//#include"memMgr.h"
#include"fileMgr.h"

#ifndef memMgr_alloc
#define memMgr_alloc(x,y) malloc(y)
#define memMgr_free(x,y) free(y)
#endif

file_t *fileMgr_open(char *name)
{
	HANDLE hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if( hFile == INVALID_HANDLE_VALUE )
		return NULL;
	file_t *file = (file_t*)memMgr_alloc(NULL, sizeof(file_t));
	file->hFile = hFile;
	return file;
}

file_t *fileMgr_create(char *name)
{
	HANDLE hFile = CreateFile(name, FILE_ALL_ACCESS, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if( hFile == INVALID_HANDLE_VALUE )
		return NULL;
	file_t *file = (file_t*)memMgr_alloc(NULL, sizeof(file_t));
	file->hFile = hFile;
	return file;
}

void fileMgr_close(file_t *file)
{
	if( file )
	{
		CloseHandle(file->hFile);
		memMgr_free(NULL, (void*)file);
	}
}


// todo: add cache for writing
void fileMgr_writeS8(file_t *file, char value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(char), &BT, NULL);
}

void fileMgr_writeS16(file_t *file, short value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(short), &BT, NULL);
}

void fileMgr_writeS32(file_t *file, int value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(int), &BT, NULL);
}

void fileMgr_writeU8(file_t *file, unsigned char value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(unsigned char), &BT, NULL);
}

void fileMgr_writeU16(file_t *file, unsigned short value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(unsigned short), &BT, NULL);
}

void fileMgr_writeU32(file_t *file, unsigned int value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(unsigned int), &BT, NULL);
}

void fileMgr_writeFloat(file_t *file, float value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(float), &BT, NULL);
}

void fileMgr_writeData(file_t *file, void *data, int len)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)data, len, &BT, NULL);
}


char fileMgr_readS8(file_t *file)
{
	DWORD BT;
	char value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(char), &BT, NULL);
	return value;
}

short fileMgr_readS16(file_t *file)
{
	DWORD BT;
	short value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(short), &BT, NULL);
	return value;
}

int fileMgr_readS32(file_t *file)
{
	DWORD BT;
	int value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(int), &BT, NULL);
	return value;
}

unsigned char fileMgr_readU8(file_t *file)
{
	DWORD BT;
	unsigned char value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(unsigned char), &BT, NULL);
	return value;
}

unsigned short fileMgr_readU16(file_t *file)
{
	DWORD BT;
	unsigned short value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(unsigned short), &BT, NULL);
	return value;
}

unsigned int fileMgr_readU32(file_t *file)
{
	DWORD BT;
	unsigned int value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(unsigned int), &BT, NULL);
	return value;
}

float fileMgr_readFloat(file_t *file)
{
	DWORD BT;
	float value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(float), &BT, NULL);
	return value;
}

void fileMgr_readData(file_t *file, void *data, int len)
{
	DWORD BT;
	ReadFile(file->hFile, (LPVOID)data, len, &BT, NULL);
}


char *fileMgr_readLine(file_t *file)
{
	// todo: optimize this..
	DWORD currentSeek = SetFilePointer(file->hFile, 0, NULL, FILE_CURRENT);
	DWORD fileSize = GetFileSize(file->hFile, NULL);
	DWORD maxLen = fileSize - currentSeek;
	if( maxLen == 0 )
		return NULL; // eof reached
	// begin parsing
	char *cstr = (char*)memMgr_alloc(NULL, 512);
	int size = 0;
	int limit = 512;
	while( maxLen )
	{
		maxLen--;
		char n = fileMgr_readS8(file);
		if( n == '\r' )
			continue; // skip
		if( n == '\n' )
			break; // line end
		cstr[size] = n;
		size++;
		if( size == limit )
			__debugbreak();
	}
	cstr[size] = '\0';
	return cstr;
}

void fileMgr_setSeek(file_t *file, unsigned int seek)
{
	SetFilePointer(file->hFile, seek, NULL, FILE_BEGIN);
}

unsigned int fileMgr_getSeek(file_t *file)
{
	return SetFilePointer(file->hFile, 0, NULL, FILE_CURRENT);
}
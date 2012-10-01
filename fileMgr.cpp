#include"framework.h"

file_t *fileMgr_open(sint8 *name)
{
	HANDLE hFile = CreateFile((LPCSTR)name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if( hFile == INVALID_HANDLE_VALUE )
		return NULL;
	file_t *file = (file_t*)malloc(sizeof(file_t));
	file->hFile = hFile;
	return file;
}

file_t *fileMgr_create(sint8 *name)
{
	HANDLE hFile = CreateFile((LPCSTR)name, FILE_ALL_ACCESS, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if( hFile == INVALID_HANDLE_VALUE )
		return NULL;
	file_t *file = (file_t*)malloc(sizeof(file_t));
	file->hFile = hFile;
	return file;
}

void fileMgr_close(file_t *file)
{
	if( file )
	{
		CloseHandle(file->hFile);
		free((void*)file);
	}
}


// todo: add cache for writing
void fileMgr_writeS8(file_t *file, sint8 value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(sint8), &BT, NULL);
}

void fileMgr_writeS16(file_t *file, sint16 value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(sint16), &BT, NULL);
}

void fileMgr_writeS32(file_t *file, sint32 value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(sint32), &BT, NULL);
}

void fileMgr_writeU8(file_t *file, uint8 value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(uint8), &BT, NULL);
}

void fileMgr_writeU16(file_t *file, uint16 value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(uint16), &BT, NULL);
}

void fileMgr_writeU32(file_t *file, uint32 value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(uint32), &BT, NULL);
}

void fileMgr_writeFloat(file_t *file, float value)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)&value, sizeof(float), &BT, NULL);
}

void fileMgr_writeData(file_t *file, void *data, sint32 len)
{
	DWORD BT;
	WriteFile(file->hFile, (LPCVOID)data, len, &BT, NULL);
}


sint8 fileMgr_readS8(file_t *file)
{
	DWORD BT;
	sint8 value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(sint8), &BT, NULL);
	return value;
}

sint16 fileMgr_readS16(file_t *file)
{
	DWORD BT;
	sint16 value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(sint16), &BT, NULL);
	return value;
}

sint32 fileMgr_readS32(file_t *file)
{
	DWORD BT;
	sint32 value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(sint32), &BT, NULL);
	return value;
}

uint8 fileMgr_readU8(file_t *file)
{
	DWORD BT;
	uint8 value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(uint8), &BT, NULL);
	return value;
}

uint16 fileMgr_readU16(file_t *file)
{
	DWORD BT;
	uint16 value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(uint16), &BT, NULL);
	return value;
}

uint32 fileMgr_readU32(file_t *file)
{
	DWORD BT;
	uint32 value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(uint32), &BT, NULL);
	return value;
}

float fileMgr_readFloat(file_t *file)
{
	DWORD BT;
	float value;
	ReadFile(file->hFile, (LPVOID)&value, sizeof(float), &BT, NULL);
	return value;
}

void fileMgr_readData(file_t *file, void *data, sint32 len)
{
	DWORD BT;
	ReadFile(file->hFile, (LPVOID)data, len, &BT, NULL);
}


sint8 *fileMgr_readLine(file_t *file)
{
	// todo: optimize this..
	DWORD currentSeek = SetFilePointer(file->hFile, 0, NULL, FILE_CURRENT);
	DWORD fileSize = GetFileSize(file->hFile, NULL);
	DWORD maxLen = fileSize - currentSeek;
	if( maxLen == 0 )
		return NULL; // eof reached
	// begin parsing
	sint8 *cstr = (sint8*)malloc(512);
	sint32 size = 0;
	sint32 limit = 512;
	while( maxLen )
	{
		maxLen--;
		sint8 n = fileMgr_readS8(file);
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

void fileMgr_setSeek(file_t *file, uint32 seek)
{
	SetFilePointer(file->hFile, seek, NULL, FILE_BEGIN);
}

uint32 fileMgr_getSeek(file_t *file)
{
	return SetFilePointer(file->hFile, 0, NULL, FILE_CURRENT);
}
#include <Windows.h>
#include <stdio.h>
#include "Packing.h"

void pym_init(pyUnmarshalString_t *pms, unsigned char *rawData, int len)
{
	pms->buffer = rawData;
	pms->len = len;
	pms->idx = 0;
	pms->unpackErrorEncountered = false;
	pms->stackIndex = 0;
}

bool pym_unpackTuple_begin(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x80 )
	{
		int c = p & 0x0F;
		int tupleLen;
		if( c <= 0xC )
		{
			tupleLen = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			tupleLen = *(unsigned char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			tupleLen = *(unsigned short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			tupleLen = *(unsigned int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// add tuple container
		pms->containerStack[pms->stackIndex].type = 't';
		pms->containerStack[pms->stackIndex].size = tupleLen;
		pms->containerStack[pms->stackIndex].subelementsLeft = tupleLen;
		// dont reduce parent yet

		pms->stackIndex++;
		return true;
	}
	else
	{
		pms->unpackErrorEncountered = true;
		return false;
	}
}

bool pym_unpackDict_begin(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x60 )
	{
		int c = p & 0x0F;
		int dictSize;
		if( c <= 0xC )
		{
			dictSize = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			dictSize = *(unsigned char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			dictSize = *(unsigned short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			dictSize = *(unsigned int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// add dict container
		pms->containerStack[pms->stackIndex].type = 'd';
		pms->containerStack[pms->stackIndex].size = dictSize;
		pms->containerStack[pms->stackIndex].subelementsLeft = dictSize*2;
		pms->stackIndex++;
		return true;
	}
	else
	{
		pms->unpackErrorEncountered = true;
		return false;
	}
}

int pym_getContainerSize(pyUnmarshalString_t *pms)
{
	if( pms->stackIndex == 0 )
		return 0;
	return pms->containerStack[pms->stackIndex-1].size;
}

int pym_unpackInt(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x10 )
	{
		int c = p & 0x0F;
		int value;
		if( c <= 0xC )
		{
			value = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			value = *(signed char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			value = *(signed short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			value = *(signed int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return value;
	}
	else if( (p&0xF0) == 0x20 )
	{
		long long value = pym_unpackLongLong(pms);
		return (int)value;
	}
	return 0;
}

long long pym_unpackLongLong(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x20 )
	{
		int c = p & 0x0F;
		long long value;
		if( c <= 0xC )
		{
			__debugbreak();
		}
		else if( c == 0xD )
		{
			__debugbreak();
		}
		else if( c == 0xE )
		{
			__debugbreak();
		}
		else
		{
			value = *(signed long long*)(pms->buffer+pms->idx+1);
			pms->idx += 9;
		}
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return value;
	}
	else if( (p&0xF0) == 0x10 )
	{
		long long value = pym_unpackInt(pms);
		return value;
	}
	return 0;
}

int pym_unpackUnicode(pyUnmarshalString_t *pms, char *dst, int limit)
{
	// pyUnmarshalString_t *pms
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x50 )
	{
		int c = p & 0x0F;
		int slen;
		if( c <= 0xC )
		{
			slen = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			slen = *(unsigned char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			slen = *(unsigned short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			slen = *(unsigned int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// read string data
		int rlen = min(slen,limit-1);
		for(int i=0; i<rlen; i++)
		{
			dst[i] = (char)pms->buffer[pms->idx+i];
		}
		dst[rlen] = '\0';
		pms->idx += slen;
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return rlen;
	}
	return -1;
}

float pym_unpackFloat(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x30 )
	{
		float value;
		if( p == 0x31 )
		{
			value = 1.0f;
			pms->idx++;
		}
		else if( p == 0x3F )
		{
			value = *(float*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		else if( p == 0x30 )
		{
			value = 0.0f;
			pms->idx++;
		}
		else
			__debugbreak();
		
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return value;
	}
	return 0;
}

bool pym_unpackBool(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	bool ret = false;
	if (p == 'T')
	{ ret = true; }
	else if (p == 'F')
	{ ret = false; }
	else 
	{ printf("Error unpacking bool\n"); }

	pms->idx++;

	// reduce parent
	if( pms->stackIndex )
	{
		while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
		{
			pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
			pms->stackIndex--;
		}
	}
	return ret;
}

bool pym_unpack_isNoneStruct(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	return p == 0;
}

bool pym_unpackNoneStruct(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( p )
		return false;
	pms->idx++;
	return true;
}
#include"framework.h"
#include"Packing.h"
#include<stdio.h>

void pym_init(pyUnmarshalString_t *pms, uint8 *rawData, sint32 len)
{
	pms->buffer = rawData;
	pms->len = len;
	pms->idx = 0;
	pms->unpackErrorEncountered = false;
	pms->stackIndex = 0;
}

bool pym_unpackTuple_begin(pyUnmarshalString_t *pms)
{
	uint8 p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x80 )
	{
		sint32 c = p & 0x0F;
		sint32 tupleLen;
		if( c <= 0xC )
		{
			tupleLen = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			tupleLen = *(uint8*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			tupleLen = *(uint16*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			tupleLen = *(uint32*)(pms->buffer+pms->idx+1);
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

/*
 * Tries to unpack a list
 * Returns the number of items in this list or -1 on error
 */
sint32 pym_unpackList_begin(pyUnmarshalString_t *pms)
{
	uint8 p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x70 )
	{
		sint32 c = p & 0x0F;
		sint32 listLen;
		if( c <= 0xC )
		{
			listLen = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			listLen = *(uint8*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			listLen = *(uint16*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			listLen = *(uint32*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// add list container
		pms->containerStack[pms->stackIndex].type = 'l';
		pms->containerStack[pms->stackIndex].size = listLen;
		pms->containerStack[pms->stackIndex].subelementsLeft = listLen;
		// dont reduce parent yet
		pms->stackIndex++;
		return listLen;
	}
	else
	{
		pms->unpackErrorEncountered = true;
		return -1;
	}
}

bool pym_unpackDict_begin(pyUnmarshalString_t *pms)
{
	uint8 p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x60 )
	{
		sint32 c = p & 0x0F;
		sint32 dictSize;
		if( c <= 0xC )
		{
			dictSize = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			dictSize = *(uint8*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			dictSize = *(uint16*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			dictSize = *(uint32*)(pms->buffer+pms->idx+1);
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

sint32 pym_getContainerSize(pyUnmarshalString_t *pms)
{
	if( pms->stackIndex == 0 )
		return 0;
	return pms->containerStack[pms->stackIndex-1].size;
}

sint32 pym_unpackInt(pyUnmarshalString_t *pms)
{
	uint8 p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x10 )
	{
		sint32 c = p & 0x0F;
		sint32 value;
		if( c <= 0xC )
		{
			value = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			value = *(sint8*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			value = *(sint16*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			value = *(sint32*)(pms->buffer+pms->idx+1);
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
		return (sint32)value;
	}
	return 0;
}

long long pym_unpackLongLong(pyUnmarshalString_t *pms)
{
	uint8 p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x20 )
	{
		sint32 c = p & 0x0F;
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

sint32 pym_unpackUnicode(pyUnmarshalString_t *pms, sint8 *dst, sint32 limit)
{
	// pyUnmarshalString_t *pms
	uint8 p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x50 )
	{
		sint32 c = p & 0x0F;
		sint32 slen;
		if( c <= 0xC )
		{
			slen = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			slen = *(uint8*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			slen = *(uint16*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			slen = *(uint32*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// read string data
		sint32 rlen = min(slen,limit-1);
		for(sint32 i=0; i<rlen; i++)
		{
			dst[i] = (sint8)pms->buffer[pms->idx+i];
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
	uint8 p = pms->buffer[pms->idx];
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
	uint8 p = pms->buffer[pms->idx];
	bool ret = false;
	if (p == 0x11)
	{ ret = true; }
	else if (p == 0x10)
	{ ret = false; }
	else if (p == 0x00) // int 0 ?
	{ ret = false; }
	else if (p == 0x01) // int 1 ?
	{ ret = true; }
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
	uint8 p = pms->buffer[pms->idx];
	return p == 0;
}

bool pym_unpackNoneStruct(pyUnmarshalString_t *pms)
{
	uint8 p = pms->buffer[pms->idx];
	if( p )
		return false;
	pms->idx++;
	return true;
}
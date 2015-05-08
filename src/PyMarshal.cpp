#include "Global.h"

// new marshal lib

void pym_init(pyMarshalString_t *pms)
{
	pms->stackIdx = 0;
	pms->bufferSize = 2048;
	pms->idx = 0;
}

void pym_tuple_begin(pyMarshalString_t *pms)
{
	if( pms->idx+5 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
	pms->containerStack[pms->stackIdx].type = 't';
	pms->containerStack[pms->stackIdx].startOffset = pms->idx;
	pms->containerStack[pms->stackIdx].subelements = 0;
	// tuple data
	pms->buffer[pms->idx] = 0x28; pms->idx++;
	pms->idx += 4;
	pms->stackIdx++;
}

void pym_tuple_end(pyMarshalString_t *pms)
{
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 't' )
	{
		__debugbreak();
	}
	*(uint32*)(pms->buffer+pms->containerStack[pms->stackIdx].startOffset+1) = pms->containerStack[pms->stackIdx].subelements;
}

void pym_list_begin(pyMarshalString_t *pms)
{
	if( pms->idx+5 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	// check length
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
	pms->containerStack[pms->stackIdx].type = 'l';
	pms->containerStack[pms->stackIdx].startOffset = pms->idx;
	pms->containerStack[pms->stackIdx].subelements = 0;
	// list data
	pms->buffer[pms->idx] = 0x5B; pms->idx++;
	pms->idx += 4;

	//*(uint32*)&(pms->buffer+pms->idx) = 0x00000000; pys->CLen += 4; //Elementcount - Dont forget to update this

	pms->stackIdx++;
}

void pym_list_end(pyMarshalString_t *pms)
{
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 'l' )
	{
		__debugbreak();
	}
	*(uint32*)(pms->buffer+pms->containerStack[pms->stackIdx].startOffset+1) = pms->containerStack[pms->stackIdx].subelements;
}

void pym_dict_begin(pyMarshalString_t *pms)
{
	if( pms->idx+1 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
	pms->containerStack[pms->stackIdx].type = 'd';
	pms->containerStack[pms->stackIdx].startOffset = pms->idx;
	pms->containerStack[pms->stackIdx].subelements = 0;
	// dict data
	pms->buffer[pms->idx] = 0x7B; pms->idx++;
	pms->stackIdx++;
}

void pym_dict_end(pyMarshalString_t *pms)
{
	if( pms->idx+1 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 'd' )
	{
		__debugbreak();
	}
	pms->buffer[pms->idx] = 0x30; pms->idx++;
}

void pym_dict_addKey(pyMarshalString_t *pms, sint8 *s)
{
	uint32 slen = strlen((char*)s);
	if( pms->idx+slen+4 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = 0x74; pms->idx++;
	*(sint32*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(uint32 i=0; i<slen; i++)
	{
		*(sint8*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void pym_addInt(pyMarshalString_t *pms, sint32 value)
{
	if( pms->idx+5 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = 0x69; pms->idx++;
	*(sint32*)(pms->buffer+pms->idx) = value; pms->idx += 4;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

// Python longs are signed numbers with an arbitrary size (rather than 8, 16, 32 or 64 bit)
// To improve speed and avoid using bignum libs we use fake longs that are always 64-bit
void pym_addLong(pyMarshalString_t *pms, long long value)
{
	if( pms->idx+4+10 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = 'l'; pms->idx++;
	*(long*)(pms->buffer+pms->idx) = 5; pms->idx += 4; // number of 'digits' - one digit has 15 bits and is stored as a sint16 (64/15 = 4,2666 --> 5)
	// write 5 15-bit sint16s
	for(uint32 i=0; i<5; i++)
	{
		*(sint16*)(pms->buffer+pms->idx) = (value&0x7FFF); pms->idx += 2; // write 15 bits
		value >>= 15; // get the next 15 bits
	}

	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void pym_addString(pyMarshalString_t *pms, sint8 *s)
{
	uint32 slen = strlen((char*)s);
	if( pms->idx+slen+4 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = 's'; pms->idx++;
	*(sint32*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(uint32 i=0; i<slen; i++)
	{
		*(sint8*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void pym_addUnicode(pyMarshalString_t *pms, sint8 *s)
{
	uint32 slen = strlen((char*)s);
	if( pms->idx+slen+4 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = 'u'; pms->idx++;
	*(sint32*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(uint32 i=0; i<slen; i++)
	{
		*(sint8*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void pym_addNoneStruct(pyMarshalString_t *pms)
{
	if( pms->idx+1 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = 0x4E; pms->idx++;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void pym_addBool(pyMarshalString_t *pms, bool b)
{
	if( pms->idx+1 > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = b?'T':'F'; pms->idx++;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void pym_addFloat(pyMarshalString_t *pms, float value)
{
	// convert to string
	sint32 len = 0;
	sint8 floatData[128];
	sprintf((char*)floatData, "%f", value);
	len = strlen((char*)floatData);
	if( pms->idx+1+1+len > pms->bufferSize )
	{
		printf("%s: Exceeding buffer size, cannot write marshal data\n", __FUNCTION__);
	}
	pms->buffer[pms->idx] = 0x66; pms->idx++;
	// write len
	pms->buffer[pms->idx] = len; pms->idx++;
	// write float as string
	for(sint32 i=0; i<len; i++)
	{
		*(sint8*)(pms->buffer+pms->idx) = floatData[i]; pms->idx++;
	}
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

uint8 *pym_getData(pyMarshalString_t *pms)
{
	return pms->buffer;
}

uint32 pym_getLen(pyMarshalString_t *pms)
{
	return pms->idx;
}
#include "Global.h"

// new marhsal lib

void pym_init(pyMarshalString_t *pms)
{
	pms->stackIdx = 0;
	pms->idx = 0;
}

void pym_tuple_begin(pyMarshalString_t *pms)
{
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
	*(unsigned int*)(pms->buffer+pms->containerStack[pms->stackIdx].startOffset+1) = pms->containerStack[pms->stackIdx].subelements;
}

void pym_list_begin(pyMarshalString_t *pms)
{
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
	pms->containerStack[pms->stackIdx].type = 'l';
	pms->containerStack[pms->stackIdx].startOffset = pms->idx;
	pms->containerStack[pms->stackIdx].subelements = 0;
	
	// list data
	pms->buffer[pms->idx] = 0x5B; pms->idx++;
	pms->idx += 4;

	//*(unsigned int*)&(pms->buffer+pms->idx) = 0x00000000; pys->CLen += 4; //Elementcount - Dont forget to update this

	pms->stackIdx++;
}

void pym_list_end(pyMarshalString_t *pms)
{
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 'l' )
	{
		__debugbreak();
	}
	*(unsigned int*)(pms->buffer+pms->containerStack[pms->stackIdx].startOffset+1) = pms->containerStack[pms->stackIdx].subelements;
}

void pym_dict_begin(pyMarshalString_t *pms)
{
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
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 'd' )
	{
		__debugbreak();
	}
	pms->buffer[pms->idx] = 0x30; pms->idx++;
}

void pym_dict_addKey(pyMarshalString_t *pms, char *s)
{
	unsigned int slen = strlen(s);
	pms->buffer[pms->idx] = 0x74; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(unsigned int i=0; i<slen; i++)
	{
		*(char*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void pym_addInt(pyMarshalString_t *pms, int value)
{
	pms->buffer[pms->idx] = 0x69; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = value; pms->idx += 4;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void pym_addString(pyMarshalString_t *pms, char *s)
{
	unsigned int slen = strlen(s);
	pms->buffer[pms->idx] = 's'; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(unsigned int i=0; i<slen; i++)
	{
		*(char*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void pym_addUnicode(pyMarshalString_t *pms, char *s)
{
	unsigned int slen = strlen(s);
	pms->buffer[pms->idx] = 'u'; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(unsigned int i=0; i<slen; i++)
	{
		*(char*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void pym_addNoneStruct(pyMarshalString_t *pms)
{
	pms->buffer[pms->idx] = 0x4E; pms->idx++;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void pym_addBool(pyMarshalString_t *pms, bool b)
{
	pms->buffer[pms->idx] = b?'T':'F'; pms->idx++;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void pym_addFloat(pyMarshalString_t *pms, float value)
{
	pms->buffer[pms->idx] = 0x66; pms->idx++;
	// convert to string
	int len = 0;
	char floatData[128];
	sprintf(floatData, "%f", value);
	// write len
	len = strlen(floatData);
	pms->buffer[pms->idx] = len; pms->idx++;
	// write float as string
	for(int i=0; i<len; i++)
	{
		*(char*)(pms->buffer+pms->idx) = floatData[i]; pms->idx++;
	}
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

unsigned char *pym_getData(pyMarshalString_t *pms)
{
	return pms->buffer;
}

unsigned int pym_getLen(pyMarshalString_t *pms)
{
	return pms->idx;
}
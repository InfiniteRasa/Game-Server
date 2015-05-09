
/*
	Helper for creating and parsing PyMarshal Strings without having Python.
	Also this should be much faster than the original Python methods.
*/

typedef struct  
{
	sint8 type; //'t' tuple, 'd' dict
	// general
	sint32 startOffset;
	sint32 subelements;
	
	// dict info

	// tuple info
	//sint32 tupleSize;
}containerStackEntry_t;

typedef struct  
{
	uint8 buffer[2048];
	sint32 bufferSize;
	sint32 idx;
	containerStackEntry_t containerStack[16];
	sint32 stackIdx;
}pyMarshalString_t;

// new marshal packing lib

void pym_init(pyMarshalString_t *pms);

void pym_tuple_begin(pyMarshalString_t *pms);
void pym_tuple_end(pyMarshalString_t *pms);

void pym_list_begin(pyMarshalString_t *pms);
void pym_list_end(pyMarshalString_t *pms);

void pym_dict_begin(pyMarshalString_t *pms);
void pym_dict_end(pyMarshalString_t *pms);
void pym_dict_addKey(pyMarshalString_t *pms, sint8 *s);

void pym_addInt(pyMarshalString_t *pms, sint32 value);
void pym_addLong(pyMarshalString_t *pms, long long value);
void pym_addString(pyMarshalString_t *pms, sint8 *s);
void pym_addUnicode(pyMarshalString_t *pms, sint8 *s);
void pym_addNoneStruct(pyMarshalString_t *pms);
void pym_addBool(pyMarshalString_t *pms, bool b);
void pym_addFloat(pyMarshalString_t *pms, float value);

uint8 *pym_getData(pyMarshalString_t *pms);
uint32 pym_getLen(pyMarshalString_t *pms);

// new marshal unpacking lib

typedef struct  
{
	sint8 type; // 'd' - dict, 't' - tuple
	uint32 size;
	uint32 subelementsLeft;
}pyUnmarshalContainer_t;

typedef struct  
{
	uint8 *buffer;
	sint32 idx;
	sint32 len;
	bool unpackErrorEncountered;
	pyUnmarshalContainer_t containerStack[16];
	sint32 stackIndex;
}pyUnmarshalString_t;

void pym_init(pyUnmarshalString_t *pms, uint8 *rawData, sint32 len);
bool pym_unpackTuple_begin(pyUnmarshalString_t *pms);
sint32 pym_unpackList_begin(pyUnmarshalString_t *pms);
bool pym_unpackDict_begin(pyUnmarshalString_t *pms);
sint32 pym_getContainerSize(pyUnmarshalString_t *pms);
sint32 pym_unpackInt(pyUnmarshalString_t *pms);
long long pym_unpackLongLong(pyUnmarshalString_t *pms);
sint32 pym_unpackUnicode(pyUnmarshalString_t *pms, sint8 *dst, sint32 limit);
float pym_unpackFloat(pyUnmarshalString_t *pms);
bool pym_unpackBool(pyUnmarshalString_t *pms);
bool pym_unpackNoneStruct(pyUnmarshalString_t *pms);

bool pym_unpack_isNoneStruct(pyUnmarshalString_t *pms);
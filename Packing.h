
/*
	Helper for creating and parsing PyMarshal Strings without having Python.
	Also this should be much faster than the original Python methods.
*/

typedef struct  
{
	char type; //'t' tuple, 'd' dict
	// general
	int startOffset;
	int subelements;
	
	// dict info

	// tuple info
	//int tupleSize;
}containerStackEntry_t;

typedef struct  
{
	unsigned char buffer[1024];
	int idx;
	containerStackEntry_t containerStack[16];
	int stackIdx;
}pyMarshalString_t;

// new marshal packing lib

void pym_init(pyMarshalString_t *pms);

void pym_tuple_begin(pyMarshalString_t *pms);
void pym_tuple_end(pyMarshalString_t *pms);

void pym_list_begin(pyMarshalString_t *pms);
void pym_list_end(pyMarshalString_t *pms);

void pym_dict_begin(pyMarshalString_t *pms);
void pym_dict_end(pyMarshalString_t *pms);
void pym_dict_addKey(pyMarshalString_t *pms, char *s);

void pym_addInt(pyMarshalString_t *pms, int value);
void pym_addString(pyMarshalString_t *pms, char *s);
void pym_addUnicode(pyMarshalString_t *pms, char *s);
void pym_addNoneStruct(pyMarshalString_t *pms);
void pym_addBool(pyMarshalString_t *pms, bool b);
void pym_addFloat(pyMarshalString_t *pms, float value);

unsigned char *pym_getData(pyMarshalString_t *pms);
unsigned int pym_getLen(pyMarshalString_t *pms);

// new marshal unpacking lib

typedef struct  
{
	char type; // 'd' - dict, 't' - tuple
	unsigned int size;
	unsigned int subelementsLeft;
}pyUnmarshalContainer_t;

typedef struct  
{
	unsigned char *buffer;
	int idx;
	int len;
	bool unpackErrorEncountered;
	pyUnmarshalContainer_t containerStack[16];
	int stackIndex;
}pyUnmarshalString_t;

void pym_init(pyUnmarshalString_t *pms, unsigned char *rawData, int len);
bool pym_unpackTuple_begin(pyUnmarshalString_t *pms);
bool pym_unpackDict_begin(pyUnmarshalString_t *pms);
int pym_getContainerSize(pyUnmarshalString_t *pms);
int pym_unpackInt(pyUnmarshalString_t *pms);
long long pym_unpackLongLong(pyUnmarshalString_t *pms);
int pym_unpackUnicode(pyUnmarshalString_t *pms, char *dst, int limit);
float pym_unpackFloat(pyUnmarshalString_t *pms);
bool pym_unpackBool(pyUnmarshalString_t *pms);
bool pym_unpackNoneStruct(pyUnmarshalString_t *pms);

bool pym_unpack_isNoneStruct(pyUnmarshalString_t *pms);
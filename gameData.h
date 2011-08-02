void gameData_load();

int gameData_getEquipmentClassIdSlot(int classId);
int gameData_getStarterItemTemplateClassId(int templateId);

typedef struct  
{
	int contextId;
	char name[64];
	int version;
	int baseRegionId;
}gameData_mapInfo_t;

extern int mapInfoCount;
extern gameData_mapInfo_t *mapInfoArray;


#define ITEMTYPE_WEAPON	1
#define ITEMTYPE_ARMOR	2


typedef struct  
{
	unsigned int classId;
	unsigned int templateId;
	char type;
}itemTemplate_t;

itemTemplate_t *gameData_getItemTemplateById(unsigned int templateId);
itemTemplate_t *gameData_getItemTemplateByName(char *name);

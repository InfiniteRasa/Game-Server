typedef struct  
{
	char *optionName;
	char *optionData;
}sData_optionLine_t;

typedef struct  
{
	file_t *file;
	// current category settings
	char *categoryName;
	DWORD categoryDataOffset;
	// option data
	int optionLineCount;
	sData_optionLine_t *optionLine;
}sData_t;

sData_t *sData_open(char *path);
bool sData_nextCategory(sData_t *sData);
char *sData_currentCategoryName(sData_t *sData);
char *sData_findOption(sData_t *sData, char *optionName);
void sData_close(sData_t *sData);
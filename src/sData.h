typedef struct  
{
	sint8 *optionName;
	sint8 *optionData;
}sData_optionLine_t;

typedef struct  
{
	file_t *file;
	// current category settings
	sint8 *categoryName;
	DWORD categoryDataOffset;
	// option data
	sint32 optionLineCount;
	sData_optionLine_t *optionLine;
}sData_t;

sData_t *sData_open(sint8 *path);
bool sData_nextCategory(sData_t *sData);
sint8 *sData_currentCategoryName(sData_t *sData);
sint8 *sData_findOption(sData_t *sData, sint8 *optionName);
void sData_close(sData_t *sData);
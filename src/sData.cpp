#include"framework.h"

static sint8 sData_whitespaceList[] = {' ','\t'};

sData_t *sData_open(sint8 *path)
{
	file_t *file = fileMgr_open(path);
	if( !file )
		return NULL;
	sData_t *sData = (sData_t*)malloc(sizeof(sData_t));
	sData->file = file;
	sData->categoryName = NULL;
	sData->categoryDataOffset = 0; // category data start
	sData->optionLineCount = 0;
	sData->optionLine = NULL;
	return sData;
}

void _sData_preloadCategory(sData_t *sData)
{
	// release data
	if( sData->optionLine )
	{
		for(sint32 i=0; i<sData->optionLineCount; i++)
		{
			if( sData->optionLine[i].optionName )
				free(sData->optionLine[i].optionName);
			if( sData->optionLine[i].optionData )
				free(sData->optionLine[i].optionData);
		}
		free(sData->optionLine);
	}
	// count lines
	fileMgr_setSeek(sData->file, sData->categoryDataOffset);
	sint8 *line = fileMgr_readLine(sData->file);
	sint32 lineCount = 0;
	while( line )
	{
		// todo: trim left
		if( line[0] == '[' )
		{
			break;
		}
		sint8 *x = line;
		// skip whitespaces
		while( *x )
		{
			if( *x != sData_whitespaceList[0] && *x != sData_whitespaceList[1] )
				break;
			x++;
		}
		// data lines must start with a letter
		if( (*x >= 'a' && *x <= 'z') || (*x >= 'A' && *x <= 'Z') )
			lineCount++;
		// free line and get next
		free(line);
		line = fileMgr_readLine(sData->file);
	}
	// allocate space for lines
	sData->optionLineCount = lineCount;
	if( lineCount == 0 )
	{
		sData->optionLine = NULL;
		return;
	}
	sData->optionLine = (sData_optionLine_t*)malloc(sizeof(sData_optionLine_t) * lineCount);
	// read all data lines
	lineCount = 0;
	fileMgr_setSeek(sData->file, sData->categoryDataOffset);
	line = fileMgr_readLine(sData->file);
	lineCount = 0;
	while( line )
	{
		// todo: trim left
		if( line[0] == '[' )
		{
			break;
		}
		sint8 *x = line;
		// skip whitespaces
		while( *x )
		{
			if( *x != sData_whitespaceList[0] && *x != sData_whitespaceList[1] )
				break;
			x++;
		}
		// data lines must start with a letter
		if( (*x >= 'a' && *x <= 'z') || (*x >= 'A' && *x <= 'Z') )
		{
			sint32 splitIdx = -1;
			sint32 tLen = strlen((char*)x);
			// find '='
			for(sint32 i=0; i<tLen; i++)
			{
				if( x[i] == '=' )
				{
					splitIdx = i;
					break;
				}
			}
			if( splitIdx == -1 )
			{
				// only name set...
				// cover with empty data string
				sData->optionLine[lineCount].optionName = (sint8*)malloc(tLen+1);
				for(sint32 p=0; p<tLen; p++)
					sData->optionLine[lineCount].optionName[p] = x[p];
				sData->optionLine[lineCount].optionName[tLen] = '\0';
				sData->optionLine[lineCount].optionData = (sint8*)malloc(1);
				sData->optionLine[lineCount].optionData[0] = '\0';
			}
			else
			{
				// name
				sData->optionLine[lineCount].optionName = (sint8*)malloc(splitIdx+1);
				for(sint32 p=0; p<splitIdx; p++)
					sData->optionLine[lineCount].optionName[p] = x[p];
				sData->optionLine[lineCount].optionName[splitIdx] = '\0';
				// skip '='
				splitIdx++;
				// data - but skip whitespaces first
				sint32 whiteSpaceCount = 0;
				for(sint32 i=0; i<tLen-splitIdx; i++)
				{
					if( x[i+splitIdx] != ' ' && x[i+splitIdx] != '\t' )
						break;
					whiteSpaceCount++;
				}
				sData->optionLine[lineCount].optionData = (sint8*)malloc(tLen-whiteSpaceCount-splitIdx+1);
				for(sint32 p=0; p<(tLen-splitIdx-whiteSpaceCount); p++)
					sData->optionLine[lineCount].optionData[p] = x[p+splitIdx+whiteSpaceCount];
				sData->optionLine[lineCount].optionData[tLen-splitIdx-whiteSpaceCount] = '\0';
			}
			// cut whitespaces from the name
			sint32 nameLen = strlen((char*)sData->optionLine[lineCount].optionName);
			for(sint32 i=nameLen-1; i>0; i--)
			{
				if( sData->optionLine[lineCount].optionName[i] != sData_whitespaceList[0] && sData->optionLine[lineCount].optionName[i] != sData_whitespaceList[1] )
					break;
				sData->optionLine[lineCount].optionName[i] = '\0';
			}

			lineCount++;
		}
		// free line and get next
		free(line);
		line = fileMgr_readLine(sData->file);
	}
}

bool sData_nextCategory(sData_t *sData)
{
	if( sData->categoryName )
	{
		free(sData->categoryName);
		sData->categoryName = NULL;
	}
	fileMgr_setSeek(sData->file, sData->categoryDataOffset);
	sint8 *line = fileMgr_readLine(sData->file);
	while( line )
	{
		// todo: trim left
		if( line[0] == '[' )
		{
			// category beginns
			sData->categoryDataOffset = fileMgr_getSeek(sData->file);
			sint32 catLen = min(1024, strlen((char*)line)+1);
			sData->categoryName = (sint8*)malloc(catLen);
			// copy name
			for(sint32 i=0; i<1023; i++) // 1kb is the name length limit
			{
				sData->categoryName[i] = line[i+1];
				if( sData->categoryName[i] == ']' )
				{
					sData->categoryName[i] = '\0';
					break;
				}
				if( line[i+1] == '\0' )
					break;
			}
			sData->categoryName[catLen-1] = '\0';
			free(line);
			_sData_preloadCategory(sData);
			return true;
		}
		// free line and get next
		free(line);
		line = fileMgr_readLine(sData->file);
	}

	return false;
}

sint8 *sData_currentCategoryName(sData_t *sData)
{
	return sData->categoryName;
}

sint8 *sData_findOption(sData_t *sData, sint8 *optionName)
{
	sint32 len = strlen((char*)optionName);
	for(sint32 i=0; i<sData->optionLineCount; i++)
	{
		bool fit = true;
		sint32 oLen = strlen((char*)sData->optionLine[i].optionName);
		if( oLen != len )
			continue;
		for(sint32 l=0; l<len; l++)
		{
			sint8 c1 = sData->optionLine[i].optionName[l];
			sint8 c2 = optionName[l];
			// convert to upper case
			if( c1 >= 'a' && c1 <= 'z' )
				c1 += ('A'-'a');
			if( c2 >= 'a' && c2 <= 'z' )
				c2 += ('A'-'a');
			if( c1 != c2 )
			{
				fit = false;
				break;
			}
		}
		if( fit )
			return sData->optionLine[i].optionData;
	}
	return NULL;
}

void sData_close(sData_t *sData)
{
	if( sData->categoryName )
	{
		free(sData->categoryName);
		sData->categoryName = NULL;
	}
	if( sData->optionLine )
	{
		for(sint32 i=0; i<sData->optionLineCount; i++)
		{
			if( sData->optionLine[i].optionName )
				free(sData->optionLine[i].optionName);
			if( sData->optionLine[i].optionData )
				free(sData->optionLine[i].optionData);
		}
		free(sData->optionLine);
	}
	fileMgr_close(sData->file);
	free(sData);
}
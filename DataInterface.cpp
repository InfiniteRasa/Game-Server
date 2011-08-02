#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"
#include"fileMgr.h"


dataInterfaceWorkerThread_t workerThread[WORKER_THREADS];

struct  
{
	char *dbHost;
	char *dbUser;
	char *dbName;
	char *dbPass;
}dbConInfo;

MYSQL* _dataInterface_connect()
{
	MYSQL *     dbHandle;
	dbHandle = mysql_init(0);
	MYSQL *dbHandleErr = dbHandle;
	dbHandle = mysql_real_connect(dbHandle,
		dbConInfo.dbHost,
		dbConInfo.dbUser,
		dbConInfo.dbPass,  
		dbConInfo.dbName,
		0,          
		0,        
		0);     
	if( dbHandle == 0 )
	{
		printf("Mysql: ");
		puts(mysql_error(dbHandleErr));
		Sleep(1000*60);
		return 0;
	}
	// set charset utf8
	mysql_set_character_set(dbHandle, "utf8");
	return dbHandle;
}

void dataInterface_queueJob(void *jobData, void *workCallback, void *doneCallback, void *param)
{
	dataInterfaceWorkerThread_t *wt = workerThread+0;
	// diJob_getCharacterPreviewInfo_t *job, void *cb, void *param
	int nwi = (wt->queueWriteIndex+1)%WORKER_QUEUELENGTH;
	if( wt->queueReadIndex == nwi )
		while( wt->queueReadIndex != nwi ) Sleep(10);
	// append job
	wt->jobQueue[wt->queueWriteIndex].jobData = jobData;
	wt->jobQueue[wt->queueWriteIndex].param = param;
	wt->jobQueue[wt->queueWriteIndex].workCallback = (void (__cdecl *)(MYSQL *,void *,void *,void *))workCallback;
	wt->jobQueue[wt->queueWriteIndex].doneCallBack = doneCallback;
	wt->queueWriteIndex = nwi;
}

int _dataInterface_work(dataInterfaceWorkerThread_t *wt)
{
	while( 1 )
	{
		while( wt->queueReadIndex == wt->queueWriteIndex )
			Sleep(1);
		// process job
		wt->jobQueue[wt->queueReadIndex].workCallback(wt->dbCon, wt->jobQueue[wt->queueReadIndex].jobData, wt->jobQueue[wt->queueReadIndex].doneCallBack, wt->jobQueue[wt->queueReadIndex].param);
		wt->queueReadIndex = (wt->queueReadIndex+1)%WORKER_QUEUELENGTH;
	}

	return 0;
}

void _dataInterface_initWorkerThread(int index)
{
	dataInterfaceWorkerThread_t *wt = workerThread+index;
	wt->dbCon = _dataInterface_connect();
	wt->queueReadIndex = 0;
	wt->queueWriteIndex = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_dataInterface_work, wt, 0, NULL);
}



void dataInterface_init()
{
	// read connect config details
	sData_t *config = sData_open("config.txt");
	sData_nextCategory(config);
	// host
	char *option = sData_findOption(config, "DBHOST");
	if( option == NULL )
	{
		printf("dbHost option missing in config.txt\n");
		Sleep(1000*10);
		ExitProcess(-2);
	}
	dbConInfo.dbHost = strdup(option);
	// dbName
	option = sData_findOption(config, "DBNAME");
	if( option == NULL )
	{
		printf("dbName option missing in config.txt\n");
		Sleep(1000*10);
		ExitProcess(-2);
	}
	dbConInfo.dbName = strdup(option);
	// dbUser
	option = sData_findOption(config, "DBUSER");
	if( option == NULL )
	{
		printf("dbUser option missing in config.txt\n");
		Sleep(1000*10);
		ExitProcess(-2);
	}
	dbConInfo.dbUser = strdup(option);
	// dbPass
	option = sData_findOption(config, "DBPASS");
	if( option == NULL )
	{
		printf("dbPass option missing in config.txt\n");
		Sleep(1000*10);
		ExitProcess(-2);
	}
	dbConInfo.dbPass = strdup(option);
	sData_close(config);


	MYSQL *mysql = _dataInterface_connect();
	if( !mysql )
		ExitProcess(-1);
	mysql_close(mysql);

	for(int i=0; i<WORKER_THREADS; i++)
		_dataInterface_initWorkerThread(i);
}

void* dataInterface_allocJob(int size)
{
	return malloc(size);
}

void dataInterface_freeJob(void *job)
{
	free(job);
}
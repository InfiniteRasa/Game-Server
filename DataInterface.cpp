#include<winsock2.h>
#include<stdio.h>
#include"DataInterface.h"


DataInterfaceWorkerThread_t workerThread[WORKER_THREADS];

struct  
{
	sint8* dbHost;
	sint8* dbUser;
	sint8* dbName;
	sint8* dbPass;
	sint32 dbPort;
} dbConInfo;

struct GSInfo
{
	sint8* ServerIP;
	sint32 ServerIPHex;
	sint32 ServerPort;
	sint32 ID;
};

struct  
{
	MYSQL*		  mysql_as;
	sint8*		  dbHost;
	sint8*		  dbUser;
	sint8*	      dbName;
	sint8*		  dbPass;
	sint32		  dbPort;
	struct GSInfo gsInfo;
} AuthInfo;



MYSQL* _DataInterface_gs_connect()
{
	MYSQL* dbHandle;
	dbHandle = mysql_init(0);
	MYSQL* dbHandleErr = dbHandle;
	dbHandle = mysql_real_connect(dbHandle, dbConInfo.dbHost,
								  dbConInfo.dbUser, dbConInfo.dbPass,  
								  dbConInfo.dbName, dbConInfo.dbPort, 0, 0);     
	if( dbHandle == 0 )
	{
		printf("GS Mysql: ");
		puts(mysql_error(dbHandleErr));
		Sleep(1000*60);
		return 0;
	}
	// set charset utf8
	mysql_set_character_set(dbHandle, "utf8");
	return dbHandle;
}

MYSQL* _DataInterface_as_connect()
{
	MYSQL* dbHandle;
	dbHandle = mysql_init(0);
	MYSQL* dbHandleErr = dbHandle;
	dbHandle = mysql_real_connect(dbHandle, AuthInfo.dbHost,
								  AuthInfo.dbUser, AuthInfo.dbPass,  
								  AuthInfo.dbName, AuthInfo.dbPort, 0, 0);     
	if( dbHandle == 0 )
	{
		printf("AS Mysql: ");
		puts(mysql_error(dbHandleErr));
		Sleep(1000*60);
		return 0;
	}
	// set charset utf8
	mysql_set_character_set(dbHandle, "utf8");
	return dbHandle;
}

void DataInterface_queueJob(void *jobData, void *workCallback, void *doneCallback, void *param)
{
	DataInterfaceWorkerThread_t *wt = workerThread+0;
	// diJob_getCharacterPreviewInfo_t *job, void *cb, void *param
	sint32 nwi = (wt->queueWriteIndex+1)%WORKER_QUEUELENGTH;
	if( wt->queueReadIndex == nwi )
		while( wt->queueReadIndex != nwi ) Sleep(10);
	// append job
	wt->jobQueue[wt->queueWriteIndex].jobData = jobData;
	wt->jobQueue[wt->queueWriteIndex].param = param;
	wt->jobQueue[wt->queueWriteIndex].workCallback = (void (__cdecl *)(MYSQL *,void *,void *,void *))workCallback;
	wt->jobQueue[wt->queueWriteIndex].doneCallBack = doneCallback;
	wt->queueWriteIndex = nwi;
}

sint32 _DataInterface_work(DataInterfaceWorkerThread_t *wt)
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

void _DataInterface_initWorkerThread(sint32 index)
{
	DataInterfaceWorkerThread_t *wt = workerThread+index;
	wt->dbCon = _DataInterface_gs_connect();
	wt->queueReadIndex = 0;
	wt->queueWriteIndex = 0;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_DataInterface_work, wt, 0, NULL);
	Thread::New(NULL, (THREAD_ROUTINE)_DataInterface_work, wt);
}

void DataInterface_init()
{
	printf("Reading configuration file...\n\n");
	INIParser* Parser = new INIParser("config.ini");
	// Game Server Database
	dbConInfo.dbHost = strdup(Parser->GetString("Game Server Database", "dbHost", "localhost").c_str());
	dbConInfo.dbPort = Parser->GetInt("Game Server Database", "dbPort", 10061);
	dbConInfo.dbName = strdup(Parser->GetString("Game Server Database", "dbName", "ir_gameserver").c_str());
	dbConInfo.dbUser = strdup(Parser->GetString("Game Server Database", "dbUser", "root").c_str());
	dbConInfo.dbPass = strdup(Parser->GetString("Game Server Database", "dbPass", "usbw").c_str());
	// Auth Server Database
	AuthInfo.dbHost = strdup(Parser->GetString("Auth Server Database", "dbHost", "localhost").c_str());
	AuthInfo.dbPort = Parser->GetInt("Auth Server Database", "dbPort", 10061);
	AuthInfo.dbName = strdup(Parser->GetString("Auth Server Database", "dbName", "ir_authentication").c_str());
	AuthInfo.dbUser = strdup(Parser->GetString("Auth Server Database", "dbUser", "root").c_str());
	AuthInfo.dbPass = strdup(Parser->GetString("Auth Server Database", "dbPass", "usbw").c_str());
	// Game  Server options
	AuthInfo.gsInfo.ServerIP	= strdup(Parser->GetString("Game Server", "IPAddress", "127.0.0.1").c_str());
	AuthInfo.gsInfo.ServerIPHex = DataInterface_IPtoHex();
	AuthInfo.gsInfo.ServerPort	= Parser->GetInt("Game Server", "Port", 8001);
	AuthInfo.gsInfo.ID			= Parser->GetInt("Game Server", "ID", 234);

	delete Parser;

	printf("  GSDB Host: %s\n", dbConInfo.dbHost);
	printf("  GSDB Port: %d\n", dbConInfo.dbPort);
	printf("  GSDB Name: %s\n", dbConInfo.dbName);
	printf("  GSDB User: %s\n", dbConInfo.dbUser);
	printf("  GSDB Pass: %s\n\n", dbConInfo.dbPass);

	printf("  ASDB Host: %s\n", AuthInfo.dbHost);
	printf("  ASDB Port: %d\n", AuthInfo.dbPort);
	printf("  ASDB Name: %s\n", AuthInfo.dbName);
	printf("  ASDB User: %s\n", AuthInfo.dbUser);
	printf("  ASDB Pass: %s\n\n", AuthInfo.dbPass);

	printf("  Server ID: %d\n", AuthInfo.gsInfo.ID);
	printf("  Server IP: %s\n", AuthInfo.gsInfo.ServerIP);
	printf("  Server Port: %d\n\n", AuthInfo.gsInfo.ServerPort);

	printf("Connecting to Game Server Database...\n");
	MYSQL *mysql_gs = _DataInterface_gs_connect();
	if( !mysql_gs ) { ExitProcess(-1); }
	mysql_close(mysql_gs);

	printf("Connecting to Auth Server Database...\n");
	AuthInfo.mysql_as = _DataInterface_as_connect();
	if( !AuthInfo.mysql_as ) { ExitProcess(-1); }

	for(sint32 i=0; i<WORKER_THREADS; i++)
		_DataInterface_initWorkerThread(i);
}

void* DataInterface_allocJob(sint32 size)
{
	return malloc(size);
}

void DataInterface_freeJob(void *job)
{
	free(job);
}

void DataInterface_registerServerForAuth()
{

	sint8 queryText1[1024];
	wsprintf(queryText1, "SELECT server_id FROM game_servers WHERE host='%s' AND port='%i' AND server_id='%i'  LIMIT 1", AuthInfo.gsInfo.ServerIP, AuthInfo.gsInfo.ServerPort, AuthInfo.gsInfo.ID);
	if( mysql_query(AuthInfo.mysql_as, queryText1) )
	{
		printf("MySQL: Error checking server entry\n");
		Sleep(1000*60);
		return;
	}
	MYSQL_RES *dbResult = mysql_store_result(AuthInfo.mysql_as);
	MYSQL_ROW dbRow;
	dbRow = mysql_fetch_row(dbResult);
	if (dbRow != NULL)
	{
		printf("MySQL: The server is already registered in the database\n");
		return;
	}
	
	sint8 queryText2[1024];
	wsprintf(queryText2, "INSERT INTO game_servers ("
	"`server_id`,`host`,`port`,`age_limit`,`pk_flag`,`current_users`,`max_users`,`status`,`static`)"
	" VALUES(%i,'%s',%i,18,0,0,10,1,1);", AuthInfo.gsInfo.ID, AuthInfo.gsInfo.ServerIP, AuthInfo.gsInfo.ServerPort);
	if( mysql_query(AuthInfo.mysql_as, queryText2) )
	{
		printf("MySQL: Error registering the server in the database\n");
		while(1) Sleep(1000);	
	}
	printf("MySQL: The server has been registered in the database\n");
	return;
}

// 127.0.0.1 -> 1.0.0.127 -> 0x0100007F
uint32 DataInterface_IPtoHex()
{
	uint32 a, b, c, d, ret;
	sscanf(AuthInfo.gsInfo.ServerIP, "%d.%d.%d.%d", &a, &b, &c, &d);
	ret = a | (b<<8) | (c<<16) | (d<<24);
	return ret;
}

uint32 DataInterface_getMyIP()
{
	return (uint32)AuthInfo.gsInfo.ServerIPHex;
}

sint32 DataInterface_QuerySession(uint32 ID1, uint32 ID2, authSessionInfo_t *asiOut)
{
	sint8 queryText[1024];
	wsprintf(queryText, "SELECT account,uid FROM sessions WHERE session_id1='%u' AND session_id2='%u' LIMIT 1", ID1, ID2);
	
	if( mysql_query(AuthInfo.mysql_as, queryText) )
	{
		printf("Error in query for sessions\n");
		return 0;	
	}
	MYSQL_RES *dbResult = mysql_store_result(AuthInfo.mysql_as);
	MYSQL_ROW dbRow;
	dbRow = mysql_fetch_row(dbResult);
	if (dbRow == NULL) { return 0; }
	strcpy(asiOut->Accountname, (sint8*)dbRow[0]);
	asiOut->ID = atoi(dbRow[1]);
	printf("User ID: %u\n", asiOut->ID);
	asiOut->UID1 = ID1;
	asiOut->UID2 = ID2;
	return 1;
}
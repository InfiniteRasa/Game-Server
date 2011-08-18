int GameMain_Run(void *p1);

#define GAMEMAIN_STATE_UNAUTHED 0
#define GAMEMAIN_STATE_CHARACTERSELECTION 1
#define GAMEMAIN_STATE_RELIEVED	3 // instance was passed to an other handler, we have to remove it asap

typedef struct _clientGamemain_t
{
	SOCKET socket;
	CRITICAL_SECTION cs_send;
	CRITICAL_SECTION cs_general;
	unsigned char RecvBuffer[8*1024];
	unsigned int RecvState;
	unsigned int RecvSize;

	unsigned int sessionId1;
	unsigned int sessionId2;
	unsigned long long userID;
	
	char Accountname[20];
	TABULACRYPT2 tbc2;

	unsigned int state_bytesRecv;
	unsigned int state_bytesSend;


	int State;
	// set by gameMain, used by mapChannel
	int mapLoadContextId; // map to load
	int mapLoadSlotId; // choosen character
	// helpers
	pyMarshalString_t pyms;
	pyUnmarshalString_t pyums;
}clientGamemain_t;

void GameMain_PassClient(SOCKET s, TABULACRYPT2 *tbc2);
void GameMain_PassClientToCharacterSelection(clientGamemain_t *cgm);

int findSubsize(int current, unsigned char *data);

clientGamemain_t* GameMain_isolate(clientGamemain_t *cgm);
int findSubsize(int current, unsigned char *data);

// debugging
void HexOut(unsigned char *hex, int l);
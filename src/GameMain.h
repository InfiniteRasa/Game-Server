sint32 GameMain_Run(void *p1);

#define GAMEMAIN_STATE_UNAUTHED 0
#define GAMEMAIN_STATE_CHARACTERSELECTION 1
#define GAMEMAIN_STATE_RELIEVED	3 // instance was passed to an other handler, we have to remove it asap

typedef struct _clientGamemain_t
{
	SOCKET socket;
	TMutex cs_send;
	TMutex cs_general;
	uint8 RecvBuffer[8*1024];
	uint32 RecvState;
	uint32 RecvSize;

	uint32 sessionId1;
	uint32 sessionId2;
	unsigned long long userID;
	
	sint8 Accountname[20];
	TABULACRYPT2 tbc2;

	uint32 state_bytesRecv;
	uint32 state_bytesSend;


	sint32 State;
	// set by gameMain, used by mapChannel
	sint32 mapLoadContextId; // map to load
	sint32 mapLoadSlotId; // choosen character
	// helpers
	pyMarshalString_t pyms;
	pyUnmarshalString_t pyums;
}clientGamemain_t;

void GameMain_PassClient(SOCKET s, TABULACRYPT2 *tbc2);
void GameMain_PassClientToCharacterSelection(clientGamemain_t *cgm);

sint32 findSubsize(sint32 current, uint8 *data);

clientGamemain_t* GameMain_isolate(clientGamemain_t *cgm);
sint32 findSubsize(sint32 current, uint8 *data);

// debugging
void HexOut(uint8 *hex, sint32 l);
#include "Global.h"

CRITICAL_SECTION CS_CMgr;


#define MAX_GAMEMAIN_CLIENTS 512

clientGamemain_t Clients_GameMain[MAX_GAMEMAIN_CLIENTS];
unsigned int ClientsGameMainCount = 0;

clientGamemain_t *ClientMgr_AddToGameMain(SOCKET s)
{
	if( ClientsGameMainCount >= MAX_GAMEMAIN_CLIENTS )
		return 0;
	EnterCriticalSection(&CS_CMgr);
	//Reset struct
	memset((void*)&Clients_GameMain[ClientsGameMainCount], 0x00, sizeof(clientGamemain_t));
	//Set socket
	Clients_GameMain[ClientsGameMainCount].socket = s;
	// init critical sections
	InitializeCriticalSection(&Clients_GameMain[ClientsGameMainCount].cs_send);
	InitializeCriticalSection(&Clients_GameMain[ClientsGameMainCount].cs_general);
	//Increase count and return struct
	ClientsGameMainCount++;
	clientGamemain_t *rs = &Clients_GameMain[ClientsGameMainCount-1];
	LeaveCriticalSection(&CS_CMgr);
	return rs;
}

int _ClientMgr_GetIndexGameMain(SOCKET s)
{
	for(int i=0; i<ClientsGameMainCount; i++)
		if( Clients_GameMain[i].socket == s )
			return i;
	return -1;
}


void ClientMgr_RemoveFromGameMain(SOCKET s)
{
	EnterCriticalSection(&CS_CMgr);
	int Idx = _ClientMgr_GetIndexGameMain(s);
	if( Idx == -1 )
	{
		LeaveCriticalSection(&CS_CMgr);
		return; //No existing element
	}
	if( (Idx+1) >= ClientsGameMainCount )
	{
		//Last element - just decrease element count
		ClientsGameMainCount--;
	}
	else
	{
		//Element anywhere in list, so copy last element to new space and decrease element count
		memcpy((void*)&Clients_GameMain[Idx], (void*)&Clients_GameMain[ClientsGameMainCount-1], sizeof(clientGamemain_t));
		ClientsGameMainCount--;
	}
	LeaveCriticalSection(&CS_CMgr);
}

// remove but do not disconnect! must only be called by the GameMain thread
clientGamemain_t* GameMain_isolate(clientGamemain_t *cgm)
{
	// copy the client structure first
	clientGamemain_t *clientMain = (clientGamemain_t*)malloc(sizeof(clientGamemain_t));
	memcpy((void*)clientMain, (void*)cgm, sizeof(clientGamemain_t));
	//clientMain->RecvState = 0;
	//cgm->State = GAMEMAIN_STATE_RELIEVED;
	return clientMain;
}

void ClientMgr_AddGameMainsToFD(FD_SET *fd)
{
	for(int i=0; i<ClientsGameMainCount; i++)
		FD_SET(Clients_GameMain[i].socket, fd);
}

void ClientMgr_CallbackEventGameMain(int (*cb)(clientGamemain_t *cp), fd_set *fd)
{
	int idx = 0;
	while(idx < ClientsGameMainCount)
	{
		if( FD_ISSET(Clients_GameMain[idx].socket, fd) )
		{
			if( cb(&Clients_GameMain[idx]) )
			{	
				if( Clients_GameMain[idx].State == GAMEMAIN_STATE_RELIEVED )
				{
					// pass to mapChannel
					clientGamemain_t *newCgm = GameMain_isolate(&Clients_GameMain[idx]);
					mapChannel_t *mapChannel = mapChannel_findByContextId(newCgm->mapLoadContextId);
					if( mapChannel == NULL )
					{
						// mapChannel not live, disconnect the client!
						// todo: send him back to char selection and show error
						closesocket(newCgm->socket);
						free(newCgm);
					}
					else
						mapChannel_pass(mapChannel, newCgm); // todo: this function can fail, add proper handling	
					ClientMgr_RemoveFromGameMain(Clients_GameMain[idx].socket);
				}
				else
					idx++;
			}
			else
			{
				// error occured		
				ClientMgr_RemoveFromGameMain(Clients_GameMain[idx].socket), printf("ElementGameMain deleted\n"); //If cb fails - remove element
			}
			

		}
		else
			idx++;
	};
}

/************** Prototypes ***************/

int GameMain_ReadCallback(clientGamemain_t *cp);

int GameMain_DecodePacket(clientGamemain_t *cgm, unsigned char *data, unsigned int len);
//int GameMain_UnpackPacket(clientGamemain_t *cgm, unsigned char *data, unsigned int len);
//
//int GameMain_UnpackPacket_OP_2(clientGamemain_t *cgm, VALUESTRUCT1 *vs1);

int GameMain_processPythonRPC(clientGamemain_t *cgm, unsigned int methodID, unsigned char *pyString, int pyStringLen);

/***************** Main ******************/
int GameMain_Run(void *p1)
{
	InitializeCriticalSection(&CS_CMgr);

	//Main socket handler loop
	FD_SET fd;
	timeval sTimeout;
	sTimeout.tv_sec = 1;
	sTimeout.tv_usec = 0;
	while(1)
	{
		FD_ZERO(&fd);
		//No server socket
		//Add all connected clients/gameservers
		ClientMgr_AddGameMainsToFD(&fd);
		//Do work select
		int r = select(0, &fd, 0, 0, &sTimeout); //Dont wait forever, we need to check Gameserver online states too
		if( r )
		{
			//Check for all receivers
			ClientMgr_CallbackEventGameMain(GameMain_ReadCallback, &fd);
		}
		else
			Sleep(1);
		Sleep(1);
	};



	return 0;
}

void GameMain_PassClient(SOCKET s, TABULACRYPT2 *tbc2)
{
	clientGamemain_t *cgm = ClientMgr_AddToGameMain(s);
	if( cgm == 0 )
		__debugbreak(); //No more free slot

	memcpy((void*)&cgm->tbc2, (void*)tbc2, sizeof(TABULACRYPT2));
}

void GameMain_PassClientToCharacterSelection(clientGamemain_t *cgm)
{
	if( ClientsGameMainCount >= MAX_GAMEMAIN_CLIENTS )
	{
		// force disconnect
		closesocket(cgm->socket);
		//free(cgm);
		return;
	}
	EnterCriticalSection(&CS_CMgr);
	//Reset struct
	memcpy((void*)&Clients_GameMain[ClientsGameMainCount], (void*)cgm, sizeof(clientGamemain_t));
	// set state
	Clients_GameMain[ClientsGameMainCount].State = GAMEMAIN_STATE_CHARACTERSELECTION;
	// begin character selection
	charMgr_initCharacterSelection(&Clients_GameMain[ClientsGameMainCount]);
	//Increase count and return struct
	ClientsGameMainCount++;
	LeaveCriticalSection(&CS_CMgr);
}

int GameMain_ReadCallback(clientGamemain_t *cgm)
{
	if( cgm->RecvState < 4 )
	{
		int r = recv(cgm->socket, (char*)cgm->RecvBuffer+cgm->RecvState, 4-cgm->RecvState, 0);
		if( r == 0 || r == SOCKET_ERROR )
			return 0;
		cgm->RecvState += r;
		if( cgm->RecvState == 4 )
			cgm->RecvSize = *(unsigned int*)cgm->RecvBuffer + 4;
		return 1;
	}
	int r = recv(cgm->socket, (char*)cgm->RecvBuffer+cgm->RecvState, cgm->RecvSize-cgm->RecvState, 0);
	if( r == 0 || r == SOCKET_ERROR )
		return 0;
	cgm->RecvState += r;
	
	if( cgm->RecvState == cgm->RecvSize )
	{
		//Full packet received
		//Everything is encrypted, so do decryption job here
		
		if (cgm->RecvSize==852)
		{
			cgm->RecvState = 0;
			return 1;
		}
		

		Tabula_Decrypt2(&cgm->tbc2, (unsigned int*)(cgm->RecvBuffer+4), cgm->RecvSize);		
		int r = 0;
		int AlignBytes = cgm->RecvBuffer[4]%9;

		unsigned char *Buffer = cgm->RecvBuffer + 4 + AlignBytes;
		int Size = cgm->RecvSize - 4 - AlignBytes;
		do{
			unsigned short Subsize = *(unsigned short*)Buffer;
			// 20110729 - thuvvik if/else to avoid gamecrash
			if (Subsize == 43 && Size == 12)
			{
				r = 1;
			}
			else
			{
				if (Subsize >4000)
				{
					int foundSubsize = findSubsize(Subsize, Buffer);
					Subsize= foundSubsize;
				}
				

				r = GameMain_DecodePacket(cgm, Buffer, Subsize);
			}

			if( r == 0 )
				return 0;
			Buffer += Subsize;
			Size -= Subsize;
		}while(Size > 0);
		cgm->RecvState = 0;
		return r;
	}
	return 1;
}

int findSubsize(int current, unsigned char *data)
{
	int iIndex = 0;
	int zeroFound = 0;
	for (iIndex =0; iIndex < current; iIndex++)
	{
		if ((*(unsigned short*)(data+iIndex)) == 0)
			zeroFound ++;

		if (zeroFound==2)
		{
			if ((iIndex -2) < 0)
				printf("findSubsize::HOLYSHIT !!!\n");
			return iIndex -2;
		}
	}
}

/***Debug***/
void HexOut(unsigned char *hex, int l)
{
	for(int p=0; p<l; p++)
	{
		printf("%02X ", hex[p]);
		if( ((p+1)&0xF) == 0 )
			printf("\n");
	}
	printf("\n");
	for(int p=0; p<l; p++)
	{
		if( hex[p] > 0x10 )
			printf("%c", hex[p]);
		else
			printf("%c", '.');
		if( ((p+1)&0xF) == 0 )
			printf("\n");
	}
}

int GameMain_DecodePacket(clientGamemain_t *cgm, unsigned char *data, unsigned int len)
{
	if( len >= 0xFFFF )
		__debugbreak();
	if( len < 4 )
		return 0;
	//printf("Packet - Size: %d\n", len);
	//HexOut(data, len);
	//printf("\n\n");

	int pIdx = 0;
	// read subSize
	unsigned int subSize = *(unsigned short*)(data+pIdx); pIdx += 2; // redundancy with param len
	// read major opcode
	unsigned int majorOpc = *(unsigned short*)(data+pIdx); pIdx += 2;
	if( majorOpc )
	{
		return 1; // ignore the packet
	}
	// read header A
	unsigned char ukn1 = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( ukn1 != 2 )
		__debugbreak();
	
	unsigned char opcode = *(unsigned char*)(data+pIdx); pIdx +=1; // not 100% sure	
	unsigned char ukn2 = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( ukn2 != 0 )
		__debugbreak();
	unsigned char xorCheckA = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( xorCheckA != 3 ) // we only know headerA length of 3 for now
		__debugbreak();

	unsigned int hdrB_start = pIdx;
	unsigned char ukn3 = *(unsigned char*)(data+pIdx); pIdx +=1;
	if( ukn3 != 3 )
		__debugbreak();
	// different handling now
	if( opcode == 0x02 )
	{
		// expect header B part 1 (0x29)
		if( *(unsigned char*)(data+pIdx) != 0x29 )
			__debugbreak(); // wrong
		pIdx++;
		// most likely the register-me-packet
		unsigned char ukn02_1 = *(unsigned char*)(data+pIdx); pIdx++;
		if( ukn02_1 != 3 ) __debugbreak();
		unsigned char ukn02_2 = *(unsigned char*)(data+pIdx); pIdx++;
		if( ukn02_2 != 1 ) __debugbreak();
		unsigned char preffix02_1 = *(unsigned char*)(data+pIdx); pIdx++;
		if( preffix02_1 != 7 ) __debugbreak(); // 7 --> 32-bit int
		unsigned int sessionId2 = *(unsigned int*)(data+pIdx); pIdx += 4;
		unsigned char preffix02_2 = *(unsigned char*)(data+pIdx); pIdx++;
		if( preffix02_2 != 7 ) __debugbreak(); // 7 --> 32-bit int
		unsigned int sessionId1 = *(unsigned int*)(data+pIdx); pIdx += 4;
		unsigned char ukn02_3 = *(unsigned char*)(data+pIdx); pIdx++;
		if( ukn02_3 != 0xD ) __debugbreak();
		// part 2 (0xCB)
		if( *(unsigned char*)(data+pIdx) != 0xCB )
			__debugbreak(); // wrong
		pIdx++;
		// read version
		unsigned int versionLen = *(unsigned char*)(data+pIdx); pIdx++;
		bool wrongVersion = false;
		if( versionLen != 8 )
			wrongVersion = true;
		if( wrongVersion == false )
		{
			if( memcmp(data+pIdx, "1.16.5.0", versionLen) ) // 1.11.6.0 - 1.16.5.0
				wrongVersion = true;
		}
		pIdx += versionLen;
		
		if( wrongVersion ) { printf("Client version missmatch\n"); }
			// return 0;//__debugbreak(); // shit has wrong version
		
		unsigned char ukn02_4 = *(unsigned char*)(data+pIdx); pIdx++;
		if( ukn02_4 != 0x2A )
			return 0;//__debugbreak();

		authSessionInfo_t asi;
		if( !dataInterface_QuerySession(sessionId1, sessionId2, &asi) )
		{
			closesocket(cgm->socket);
			return 0;
		}
		cgm->State = 1;

		strcpy(cgm->Accountname, asi.Accountname);
		cgm->userID = asi.ID;
		cgm->sessionId1 = sessionId1;
		cgm->sessionId2 = sessionId2;

		//Begin char selection
		charMgr_initCharacterSelection(cgm);

		return 1;
	}
	else if( opcode == 0x0C )
	{
		// expect header B part 1 (0x29)
		if( *(unsigned char*)(data+pIdx) == 0x00 )
			return 1; // empty packet?
		if( *(unsigned char*)(data+pIdx) != 0x29 )
			__debugbreak(); // wrong
		pIdx++;
		unsigned char ukn0C_1 = *(unsigned char*)(data+pIdx); pIdx++;
		if( ukn0C_1 != 3 ) __debugbreak();
		unsigned char ukn0C_2 = *(unsigned char*)(data+pIdx); pIdx++;
		//if( ukn0C_2 != 1 && ukn0C_2 != 3 && ukn0C_2 != 9 ) __debugbdfdsfreak(); // server entityId?
		if( ukn0C_2 == 0 || ukn0C_2 > 0x10 ) __debugbreak(); // server entityId?
		unsigned char preffix0C_1 = *(unsigned char*)(data+pIdx); pIdx++;
		if( preffix0C_1 != 7 ) __debugbreak(); // 7 --> 32-bit int
		unsigned int methodID = *(unsigned int*)(data+pIdx); pIdx += 4;
		unsigned char ukn0C_3 = *(unsigned char*)(data+pIdx); pIdx++; // entityID?
		if( ukn0C_3 != 1 ) __debugbreak();
		// part 2 (0xCB)
		if( *(unsigned char*)(data+pIdx) != 0xCB )
			__debugbreak(); // wrong
		pIdx++;
		unsigned int dataLen = 0;
		unsigned int lenMask = *(unsigned char*)(data+pIdx); pIdx++;
		if( (lenMask>>6) == 0 )
		{
			// 6 bit length
			dataLen = lenMask&0x3F;
		}
		else if( (lenMask>>6) == 1 )
		{
			// 14 bit length
			dataLen = (lenMask&0x3F);
			dataLen |= ((*(unsigned char*)(data+pIdx))<<6);
			pIdx++;
		}
		else
			__debugbreak();
		if (dataLen >0)
			GameMain_processPythonRPC(cgm, methodID, data+pIdx, dataLen);
		pIdx += dataLen;
		// xor check...

		
	}
	else
		return 1;
	return 1;
}

int GameMain_processPythonRPC(clientGamemain_t *cgm, unsigned int methodID, unsigned char *pyString, int pyStringLen)
{
	// check if 'O'
	if( *pyString != 'O' )
		__debugbreak(); // oh shit...
	pyString++; pyStringLen--;

	switch( methodID )
	{
	case 149: // RequestCharacterName
		return charMgr_recv_requestCharacterName(cgm, pyString, pyStringLen);
	case 516: // RequestFamilyName
		return charMgr_recv_requestFamilyName(cgm, pyString, pyStringLen);
	case 512: // RequestCreateCharacterInSlot
		return charMgr_recv_requestCreateCharacterInSlot(cgm, pyString, pyStringLen);
	case 513: // RequestDeleteCharacterInSlot
		return charMgr_recv_requestDeleteCharacterInSlot(cgm, pyString, pyStringLen);
	case 523: // RequestSwitchToCharacterInSlot
		return charMgr_recv_requestSwitchToCharacterInSlot(cgm, pyString, pyStringLen);
	default:
		return 1;
		// no handler for that
		// StoreUserClientInformation = 775 From ./generated/client/methodid.pyo
	};
 	
	// 149
	// 00001AA7     64 - LOAD_CONST          'RequestCharacterName'
	

	return 1;
}
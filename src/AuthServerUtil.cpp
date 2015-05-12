#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Winsock2.h>
#include <Windows.h>

#include"framework.h"
#include"AuthServerUtil.h"

#define GS_FLAG_AUTOCHECK			1
#define GS_FLAG_SERVERDOESCHECK		2
#define GS_FLAG_ONLINE				4

uint32 AuthServer_selectedIP = 0;
sint8 AuthServer_IP[] = "127.0.0.1";
sint8 AuthServer_AlternativeIP[] = "127.0.0.1"; // used if we cant connect to first IP
uint16 AuthServer_Port = 9887;
sint8 AuthServer_Password[] = "Hurray";
sint8 AuthServer_Name[] = "NoUseName";

uint8 LocalID = 0x35;
uint8 LocalFlag = GS_FLAG_ONLINE;
uint32 LocalIP = 0x0100007F;
uint16 LocalPORT = 8001;


void GSMgr_GenerateSecretKey(sint8 *pw, uint8 *SecretKey)
{
	memset((void*)SecretKey, 0x00, 10);
	sint32 l = strlen(pw);
	//pw input
	for(sint32 i=0; i<l; i++)
		SecretKey[i%10] += pw[i];
	for(sint32 i=0; i<l; i++)
		SecretKey[(i+3)%10] ^= ((pw[i]<<1) | (pw[i]>>7));
	for(sint32 i=0; i<l; i++)
		SecretKey[(i+5)%10] ^= ~((pw[l-1-i]<<3) | (pw[l-1-i]>>5));
	//Some scrambling
	for(sint32 i=0; i<10; i++)
		SecretKey[i] = (SecretKey[i]>>4) | (SecretKey[i]<<4);
	//Some magic xoring
	for(sint32 i=0; i<10; i++)
		SecretKey[i] = SecretKey[i] ^ ((i^4+i+i*34+(i>>3)*45)&0xFF);
}

sint32 AuthServerUtil_Register()
{
	SOCKET s;
	SOCKADDR_IN addr;
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(AuthServer_Port);
	addr.sin_addr.s_addr=inet_addr(AuthServer_IP); 
	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	sint32 rc=connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR));
	if(rc==SOCKET_ERROR)
	{
		addr.sin_addr.s_addr=inet_addr(AuthServer_AlternativeIP);
		rc=connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR));
		if(rc==SOCKET_ERROR)
		{
			//Error - Very bad, that means we cannot connect to the AuthServer
			return 0; //Ignore it for now
		}
	}
	//Build and send register packet
	uint8 Packet[0x80];
	Packet[0] = 0xF1; //Opcode
	GSMgr_GenerateSecretKey(AuthServer_Password, (Packet+1)); //Secret key
	Packet[11] = LocalID; //ID
	*(uint32*)&Packet[12] = LocalIP; //IP
	*(uint16*)&Packet[16] = LocalPORT; //Port
	*(uint16*)&Packet[18] = LocalFlag; //Flags
	//Name
	memset((void*)&Packet[19], 0x00, 32);
	strcpy((sint8*)&Packet[19], AuthServer_Name);
	//Send
	send(s, (sint8*)Packet, 0x33, 0);
	//Recv answer
	uint8 RecvBuffer[2];
	recv(s, (sint8*)RecvBuffer, 2, 0);
	closesocket(s);
	if( RecvBuffer[0] != 0xF1 )
		return 0;
	if( RecvBuffer[1] != 0x00 )
		return 0;
	AuthServer_selectedIP = addr.sin_addr.s_addr;
	return 1;
}

sint32 AuthServerUtil_QuerySession(uint32 ID1, uint32 ID2, authSessionInfo_t *asiOut)
{
	SOCKET s;
	SOCKADDR_IN addr;
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(AuthServer_Port);
	addr.sin_addr.s_addr=AuthServer_selectedIP; 
	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	sint32 rc=connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR));

	if(rc==SOCKET_ERROR)
	{
		//Error - Very bad, that means we cannot connect to the AuthServer
		return 0; //Ignore it for now
	}

	//Build and send session-query packet
	uint8 Packet[0x80];
	Packet[0] = 0x34; //Opcode
	GSMgr_GenerateSecretKey(AuthServer_Password, (Packet+1)); //Secret key
	*(uint32*)&Packet[11] = ID1; //Session ID1
	*(uint32*)&Packet[15] = ID2; //Session ID2
	//Send
	send(s, (sint8*)Packet, 19, 0);

	//Receive opcode
	uint8 ROp;
	recv(s, (sint8*)&ROp, 1, 0);
	if( ROp != 0x34 && ROp != 0x35 )
		return 0;
	//If error return failure
	if( ROp == 0x35 )
		return 0;
	//If success read full packet
	uint8 rv[23];
	sint32 r = recv(s, (sint8*)rv, 22, 0);
	if( r != 22 )
		__debugbreak();
	strcpy(asiOut->Accountname, (sint8*)(rv)); //We will overcopy one byte to much for 14-byte names, but we dont have to care
	asiOut->UID1 = *(uint32*)(rv+14);
	asiOut->UID2 = *(uint32*)(rv+18);
	//Return success
	return 1;
}

uint32 AuthServerUtil_GetMyIP()
{
	return LocalIP;
}
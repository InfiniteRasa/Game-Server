#include <Winsock2.h>
#include <Windows.h>

#include "AuthServerUtil.h"

#define GS_FLAG_AUTOCHECK			1
#define GS_FLAG_SERVERDOESCHECK		2
#define GS_FLAG_ONLINE				4

unsigned int AuthServer_selectedIP = 0;
char AuthServer_IP[] = "127.0.0.1";
char AuthServer_AlternativeIP[] = "127.0.0.1"; // used if we cant connect to first IP
unsigned short AuthServer_Port = 9887;
char AuthServer_Password[] = "Hurray";
char AuthServer_Name[] = "NoUseName";

unsigned char LocalID = 0x35;
unsigned char LocalFlag = GS_FLAG_ONLINE;
unsigned int LocalIP = 0x0100007F;
unsigned short LocalPORT = 8001;


void GSMgr_GenerateSecretKey(char *pw, unsigned char *SecretKey)
{
	memset((void*)SecretKey, 0x00, 10);
	int l = strlen(pw);
	//pw input
	for(int i=0; i<l; i++)
		SecretKey[i%10] += pw[i];
	for(int i=0; i<l; i++)
		SecretKey[(i+3)%10] ^= ((pw[i]<<1) | (pw[i]>>7));
	for(int i=0; i<l; i++)
		SecretKey[(i+5)%10] ^= ~((pw[l-1-i]<<3) | (pw[l-1-i]>>5));
	//Some scrambling
	for(int i=0; i<10; i++)
		SecretKey[i] = (SecretKey[i]>>4) | (SecretKey[i]<<4);
	//Some magic xoring
	for(int i=0; i<10; i++)
		SecretKey[i] = SecretKey[i] ^ ((i^4+i+i*34+(i>>3)*45)&0xFF);
}

int AuthServerUtil_Register()
{
	SOCKET s;
	SOCKADDR_IN addr;
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(AuthServer_Port);
	addr.sin_addr.s_addr=inet_addr(AuthServer_IP); 
	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int rc=connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR));
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
	unsigned char Packet[0x80];
	Packet[0] = 0xF1; //Opcode
	GSMgr_GenerateSecretKey(AuthServer_Password, (Packet+1)); //Secret key
	Packet[11] = LocalID; //ID
	*(unsigned int*)&Packet[12] = LocalIP; //IP
	*(unsigned short*)&Packet[16] = LocalPORT; //Port
	*(unsigned short*)&Packet[18] = LocalFlag; //Flags
	//Name
	memset((void*)&Packet[19], 0x00, 32);
	strcpy((char*)&Packet[19], AuthServer_Name);
	//Send
	send(s, (char*)Packet, 0x33, 0);
	//Recv answer
	unsigned char RecvBuffer[2];
	recv(s, (char*)RecvBuffer, 2, 0);
	closesocket(s);
	if( RecvBuffer[0] != 0xF1 )
		return 0;
	if( RecvBuffer[1] != 0x00 )
		return 0;
	AuthServer_selectedIP = addr.sin_addr.s_addr;
	return 1;
}

int AuthServerUtil_QuerySession(unsigned int ID1, unsigned int ID2, authSessionInfo_t *asiOut)
{
	SOCKET s;
	SOCKADDR_IN addr;
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(AuthServer_Port);
	addr.sin_addr.s_addr=AuthServer_selectedIP; 
	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int rc=connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR));

	if(rc==SOCKET_ERROR)
	{
		//Error - Very bad, that means we cannot connect to the AuthServer
		return 0; //Ignore it for now
	}

	//Build and send session-query packet
	unsigned char Packet[0x80];
	Packet[0] = 0x34; //Opcode
	GSMgr_GenerateSecretKey(AuthServer_Password, (Packet+1)); //Secret key
	*(unsigned int*)&Packet[11] = ID1; //Session ID1
	*(unsigned int*)&Packet[15] = ID2; //Session ID2
	//Send
	send(s, (char*)Packet, 19, 0);

	//Receive opcode
	unsigned char ROp;
	recv(s, (char*)&ROp, 1, 0);
	if( ROp != 0x34 && ROp != 0x35 )
		return 0;
	//If error return failure
	if( ROp == 0x35 )
		return 0;
	//If success read full packet
	unsigned char rv[23];
	int r = recv(s, (char*)rv, 22, 0);
	if( r != 22 )
		__debugbreak();
	strcpy(asiOut->Accountname, (char*)(rv)); //We will overcopy one byte to much for 14-byte names, but we dont have to care
	asiOut->UID1 = *(unsigned int*)(rv+14);
	asiOut->UID2 = *(unsigned int*)(rv+18);
	//Return success
	return 1;
}

unsigned int AuthServerUtil_GetMyIP()
{
	return LocalIP;
}
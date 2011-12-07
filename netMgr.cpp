#include "Global.h"


typedef struct _packetBuffer_t
{
	unsigned char PO_SendBuffer[1024*8];
	unsigned int PO_CLen;
}packetBuffer_t;

void PacketOut_ResetGame(packetBuffer_t *pb)
{
	pb->PO_CLen = 4;
}

void PacketOut_AddByte(packetBuffer_t *pb, unsigned char B)
{
	pb->PO_SendBuffer[pb->PO_CLen] = B;
	pb->PO_CLen++;
}

void PacketOut_AddWord(packetBuffer_t *pb, unsigned int D)
{
	*(unsigned short*)&(pb->PO_SendBuffer[pb->PO_CLen]) = D;
	pb->PO_CLen += 2;
}

void PacketOut_AddDword(packetBuffer_t *pb, unsigned int D)
{
	*(unsigned int*)&(pb->PO_SendBuffer[pb->PO_CLen]) = D;
	pb->PO_CLen += 4;
}

void PacketOut_AddMem(packetBuffer_t *pb, unsigned char *d, int len) //Can be optimized
{
	for(int i=0; i<len; i++)
		PacketOut_AddByte(pb, d[i]);
}

void PacketOut_Send(packetBuffer_t *pb, SOCKET s)
{
	//Set 4-byte len
	*(unsigned int*)pb->PO_SendBuffer = pb->PO_CLen - 4;
	//Send
	send(s, (char*)pb->PO_SendBuffer, pb->PO_CLen, 0);
}


//packetBuffer_t SPB;

#define GEN_XOR_BYTE(x) ((x&0xFF)^((x>>8)&0xFF)^((x>>16)&0xFF)^((x>>24)&0xFF))

void netMgr_pythonAddMethodCallRaw(clientGamemain_t *cgm, unsigned int entityId, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen)
{
	packetBuffer_t SPB;
	PacketOut_ResetGame(&SPB);
	//Align bytes [4]
	PacketOut_AddByte(&SPB, 0x01);
	int LenBegin = SPB.PO_CLen;
	//Subsize [2]
	PacketOut_AddWord(&SPB, 41);
	//Opcode [2]
	PacketOut_AddByte(&SPB, 0x00);//Packet 0 - if you specify something other than 0(except 0xFF) there will be some additional preleading data?
	PacketOut_AddByte(&SPB, 1);

	int XORCheck = SPB.PO_CLen;
	//datablock 1 (header)
	PacketOut_AddByte(&SPB, 0); //2:6 mask
	PacketOut_AddByte(&SPB, (7<<1) | (0)); //Opcode and flag --> 7 means the main packet handler
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Checksum 1 - after this we also reset checksum counter
	//data block 2 (header 2)
	PacketOut_AddByte(&SPB, 0x00); //Compression
	//OPD1:
	PacketOut_AddByte(&SPB, 0x01); //Selection table for opcode 7 --> 00782470   . 83EC 34        SUB ESP,34
	//A80 - EntityID
	if( entityId == 0 )
		PacketOut_AddByte(&SPB, 0);
	else
		while( entityId ) // todo: unroll loop for better performance?
		{
			if( entityId&~0x7F )
				PacketOut_AddByte(&SPB, (entityId&0x7F)|0x80);
			else
				PacketOut_AddByte(&SPB, entityId&0x7F);
			entityId >>= 7;
		}
	//B80 - MethodID
	if( MethodID <= 0x7F )
	{
		PacketOut_AddByte(&SPB, MethodID);
	}
	else if( MethodID <= 0x3FFF )
	{
		PacketOut_AddByte(&SPB, 0x80 | (MethodID&0x7F));
		PacketOut_AddByte(&SPB, MethodID>>7);	
	}
	else if( MethodID <= 0x1FFFFF )
	{
		PacketOut_AddByte(&SPB, 0x80 | (MethodID&0x7F)); 
		PacketOut_AddByte(&SPB, 0x80 | ((MethodID>>7)&0x7F)); 
		PacketOut_AddByte(&SPB, MethodID>>14);			
	}
	else if( MethodID <= 0xFFFFFFF )
	{
		PacketOut_AddByte(&SPB, 0x80 | (MethodID&0x7F)); 
		PacketOut_AddByte(&SPB, 0x80 | ((MethodID>>7)&0x7F)); 
		PacketOut_AddByte(&SPB, 0x80 | ((MethodID>>14)&0x7F)); 
		PacketOut_AddByte(&SPB, MethodID>>21);			
	}
	else
		__debugbreak();

	//Parameter block:
	//PacketOut_AddByte(&SPB, 0x01);
	//Masklike size - we always specify two bytes!
	PacketOut_AddByte(&SPB, 0xFF); //Size of parameter block
	PacketOut_AddByte(&SPB, 0xFF); //Size of parameter block
	int ParamBlockBegin = SPB.PO_CLen; 
	PacketOut_AddByte(&SPB, 'M');
	PacketOut_AddDword(&SPB, pyObjLen);
	PacketOut_AddMem(&SPB, pyObjString, pyObjLen);
	PacketOut_AddByte(&SPB, 0x66);

	int PBLen = SPB.PO_CLen - ParamBlockBegin;
	if( PBLen > 0xFFF )
		__debugbreak();



	SPB.PO_SendBuffer[ParamBlockBegin-2] = (PBLen&0x3F) | 0x40;
	SPB.PO_SendBuffer[ParamBlockBegin-1] = PBLen>>6;
	//Follower
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Mask is skipped and we have checksum here?
	//Last checksum:
	PacketOut_AddByte(&SPB, 0x06); //Checksum 2
	PacketOut_AddByte(&SPB, 0x06); //Checksum 2
	//OPD3:
	//Not given

	//Align len to 8 (-1)
	int LenNow = SPB.PO_CLen - LenBegin;
	int PaddingNeeded = (8-((LenNow+1)%8))%8; //+1 because of fixed align bytes at the top
	for(int x=0; x<PaddingNeeded; x++)
		PacketOut_AddByte(&SPB, '?');
	//Write len
	*(unsigned short*)&SPB.PO_SendBuffer[LenBegin] = SPB.PO_CLen - LenBegin;

	EnterCriticalSection(&cgm->cs_send);
	Tabula_Encrypt2(&cgm->tbc2, (unsigned int*)(&SPB.PO_SendBuffer[4]), SPB.PO_CLen-4);
	PacketOut_Send(&SPB, cgm->socket);
	cgm->state_bytesSend += SPB.PO_CLen;
	LeaveCriticalSection(&cgm->cs_send);
}

void netMgr_pythonAddMethodCallRaw(mapChannelClient_t **clientList, int clientCount, unsigned int entityId, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen)
{
	packetBuffer_t SPB;
	PacketOut_ResetGame(&SPB);
	//Align bytes [4]
	PacketOut_AddByte(&SPB, 0x01);
	int LenBegin = SPB.PO_CLen;
	//Subsize [2]
	PacketOut_AddWord(&SPB, 41);
	//Opcode [2]
	PacketOut_AddByte(&SPB, 0x00);//Packet 0 - if you specify something other than 0(except 0xFF) there will be some additional preleading data?
	PacketOut_AddByte(&SPB, 1);

	int XORCheck = SPB.PO_CLen;
	//datablock 1 (header)
	PacketOut_AddByte(&SPB, 0); //2:6 mask
	PacketOut_AddByte(&SPB, (7<<1) | (0)); //Opcode and flag --> 7 means the main packet handler
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Checksum 1 - after this we also reset checksum counter
	//data block 2 (header 2)
	PacketOut_AddByte(&SPB, 0x00); //Compression
	//OPD1:
	PacketOut_AddByte(&SPB, 0x01); //Selection table for opcode 7 --> 00782470   . 83EC 34        SUB ESP,34
	//A80 - EntityID
	if( entityId == 0 )
		PacketOut_AddByte(&SPB, 0);
	else
		while( entityId ) // todo: unroll loop for better performance?
		{
			if( entityId&~0x7F )
				PacketOut_AddByte(&SPB, (entityId&0x7F)|0x80);
			else
				PacketOut_AddByte(&SPB, entityId&0x7F);
			entityId >>= 7;
		}
		//B80 - MethodID
		if( MethodID <= 0x7F )
		{
			PacketOut_AddByte(&SPB, MethodID);
		}
		else if( MethodID <= 0x3FFF )
		{
			PacketOut_AddByte(&SPB, 0x80 | (MethodID&0x7F));
			PacketOut_AddByte(&SPB, MethodID>>7);	
		}
		else if( MethodID <= 0x1FFFFF )
		{
			PacketOut_AddByte(&SPB, 0x80 | (MethodID&0x7F)); 
			PacketOut_AddByte(&SPB, 0x80 | ((MethodID>>7)&0x7F)); 
			PacketOut_AddByte(&SPB, MethodID>>14);			
		}
		else if( MethodID <= 0xFFFFFFF )
		{
			PacketOut_AddByte(&SPB, 0x80 | (MethodID&0x7F)); 
			PacketOut_AddByte(&SPB, 0x80 | ((MethodID>>7)&0x7F)); 
			PacketOut_AddByte(&SPB, 0x80 | ((MethodID>>14)&0x7F)); 
			PacketOut_AddByte(&SPB, MethodID>>21);			
		}
		else
			__debugbreak();

		//Parameter block:
		//PacketOut_AddByte(&SPB, 0x01);
		//Masklike size - we always specify two bytes!
		PacketOut_AddByte(&SPB, 0xFF); //Size of parameter block
		PacketOut_AddByte(&SPB, 0xFF); //Size of parameter block
		int ParamBlockBegin = SPB.PO_CLen; 
		PacketOut_AddByte(&SPB, 'M');
		PacketOut_AddDword(&SPB, pyObjLen);
		PacketOut_AddMem(&SPB, pyObjString, pyObjLen);
		PacketOut_AddByte(&SPB, 0x66);

		int PBLen = SPB.PO_CLen - ParamBlockBegin;
		if( PBLen > 0xFFF )
			__debugbreak();



		SPB.PO_SendBuffer[ParamBlockBegin-2] = (PBLen&0x3F) | 0x40;
		SPB.PO_SendBuffer[ParamBlockBegin-1] = PBLen>>6;
		//Follower
		PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Mask is skipped and we have checksum here?
		//Last checksum:
		PacketOut_AddByte(&SPB, 0x06); //Checksum 2
		PacketOut_AddByte(&SPB, 0x06); //Checksum 2
		//OPD3:
		//Not given

		//Align len to 8 (-1)
		int LenNow = SPB.PO_CLen - LenBegin;
		int PaddingNeeded = (8-((LenNow+1)%8))%8; //+1 because of fixed align bytes at the top
		for(int x=0; x<PaddingNeeded; x++)
			PacketOut_AddByte(&SPB, '?');
		//Write len
		*(unsigned short*)&SPB.PO_SendBuffer[LenBegin] = SPB.PO_CLen - LenBegin;

		for(int i=0; i<clientCount; i++)
		{
			clientGamemain_t *cgm = clientList[i]->cgm;
			EnterCriticalSection(&cgm->cs_send);
			Tabula_Encrypt2(&cgm->tbc2, (unsigned int*)(&SPB.PO_SendBuffer[4]), SPB.PO_CLen-4);
			PacketOut_Send(&SPB, cgm->socket);
			cgm->state_bytesSend += SPB.PO_CLen;
			LeaveCriticalSection(&cgm->cs_send);
		}
}


void netMgr_testOpc(clientGamemain_t *cgm)
{
	packetBuffer_t SPB;
	PacketOut_ResetGame(&SPB);
	//Align bytes [4]
	PacketOut_AddByte(&SPB, 0x01);
	int LenBegin = SPB.PO_CLen;
	//Subsize [2]
	PacketOut_AddWord(&SPB, 41);
	//Opcode [2]
	PacketOut_AddByte(&SPB, 0x00);//Packet 0 - if you specify something other than 0(except 0xFF) there will be some additional preleading data?
	PacketOut_AddByte(&SPB, 1);

	int XORCheck = SPB.PO_CLen;
	//datablock 1 (header)
	PacketOut_AddByte(&SPB, 0); //2:6 mask
	PacketOut_AddByte(&SPB, (5<<1) | (0)); //Opcode and flag --> 5 means the main packet handler
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Checksum 1 - after this we also reset checksum counter
	//data block 2 (header 2)
	//PacketOut_AddByte(&SPB, 0x00); //Compression

	// byte:		2:6 mask
	PacketOut_AddByte(&SPB, 0x00);
	// zByte:		? (00782840)
	PacketOut_AddByte(&SPB, 0x13);
	// gv64			? (00782840)
	PacketOut_AddByte(&SPB, 0x14);

	// xor
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck));



	//OPD1:
	//PacketOut_AddByte(&SPB, 0x01); //Selection table for opcode 7 --> 00782470   . 83EC 34        SUB ESP,34
	
		//Align len to 8 (-1)
		int LenNow = SPB.PO_CLen - LenBegin;
		int PaddingNeeded = (8-((LenNow+1)%8))%8; //+1 because of fixed align bytes at the top
		for(int x=0; x<PaddingNeeded; x++)
			PacketOut_AddByte(&SPB, '?');
		//Write len
		*(unsigned short*)&SPB.PO_SendBuffer[LenBegin] = SPB.PO_CLen - LenBegin;

		EnterCriticalSection(&cgm->cs_send);
		Tabula_Encrypt2(&cgm->tbc2, (unsigned int*)(&SPB.PO_SendBuffer[4]), SPB.PO_CLen-4);
		PacketOut_Send(&SPB, cgm->socket);
		cgm->state_bytesSend += SPB.PO_CLen;
		LeaveCriticalSection(&cgm->cs_send);
}

void netMgr_entityMovementTest(clientGamemain_t *cgm, unsigned char *pyObjString, int pyObjLen)
{
	packetBuffer_t SPB;
	PacketOut_ResetGame(&SPB);
	//Align bytes [4]
	PacketOut_AddByte(&SPB, 0x01);
	int LenBegin = SPB.PO_CLen;
	//Subsize [2]
	PacketOut_AddWord(&SPB, 41);
	//Opcode [2]
	PacketOut_AddByte(&SPB, 0x00); //Packet 0 - if you specify something other than 0(except 0xFF) there will be some additional preleading data?
	PacketOut_AddByte(&SPB, 1);   // ?? 0x80
	int XORCheck = SPB.PO_CLen;
	//datablock 1 (header)
	PacketOut_AddByte(&SPB, 0x00); //2:6 mask
	PacketOut_AddByte(&SPB, (4<<1) | (0)); //Opcode and flag --> 4 movementHandler B
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Checksum 1 - after this we also reset checksum counter
	//data block 2 (header 2)
	PacketOut_AddByte(&SPB, 0); // unknown (subtype? size?) Subopcode? (see +0x0054629B)


	//PacketOut_AddByte(&SPB, 1); // formatA(1)
	//PacketOut_AddByte(&SPB, 3); // formatC(3)
	//PacketOut_AddByte(&SPB, 2); // formatC, unknown (transition Id?)


	PacketOut_AddByte(&SPB, 2); // formatB(2)
	// valGroup
	//PacketOut_AddByte(&SPB, 30); // formatB (entityId)
	unsigned long long entityId = 1000;
	while( entityId )
	{
		if( entityId&~0x7F )
			PacketOut_AddByte(&SPB, (entityId&0x7F)|0x80);
		else
			PacketOut_AddByte(&SPB, entityId&0x7F);
		entityId >>= 7;
	}



	// formatA
	PacketOut_AddByte(&SPB, 0x00); // unknown
	// posX
	PacketOut_AddByte(&SPB, 0);
	PacketOut_AddByte(&SPB, 0);
	PacketOut_AddByte(&SPB, 5);
	// posY
	PacketOut_AddByte(&SPB, 0x1); // *256
	PacketOut_AddByte(&SPB, 0); // full
	PacketOut_AddByte(&SPB, 0); // 256th
	// posZ
	PacketOut_AddByte(&SPB, 0);
	PacketOut_AddByte(&SPB, 0);
	PacketOut_AddByte(&SPB, 5);
	// next (encoded-short)
	PacketOut_AddByte(&SPB, 10);
	// next (flags?)
	PacketOut_AddByte(&SPB, 0);
	// next (encoded-short)
	PacketOut_AddByte(&SPB, 11);
	// next (encoded-short)
	PacketOut_AddByte(&SPB, 12);
	// done...
	//Follower
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen;

	//Align len to 8 (-1)
	int LenNow = SPB.PO_CLen - LenBegin;
	int PaddingNeeded = (8-((LenNow+1)%8))%8; //+1 because of fixed align bytes at the top
	for(int x=0; x<PaddingNeeded; x++)
		PacketOut_AddByte(&SPB, '?');
	//Write len
	*(unsigned short*)&SPB.PO_SendBuffer[LenBegin] = SPB.PO_CLen - LenBegin;

	EnterCriticalSection(&cgm->cs_send);
	Tabula_Encrypt2(&cgm->tbc2, (unsigned int*)(&SPB.PO_SendBuffer[4]), SPB.PO_CLen-4);
	PacketOut_Send(&SPB, cgm->socket);
	cgm->state_bytesSend += SPB.PO_CLen;
	LeaveCriticalSection(&cgm->cs_send);
}

void netMgr_cellDomain_sendEntityMovement(mapChannelClient_t *aggregator, netCompressedMovement_t *movement, bool skipOwner)
{
	packetBuffer_t SPB;

	int playerCount = 0;
	mapChannelClient_t **playerList = cellMgr_getNotifiedPlayers(aggregator, &playerCount);
	if( !playerList || !playerCount )
		return;
	for(int i=0; i<playerCount; i++)
	{
		mapChannelClient_t *mc = playerList[i];
		if( mc->player == NULL )
			continue;
		if( skipOwner && mc->player->actor->entityId == movement->entityId )
			continue;
		PacketOut_ResetGame(&SPB);
		//Align bytes [4]
		PacketOut_AddByte(&SPB, 0x01);
		int LenBegin = SPB.PO_CLen;
		//Subsize [2]
		PacketOut_AddWord(&SPB, 41);
		//Opcode [2]
		PacketOut_AddByte(&SPB, 0x00); //Packet 0 - if you specify something other than 0(except 0xFF) there will be some additional preleading data?
		PacketOut_AddByte(&SPB, 1);   // ?? 0x80
		int XORCheck = SPB.PO_CLen;
		//datablock 1 (header)
		PacketOut_AddByte(&SPB, 0x00); //2:6 mask
		PacketOut_AddByte(&SPB, (4<<1) | (0)); //Opcode and flag --> 4 movementHandler B
		PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Checksum 1 - after this we also reset checksum counter
		//data block 2 (header 2)
		PacketOut_AddByte(&SPB, 0); // unknown

		PacketOut_AddByte(&SPB, 2); // formatB(2)
		unsigned long long entityId = movement->entityId;
		while( entityId )
		{
			if( entityId&~0x7F )
				PacketOut_AddByte(&SPB, (entityId&0x7F)|0x80);
			else
				PacketOut_AddByte(&SPB, entityId&0x7F);
			entityId >>= 7;
		}

		// formatA
		PacketOut_AddByte(&SPB, 0x00); // unknown
		// posX
		PacketOut_AddByte(&SPB, (movement->posX24b>>16)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posX24b>>8)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posX24b>>0)&0xFF);
		// posY
		PacketOut_AddByte(&SPB, (movement->posY24b>>16)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posY24b>>8)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posY24b>>0)&0xFF);
		// posZ
		PacketOut_AddByte(&SPB, (movement->posZ24b>>16)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posZ24b>>8)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posZ24b>>0)&0xFF);
		// velocity (encoded-short)
		if( movement->velocity <= 0x7F )
		{
			PacketOut_AddByte(&SPB, movement->velocity);
		}
		else if( movement->velocity <= 0x3FFF )
		{
			PacketOut_AddByte(&SPB, (movement->velocity&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->velocity>>7);
		}
		else
		{
			PacketOut_AddByte(&SPB, (movement->velocity&0x7F)|0x80);
			PacketOut_AddByte(&SPB, ((movement->velocity>>7)&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->velocity>>14);
		}
		//PacketOut_AddByte(&SPB, 10);
		// next (flags?)
		PacketOut_AddByte(&SPB, movement->flag);
		// next-viewX (encoded-short)
		if( movement->viewX <= 0x7F )
		{
			PacketOut_AddByte(&SPB, movement->viewX);
		}
		else if( movement->viewX <= 0x3FFF )
		{
			PacketOut_AddByte(&SPB, (movement->viewX&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewX>>7);
		}
		else
		{
			PacketOut_AddByte(&SPB, (movement->viewX&0x7F)|0x80);
			PacketOut_AddByte(&SPB, ((movement->viewX>>7)&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewX>>14);
		}
		// next (encoded-short)
		if( movement->viewY <= 0x7F )
		{
			PacketOut_AddByte(&SPB, movement->viewY);
		}
		else if( movement->viewY <= 0x3FFF )
		{
			PacketOut_AddByte(&SPB, (movement->viewY&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewY>>7);
		}
		else
		{
			PacketOut_AddByte(&SPB, (movement->viewY&0x7F)|0x80);
			PacketOut_AddByte(&SPB, ((movement->viewY>>7)&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewY>>14);
		}
		// post-checksum
		PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen;
		//Alignment
		int LenNow = SPB.PO_CLen - LenBegin;
		int PaddingNeeded = (8-((LenNow+1)%8))%8;
		for(int x=0; x<PaddingNeeded; x++)
			PacketOut_AddByte(&SPB, '?');
		//Write len
		*(unsigned short*)&SPB.PO_SendBuffer[LenBegin] = SPB.PO_CLen - LenBegin;
		EnterCriticalSection(&mc->cgm->cs_send);
		Tabula_Encrypt2(&mc->cgm->tbc2, (unsigned int*)(&SPB.PO_SendBuffer[4]), SPB.PO_CLen-4);
		PacketOut_Send(&SPB, mc->cgm->socket);
		mc->cgm->state_bytesSend += SPB.PO_CLen;
		LeaveCriticalSection(&mc->cgm->cs_send);
	}
}


void netMgr_cellDomain_sendEntityMovement(mapChannel_t *mapChannel, actor_t *aggregator, netCompressedMovement_t *movement)
{
	packetBuffer_t SPB;

	int playerCount = 0;
	mapChannelClient_t **playerList = cellMgr_getNotifiedPlayers(mapChannel, aggregator, &playerCount);
	if( !playerList || !playerCount )
		return;
	for(int i=0; i<playerCount; i++)
	{
		mapChannelClient_t *mc = playerList[i];
		if( mc->player == NULL )
			continue;
		PacketOut_ResetGame(&SPB);
		//Align bytes [4]
		PacketOut_AddByte(&SPB, 0x01);
		int LenBegin = SPB.PO_CLen;
		//Subsize [2]
		PacketOut_AddWord(&SPB, 41);
		//Opcode [2]
		PacketOut_AddByte(&SPB, 0x00); //Packet 0 - if you specify something other than 0(except 0xFF) there will be some additional preleading data?
		PacketOut_AddByte(&SPB, 1);   // ?? 0x80
		int XORCheck = SPB.PO_CLen;
		//datablock 1 (header)
		PacketOut_AddByte(&SPB, 0x00); //2:6 mask
		PacketOut_AddByte(&SPB, (4<<1) | (0)); //Opcode and flag --> 4 movementHandler B
		PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Checksum 1 - after this we also reset checksum counter
		//data block 2 (header 2)
		PacketOut_AddByte(&SPB, 0); // unknown

		PacketOut_AddByte(&SPB, 2); // formatB(2)
		unsigned long long entityId = movement->entityId;
		while( entityId )
		{
			if( entityId&~0x7F )
				PacketOut_AddByte(&SPB, (entityId&0x7F)|0x80);
			else
				PacketOut_AddByte(&SPB, entityId&0x7F);
			entityId >>= 7;
		}

		// formatA
		PacketOut_AddByte(&SPB, 0x00); // unknown
		// posX
		PacketOut_AddByte(&SPB, (movement->posX24b>>16)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posX24b>>8)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posX24b>>0)&0xFF);
		// posY
		PacketOut_AddByte(&SPB, (movement->posY24b>>16)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posY24b>>8)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posY24b>>0)&0xFF);
		// posZ
		PacketOut_AddByte(&SPB, (movement->posZ24b>>16)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posZ24b>>8)&0xFF);
		PacketOut_AddByte(&SPB, (movement->posZ24b>>0)&0xFF);
		// velocity (encoded-short)
		if( movement->velocity <= 0x7F )
		{
			PacketOut_AddByte(&SPB, movement->velocity);
		}
		else if( movement->velocity <= 0x3FFF )
		{
			PacketOut_AddByte(&SPB, (movement->velocity&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->velocity>>7);
		}
		else
		{
			PacketOut_AddByte(&SPB, (movement->velocity&0x7F)|0x80);
			PacketOut_AddByte(&SPB, ((movement->velocity>>7)&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->velocity>>14);
		}
		//PacketOut_AddByte(&SPB, 10);
		// next (flags?)
		PacketOut_AddByte(&SPB, movement->flag);
		// next-viewX (encoded-short)
		if( movement->viewX <= 0x7F )
		{
			PacketOut_AddByte(&SPB, movement->viewX);
		}
		else if( movement->viewX <= 0x3FFF )
		{
			PacketOut_AddByte(&SPB, (movement->viewX&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewX>>7);
		}
		else
		{
			PacketOut_AddByte(&SPB, (movement->viewX&0x7F)|0x80);
			PacketOut_AddByte(&SPB, ((movement->viewX>>7)&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewX>>14);
		}
		// next (encoded-short)
		if( movement->viewY <= 0x7F )
		{
			PacketOut_AddByte(&SPB, movement->viewY);
		}
		else if( movement->viewY <= 0x3FFF )
		{
			PacketOut_AddByte(&SPB, (movement->viewY&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewY>>7);
		}
		else
		{
			PacketOut_AddByte(&SPB, (movement->viewY&0x7F)|0x80);
			PacketOut_AddByte(&SPB, ((movement->viewY>>7)&0x7F)|0x80);
			PacketOut_AddByte(&SPB, movement->viewY>>14);
		}
		// post-checksum
		PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen;
		//Alignment
		int LenNow = SPB.PO_CLen - LenBegin;
		int PaddingNeeded = (8-((LenNow+1)%8))%8;
		for(int x=0; x<PaddingNeeded; x++)
			PacketOut_AddByte(&SPB, '?');
		//Write len
		*(unsigned short*)&SPB.PO_SendBuffer[LenBegin] = SPB.PO_CLen - LenBegin;
		EnterCriticalSection(&mc->cgm->cs_send);
		Tabula_Encrypt2(&mc->cgm->tbc2, (unsigned int*)(&SPB.PO_SendBuffer[4]), SPB.PO_CLen-4);
		PacketOut_Send(&SPB, mc->cgm->socket);
		mc->cgm->state_bytesSend += SPB.PO_CLen;
		LeaveCriticalSection(&mc->cgm->cs_send);
	}
}


void netMgr_sendEntityMovement(clientGamemain_t *cgm, netCompressedMovement_t *movement)
{
	packetBuffer_t SPB;

	PacketOut_ResetGame(&SPB);
	//Align bytes [4]
	PacketOut_AddByte(&SPB, 0x01);
	int LenBegin = SPB.PO_CLen;
	//Subsize [2]
	PacketOut_AddWord(&SPB, 41);
	//Opcode [2]
	PacketOut_AddByte(&SPB, 0x00); //Packet 0 - if you specify something other than 0(except 0xFF) there will be some additional preleading data?
	PacketOut_AddByte(&SPB, 1);   // ?? 0x80
	int XORCheck = SPB.PO_CLen;
	//datablock 1 (header)
	PacketOut_AddByte(&SPB, 0x00); //2:6 mask
	PacketOut_AddByte(&SPB, (4<<1) | (0)); //Opcode and flag --> 4 movementHandler B
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen; //Checksum 1 - after this we also reset checksum counter
	//data block 2 (header 2)
	PacketOut_AddByte(&SPB, 0); // unknown

	PacketOut_AddByte(&SPB, 2); // formatB(2)
	unsigned long long entityId = movement->entityId;
	while( entityId )
	{
		if( entityId&~0x7F )
			PacketOut_AddByte(&SPB, (entityId&0x7F)|0x80);
		else
			PacketOut_AddByte(&SPB, entityId&0x7F);
		entityId >>= 7;
	}

	// formatA
	PacketOut_AddByte(&SPB, 0x00); // unknown
	// posX
	PacketOut_AddByte(&SPB, (movement->posX24b>>16)&0xFF);
	PacketOut_AddByte(&SPB, (movement->posX24b>>8)&0xFF);
	PacketOut_AddByte(&SPB, (movement->posX24b>>0)&0xFF);
	// posY
	PacketOut_AddByte(&SPB, (movement->posY24b>>16)&0xFF);
	PacketOut_AddByte(&SPB, (movement->posY24b>>8)&0xFF);
	PacketOut_AddByte(&SPB, (movement->posY24b>>0)&0xFF);
	// posZ
	PacketOut_AddByte(&SPB, (movement->posZ24b>>16)&0xFF);
	PacketOut_AddByte(&SPB, (movement->posZ24b>>8)&0xFF);
	PacketOut_AddByte(&SPB, (movement->posZ24b>>0)&0xFF);
	// velocity (encoded-short)
	if( movement->velocity <= 0x7F )
	{
		PacketOut_AddByte(&SPB, movement->velocity);
	}
	else if( movement->velocity <= 0x3FFF )
	{
		PacketOut_AddByte(&SPB, (movement->velocity&0x7F)|0x80);
		PacketOut_AddByte(&SPB, movement->velocity>>7);
	}
	else
	{
		PacketOut_AddByte(&SPB, (movement->velocity&0x7F)|0x80);
		PacketOut_AddByte(&SPB, ((movement->velocity>>7)&0x7F)|0x80);
		PacketOut_AddByte(&SPB, movement->velocity>>14);
	}
	//PacketOut_AddByte(&SPB, 10);
	// next (flags?)
	PacketOut_AddByte(&SPB, movement->flag);
	// next-viewX (encoded-short)
	if( movement->viewX <= 0x7F )
	{
		PacketOut_AddByte(&SPB, movement->viewX);
	}
	else if( movement->viewX <= 0x3FFF )
	{
		PacketOut_AddByte(&SPB, (movement->viewX&0x7F)|0x80);
		PacketOut_AddByte(&SPB, movement->viewX>>7);
	}
	else
	{
		PacketOut_AddByte(&SPB, (movement->viewX&0x7F)|0x80);
		PacketOut_AddByte(&SPB, ((movement->viewX>>7)&0x7F)|0x80);
		PacketOut_AddByte(&SPB, movement->viewX>>14);
	}
	// next (encoded-short)
	if( movement->viewY <= 0x7F )
	{
		PacketOut_AddByte(&SPB, movement->viewY);
	}
	else if( movement->viewY <= 0x3FFF )
	{
		PacketOut_AddByte(&SPB, (movement->viewY&0x7F)|0x80);
		PacketOut_AddByte(&SPB, movement->viewY>>7);
	}
	else
	{
		PacketOut_AddByte(&SPB, (movement->viewY&0x7F)|0x80);
		PacketOut_AddByte(&SPB, ((movement->viewY>>7)&0x7F)|0x80);
		PacketOut_AddByte(&SPB, movement->viewY>>14);
	}
	// post-checksum
	PacketOut_AddByte(&SPB, GEN_XOR_BYTE(SPB.PO_CLen-XORCheck)); XORCheck = SPB.PO_CLen;
	//Alignment
	int LenNow = SPB.PO_CLen - LenBegin;
	int PaddingNeeded = (8-((LenNow+1)%8))%8;
	for(int x=0; x<PaddingNeeded; x++)
		PacketOut_AddByte(&SPB, '?');
	//Write len
	*(unsigned short*)&SPB.PO_SendBuffer[LenBegin] = SPB.PO_CLen - LenBegin;
	EnterCriticalSection(&cgm->cs_send);
	Tabula_Encrypt2(&cgm->tbc2, (unsigned int*)(&SPB.PO_SendBuffer[4]), SPB.PO_CLen-4);
	PacketOut_Send(&SPB, cgm->socket);
	cgm->state_bytesSend += SPB.PO_CLen;
	LeaveCriticalSection(&cgm->cs_send);
}



void netMgr_pythonAddMethodCallRaw(mapChannel_t *broadCastChannel, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen)
{
	EnterCriticalSection(&broadCastChannel->criticalSection);
	for(int i=0; i<broadCastChannel->playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(broadCastChannel->playerList[i]->cgm, EntityID, MethodID, pyObjString, pyObjLen);
	}
	LeaveCriticalSection(&broadCastChannel->criticalSection);
}

void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannelClient_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen)
{
	int playerCount = 0;
	mapChannelClient_t **playerList = cellMgr_getNotifiedPlayers(aggregator, &playerCount);
	if( !playerList || !playerCount )
		return;
	for(int i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, EntityID, MethodID, pyObjString, pyObjLen);
	}
}

void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel_t *mapChannel, actor_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen)
{
	int playerCount = 0;
	mapChannelClient_t **playerList = cellMgr_getNotifiedPlayers(mapChannel, aggregator, &playerCount);
	if( !playerList || !playerCount )
		return;
	for(int i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, EntityID, MethodID, pyObjString, pyObjLen);
	}
}


void netMgr_cellDomain_pythonAddMethodCallRawIgnoreSelf(mapChannelClient_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen)
{
	int playerCount = 0;
	mapChannelClient_t **playerList = cellMgr_getNotifiedPlayers(aggregator, &playerCount);
	if( !playerList || !playerCount )
		return;
	for(int i=0; i<playerCount; i++)
	{
		if( playerList[i] == aggregator )
			return;
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, EntityID, MethodID, pyObjString, pyObjLen);
	}
}


void netMgr_cellDomain_pythonAddMethodCallRaw(mapChannel_t *mapChannel, dynObject_t *aggregator, unsigned int EntityID, unsigned int MethodID, unsigned char *pyObjString, int pyObjLen)
{
	int playerCount = 0;
	mapChannelClient_t **playerList = cellMgr_getNotifiedPlayers(mapChannel, aggregator, &playerCount);
	if( !playerList || !playerCount )
		return;
	for(int i=0; i<playerCount; i++)
	{
		netMgr_pythonAddMethodCallRaw(playerList[i]->cgm, EntityID, MethodID, pyObjString, pyObjLen);
	}
}
sint32 AuthServerUtil_Register();
uint32 AuthServerUtil_GetMyIP();

typedef struct
{
	sint8 Accountname[15];
	union
	{
		struct
		{
			uint32 UID1;
			uint32 UID2;
		};
	unsigned long long uid;
	};
}authSessionInfo_t;

sint32 AuthServerUtil_QuerySession(uint32 ID1, uint32 ID2, authSessionInfo_t *asiOut);
int AuthServerUtil_Register();
unsigned int AuthServerUtil_GetMyIP();

typedef struct
{
	char Accountname[15];
	union
	{
		struct
		{
			unsigned int UID1;
			unsigned int UID2;
		};
	unsigned long long uid;
	};
}authSessionInfo_t;

int AuthServerUtil_QuerySession(unsigned int ID1, unsigned int ID2, authSessionInfo_t *asiOut);
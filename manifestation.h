


typedef struct _manifestation_t
{
	actor_t *actor;
	//long long controllerUserId;
	mapChannelClient_t *controllerUser;
	int raceId;
	int classId;
	int level;
	bool genderIsMale;
	unsigned long long targetEntityId;
	unsigned long long actionEntityId;
	int abilityDrawer[5*5];
	int abilityLvDrawer[5*5];
	char currentAbilityDrawer;
}manifestation_t;

void manifestation_createPlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner, di_characterData_t *characterData);
void manifestation_removePlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner);

void manifestation_setAppearanceItem(manifestation_t *manifestation, int itemClassId, unsigned int hueAARRGGBB);
void manifestation_updateAppearance(mapChannelClient_t *owner);
void manifestation_updateWeaponReadyState(mapChannelClient_t *client);

void manifestation_recv_SetTargetId(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen);
void manifestation_recv_ClearTargetId(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen);
void manifestation_recv_ToggleRun(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen);

void manifestation_recv_RequestArmAbility(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen);
void manifestation_recv_RequestSetAbilitySlot(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen);

void manifestation_recv_RequestPerformAbility(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen);

void manifestation_recv_StartAutoFire(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen);
void manifestation_recv_StopAutoFire(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen);
void manifestation_recv_AutoFireKeepAlive(mapChannelClient_t *client, unsigned char *pyString, int pyStringLen);

// cell mgr specific
void manifestation_cellIntroducePlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, int playerCount);
void manifestation_cellIntroduceClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, int playerCount);
void manifestation_cellDiscardClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, int playerCount);
void manifestation_cellDiscardPlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, int playerCount);
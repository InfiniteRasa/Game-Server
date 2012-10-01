


typedef struct _manifestation_t
{
	actor_t *actor;
	//long long controllerUserId;
	mapChannelClient_t *controllerUser;
	sint32 raceId;
	sint32 classId;
	sint32 level;
	bool genderIsMale;
	unsigned long long targetEntityId;
	unsigned long long actionEntityId;
	sint32 abilityDrawer[5*5];
	sint32 abilityLvDrawer[5*5];
	sint8 currentAbilityDrawer;
}manifestation_t;

void manifestation_createPlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner, di_characterData_t *characterData);
void manifestation_removePlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner);

void manifestation_removeAppearanceItem(manifestation_t *manifestation, sint32 itemClassId);
void manifestation_setAppearanceItem(manifestation_t *manifestation, sint32 itemClassId, uint32 hueAARRGGBB);
void manifestation_updateAppearance(mapChannelClient_t *owner);
void manifestation_updateWeaponReadyState(mapChannelClient_t *client);

void manifestation_recv_SetTargetId(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void manifestation_recv_ClearTargetId(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void manifestation_recv_ToggleRun(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);

void manifestation_recv_RequestArmAbility(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void manifestation_recv_RequestSetAbilitySlot(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);

void manifestation_recv_RequestPerformAbility(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);

void manifestation_recv_RequestVisualCombatMode(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void manifestation_recv_StartAutoFire(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void manifestation_recv_StopAutoFire(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void manifestation_recv_AutoFireKeepAlive(mapChannelClient_t *client, uint8 *pyString, sint32 pyStringLen);
void manifestion_recv_revive(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);

// cell mgr specific
void manifestation_cellIntroducePlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);
void manifestation_cellIntroduceClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);
void manifestation_cellDiscardClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);
void manifestation_cellDiscardPlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);
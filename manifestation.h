


typedef struct _manifestation_t
{
	actor_t *actor;
	//long long controllerUserId;
	mapChannelClient_t *controllerUser;
	sint32 raceId;
	sint32 classId;
	//sint32 level; --> moved to actor.stats.level
	bool genderIsMale;
	unsigned long long targetEntityId;
	unsigned long long actionEntityId;
	sint32 abilityDrawer[5*5];
	sint32 abilityLvDrawer[5*5];
	sint8 currentAbilityDrawer;
	// manifestation (player) specific stats
	uint32 credits;  // credit type 0 (fund)
	uint32 prestige; // credit type 1 (prestige)
	uint32 experience;
}manifestation_t;

void manifestation_createPlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner, di_characterData_t *characterData);
void manifestation_removePlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner);

void manifestation_removeAppearanceItem(manifestation_t *manifestation, sint32 itemClassId);
void manifestation_setAppearanceItem(manifestation_t *manifestation, sint32 itemClassId, uint32 hueAARRGGBB);
void manifestation_updateAppearance(mapChannelClient_t *owner);
void manifestation_updateWeaponReadyState(mapChannelClient_t *client);

void manifestation_GainCredits(mapChannelClient_t *cm, sint32 credits);
void manifestation_GainPrestige(mapChannelClient_t *cm, sint32 prestige);
void manifestation_GainExperience(mapChannelClient_t *cm, sint32 experience);

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

/*
 * Amount of experience needed to reach the given level
 * Taken from character.pyo
 */
static sint32 experienceTable[50+1] =
{
	0, // level 1
	3000, // level 2
	10500, // level 3
	24000, // level 4
	43000, // level 5
	69500, // level 6
	103500, // level 7
	148500, // level 8
	204000, // level 9
	272500, // level 10
	356000, // level 11
	456500, // level 12
	577000, // level 13
	719500, // level 14
	886500, // level 15
	1082500, // level 16
	1310500, // level 17
	1575000, // level 18
	1879500, // level 19
	2230000, // level 20
	2632500, // level 21
	3091500, // level 22
	3616500, // level 23
	4212000, // level 24
	4888500, // level 25
	5654500, // level 26
	6520500, // level 27
	7497500, // level 28
	8598500, // level 29
	9837000, // level 30
	11228000, // level 31
	12788500, // level 32
	14538500, // level 33
	16496000, // level 34
	18683000, // level 35
	21125000, // level 36
	23850000, // level 37
	26887500, // level 38
	30269000, // level 39
	34031000, // level 40
	38211000, // level 41
	42853500, // level 42
	48006500, // level 43
	53721000, // level 44
	60055000, // level 45
	67069500, // level 46
	74833500, // level 47
	83421500, // level 48
	92917500, // level 49
	103410000, // level 50
	0x7FFFFFFF // max possible value (dont remove)
};

sint32 manifestation_getLevelByExperience(sint32 xp);
sint32 manifestation_getLevelNeededExperience(sint32 level);
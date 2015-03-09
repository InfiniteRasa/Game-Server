#include"gm.h" // game master structs

/*
 * Skill related stuff
 */

#define SKILL_COUNT		73
#define LOGOS_COUNT		409

// table for skillIndex to skillID mapping
static sint32 skillIdx2ID[SKILL_COUNT] =
{
1,8,14,19,20,21,22,23,24,
25,26,28,30,31,32,34,35,
36,37,39,40,43,47,48,49,
50,54,55,57,58,63,66,67,
68,72,73,77,79,80,82,89,
92,102,110,111,113,114,121,135,
136,147,148,149,150,151,152,153,
154,155,156,157,158,159,160,161,
162,163,164,165,166,172,173,174
};

// table for skillID to skillIndex mapping
static sint32 skillID2Idx[200] =
{
	-1,0,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,2,-1,-1,-1,-1,3,
	4,5,6,7,8,9,10,-1,11,-1,12,13,14,-1,15,16,17,18,-1,19,
	20,-1,-1,21,-1,-1,-1,22,23,24,25,-1,-1,-1,26,27,-1,28,29,-1,
	-1,-1,-1,30,-1,-1,31,32,33,-1,-1,-1,34,35,-1,-1,-1,36,-1,37,
	38,-1,39,-1,-1,-1,-1,-1,-1,40,-1,-1,41,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,42,-1,-1,-1,-1,-1,-1,-1,43,44,-1,45,46,-1,-1,-1,-1,-1,
	-1,47,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,48,49,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,50,51,52,53,54,55,56,57,58,59,60,61,62,
	63,64,65,66,67,68,69,-1,-1,-1,-1,-1,70,71,72,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

// table for skillIndex to ability mapping
static sint32 skillIdx2AbilityID[SKILL_COUNT] = 
{
	-1, -1, -1, -1, 137, -1, -1, -1, -1, 178, 177, 158, -1, -1, 
	197, 186, 188, 162, 187, -1, -1, 233, 234, -1, 194, -1, -1, 
	-1, -1, -1, 301, -1, -1, 185, 251, 240, 302, 232, 229, -1, 
	231, 305, 392, 252, 282, 381, 267, 298, 246, 253, 307, 393, 
	281, 390, 295, 304, 386, 193, 385, 176, 260, 384, 383, 303, 
	388, 389, 387, 380, 401, 430, 262, 421, 446
};

#define manifestation_getSkillIndexById(skid) ((skid)<0?-1:((skid)>=200?-1:(skillID2Idx[(skid)])))
#define manifestation_getSkillIdByIndex(skidx) ((skidx)<0?-1:((skidx)>=(SKILL_COUNT)?-1:(skillIdx2ID[(skidx)])))

// skillIndex constants (used to access the manifestation->skill array)
#define SKILL_IDX_T1_RECRUIT_FIREARMS 0
#define SKILL_IDX_T1_RECRUIT_HAND_TO_HAND 1
#define SKILL_IDX_T2_SPECIALIST_TOOLS 2
#define SKILL_IDX_T1_RECRUIT_MOTOR_ASSIST_ARMOR 3
#define SKILL_IDX_T4_DEMOLITIONIST_SIG_EXPLOSIVE_WAVE 4
#define SKILL_IDX_T2_SOLDIER_REFLECTIVE_ARMOR 5
#define SKILL_IDX_T2_SOLDIER_MACHINE_GUN 6
#define SKILL_IDX_T4_GUARDIAN_STAFF 7
#define SKILL_IDX_T3_COMMANDO_LAUNCHERS 8
#define SKILL_IDX_T2_SOLDIER_SHRAPNEL 9
#define SKILL_IDX_T4_GUARDIAN_REFLECTION 10
#define SKILL_IDX_T3_COMMANDO_FORCE_BLAST 11
#define SKILL_IDX_T2_SPECIALIST_HAZMAT_ARMOR 12
#define SKILL_IDX_T2_SPECIALIST_LEECH_GUN 13
#define SKILL_IDX_T4_ENGINEER_TURRET 14
#define SKILL_IDX_T3_BIOTECHNICIAN_CURE 15
#define SKILL_IDX_T3_BIOTECHNICIAN_RECONSTRUCTION 16
#define SKILL_IDX_T2_SPECIALIST_RUIN 17
#define SKILL_IDX_T4_MEDIC_VIRAL_CONVERSION 18
#define SKILL_IDX_T3_COMMANDO_GRAVITON_ARMOR 19
#define SKILL_IDX_T4_GRENADIER_PROPELLANT_GUN 20
#define SKILL_IDX_T4_GUARDIAN_CONVERSION 21
#define SKILL_IDX_T4_GRENADIER_SIG_CONCUSSIVE_WAVE 22
#define SKILL_IDX_T3_RANGER_STEALTH_ARMOR 23
#define SKILL_IDX_T1_RECRUIT_LIGHTNING 24
#define SKILL_IDX_T4_SNIPER_TORQUE_SHELL_RIFLE 25
#define SKILL_IDX_T3_RANGER_TACTICAL_EVASION 26
#define SKILL_IDX_T3_RANGER_NET_GUN 27
#define SKILL_IDX_T3_SAPPER_MECH_ARMOR 28
#define SKILL_IDX_T3_SAPPER_POLARITY_GUN 29
#define SKILL_IDX_T4_DEMOLITIONIST_REALITY_RIPPER 30
#define SKILL_IDX_T3_BIOTECHNICIAN_BIO_ARMOR 31
#define SKILL_IDX_T3_BIOTECHNICIAN_INJECTION_GUN 32
#define SKILL_IDX_T4_EXOBIOLOGIST_HORTIMUNCULUS 33
#define SKILL_IDX_T4_EXOBIOLOGIST_CADAVER_IMMOLATION 34
#define SKILL_IDX_T4_EXOBIOLOGIST_REANIMATION 35
#define SKILL_IDX_T3_COMMANDO_RUSHING_BLOW 36
#define SKILL_IDX_T4_GRENADIER_SCATTERBOMBS 37
#define SKILL_IDX_T4_GRENADIER_TECTONIC_STRIKE 38
#define SKILL_IDX_T4_SPY_BLADE 39
#define SKILL_IDX_T4_GUARDIAN_VORTEX 40
#define SKILL_IDX_T4_GUARDIAN_SIG_SHIELD_WAVE 41
#define SKILL_IDX_T4_SPY_POLYMORPH 42
#define SKILL_IDX_T4_SPY_SIG_CLOAK_WAVE 43
#define SKILL_IDX_T3_SAPPER_CRAB_MINES 44
#define SKILL_IDX_T4_DEMOLITIONIST_CONTROLLED_FISSION 45
#define SKILL_IDX_T4_DEMOLITIONIST_SELF_DESTRUCT 46
#define SKILL_IDX_T4_ENGINEER_FEEDBACK 47
#define SKILL_IDX_T4_MEDIC_DISEASE 48
#define SKILL_IDX_T4_EXOBIOLOGIST_CREATE_CLONE 49
#define SKILL_IDX_T2_SOLDIER_RAGE 50
#define SKILL_IDX_T4_SPY_TRAITOR 51
#define SKILL_IDX_T4_SNIPER_SIG_CRIT_WAVE 52
#define SKILL_IDX_T4_SNIPER_SHREDDER_AMMO 53
#define SKILL_IDX_T4_SNIPER_TARGET_PAINTING 54
#define SKILL_IDX_T4_MEDIC_MIND_CONTROL 55
#define SKILL_IDX_T4_MEDIC_RESISTANCE 56
#define SKILL_IDX_T4_MEDIC_SIG_REGENERATION_WAVE 57
#define SKILL_IDX_T4_GRENADIER_SACRIFICE 58
#define SKILL_IDX_T4_EXOBIOLOGIST_SIG_REANIMATION_WAVE 59
#define SKILL_IDX_T4_ENGINEER_SIG_BASE_WAVE 60
#define SKILL_IDX_T4_ENGINEER_TRAP 61
#define SKILL_IDX_T4_DEMOLITIONIST_EXPLOSIVE_NANITES 62
#define SKILL_IDX_T3_SAPPER_HACK 63
#define SKILL_IDX_T4_SPY_POLARITY_FIELD 64
#define SKILL_IDX_T3_RANGER_SPOTTER 65
#define SKILL_IDX_T3_RANGER_FIRE_SUPPORT 66
#define SKILL_IDX_T3_COMMANDO_SCOURGE 67
#define SKILL_IDX_T1_RECRUIT_SPRINT 68
#define SKILL_IDX_T4_SNIPER_CALLED_SHOT 69
#define SKILL_IDX_T4_ENGINEER_BOT_CONSTRUCTION 70
#define SKILL_IDX_T3_BIOTECHNICIAN_BIO_AUGMENTATION 71
#define SKILL_IDX_T3_SAPPER_SHIELD_EXTENDER 72

// skillID constants (the values used in the game)
#define SKILL_ID_T1_RECRUIT_FIREARMS 1
#define SKILL_ID_T1_RECRUIT_HAND_TO_HAND 8
#define SKILL_ID_T2_SPECIALIST_TOOLS 14
#define SKILL_ID_T1_RECRUIT_MOTOR_ASSIST_ARMOR 19
#define SKILL_ID_T4_DEMOLITIONIST_SIG_EXPLOSIVE_WAVE 20
#define SKILL_ID_T2_SOLDIER_REFLECTIVE_ARMOR 21
#define SKILL_ID_T2_SOLDIER_MACHINE_GUN 22
#define SKILL_ID_T4_GUARDIAN_STAFF 23
#define SKILL_ID_T3_COMMANDO_LAUNCHERS 24
#define SKILL_ID_T2_SOLDIER_SHRAPNEL 25
#define SKILL_ID_T4_GUARDIAN_REFLECTION 26
#define SKILL_ID_T3_COMMANDO_FORCE_BLAST 28
#define SKILL_ID_T2_SPECIALIST_HAZMAT_ARMOR 30
#define SKILL_ID_T2_SPECIALIST_LEECH_GUN 31
#define SKILL_ID_T4_ENGINEER_TURRET 32
#define SKILL_ID_T3_BIOTECHNICIAN_CURE 34
#define SKILL_ID_T3_BIOTECHNICIAN_RECONSTRUCTION 35
#define SKILL_ID_T2_SPECIALIST_RUIN 36
#define SKILL_ID_T4_MEDIC_VIRAL_CONVERSION 37
#define SKILL_ID_T3_COMMANDO_GRAVITON_ARMOR 39
#define SKILL_ID_T4_GRENADIER_PROPELLANT_GUN 40
#define SKILL_ID_T4_GUARDIAN_CONVERSION 43
#define SKILL_ID_T4_GRENADIER_SIG_CONCUSSIVE_WAVE 47
#define SKILL_ID_T3_RANGER_STEALTH_ARMOR 48
#define SKILL_ID_T1_RECRUIT_LIGHTNING 49
#define SKILL_ID_T4_SNIPER_TORQUE_SHELL_RIFLE 50
#define SKILL_ID_T3_RANGER_TACTICAL_EVASION 54
#define SKILL_ID_T3_RANGER_NET_GUN 55
#define SKILL_ID_T3_SAPPER_MECH_ARMOR 57
#define SKILL_ID_T3_SAPPER_POLARITY_GUN 58
#define SKILL_ID_T4_DEMOLITIONIST_REALITY_RIPPER 63
#define SKILL_ID_T3_BIOTECHNICIAN_BIO_ARMOR 66
#define SKILL_ID_T3_BIOTECHNICIAN_INJECTION_GUN 67
#define SKILL_ID_T4_EXOBIOLOGIST_HORTIMUNCULUS 68
#define SKILL_ID_T4_EXOBIOLOGIST_CADAVER_IMMOLATION 72
#define SKILL_ID_T4_EXOBIOLOGIST_REANIMATION 73
#define SKILL_ID_T3_COMMANDO_RUSHING_BLOW 77
#define SKILL_ID_T4_GRENADIER_SCATTERBOMBS 79
#define SKILL_ID_T4_GRENADIER_TECTONIC_STRIKE 80
#define SKILL_ID_T4_SPY_BLADE 82
#define SKILL_ID_T4_GUARDIAN_VORTEX 89
#define SKILL_ID_T4_GUARDIAN_SIG_SHIELD_WAVE 92
#define SKILL_ID_T4_SPY_POLYMORPH 102
#define SKILL_ID_T4_SPY_SIG_CLOAK_WAVE 110
#define SKILL_ID_T3_SAPPER_CRAB_MINES 111
#define SKILL_ID_T4_DEMOLITIONIST_CONTROLLED_FISSION 113
#define SKILL_ID_T4_DEMOLITIONIST_SELF_DESTRUCT 114
#define SKILL_ID_T4_ENGINEER_FEEDBACK 121
#define SKILL_ID_T4_MEDIC_DISEASE 135
#define SKILL_ID_T4_EXOBIOLOGIST_CREATE_CLONE 136
#define SKILL_ID_T2_SOLDIER_RAGE 147
#define SKILL_ID_T4_SPY_TRAITOR 148
#define SKILL_ID_T4_SNIPER_SIG_CRIT_WAVE 149
#define SKILL_ID_T4_SNIPER_SHREDDER_AMMO 150
#define SKILL_ID_T4_SNIPER_TARGET_PAINTING 151
#define SKILL_ID_T4_MEDIC_MIND_CONTROL 152
#define SKILL_ID_T4_MEDIC_RESISTANCE 153
#define SKILL_ID_T4_MEDIC_SIG_REGENERATION_WAVE 154
#define SKILL_ID_T4_GRENADIER_SACRIFICE 155
#define SKILL_ID_T4_EXOBIOLOGIST_SIG_REANIMATION_WAVE 156
#define SKILL_ID_T4_ENGINEER_SIG_BASE_WAVE 157
#define SKILL_ID_T4_ENGINEER_TRAP 158
#define SKILL_ID_T4_DEMOLITIONIST_EXPLOSIVE_NANITES 159
#define SKILL_ID_T3_SAPPER_HACK 160
#define SKILL_ID_T4_SPY_POLARITY_FIELD 161
#define SKILL_ID_T3_RANGER_SPOTTER 162
#define SKILL_ID_T3_RANGER_FIRE_SUPPORT 163
#define SKILL_ID_T3_COMMANDO_SCOURGE 164
#define SKILL_ID_T1_RECRUIT_SPRINT 165
#define SKILL_ID_T4_SNIPER_CALLED_SHOT 166
#define SKILL_ID_T4_ENGINEER_BOT_CONSTRUCTION 172
#define SKILL_ID_T3_BIOTECHNICIAN_BIO_AUGMENTATION 173
#define SKILL_ID_T3_SAPPER_SHIELD_EXTENDER 174


/*
 * Manifestation/player struct
 */
typedef struct _manifestation_t
{
	actor_t *actor;
	actorAppearanceData_t appearanceData[21];
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
	// skills
	uint8 skill[SKILL_COUNT];
	// manifestation (player) specific stats
	uint32 credits;  // credit type 0 (fund)
	uint32 prestige; // credit type 1 (prestige)
	uint32 experience;
	uint32 spentAttributePoints_body;
	uint32 spentAttributePoints_mind;
	uint32 spentAttributePoints_spirit;
	// mission log
	missionLogEntry_t missionLog[MISSION_LOG_LIMIT]; // must be sorted by missionId to allow binary search
	sint32 activeMissionCount; // number of missions in the mission log
	// mission state map (completed/not completed bitmask of ALL missions)
	uint8* missionStateMap;
	// gamemaster/admin data
	gmData_t* gmData;
}manifestation_t;

void manifestation_createPlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner, di_characterData_t *characterData);
void manifestation_removePlayerCharacter(mapChannel_t *mapChannel, mapChannelClient_t *owner);

void manifestation_removeAppearanceItem(manifestation_t *manifestation, sint32 itemClassId);
void manifestation_setAppearanceItem(manifestation_t *manifestation, sint32 itemClassId, uint32 hueAARRGGBB);
void manifestation_updateAppearance(mapChannelClient_t *owner);
void manifestation_updateWeaponReadyState(mapChannelClient_t *client);

void manifestation_updateStatsValues(mapChannelClient_t *client, bool fullreset);

void manifestation_GainCredits(mapChannelClient_t *cm, sint32 credits);
void manifestation_GainPrestige(mapChannelClient_t *cm, sint32 prestige);
void manifestation_GainExperience(mapChannelClient_t *cm, sint32 experience);
void manifestation_recv_AllocateAttributePoints(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void manifestation_SendAvailableAllocationPoints(mapChannelClient_t *cm);
void manifestation_buildAttributeInfoPacket(mapChannelClient_t *client, pyMarshalString_t* pms);
void manifestation_recv_LevelSkills(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
sint32 manifestation_getSkillPointsAvailable(mapChannelClient_t *cm);

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
void manifestation_recv_Revive(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);

// cell mgr specific
void manifestation_cellIntroducePlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);
void manifestation_cellIntroduceClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);
void manifestation_cellDiscardClientToPlayers(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);
void manifestation_cellDiscardPlayersToClient(mapChannel_t *mapChannel, mapChannelClient_t *client, mapChannelClient_t **playerList, sint32 playerCount);

// update callback
void manifestation_updatePlayer(mapChannelClient_t* mapChannelClient, sint32 tick);

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
void manifestation_SendAbilityDrawerFull(mapChannelClient_t *cm);


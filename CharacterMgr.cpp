#include "Global.h"

void charMgr_beginCharacterSelection(clientGamemain_t *cgm)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, "Change Me"); // familyName // this should be null if hasCharacters is 0
	pym_addInt(&pms, 0); // hasCharacters
	pym_addInt(&pms, cgm->userID); // userId
	//pym_addInt(&pms, 5); // enabledRaceList
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1); // human
	pym_addInt(&pms, 2); // forean_hybrid
	pym_addInt(&pms, 3); // brann_hybrid
	pym_addInt(&pms, 4); // thrax_hybrid
	pym_tuple_end(&pms);
	pym_addInt(&pms, 1); // bCanSkipBootcamp
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, METHODID_BEGINCHARACTERSELETION, pym_getData(&pms), pym_getLen(&pms));

}

void charMgr_sendCharacterCreateSuccess(clientGamemain_t *cgm, char* familyName, int slotNum)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, slotNum); // slotNum
	pym_addUnicode(&pms, familyName); // familyName
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, METHODID_CHARACTERCREATESUCCESS, pym_getData(&pms), pym_getLen(&pms)); // 426 = CharacterCreateSuccess
}

void charMgr_sendCharacterCreateFailed(clientGamemain_t *cgm, int errorCode)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, errorCode); // errorCode
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, METHODID_USERCREATIONFAILED, pym_getData(&pms), pym_getLen(&pms));
}

void charMgr_sendCharacterDeleteSuccess(clientGamemain_t *cgm)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1); // hasCharacters
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, METHODID_CHARACTERDELETESUCCESS, pym_getData(&pms), pym_getLen(&pms));
}



void charMgr_sendGeneratedCharacterName(clientGamemain_t *cgm, bool isMale)
{
	pym_init(&cgm->pyms);
	pym_tuple_begin(&cgm->pyms);
	if( isMale )
		pym_addUnicode(&cgm->pyms, "Richard");
	else
		pym_addUnicode(&cgm->pyms, "Rachel");

	pym_tuple_end(&cgm->pyms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, METHODID_GENERATEDCHARACTERNAME, pym_getData(&cgm->pyms), pym_getLen(&cgm->pyms));
}


void charMgr_sendGeneratedFamilyName(clientGamemain_t *cgm)
{
	pym_init(&cgm->pyms);
	pym_tuple_begin(&cgm->pyms);
	pym_addUnicode(&cgm->pyms, "Garriott");
	pym_tuple_end(&cgm->pyms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, 456, pym_getData(&cgm->pyms), pym_getLen(&cgm->pyms));
}


// podIdx --> 0 to 15
void _charMgr_sendUpdateEmptyPod(clientGamemain_t *cgm, int podIdx)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	//SlotId
	pym_dict_addKey(&pms, "SlotId");
	pym_addInt(&pms, podIdx);
	//IsSelected
	pym_dict_addKey(&pms, "IsSelected");
	if( podIdx == 1 )
		pym_addInt(&pms, 1);
	else
		pym_addInt(&pms, 0);
	//BodyData
	pym_dict_addKey(&pms, "BodyData");
	pym_addNoneStruct(&pms);
	pym_dict_addKey(&pms, "AppearanceData");
	pym_tuple_begin(&pms);
	pym_tuple_end(&pms);
	//CharacterData
	pym_dict_addKey(&pms, "CharacterData");
	pym_addNoneStruct(&pms);
	//UserName
	pym_dict_addKey(&pms, "UserName");
	pym_addNoneStruct(&pms);
	//GameContextId
	pym_dict_addKey(&pms, "GameContextId");
	pym_addNoneStruct(&pms);
	//LoginData
	pym_dict_addKey(&pms, "LoginData");
	pym_addNoneStruct(&pms);
	//ClanData
	pym_dict_addKey(&pms, "ClanData");
	pym_addNoneStruct(&pms);
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityID_charPodFirst+podIdx-1, METHODID_CHARACTERINFO, pym_getData(&pms), pym_getLen(&pms));
}

void charMgr_createSelectionPodEntitys(clientGamemain_t *cgm)
{
	pyMarshalString_t pms;
	for(int i=0; i<16; i++)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, entityID_charPodFirst+i); // entityID
		pym_addInt(&pms, 3543); // classID
		pym_addNoneStruct(&pms); // entityData (dunno)
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cgm, 5, METHODID_CREATEPYHSICALENTITY, pym_getData(&pms), pym_getLen(&pms));
	}
}

// slotId: 1-16
void charMgr_sendCharacterInfo(clientGamemain_t *cgm, int slotId, di_characterPreview_t *charInfo)
{
	if( charInfo == NULL )
	{
		_charMgr_sendUpdateEmptyPod(cgm, slotId);
		return;
	}

	pyMarshalString_t pms;

	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	//SlotId
	pym_dict_addKey(&pms, "SlotId");
	pym_addInt(&pms, slotId);
	//IsSelected
	pym_dict_addKey(&pms, "IsSelected");
	if( slotId == 0 )
		pym_addInt(&pms, 1);
	else
		pym_addInt(&pms, 0);
	//BodyData
	pym_dict_addKey(&pms, "BodyData");
	pym_tuple_begin(&pms);

	pym_addInt(&pms, charInfo->genderIsMale?692:691);			// 0 - genderClassId (human: m692,f691 )
	pym_addInt(&pms, 1);										// 1 - scale, actually is a float!
	pym_tuple_end(&pms);
	//CharacterData
	pym_dict_addKey(&pms, "CharacterData");
	pym_tuple_begin(&pms);

	pym_addUnicode(&pms, charInfo->unicodeName);	// 0	charname
	pym_addInt(&pms, 1);							// 1	Pos
	pym_addInt(&pms, 41);							// 2	XPPtrs
	pym_addInt(&pms, 10);							// 3	XPLvl
	pym_addInt(&pms, 111);							// 4	Body
	pym_addInt(&pms, 12);							// 5	Mind
	pym_addInt(&pms, 21);							// 6	Spirit
	pym_addInt(&pms, charInfo->classID);			// 7	Class
	pym_addInt(&pms, 3);							// 8	CloneCredits
	pym_addInt(&pms, charInfo->raceID);				// 9	RaceID

	pym_tuple_end(&pms);
	//AppearanceData
	pym_dict_addKey(&pms, "AppearanceData");

	pym_dict_begin(&pms);

	for(int i=0; i<SWAPSET_SIZE; i++)
	{
		if( charInfo->appearanceData[i].classId )
		{
			pym_addInt(&pms, i+1); // index(equipmentSlotId)
			pym_tuple_begin(&pms);
			pym_addInt(&pms, charInfo->appearanceData[i].classId); // classId
			pym_tuple_begin(&pms);
			unsigned int hueR = (charInfo->appearanceData[i].hue>>0)&0xFF;
			unsigned int hueG = (charInfo->appearanceData[i].hue>>8)&0xFF;
			unsigned int hueB = (charInfo->appearanceData[i].hue>>16)&0xFF;
			unsigned int hueA = (charInfo->appearanceData[i].hue>>24)&0xFF;
			pym_addInt(&pms, (int)hueR);
			pym_addInt(&pms, (int)hueG);
			pym_addInt(&pms, (int)hueB);
			pym_addInt(&pms, (int)hueA);
			pym_tuple_end(&pms);
			pym_tuple_end(&pms);
		}
	}
	pym_dict_end(&pms);
	//UserName
	pym_dict_addKey(&pms, "UserName");
	pym_addUnicode(&pms, charInfo->unicodeFamily);
	//GameContextId
	pym_dict_addKey(&pms, "GameContextId");
	pym_addInt(&pms, charInfo->currentContextId); // see gamecontextlanguage.txt
	//LoginData
	pym_dict_addKey(&pms, "LoginData");
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0);		// 0	numLogins
	pym_addInt(&pms, 0);		// 1	totalTimePlayed
	pym_addInt(&pms, 0);		// 2	timeSinceLastPlayed
	pym_tuple_end(&pms);
	//ClanData
	pym_dict_addKey(&pms, "ClanData");
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0);		// 0	clanID (0 marks no-clan)
	pym_addUnicode(&pms, "");	// 1	clanName
	pym_tuple_end(&pms);
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityID_charPodFirst+slotId-1, METHODID_CHARACTERINFO, pym_getData(&pms), pym_getLen(&pms));
}

int charMgr_recv_requestCharacterName(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	unsigned int gender = pym_unpackInt(&cgm->pyums); // gender (0 - male, 1 - female)
	unsigned int langID = pym_unpackInt(&cgm->pyums); // Language ID "always 1"
	charMgr_sendGeneratedCharacterName(cgm, gender==0);
	return 1;
}

int charMgr_recv_requestFamilyName(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	unsigned int langID = pym_unpackInt(&cgm->pyums); // Language ID "always 1"
	charMgr_sendGeneratedFamilyName(cgm);
	return 1;
}

void _cb_charMgr_recv_requestCreateCharacterInSlot(void *param, di_characterLayout_t *characterData)
{
	clientGamemain_t *cgm = (clientGamemain_t*)param;
	if( characterData->error )
	{
		if( characterData->error_nameAlreadyInUse )
			charMgr_sendCharacterCreateFailed(cgm, 7); // name in use
		free(characterData);
		return;
	}
	charMgr_sendCharacterCreateSuccess(cgm, characterData->unicodeFamily, characterData->slotIndex);
	charMgr_updateCharacterSelection(cgm);
	free(characterData);
}

int charMgr_recv_requestCreateCharacterInSlot(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen)
{
	di_characterLayout_t *characterData = (di_characterLayout_t*)malloc(sizeof(di_characterLayout_t));
	RtlZeroMemory((void*)characterData, sizeof(di_characterLayout_t));

	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;

	unsigned int slotNum = pym_unpackInt(&cgm->pyums);
	characterData->slotIndex = slotNum;
	char familyName[128];
	char firstName[128];
	characterData->unicodeFamily[0] = '\0';
	characterData->unicodeName[0] = '\0';
	pym_unpackUnicode(&cgm->pyums, characterData->unicodeFamily, CHARACTER_FIRSTNAMELIMIT);
	pym_unpackUnicode(&cgm->pyums, characterData->unicodeName, CHARACTER_FIRSTNAMELIMIT);
	char gender = pym_unpackInt(&cgm->pyums); // 0 --> male, 1 --> female
	float scale = pym_unpackFloat(&cgm->pyums);
	if( pym_unpackDict_begin(&cgm->pyums) == false )
		return 0;

	characterData->genderIsMale = gender == 0;
	// scale is still todo

	int aCount = pym_getContainerSize(&cgm->pyums);
	for(int i=0; i<aCount; i++)
	{
		int key = pym_unpackInt(&cgm->pyums);
		if( pym_unpackTuple_begin(&cgm->pyums) == false )
				return 0;
		int templateId = pym_unpackInt(&cgm->pyums);
		int classId = gameData_getStarterItemTemplateClassId(templateId);
		if( classId == 0 )
			return 0; // unknown starter item
		int equipmentSlotId = gameData_getEquipmentClassIdSlot(classId);
		if( equipmentSlotId == 0 )
			return 0; // unknown starter item class id
		if( key != equipmentSlotId )
			return 0; // client has unsychrounous data
		if( pym_unpackTuple_begin(&cgm->pyums) == false )
			return 0;
		int cLen = pym_getContainerSize(&cgm->pyums);
		if( cLen != 4 ) // no 4 subelements
			return 0;
		int hue1 = pym_unpackLongLong(&cgm->pyums); // R
		int hue2 = pym_unpackLongLong(&cgm->pyums); // G
		int hue3 = pym_unpackLongLong(&cgm->pyums); // B
		int hue4 = pym_unpackLongLong(&cgm->pyums); // A

		unsigned int hueRGBA = (hue1) | (hue2<<8) | (hue3<<16) | (hue4<<24);
		characterData->appearanceData[equipmentSlotId-1].classId = classId;
		characterData->appearanceData[equipmentSlotId-1].hue = hueRGBA;
	}
	// Default armor
	characterData->appearanceData[0].classId = 10908; // helm
	characterData->appearanceData[0].hue = 0xFF808080;
	characterData->appearanceData[1].classId = 7054; // boots
	characterData->appearanceData[1].hue = 0xFF808080;
	characterData->appearanceData[2].classId = 10909; // gloves
	characterData->appearanceData[2].hue = 0xFF808080;
	characterData->appearanceData[14].classId = 7052; // torso
	characterData->appearanceData[14].hue = 0xFF808080;
	characterData->appearanceData[15].classId = 7053; // legs
	characterData->appearanceData[15].hue = 0xFF808080;
	// Default armor end
	int raceId = pym_unpackInt(&cgm->pyums);
	if( raceId < 1 || raceId > 4 )
		return 0; // invalid race
	// setup other characterData
	characterData->userID = cgm->userID;
	characterData->raceID = raceId;
	characterData->classId = 1; // recruit
	// setup starting location
	characterData->currentContextId = 1985 ; // bootcamp
	characterData->posX = -218.328125f;
	characterData->posY = 100.023438f;
	characterData->posZ = -58.453125f;
	// check name for valid letters
	bool validName = true;
	int nameLength = strlen(characterData->unicodeName);
	for(int i=0; i<127; i++)
	{
		char c = characterData->unicodeName[i];
		if( !c )
			break;
		if( c >= 'a' && c <= 'z' )
			continue;
		if( c >= 'A' && c <= 'Z' )
			continue;
		if( c >= '0' && c <= '9' )
			continue;
		if( c == '_' || c == ' ' )
			continue;
		// passed through all, invalid character
		validName = false;
		break;
	}
	if( nameLength < 3 )
	{
		charMgr_sendCharacterCreateFailed(cgm, 2);
		return 1;
	}
	if( nameLength > 20 )
	{
		charMgr_sendCharacterCreateFailed(cgm, 3);
		return 1;
	}
	if( validName == false )
	{
		charMgr_sendCharacterCreateFailed(cgm, 4);
		return 1;
	}
	// queue job for character creation
	dataInterface_Character_createCharacter(characterData, _cb_charMgr_recv_requestCreateCharacterInSlot, cgm);
	return 1;
}

void _cb_charMgr_recv_requestDeleteCharacterInSlot(void *param, diJob_deleteCharacter_t *jobData)
{
	clientGamemain_t *cgm = (clientGamemain_t*)param;
	charMgr_sendCharacterDeleteSuccess(cgm);
	if( jobData->error == false )
		if( jobData->slotId >= 1 && jobData->slotId <= 12 )
			_charMgr_sendUpdateEmptyPod(cgm, jobData->slotId);
}

int charMgr_recv_requestDeleteCharacterInSlot(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	unsigned int slotId = pym_unpackInt(&cgm->pyums); // slotIndex
	dataInterface_Character_deleteCharacter(cgm->userID, slotId, _cb_charMgr_recv_requestDeleteCharacterInSlot, cgm);
	return 1;
}

void _cb_charMgr_initCharacterSelection(void *param, diJob_getCharacterPreviewInfo_t *jobData)
{
	for(int i=0; i<16; i++)
		charMgr_sendCharacterInfo((clientGamemain_t*)param, i+1, jobData->outPreviewData[i]);
}

void charMgr_initCharacterSelection(clientGamemain_t *cgm)
{
	charMgr_beginCharacterSelection(cgm);
	charMgr_createSelectionPodEntitys(cgm);
	// request character info
	dataInterface_Character_getCharacterPreviewInfo(cgm->userID, -1, _cb_charMgr_initCharacterSelection, cgm);
}

void charMgr_updateCharacterSelection(clientGamemain_t *cgm)
{
	// request character info
	dataInterface_Character_getCharacterPreviewInfo(cgm->userID, -1, _cb_charMgr_initCharacterSelection, cgm);
}

void _cb_charMgr_recv_requestSwitchToCharacterInSlot(void *param, diJob_getCharacterPreviewInfo_t *jobData)
{
	clientGamemain_t *cgm = (clientGamemain_t*)param;
	int slotIndex = jobData->slotIndex;
	if( slotIndex < 1 || slotIndex > 16 )
		return;
	// check if character was found
	di_characterPreview_t *characterData;
	characterData = jobData->outPreviewData[slotIndex-1];
	if( !characterData )
		return;
	cgm->mapLoadSlotId = slotIndex;
	// send PreWonkavate (clientMethod.134)
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0); // wonkType - actually not used by the game
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, 134, pym_getData(&pms), pym_getLen(&pms));
	// send Wonkavate (inputstateRouter.242)
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, characterData->currentContextId);	// gameContextId (alias mapId)
	cgm->mapLoadContextId = characterData->currentContextId;
	pym_addInt(&pms, 0);	// instanceId ( not important for now )
	// find map version
	int mapVersion = 0;
	for(int i=0; i<mapInfoCount; i++)
	{
		if( mapInfoArray[i].contextId == characterData->currentContextId )
		{
			mapVersion = mapInfoArray[i].version;
			break;
		}
	}
	pym_addInt(&pms, mapVersion);	// templateVersion ( from the map file? )
	pym_tuple_begin(&pms);  // startPosition
	pym_addInt(&pms, characterData->posX); // x (todo: send as float)
	pym_addInt(&pms, characterData->posY); // y (todo: send as float)
	pym_addInt(&pms, characterData->posZ); // z (todo: send as float)
	pym_tuple_end(&pms);
	pym_addInt(&pms, 0);	// startRotation (todo, read from db and send as float)
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 6, METHODID_WONKAVATE, pym_getData(&pms), pym_getLen(&pms));
	// early pass the client to the mapChannel ( since it must load character )
	cgm->State = GAMEMAIN_STATE_RELIEVED; // the gameMain thread will pass the client to the mapChannel
	return;
}

int charMgr_recv_requestSwitchToCharacterInSlot(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	unsigned int slotId = pym_unpackInt(&cgm->pyums); // slotIndex
	//bool canSkipBootcamp = pym_readBool(&cgm->pymus); --> bool: 02(true false?)

	// request character info
	dataInterface_Character_getCharacterPreviewInfo(cgm->userID, slotId, _cb_charMgr_recv_requestSwitchToCharacterInSlot, cgm);
	return true;
}

#include"Global.h"

void charMgr_beginCharacterSelection(clientGamemain_t *cgm)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addUnicode(&pms, (sint8*)"Name"); // familyName // this should be null if hasCharacters is 0
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
	netMgr_pythonAddMethodCallRaw(cgm, 5, BeginCharacterSelection, pym_getData(&pms), pym_getLen(&pms));

}

void charMgr_sendCharacterCreateSuccess(clientGamemain_t *cgm, sint8* familyName, sint32 slotNum)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, slotNum); // slotNum
	pym_addUnicode(&pms, familyName); // familyName
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, CharacterCreateSuccess, pym_getData(&pms), pym_getLen(&pms)); // 426 = CharacterCreateSuccess
}

void charMgr_sendCharacterCreateFailed(clientGamemain_t *cgm, sint32 errorCode)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, errorCode); // errorCode
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, UserCreationFailed, pym_getData(&pms), pym_getLen(&pms));
}

void charMgr_sendCharacterDeleteSuccess(clientGamemain_t *cgm)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 1); // hasCharacters
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, CharacterDeleteSuccess, pym_getData(&pms), pym_getLen(&pms));
}



void charMgr_sendGeneratedCharacterName(clientGamemain_t *cgm, bool isMale)
{
	pym_init(&cgm->pyms);
	pym_tuple_begin(&cgm->pyms);
	if( isMale )
		pym_addUnicode(&cgm->pyms, (sint8*)"Richard");
	else
		pym_addUnicode(&cgm->pyms, (sint8*)"Rachel");
	pym_tuple_end(&cgm->pyms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, GeneratedCharacterName, pym_getData(&cgm->pyms), pym_getLen(&cgm->pyms));
}


void charMgr_sendGeneratedFamilyName(clientGamemain_t *cgm)
{
	pym_init(&cgm->pyms);
	pym_tuple_begin(&cgm->pyms);
	pym_addUnicode(&cgm->pyms, (sint8*)"Garriott");
	pym_tuple_end(&cgm->pyms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, 456, pym_getData(&cgm->pyms), pym_getLen(&cgm->pyms));
}


// podIdx --> 0 to 15
void _charMgr_sendUpdateEmptyPod(clientGamemain_t *cgm, sint32 podIdx)
{
	pyMarshalString_t pms;
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_dict_begin(&pms);
	//SlotId
	pym_dict_addKey(&pms, (sint8*)"SlotId");
	pym_addInt(&pms, podIdx);
	//IsSelected
	pym_dict_addKey(&pms, (sint8*)"IsSelected");
	if( podIdx == 1 )
		pym_addInt(&pms, 1);
	else
		pym_addInt(&pms, 0);
	//BodyData
	pym_dict_addKey(&pms, (sint8*)"BodyData");
	pym_addNoneStruct(&pms);
	pym_dict_addKey(&pms, (sint8*)"AppearanceData");
	pym_tuple_begin(&pms);
	pym_tuple_end(&pms);
	//CharacterData
	pym_dict_addKey(&pms, (sint8*)"CharacterData");
	pym_addNoneStruct(&pms);
	//UserName
	pym_dict_addKey(&pms, (sint8*)"UserName");
	pym_addNoneStruct(&pms);
	//GameContextId
	pym_dict_addKey(&pms, (sint8*)"GameContextId");
	pym_addNoneStruct(&pms);
	//LoginData
	pym_dict_addKey(&pms, (sint8*)"LoginData");
	pym_addNoneStruct(&pms);
	//ClanData
	pym_dict_addKey(&pms, (sint8*)"ClanData");
	pym_addNoneStruct(&pms);
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityID_charPodFirst+podIdx-1, CharacterInfo, pym_getData(&pms), pym_getLen(&pms));
}

void charMgr_createSelectionPodEntitys(clientGamemain_t *cgm)
{
	pyMarshalString_t pms;
	for(sint32 i=0; i<16; i++)
	{
		pym_init(&pms);
		pym_tuple_begin(&pms);
		pym_addInt(&pms, entityID_charPodFirst+i); // entityID
		pym_addInt(&pms, 3543); // classID
		pym_addNoneStruct(&pms); // entityData (dunno)
		pym_tuple_end(&pms);
		netMgr_pythonAddMethodCallRaw(cgm, 5, CreatePhysicalEntity, pym_getData(&pms), pym_getLen(&pms));
	}
}

// slotId: 1-16
void charMgr_sendCharacterInfo(clientGamemain_t *cgm, sint32 slotId, di_characterPreview_t *charInfo)
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
	pym_dict_addKey(&pms, (sint8*)"SlotId");
	pym_addInt(&pms, slotId);
	//IsSelected
	pym_dict_addKey(&pms, (sint8*)"IsSelected");
	if( slotId == 0 )
		pym_addInt(&pms, 1);
	else
		pym_addInt(&pms, 0);
	//BodyData
	pym_dict_addKey(&pms, (sint8*)"BodyData");
	pym_tuple_begin(&pms);

	pym_addInt(&pms, charInfo->genderIsMale?692:691);			// 0 - genderClassId (human: m692,f691 )
	pym_addInt(&pms, 1);										// 1 - scale, actually is a float!
	pym_tuple_end(&pms);
	//CharacterData
	pym_dict_addKey(&pms, (sint8*)"CharacterData");
	pym_tuple_begin(&pms);

	pym_addUnicode(&pms, charInfo->unicodeName);	// 0	charname
	pym_addInt(&pms, 1);							// 1	Pos
	pym_addInt(&pms, charInfo->experience);			// 2	XPPtrs
	pym_addInt(&pms, charInfo->level);				// 3	XPLvl
	pym_addInt(&pms, charInfo->body);				// 4	Body
	pym_addInt(&pms, charInfo->mind);				// 5	Mind
	pym_addInt(&pms, charInfo->spirit);				// 6	Spirit
	pym_addInt(&pms, charInfo->classID);			// 7	Class
	pym_addInt(&pms, charInfo->clonecredits);		// 8	CloneCredits
	pym_addInt(&pms, charInfo->raceID);				// 9	RaceID

	pym_tuple_end(&pms);
	//AppearanceData
	pym_dict_addKey(&pms, (sint8*)"AppearanceData");

	pym_dict_begin(&pms);

	for(sint32 i=0; i<SWAPSET_SIZE; i++)
	{
		if( charInfo->appearanceData[i].classId )
		{
			pym_addInt(&pms, i+1); // index(equipmentSlotId)
			pym_tuple_begin(&pms);
			pym_addInt(&pms, charInfo->appearanceData[i].classId); // classId
			pym_tuple_begin(&pms);
			uint32 hueR = (charInfo->appearanceData[i].hue>>0)&0xFF;
			uint32 hueG = (charInfo->appearanceData[i].hue>>8)&0xFF;
			uint32 hueB = (charInfo->appearanceData[i].hue>>16)&0xFF;
			uint32 hueA = (charInfo->appearanceData[i].hue>>24)&0xFF;
			pym_addInt(&pms, (sint32)hueR);
			pym_addInt(&pms, (sint32)hueG);
			pym_addInt(&pms, (sint32)hueB);
			pym_addInt(&pms, (sint32)hueA);
			pym_tuple_end(&pms);
			pym_tuple_end(&pms);
		}
	}
	pym_dict_end(&pms);
	//UserName
	pym_dict_addKey(&pms, (sint8*)"UserName");
	pym_addUnicode(&pms, charInfo->unicodeFamily);
	//GameContextId
	pym_dict_addKey(&pms, (sint8*)"GameContextId");
	pym_addInt(&pms, charInfo->currentContextId); // see gamecontextlanguage.txt
	//LoginData
	pym_dict_addKey(&pms, (sint8*)"LoginData");
	pym_tuple_begin(&pms);
	pym_addInt(&pms, charInfo->numLogins);		// 0	numLogins
	pym_addInt(&pms, charInfo->totalTimePlayed);		// 1	totalTimePlayed
	pym_addInt(&pms, charInfo->timeSinceLastPlayed);		// 2	timeSinceLastPlayed
	pym_tuple_end(&pms);
	//ClanData
	pym_dict_addKey(&pms, (sint8*)"ClanData");
	pym_tuple_begin(&pms);
	pym_addInt(&pms, 0);		// 0	clanID (0 marks no-clan)
	pym_addUnicode(&pms, "");	// 1	clanName
	pym_tuple_end(&pms);
	pym_dict_end(&pms);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, entityID_charPodFirst+slotId-1, CharacterInfo, pym_getData(&pms), pym_getLen(&pms));
}

sint32 charMgr_recv_requestCharacterName(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	uint32 gender = pym_unpackInt(&cgm->pyums); // gender (0 - male, 1 - female)
	uint32 langID = pym_unpackInt(&cgm->pyums); // Language ID "always 1"
	charMgr_sendGeneratedCharacterName(cgm, gender==0);
	return 1;
}

sint32 charMgr_recv_requestFamilyName(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	uint32 langID = pym_unpackInt(&cgm->pyums); // Language ID "always 1"
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

sint32 charMgr_recv_requestCreateCharacterInSlot(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen)
{
	di_characterLayout_t *characterData = (di_characterLayout_t*)malloc(sizeof(di_characterLayout_t));
	RtlZeroMemory((void*)characterData, sizeof(di_characterLayout_t));

	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;

	uint32 slotNum = pym_unpackInt(&cgm->pyums);
	characterData->slotIndex = slotNum;
	//sint8 familyName[128];
	//sint8 firstName[128];
	characterData->unicodeFamily[0] = '\0';
	characterData->unicodeName[0] = '\0';
	pym_unpackUnicode(&cgm->pyums, characterData->unicodeFamily, CHARACTER_FIRSTNAMELIMIT);
	pym_unpackUnicode(&cgm->pyums, characterData->unicodeName, CHARACTER_FIRSTNAMELIMIT);
	sint8 gender = pym_unpackInt(&cgm->pyums); // 0 --> male, 1 --> female
	float scale = pym_unpackFloat(&cgm->pyums);
	if( pym_unpackDict_begin(&cgm->pyums) == false )
		return 0;

	characterData->genderIsMale = gender == 0;
	// scale is still todo

	sint32 aCount = pym_getContainerSize(&cgm->pyums);
	for(sint32 i=0; i<aCount; i++)
	{
		sint32 key = pym_unpackInt(&cgm->pyums);
		if( pym_unpackTuple_begin(&cgm->pyums) == false )
				return 0;
		sint32 templateId = pym_unpackInt(&cgm->pyums);
		sint32 classId = gameData_getStarterItemTemplateClassId(templateId);
		if( classId == 0 )
			return 0; // unknown starter item
		sint32 equipmentSlotId = gameData_getEquipmentClassIdSlot(classId);
		if( equipmentSlotId == 0 )
			return 0; // unknown starter item class id
		if( key != equipmentSlotId )
			return 0; // client has unsychrounous data
		if( pym_unpackTuple_begin(&cgm->pyums) == false )
			return 0;
		sint32 cLen = pym_getContainerSize(&cgm->pyums);
		if( cLen != 4 ) // no 4 subelements
			return 0;
		sint32 hue1 = pym_unpackLongLong(&cgm->pyums); // R
		sint32 hue2 = pym_unpackLongLong(&cgm->pyums); // G
		sint32 hue3 = pym_unpackLongLong(&cgm->pyums); // B
		sint32 hue4 = pym_unpackLongLong(&cgm->pyums); // A

		uint32 hueRGBA = (hue1) | (hue2<<8) | (hue3<<16) | (hue4<<24);
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
	sint32 raceId = pym_unpackInt(&cgm->pyums);
	if( raceId < 1 || raceId > 4 )
		return 0; // invalid race
	// setup other characterData
	characterData->userID = cgm->userID;
	characterData->raceID = raceId;
	characterData->classId = 1; // recruit
	// setup starting location
	characterData->currentContextId = 1220 ; // wilderness (alia das)
	characterData->posX = 894.9f;
	characterData->posY = 307.9f;
	characterData->posZ = 347.1f;
	// check name for valid letters
	bool validName = true;
	sint32 nameLength = strlen((char*)characterData->unicodeName);
	for(sint32 i=0; i<127; i++)
	{
		sint8 c = characterData->unicodeName[i];
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
	DataInterface_Character_createCharacter(characterData, _cb_charMgr_recv_requestCreateCharacterInSlot, cgm);
	return 1;
}

void _cb_charMgr_recv_requestDeleteCharacterInSlot(void *param, diJob_deleteCharacter_t *jobData)
{
	clientGamemain_t *cgm = (clientGamemain_t*)param;
	charMgr_sendCharacterDeleteSuccess(cgm);
	if( jobData->error == false )
		if( jobData->slotId >= 1 && jobData->slotId <= 16 )
			_charMgr_sendUpdateEmptyPod(cgm, jobData->slotId);
}

sint32 charMgr_recv_requestDeleteCharacterInSlot(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	uint32 slotId = pym_unpackInt(&cgm->pyums); // slotIndex
	DataInterface_Character_deleteCharacter(cgm->userID, slotId, _cb_charMgr_recv_requestDeleteCharacterInSlot, cgm);
	return 1;
}

void _cb_charMgr_initCharacterSelection(void *param, diJob_getCharacterPreviewInfo_t *jobData)
{
	for(sint32 i=0; i<16; i++)
		charMgr_sendCharacterInfo((clientGamemain_t*)param, i+1, jobData->outPreviewData[i]);
}

void charMgr_initCharacterSelection(clientGamemain_t *cgm)
{
	charMgr_beginCharacterSelection(cgm);
	charMgr_createSelectionPodEntitys(cgm);
	// request character info
	DataInterface_Character_getCharacterPreviewInfo(cgm->userID, -1, _cb_charMgr_initCharacterSelection, cgm);
}

void charMgr_updateCharacterSelection(clientGamemain_t *cgm)
{
	// request character info
	DataInterface_Character_getCharacterPreviewInfo(cgm->userID, -1, _cb_charMgr_initCharacterSelection, cgm);
}

/*selects the current character*/
void _cb_charMgr_recv_requestSwitchToCharacterInSlot(void *param, diJob_getCharacterPreviewInfo_t *jobData)
{
	clientGamemain_t *cgm = (clientGamemain_t*)param;
	sint32 slotIndex = jobData->slotIndex;
	if( slotIndex < 1 || slotIndex > 16 )
		return;
	// check if character was found
	di_characterPreview_t *characterData;
	characterData = jobData->outPreviewData[slotIndex-1];
	if( !characterData )
		return;
	cgm->mapLoadSlotId = slotIndex;

	pyMarshalString_t pms;
	// Test: send GM enabled
	pym_init(&pms);
	pym_tuple_begin(&pms);
	pym_addBool(&pms, true);
	pym_tuple_end(&pms);
	netMgr_pythonAddMethodCallRaw(cgm, 5, 366, pym_getData(&pms), pym_getLen(&pms));
	// send PreWonkavate (clientMethod.134)
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
	sint32 mapVersion = 0;
	for(sint32 i=0; i<mapInfoCount; i++)
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
	netMgr_pythonAddMethodCallRaw(cgm, 6, Wonkavate, pym_getData(&pms), pym_getLen(&pms));
	// early pass the client to the mapChannel ( since it must load character )
	cgm->State = GAMEMAIN_STATE_RELIEVED; // the gameMain thread will pass the client to the mapChannel
	return;
}

sint32 charMgr_recv_requestSwitchToCharacterInSlot(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen)
{
	pym_init(&cgm->pyums, pyString, pyStringLen);
	if( pym_unpackTuple_begin(&cgm->pyums) == false )
		return 0;
	uint32 slotId = pym_unpackInt(&cgm->pyums); // slotIndex
	//bool canSkipBootcamp = pym_readBool(&cgm->pymus); --> bool: 02(true false?)

	// request character info
	DataInterface_Character_getCharacterPreviewInfo(cgm->userID, slotId, _cb_charMgr_recv_requestSwitchToCharacterInSlot, cgm);
	return true;
}

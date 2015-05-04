void charMgr_beginCharacterSelection(clientGamemain_t *cgm);
void charMgr_createSelectionPodEntitys(clientGamemain_t *cgm);
void charMgr_sendCharacterInfo(clientGamemain_t *cgm);
void charMgr_initCharacterSelection(clientGamemain_t *cgm);
void charMgr_updateCharacterSelection(clientGamemain_t *cgm);

 // recv
sint32 charMgr_recv_requestCharacterName(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen);
sint32 charMgr_recv_requestFamilyName(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen);
sint32 charMgr_recv_requestCreateCharacterInSlot(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen);
sint32 charMgr_recv_requestDeleteCharacterInSlot(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen);
sint32 charMgr_recv_requestSwitchToCharacterInSlot(clientGamemain_t *cgm, uint8 *pyString, sint32 pyStringLen);
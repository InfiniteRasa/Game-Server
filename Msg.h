void charMgr_beginCharacterSelection(clientGamemain_t *cgm);
void charMgr_createSelectionPodEntitys(clientGamemain_t *cgm);
void charMgr_sendCharacterInfo(clientGamemain_t *cgm);
void charMgr_initCharacterSelection(clientGamemain_t *cgm);
void charMgr_updateCharacterSelection(clientGamemain_t *cgm);

 // recv
int charMgr_recv_requestCharacterName(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen);
int charMgr_recv_requestCreateCharacterInSlot(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen);
int charMgr_recv_requestDeleteCharacterInSlot(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen);
int charMgr_recv_requestSwitchToCharacterInSlot(clientGamemain_t *cgm, unsigned char *pyString, int pyStringLen);
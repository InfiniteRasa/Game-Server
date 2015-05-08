void communicator_loginOk(mapChannel_t *mapChannel, mapChannelClient_t *player);
void communicator_playerEnterMap(mapChannelClient_t *client);
void communicator_playerChangeMap(mapChannelClient_t *client);
void communicator_playerExitMap(mapChannelClient_t *client);

void communicator_init();

void communicator_systemMessage(mapChannelClient_t *client, sint8 *message);

void communicator_registerPlayer(mapChannelClient_t *client);
void communicator_unregisterPlayer(mapChannelClient_t *client);

void communicator_joinDefaultLocalChannel(mapChannelClient_t *client, sint32 channelId);
void communicator_leaveChannel(mapChannelClient_t *client, sint32 channelId);

// input
void communicator_recv_radialChat(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void communicator_recv_shout(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void communicator_recv_channelChat(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);
void communicator_recv_whisper(mapChannelClient_t *cm, uint8 *pyString, sint32 pyStringLen);

#pragma once

// is this enum used somewhere?
enum
{
	entityID_selfdefStart =	100,
	// character selection pods
	entityID_charPodFirst,	
	entityID_charPodLast = entityID_charPodFirst+15,	

};

// sysentity ids
enum SysEntity
{
	ClientInputStateRouterId		= 1,
	AreaServerId					= 2,
	AreaServerMapManagerId			= 3,
	AreaServerUserManagerId			= 4,
	ClientMethodId					= 5,
	CurrentInputStateId				= 6,
	ClientGameMapId					= 7,
	CommunicatorId					= 8,
	ClientInventoryManagerId		= 9,
	ClientServerFlagManagerId		= 10,
	ClientPartyManagerId			= 11,
	ClientGameUIManagerId			= 12,
	ClientAudioManagerId			= 13,
	ClientObjectAnimationManagerId	= 15,
	ClientTradeManagerId			= 16,
	ClientAdventureLauncherId		= 17,
	ClientGuildManagerId			= 19,
	ClientPetitionManagerId			= 22,
	ClientWargameManagerId			= 23,
	ClientClanManagerId				= 24,
	ClientSocialManagerId			= 25,
	ClientMapStateId				= 26,
	ClientAuctionHouseManagerId		= 14,
	ClientPrestigeSystemId			= 27,
	ClientControlPointManagerId		= 28,
	ClientLookingForGroupManagerId	= 29,
	ClientTeamManagerId				= 30
};
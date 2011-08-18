#include "Global.h"

/*
84 
  1E 91 01
  11 
  2F 00 10 00 00 00 00 00 00 00 66
*/ 


void manifestation_recv_RequestPerformAbility(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	/*
	ServerArgs = (self.actionId, self.actionArgId, target, self.itemId)
    if self.useClientYaw: serverArgs += (actor.body.GetYaw())
    gameclient.SendCallActorMethod('RequestPerformAbility', serverArgs)
	*/
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int actionId = pym_unpackInt(&pums);
	int actionArgId = pym_unpackInt(&pums); //(level / actionArg?)
	unsigned long long targetEntityId = pym_unpackLongLong(&pums);
	// unpack item id
	// unpack yaw
	if( pums.unpackErrorEncountered )
		return;

	switch( actionId )
	{
	case 194: // Lightning
		printf("Lightning: Target %u\n", (unsigned int)targetEntityId);
		missile_launch(cm->mapChannel, cm->player->actor, targetEntityId, MISSILE_LIGHTNING, 20);
		gameEffect_attach(cm->mapChannel, targetEntityId, 86, 1); // stun
		return;
	case 401: // Sprint
		gameEffect_attach(cm->mapChannel, cm->player->actor, EFFECTID_SPRINT, actionArgId);
		return;
	default:
		printf("Unknown Ability: ID %i ArgID %i Target %u64\n", actionId, actionArgId, targetEntityId);
		return;
	};
	
	//missile_launch(cm->mapChannel, client->player->actor, client->player->targetEntityId, MISSILE_PISTOL, 10);
	// Recv_GameEffectAttached(typeId, level, sourceId, *args) -> None
	// Notification that a game effect was attached to this entity.
	// 247


	

	//pyMarshalString_t pms;
	//pym_init(&pms);
	//pym_tuple_begin(&pms);

	//// test
	////pym_addInt(&pms, (int)247); // effectId?
	////pym_addInt(&pms, 1); // level
	////pym_addInt(&pms, (int)0); // sourceId(?)


	//		pym_addInt(&pms, (int)247); //typeId
	//		pym_addInt(&pms, (int)247);//effectId
	//		pym_addInt(&pms, (int)1);//level
	//		pym_addInt(&pms, (int)0);//sourceId
	//		pym_addInt(&pms, (int)1);//announce
	//		pym_dict_begin(&pms);//tooltipDict
	//			// keys:
	//			// 'duration'
	//			// 'damageType'
	//			// 'attrId'
	//			// 'isActive'
	//			// 'isBuff'
	//			// 'isDebuff'
	//			// 'isNegativeEffect'
	//		pym_dict_end(&pms);
	//		//args ( variable)

	//pym_dict_begin(&pms);
	//pym_dict_end(&pms);

	//pym_tuple_end(&pms);
	//netMgr_cellDomain_pythonAddMethodCallRaw(cm, cm->player->actor->entityId, 74, pym_getData(&pms), pym_getLen(&pms));
	//printf("Ability cast\n");
	// Recv_StateChange
	// _Transition
	//pyMarshalString_t pms;
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_dict_begin(&pms);

	//// test
	//pym_addInt(&pms, (int)20); // key = 20 = "WINDUP"?
	//pym_addNoneStruct(&pms);

	////pym_addInt(&pms, (int)abilityActionId); // actionId
	////pym_addInt(&pms, (int)unknown); // actionLevel/Arg
	//pym_dict_end(&pms);
	//pym_tuple_end(&pms);
	//netMgr_cellDomain_pythonAddMethodCallRaw(cm, cm->player->actor->entityId, 206, pym_getData(&pms), pym_getLen(&pms));

	//// Recv_PerformWindup (126)
	////pyMarshalString_t pms;
	//pym_init(&pms);
	//pym_tuple_begin(&pms);
	//pym_addInt(&pms, (int)abilityActionId); // actionId
	//pym_addInt(&pms, (int)unknown); // actionLevel/Arg
	//pym_tuple_end(&pms);
	//netMgr_cellDomain_pythonAddMethodCallRaw(cm, cm->player->actor->entityId, 126, pym_getData(&pms), pym_getLen(&pms));

}
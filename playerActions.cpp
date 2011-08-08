#include "Global.h"

/*
84 
  1E 91 01
  11 
  2F 00 10 00 00 00 00 00 00 00 66
*/ 


void manifestation_recv_RequestPerformAbility(mapChannelClient_t *cm, unsigned char *pyString, int pyStringLen)
{
	pyUnmarshalString_t pums;
	pym_init(&pums, pyString, pyStringLen);
	if( !pym_unpackTuple_begin(&pums) )
		return;
	int abilityActionId = pym_unpackInt(&pums);
	int unknown = pym_unpackInt(&pums); //(level / actionArg?)
	unsigned long long targetEntityId = pym_unpackLongLong(&pums);
	
	if( pums.unpackErrorEncountered )
		return;

	// Recv_GameEffectAttached(typeId, level, sourceId, *args) -> None
	// Notification that a game effect was attached to this entity.
	// 247


	gameEffect_attach(cm->mapChannel, cm->player->actor, EFFECTID_SPRINT, 1);

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
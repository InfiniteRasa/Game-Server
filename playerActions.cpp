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
/*
000000FC     64 - LOAD_CONST          1
000000FF     5A - STORE_NAME          'STANDING'
00000102     64 - LOAD_CONST          2
00000105     5A - STORE_NAME          'SITTING'
00000108     64 - LOAD_CONST          3
0000010B     5A - STORE_NAME          'LYING_DOWN'
0000010E     64 - LOAD_CONST          4
00000111     5A - STORE_NAME          'SWIMMING'
00000114     64 - LOAD_CONST          5
00000117     5A - STORE_NAME          'DEAD'
0000011A     64 - LOAD_CONST          6
0000011D     5A - STORE_NAME          'STOPPED'
00000120     64 - LOAD_CONST          7
00000123     5A - STORE_NAME          'SLOW'
00000126     64 - LOAD_CONST          8
00000129     5A - STORE_NAME          'FAST'
0000012C     64 - LOAD_CONST          9
0000012F     5A - STORE_NAME          'FLYING'
00000132     64 - LOAD_CONST          10
00000135     5A - STORE_NAME          'FLAILING'
00000138     64 - LOAD_CONST          11
0000013B     5A - STORE_NAME          'NORMAL'
0000013E     64 - LOAD_CONST          12
00000141     5A - STORE_NAME          'UNCONTROLLED'
00000144     64 - LOAD_CONST          13
00000147     5A - STORE_NAME          'STUNNED'
0000014A     64 - LOAD_CONST          14
0000014D     5A - STORE_NAME          'CROUCHED'
00000150     64 - LOAD_CONST          15
00000153     5A - STORE_NAME          'AT_PEACE'
00000156     64 - LOAD_CONST          17
00000159     5A - STORE_NAME          'COMBAT_ENGAGED'
0000015C     64 - LOAD_CONST          18
0000015F     5A - STORE_NAME          'IDLE'
00000162     64 - LOAD_CONST          19
00000165     5A - STORE_NAME          'RECOVERY'
00000168     64 - LOAD_CONST          20
0000016B     5A - STORE_NAME          'WINDUP'
0000016E     64 - LOAD_CONST          21
00000171     5A - STORE_NAME          'NO_TOOL'
00000174     64 - LOAD_CONST          24
00000177     5A - STORE_NAME          'TOOL_READY'
0000017A     64 - LOAD_CONST          25
0000017D     5A - STORE_NAME          'SPECIAL'
00000180     64 - LOAD_CONST          26
00000183     5A - STORE_NAME          'DYING'
*/

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
#include "Global.h"
#include <time.h>
/*
Behavior controller

responsible for:
npc and creature movement, decisions, combat or any actions.
*/


/*
* Called every 250 milliseconds for every NPC on the map
*/
void controller_npcThink(mapChannel_t *mapChannel, npc_t *npc)
{

}


float calcAngle(float a_x, float a_y)
{
	//float a_x = p2_x - p1_x;
	//float a_y = p2_y - p1_y;

	float b_x = 1.0;
	float b_y = 0.0;

	float dif = a_x*a_x+a_y*a_y;
	if( dif < 0.01f )
		return 0.0;

	return acos((a_x*b_x+a_y*b_y)/sqrt(dif));
}

void updateEntityMovementW2(float difX, float difY, float difZ, creature_t *creature,mapChannel_t *mapChannel, float speeddiv, bool isMoved)
{


	float length = 1.0f / sqrt(difX*difX + difY*difY + difZ*difZ);
	float velocity = 0.0f;
	difX *= length;
	difY *= length;
	difZ *= length;

	float vX = atan2(-difX, -difZ);
	// multiplicate with speed
	if( isMoved == true) velocity = creature->velocity * speeddiv;
	else velocity = 0.0f;

	float speed = velocity * ( 250.0f / 1000.0f);
	//float rspeed = creature->rotspeed * (250.0f / 1000.0f);
	difX *= speed;
	difY *= speed;
	difZ *= speed;
	// move unit
	if(isMoved == true)
	{
		creature->actor.posX += difX;
		creature->actor.posY += difY;
		creature->actor.posZ += difZ;
	}
	// send movement update
	netCompressedMovement_t movement = {0};
	movement.entityId = creature->actor.entityId;
	movement.posX24b = (uint32)(creature->actor.posX*256.0f);
	movement.posY24b = (uint32)(creature->actor.posY*256.0f);
	movement.posZ24b = (uint32)(creature->actor.posZ*256.0f);

	movement.flag = 0x08;
	movement.viewX = (uint16)(sint16)(vX*10240.0f);
	movement.viewY = 0;//(uint16)(sint16)(vX*10240.0f);//(sint32)(calcAngle(difX, difZ)*1024.0f);
	movement.velocity = (uint16)(velocity * 1024.0f);
	netMgr_cellDomain_sendEntityMovement(mapChannel, &creature->actor, &movement);

}

#define FACTION_BANE	0
#define FACTION_AFS		1

#define WANDER_IDLE			0
#define WANDER_MOVING		1

void controller_setActionWander(creature_t *creature)
{
	creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
	creature->controller.actionWander.state = WANDER_IDLE;
	creature->controller.pathIndex = 0;
	creature->controller.pathLength = 0;
}

void controller_setActionFighting(creature_t *creature, uint64 targetEntityId)
{
	creature->controller.currentAction = BEHAVIOR_ACTION_FIGHTING;
	creature->controller.pathIndex = 0;
	creature->controller.pathLength = 0;
	creature->controller.actionFighting.targetEntityId = targetEntityId;
	creature->lastagression = GetTickCount();
}


/*
 * Checks for enemy creatures and players within the given range
 */
bool controller_checkForAttackableEntityInRange(mapChannel_t *mapChannel, creature_t *creature, float range)
{
	// get area of affected cells
	sint32 minX = (sint32)(((creature->actor.posX-range) / CELL_SIZE) + CELL_BIAS);
	sint32 minZ = (sint32)(((creature->actor.posZ-range) / CELL_SIZE) + CELL_BIAS);
	sint32 maxX = (sint32)(((creature->actor.posX+range+(CELL_SIZE-0.0001f)) / CELL_SIZE) + CELL_BIAS);
	sint32 maxZ = (sint32)(((creature->actor.posZ+range+(CELL_SIZE-0.0001f)) / CELL_SIZE) + CELL_BIAS);
	float rangeSqr = range*range;
	float mPosX = creature->actor.posX;
	float mPosY = creature->actor.posY;
	float mPosZ = creature->actor.posZ;
	// check all cells for players
	for(sint32 ix=minX; ix<=maxX; ix++)
	{
		for(sint32 iz=minZ; iz<=maxZ; iz++)
		{
			mapCell_t *nMapCell = cellMgr_getCell(mapChannel, ix, iz);
			if( nMapCell )
			{
				// check players
				if( !nMapCell->ht_playerList.empty() )
				{
					std::vector<mapChannelClient_t*>::iterator itr = nMapCell->ht_playerList.begin();
					while (itr != nMapCell->ht_playerList.end())
					{
						mapChannelClient_t* player = itr[0];
						++itr;
						if( player->player->actor->stats.healthCurrent <= 0 )
							continue;
						// check distance
						float difX = (sint32)(player->player->actor->posX) - mPosX;
						float difY = (sint32)(player->player->actor->posY) - mPosY;
						float difZ = (sint32)(player->player->actor->posZ) - mPosZ;
						float dist = difX*difX + difY*difY + difZ*difZ;
						//	dist = difX*difX + difZ*difZ;
						if( dist <= rangeSqr )
						{
							// set target and change state
							if (creature->faction != FACTION_AFS ) // all non-AFS creatures attack players
							{
								controller_setActionFighting(creature, player->clientEntityId);
								return true;
							}
						}
					}	
				}
				// check creatures
				if( !nMapCell->ht_creatureList.empty() )
				{
					std::vector<creature_t*>::iterator itrCreature = nMapCell->ht_creatureList.begin();
					while (itrCreature != nMapCell->ht_creatureList.end())
					{
						creature_t* dCreature = itrCreature[0];
						++itrCreature;
						if( dCreature->actor.stats.healthCurrent <= 0 )
							continue;
						if( dCreature == creature )
							continue;
						// check distance
						float difX = (sint32)(dCreature->actor.posX) - mPosX;
						float difY = (sint32)(dCreature->actor.posY) - mPosY;
						float difZ = (sint32)(dCreature->actor.posZ) - mPosZ;
						float dist = difX*difX + difY*difY + difZ*difZ;
						//	dist = difX*difX + difZ*difZ;
						if( dist <= rangeSqr )
						{
							// set target and change state
							bool c1isBadFaction = (creature->faction != FACTION_AFS);
							bool c2isBadFaction = (dCreature->faction != FACTION_AFS);

							if(c1isBadFaction != c2isBadFaction)
							{
								controller_setActionFighting(creature, dCreature->actor.entityId);
								return true;
							}
						}
					}	
				}
			}
		}
	}


	return false;

	//sint32 tCount =0;
	//float minimumRange = 16.1f;
	//float difX = 0.0f;
	//float difY = 0.0f;
	//float difZ = 0.0f;
	//float dist = 0.0f;
	//minimumRange *= minimumRange;
	//float mPosX = creature->actor.posX;
	//float mPosZ = creature->actor.posZ;
	//mapChannelClient_t **playerList = NULL;
	//creature_t  **creatureList      = NULL;

	//if (type == 0) 
	//{
	//	tCount = mapCell->ht_playerNotifyList.size();
	//	if( tCount > 0 )
	//		playerList = &mapCell->ht_playerNotifyList[0];

	//}
	//if (type == 1)
	//{
	//	//tCount = hashTable_getCount(&mapCell->ht_creatureList);
	//	//creatureList = (creature_t **)hashTable_getValueArray(&mapCell->ht_creatureList);
	//}	

	//// check players in range
	//for(sint32 i=0; i<tCount; i++)
	//{
	//	if( playerList == NULL) break; //no player found
	//	mapChannelClient_t *player = playerList[i];
	//	if(player->player->actor->stats.healthCurrent<=0) break;
	//	difX = (sint32)(player->player->actor->posX) - mPosX;
	//	difZ = (sint32)(player->player->actor->posZ) - mPosZ;
	//	dist = difX*difX + difZ*difZ;
	//	if( dist <= minimumRange && creature->actor.stats.healthCurrent > 0 )
	//	{
	//		//printf("Found player to attack\n");
	//		// set target and change state
	//		//friendly creatures dont attack player
	//		if (creature->faction == 0)
	//		{
	//			creature->controller.targetEntityId = player->clientEntityId;
	//			creature->controller.currentAction = BEHAVIOR_ACTION_FIGHTING;
	//			creature->controller.pathLength = 0;
	//			break;
	//		}
	//		//break;
	//	}
	//}//---for: playercount

	//// check other creatures in range
	//if(type == 0) return;
	//for(sint32 i2= 0; i2 < gridCount; i2++)
	//{
	//	//---dont check yourself
	//	if(creature->actor.entityId == entityPosGrid[i2][1])
	//	{
	//		continue;
	//	}
	//	// dont check dead
	//	creature_t* targetCreature = (creature_t*)entityMgr_get(entityPosGrid[i2][1]);
	//	if( targetCreature == NULL )
	//		continue;
	//	if(targetCreature->actor.stats.healthCurrent <= 0 )
	//		continue;
	//	//--check if in same map
	//	if(mapChannel->mapInfo->contextId == entityPosGrid[i2][0])
	//	{
	//		//difX = (sint32)creature ->actor.posX-entityPosGrid[i2][2];
	//		//difZ = (sint32)creature ->actor.posZ-entityPosGrid[i2][3];	

	//		difX = entityPosGrid[i2][2] - mPosX;
	//		difZ = entityPosGrid[i2][3] - mPosZ;
	//		dist = difX*difX + difZ*difZ;

	//		if(dist <= minimumRange)
	//		{
	//			if( creature->faction != entityPosGrid[i2][4] )
	//			{
	//				creature->controller.targetEntityId = entityPosGrid[i2][1];
	//				creature->controller.currentAction = BEHAVIOR_ACTION_FIGHTING;
	//				creature->controller.pathLength = 0;
	//				break;
	//			}
	//		}
	//	}
	//}
}

float controller_math_getDistanceSqr(float p1[3], float p2[3])
{
	float dx = p1[0] - p2[0];
	float dy = p1[1] - p2[1];
	float dz = p1[2] - p2[2];
	return dx*dx+dy*dy+dz*dz;
}

/*
 * Called every 250 milliseconds for every creature on the map
 * The tick parameter stores the amount of ms since the last call (should be 250 usually, but can go up on heavy load)
 */
void controller_creatureThink(mapChannel_t *mapChannel, creature_t *creature, sint32 tick, bool* updateIterator)
{
	*updateIterator = false; // hack to remove the creature from current map cell
	if (creature->actor.stats.healthCurrent <= 0) 	
		return; // creature dead
	creature->updatePositionCounter -= tick;
	if( creature->updatePositionCounter <= 0 )
	{
		// check for changed cell
		uint32 newLocX = (uint32)((creature->actor.posX / CELL_SIZE) + CELL_BIAS);
		uint32 newLocZ = (uint32)((creature->actor.posZ / CELL_SIZE) + CELL_BIAS);
		// calculate initial cell
		if( creature->actor.cellLocation.x != newLocX || creature->actor.cellLocation.z != newLocZ )
		{
			creature_cellUpdateLocation(mapChannel, creature, newLocX, newLocZ);
			*updateIterator = true;
		}
		creature->updatePositionCounter = CREATURE_LOCATION_UPDATE_TIME;
	}
	if(creature->controller.currentAction == BEHAVIOR_ACTION_WANDER )
	{
		// scan for enemy
		if( controller_checkForAttackableEntityInRange(mapChannel,creature,16.0f) )
		{
			// enemy found!
			return;
		}
		if(creature->wanderstate == WANDER_IDLE)
		{		
			//--- idle for int time before get new wander position
			uint32 resttime = GetTickCount();
			if( (resttime-creature->lastresttime) > 3500 )
			{
				creature->lastresttime = resttime;
				//calc target
				srand(GetTickCount());
				sint32 srndx = rand() % (sint32)creature->wander_dist;
				sint32 srndz = rand() % (sint32)creature->wander_dist;
				creature->controller.actionWander.wanderDestionation[0] = creature->homePos.x + (float)srndx;
				creature->controller.actionWander.wanderDestionation[1] = creature->homePos.y+1.0f;
				creature->controller.actionWander.wanderDestionation[2] = creature->homePos.z + (float)srndz;			
				//next step approaching
				creature->wanderstate = WANDER_MOVING;
			}
		}	
		if(creature->wanderstate == WANDER_MOVING)
		{   
			// following path
			if( creature->controller.pathLength == 0 )
			{
				if( creature->actor.posX != creature->actor.posX )
					__debugbreak();
				// no path, generate new one
				float startPos[3];
				float endPos[3];
				startPos[0] = creature->actor.posX;
				startPos[1] = creature->actor.posY;
				startPos[2] = creature->actor.posZ;
				endPos[0] = creature->controller.actionWander.wanderDestionation[0];
				endPos[1] = creature->controller.actionWander.wanderDestionation[1];
				endPos[2] = creature->controller.actionWander.wanderDestionation[2];	
				creature->controller.pathIndex = 0;
				creature->controller.pathLength = navmesh_getPath(mapChannel, startPos, endPos, creature->controller.path, false);
				if( creature->controller.pathLength == 0 )
				{
					// path could not be generated or too short
					// leave state and go idle mode
					creature->wanderstate = WANDER_IDLE;
					return;
				}
			}
			// get distance
			float* nextPathNodePos = creature->controller.path+(creature->controller.pathIndex*3);
			float difX = nextPathNodePos[0] - creature->actor.posX;
			float difY = nextPathNodePos[1] - creature->actor.posY;
			float difZ = nextPathNodePos[2] - creature->actor.posZ;
			float dist = difX*difX + difZ*difZ;
			//wander target location reached
			if( dist > 0.01f ) // to avoid division by zero
				updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.29f,true);
			if(dist < 0.8f) 
			{
				creature->controller.pathIndex++; // goto next node
				if( creature->controller.pathIndex >= creature->controller.pathLength )
				{
					creature->controller.pathLength = 0;
					creature->wanderstate = 0;
					return;
				}
			}

		}
	}
	else if(creature->controller.currentAction == BEHAVIOR_ACTION_FIGHTING )
	{
		// get target
		void *target = entityMgr_get(creature->controller.actionFighting.targetEntityId);
		if( target == NULL )
		{
			// target disappeared (player logout or deleted for some reason), leave combat mode
			controller_setActionWander(creature);
			return;
		}
		// leave combat after 
		sint32 aggtime = GetTickCount();
		if(aggtime - creature->lastagression > creature->agression)
		{
			controller_setActionWander(creature);
			return;
		}			
		// get position of target
		float targetX = 0.0f;
		float targetY = 0.0f;
		float targetZ = 0.0f;
		if( entityMgr_getEntityType(creature->controller.actionFighting.targetEntityId) == ENTITYTYPE_CLIENT )
		{
			mapChannelClient_t *client = (mapChannelClient_t*)target;
			// if target dead, set wander state
			if( client->player->actor->stats.healthCurrent <= 0 )
			{
				controller_setActionWander(creature);
				return;
			}
			targetX = client->player->actor->posX;
			targetY = client->player->actor->posY;
			targetZ = client->player->actor->posZ;
		}
		else if( entityMgr_getEntityType(creature->controller.actionFighting.targetEntityId) == ENTITYTYPE_CREATURE )
		{  
			creature_t  *targetCreature = (creature_t*)target;
			if( (targetCreature->actor.stats.healthCurrent <= 0) )
			{
				controller_setActionWander(creature);
				return;
			}
			targetX = targetCreature->actor.posX;
			targetY = targetCreature->actor.posY;
			targetZ = targetCreature->actor.posZ;
		}
		else
			__debugbreak(); // todo

		float targetDistX = (targetX - creature->actor.posX);
		float targetDistY = (targetY - creature->actor.posY);
		float targetDistZ = (targetZ - creature->actor.posZ);
		float targetDistSqr = (targetDistX*targetDistX+targetDistY*targetDistY+targetDistZ*targetDistZ);

		//if( (creature->attack_habbit == 0 && dist > creature->range) || (creature->attack_habbit == 1 ))
		//{
		//	//updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.91f,true);
		//}

		// We dont use creature->attack_habbit anymore. Instead we check range dmg > 0
		// prefer melee attack (if possible)
		uint32 time = GetTickCount();
		float meleeRange = 1.9f;
		if( creature->type->meleeAttack.damageMax != 0 && targetDistSqr >= 0.8f && targetDistSqr <= meleeRange*meleeRange )
		{
			creature->lastagression = time;
			if( ((time-creature->lastattack) > creature->attackspeed) )
			{
				// rotate
				updateEntityMovementW2(targetDistX,targetDistY,targetDistZ,creature,mapChannel,0.0f,false);
				//__debugbreak();
				creature->lastattack = time;
				sint32 dmg = creature->type->meleeAttack.damageMin + (rand()%(creature->type->meleeAttack.damageMax-creature->type->meleeAttack.damageMin+1));
				missile_launch(mapChannel, &creature->actor, creature->controller.actionFighting.targetEntityId, creature->type->meleeAttack.missile, dmg);
			}
			return;
		}
		// try range attack
		if( creature->type->rangeAttack.damageMax != 0 && targetDistSqr >= 0.9f && targetDistSqr <= creature->range*creature->range )
		{
			creature->lastagression = time;
			if( ((time-creature->lastattack) > creature->attackspeed) )
			{
				// rotate
				updateEntityMovementW2(targetDistX,targetDistY,targetDistZ,creature,mapChannel,0.0f,false);
				//__debugbreak();
				creature->lastattack = time;
				sint32 dmg = creature->type->rangeAttack.damageMin + (rand()%(creature->type->rangeAttack.damageMax-creature->type->rangeAttack.damageMin+1));
				missile_launch(mapChannel, &creature->actor, creature->controller.actionFighting.targetEntityId, creature->type->rangeAttack.missile, dmg);
			}
			return;
		}
		// after checking for melee and range attack without success, do pathing
		// invalidate path if the target moved away too far from the original path destination
		float tempPos[3];
		tempPos[0] = targetX;
		tempPos[1] = targetY;
		tempPos[2] = targetZ;
		if( creature->controller.pathLength > 0 && controller_math_getDistanceSqr(creature->controller.actionFighting.lockedTargetPosition, tempPos) > 1.0f )
		{
			creature->controller.pathLength = 0;
		}
		// generate path if there is no current
		if( creature->controller.pathLength == 0 )
		{		
			float pathTarget[3];
			if( targetDistSqr < 0.1f )
			{
				// if too near, move out of enemy by running to random point somewhere x units around the creature
				float angle = ((float)rand() / 32767.0f) * 6.28318f; // random angle
				float distance = 1.0f; // keep 1 meter distance
				pathTarget[0] = targetX + cos(angle) * distance;
				pathTarget[1] = targetY;
				pathTarget[2] = targetZ + sin(angle) * distance;
			}
			else
			{
				// run to nearest point that maintains distance to creature
				float vecV2A[2]; // vector2D victim->attacker
				vecV2A[0] = -targetDistX;
				vecV2A[1] = -targetDistZ;
				// normalize
				float vecV2ALen = sqrt(targetDistSqr);
				vecV2A[0] /= vecV2ALen;
				vecV2A[1] /= vecV2ALen;
				// use vector to calculate nearest melee point from our current position
				float distance = 1.0f; // keep 1 meter distance
				pathTarget[0] = targetX + vecV2A[0] * distance;
				pathTarget[1] = targetY;
				pathTarget[2] = targetZ + vecV2A[1] * distance;
			}
			float startPos[3];
			float endPos[3];
			startPos[0] = creature->actor.posX;
			startPos[1] = creature->actor.posY;
			startPos[2] = creature->actor.posZ;
			endPos[0] = pathTarget[0];
			endPos[1] = pathTarget[1];
			endPos[2] = pathTarget[2];	
			creature->controller.pathIndex = 0;
			creature->controller.pathLength = navmesh_getPath(mapChannel, startPos, endPos, creature->controller.path, false);
			if( creature->controller.path == 0 )
			{
				printf("Cannot find path");
				return;
			}
			// also update path target variable (using creature position, not path target position)
			creature->controller.actionFighting.lockedTargetPosition[0] = targetX;
			creature->controller.actionFighting.lockedTargetPosition[1] = targetY;
			creature->controller.actionFighting.lockedTargetPosition[2] = targetZ;
		}
		// follow path
		if( creature->controller.pathIndex < creature->controller.pathLength )
		{
			// get distance
			float* nextPathNodePos = creature->controller.path+(creature->controller.pathIndex*3);
			float difX = nextPathNodePos[0] - creature->actor.posX;
			float difY = nextPathNodePos[1] - creature->actor.posY;
			float difZ = nextPathNodePos[2] - creature->actor.posZ;
			float dist = difX*difX + difZ*difZ;
			if( dist > 0.01f ) // to avoid division by zero
				updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.85f,true);
			if(dist < 0.4f) 
			{
				creature->controller.pathIndex++; // goto next node
				if( creature->controller.pathIndex >= creature->controller.pathLength )
				{
					creature->controller.pathLength = 0;
					creature->controller.pathIndex = 0;
				}
			}
		}



		//	//########### update rotation ###############	
		//	float difXR = targetX - creature->actor.posX;
		//	float difYR = targetY - creature->actor.posY;
		//	float difZR = targetZ - creature->actor.posZ;
		//	float distR = difXR*difXR + difZR*difZR;

		//	if( creature->controller.pathLength > 0 )
		//	{
		//		// check if we need to invalidate path because target is too far from path target
		//		float pathTargetDistX = targetX - creature->controller.actionFighting.pathTarget[0];
		//		//float pathDistY = targetY - creature->controller.actionFighting.pathTarget[1]; - y is not important (for now at least)
		//		float pathTargetDistZ = targetZ - creature->controller.actionFighting.pathTarget[2];
		//		float pathTargetDist = pathTargetDistX*pathTargetDistX + pathTargetDistZ*pathTargetDistZ;
		//		if( pathTargetDist >= 2.0f )
		//		{
		//			// target too far away from original path target, invalidate current path
		//			creature->controller.pathLength = 0;
		//		}
		//	}

		//	if( (distR <= 1.0f || distR >= 3.0f) && creature->controller.pathLength == 0 )
		//	{
		//		// distance too low or too high, build path to enemy
		//		float pathTarget[3];
		//		if( distR < 0.1f )
		//		{
		//			// if too near, move out of enemy by running to random point somewhere x units around the creature
		//			float angle = ((float)rand() / 32767.0f) * 6.28318f; // random angle
		//			float distance = 1.0f; // keep 1 meter distance
		//			pathTarget[0] = targetX + cos(angle) * distance;
		//			pathTarget[1] = targetY;
		//			pathTarget[2] = targetZ + sin(angle) * distance;
		//		}
		//		else
		//		{
		//			// run to nearest point that maintains distance to creature
		//			float vecV2A[2]; // vector2D victim->attacker
		//			vecV2A[0] = -difXR;
		//			vecV2A[1] = -difZR;
		//			// normalize
		//			float vecV2ALen = sqrt(distR);
		//			vecV2A[0] /= vecV2ALen;
		//			vecV2A[1] /= vecV2ALen;
		//			// use vector to calculate nearest melee point from our current position
		//			float distance = 1.0f; // keep 1 meter distance
		//			pathTarget[0] = targetX + vecV2A[0] * distance;
		//			pathTarget[1] = targetY;
		//			pathTarget[2] = targetZ + vecV2A[1] * distance;
		//		}
		//		float startPos[3];
		//		float endPos[3];
		//		startPos[0] = creature->actor.posX;
		//		startPos[1] = creature->actor.posY;
		//		startPos[2] = creature->actor.posZ;
		//		endPos[0] = pathTarget[0];
		//		endPos[1] = pathTarget[1];
		//		endPos[2] = pathTarget[2];	
		//		creature->controller.pathIndex = 0;
		//		creature->controller.pathLength = navmesh_getPath(mapChannel, startPos, endPos, creature->controller.path, false);
		//		if( creature->controller.path == 0 )
		//			__debugbreak();
		//		// also update path target variable
		//		creature->controller.actionFighting.pathTarget[0] = targetX;
		//		creature->controller.actionFighting.pathTarget[1] = targetY;
		//		creature->controller.actionFighting.pathTarget[2] = targetZ;
		//	}
		//	//if( distR > 0.01f ) // to avoid division by zero
		//	//	updateEntityMovementW2(difXR,difYR,difZR,creature,mapChannel,0.0f,false);

		//	// follow path
		//	if( creature->controller.pathIndex < creature->controller.pathLength )
		//	{
		//		// get distance
		//		float* nextPathNodePos = creature->controller.path+(creature->controller.pathIndex*3);
		//		float difX = nextPathNodePos[0] - creature->actor.posX;
		//		float difY = nextPathNodePos[1] - creature->actor.posY;
		//		float difZ = nextPathNodePos[2] - creature->actor.posZ;
		//		float dist = difX*difX + difZ*difZ;
		//		if( dist > 0.01f ) // to avoid division by zero
		//			updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.85f,true);
		//		if(dist < 0.8f) 
		//		{
		//			creature->controller.pathIndex++; // goto next node
		//			if( creature->controller.pathIndex >= creature->controller.pathLength )
		//			{
		//				creature->controller.pathLength = 0;
		//			}
		//		}
		//	}
		//	
		//	//########### distance check #################
		//	uint32 resttime = GetTickCount();

		//	//if( (creature->movestate == 0) && ((resttime-creature->lastresttime) > 1500 ) )
		//	//{

		//	//	creature->lastresttime = resttime;	
		//	//	//---check distance to other entitys
		//	//	for(sint32 i= 0; i < gridCount; i++)
		//	//	{
		//	//		//---dont check yourself
		//	//		if(creature->actor.entityId == entityPosGrid[i][1])
		//	//		{
		//	//			continue;
		//	//		}

		//	//		//--check if in same map

		//	//		if(mapChannel->mapInfo->contextId == entityPosGrid[i][0])
		//	//		{
		//	//			difX = (sint32)creature ->actor.posX-entityPosGrid[i][2];
		//	//			difZ = (sint32)creature ->actor.posZ-entityPosGrid[i][3];	

		//	//			if( (abs(difX) <2) && (abs(difZ) < 2) )
		//	//			{
		//	//				srand(GetTickCount());
		//	//				sint32 sgnrnd = rand() % 2;
		//	//				sint32 sign = (sgnrnd == 0) ? -1: 1;
		//	//				sint32 srnd1 = rand() % 7;
		//	//				creature->wx = creature->actor.posX + (float)(srnd1 * sign);
		//	//				sgnrnd = rand() % 2;
		//	//				sign = (sgnrnd == 0) ? -1: 1;
		//	//				srnd1 = rand() % 7;
		//	//				creature->wz = creature->actor.posZ + (float)(srnd1 * sign);	
		//	//				creature->wy = targetY;
		//	//				creature->movestate = 1;
		//	//			}//---if entity to close

		//	//		}//---if: same mapid
		//	//	}//--- check whole grid
		//	//}//---movestate == 0
		//	//if(creature->movestate == 1)
		//	//{

		//	//	float difX1 = creature->wx - creature->actor.posX;
		//	//	float difZ1 = creature->wz - creature->actor.posZ;
		//	//	float dist1 = difX1*difX1 + difZ1*difZ1;
		//	///*	if( dist1 > 0.01f )
		//	//		updateEntityMovementW2(difX1,0.0f,difZ1,creature,mapChannel,0.85f,true);*/
		//	//	if(dist1 < 1.0f) 
		//	//	{

		//	//		creature->movestate = 0;
		//	//	}
		//	//	return;
		//	//}//---movestate == 1

		//	// get distance			
		//	difX = targetX - creature->actor.posX;
		//	difY = targetY - creature->actor.posY;
		//	difZ = targetZ - creature->actor.posZ;
		//	float dist = difX*difX + difZ*difZ;	

		//	float tpx,tpy,tpz;

		//	float meeleRange = 1.5f;
		//	meeleRange *= meeleRange;		 

		//	//---if player inside attack range update aggressiontimer
		//	if( (dist < meeleRange) || (dist < creature -> range) )
		//		creature->lastagression = GetTickCount();

		//	if( dist < meeleRange )
		//	{
		//		if( dist > 0.01f )
		//			updateEntityMovementW2(difX,0.0f,difZ,creature,mapChannel,0.0f,false); // rotate
		//		//melee attack (every x sec)
		//		uint32 time2 = GetTickCount();
		//		if( (time2-creature->lastattack) > creature->attackspeed && (dist < creature-> range) )
		//		{
		//			//__debugbreak();
		//			creature->lastattack = time2;			
		//			missile_launch(mapChannel, &creature->actor, creature->controller.targetEntityId, creature->type->MeleeMissile, 2);
		//		}
		//	}
		//	else
		//	{

		//		if( (creature->attack_habbit == 0 && dist > creature -> range) || (creature->attack_habbit == 1 ))
		//		{
		//			//updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.91f,true);
		//		}
		//		//range attack (every x sec)
		//		uint32 time = GetTickCount();
		//		if( ((time-creature->lastattack) > creature->attackspeed) && (dist <= creature-> range) )
		//		{
		//			//__debugbreak();
		//			creature->lastattack = time;
		//			//if( creature->controller.targetEntityId )

		//			if(creature->attack_habbit == 0)
		//				missile_launch(mapChannel, &creature->actor, creature->controller.targetEntityId, creature->type->RangeMissile, 3);
		//		}
		//	}//---range attack and movement
		//}
		//else
		//{
		//	// target disappeared, leave combat mode
		//	creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
		//	creature->wanderstate = 0;
		//}//---nor target: -> wandering
	}//---fighting
}


/*
 * Called every 250 milliseconds
 */
void controller_mapChannelThink(mapChannel_t *mapChannel)
{
	// todo: When on heavy load, the server should increase the time between calls to
	//       this function. (check player updating as a reference)
	for(sint32 i=0; i<mapChannel->mapCellInfo.loadedCellCount; i++)
	{
		mapCell_t *mapCell = mapChannel->mapCellInfo.loadedCellList[i];
		if( mapCell == NULL ) // should never happen, but still do a check for safety
			continue;
		// npcs
		if( mapCell->ht_npcList.empty() != true )
		{
			npc_t **npcList = &mapCell->ht_npcList[0];
			sint32 npcCount = mapCell->ht_npcList.size();
			for(sint32 f=0; f<npcCount; f++)
				controller_npcThink(mapChannel, npcList[f]);
		}
		// creatures
		if( mapCell->ht_creatureList.empty() != true )
		{
			creature_t **creatureList = &mapCell->ht_creatureList[0];
			sint32 creatureCount = mapCell->ht_creatureList.size();
			for(sint32 f=0; f<creatureCount; f++)
			{
				bool updateIterator = false;
				controller_creatureThink(mapChannel, creatureList[f], 250, &updateIterator); // update time hardcoded, see todo
				if( updateIterator )
				{
					// not so nice hack to remove creatures from the map cell when creature_cellUpdateLocation is called
					std::swap(mapCell->ht_creatureList.at(f), mapCell->ht_creatureList.at(creatureCount-1));
					mapCell->ht_creatureList.pop_back();
					creatureCount = mapCell->ht_creatureList.size();
					if( creatureCount == 0 )
						break;
					creatureList = &mapCell->ht_creatureList[0];
					f--;
					continue;
				}
			}
		}
	}
}

void controller_initForMapChannel(mapChannel_t *mapChannel)
{

}

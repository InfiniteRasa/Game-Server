#include "Global.h"
#include <time.h>
/*
Behavior controller

responsible for:
npc and creature movement, decisions, combat or any actions.
*/

extern sint32 gridL1;
extern sint32 gridL2;
extern sint32 gridCount;
extern sint32** entityPosGrid;

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

	//--grid update


}


void checkForEntityInRange(mapChannel_t *mapChannel, creature_t *creature,mapCell_t *mapCell,sint32 type)
{

	sint32 tCount =0;
	float minimumRange = 16.1f;
	float difX = 0.0f;
	float difY = 0.0f;
	float difZ = 0.0f;
	float dist = 0.0f;
	minimumRange *= minimumRange;
	float mPosX = creature->actor.posX;
	float mPosZ = creature->actor.posZ;
	mapChannelClient_t **playerList = NULL;
	creature_t  **creatureList      = NULL;

	if (type == 0) 
	{
		tCount = mapCell->ht_playerNotifyList.size();
		if( tCount > 0 )
			playerList = &mapCell->ht_playerNotifyList[0];

	}
	if (type == 1)
	{
		//tCount = hashTable_getCount(&mapCell->ht_creatureList);
		//creatureList = (creature_t **)hashTable_getValueArray(&mapCell->ht_creatureList);
	}	

	// check players in range
	for(sint32 i=0; i<tCount; i++)
	{
		if( playerList == NULL) break; //no player found
		mapChannelClient_t *player = playerList[i];
		if(player->player->actor->stats.healthCurrent<=0) break;
		difX = (sint32)(player->player->actor->posX) - mPosX;
		difZ = (sint32)(player->player->actor->posZ) - mPosZ;
		dist = difX*difX + difZ*difZ;
		if( dist <= minimumRange && creature->actor.stats.healthCurrent > 0 )
		{
			//printf("Found player to attack\n");
			// set target and change state
			//friendly creatures dont attack player
			if (creature->faction == 0)
			{
				creature->controller.targetEntityId = player->clientEntityId;
				creature->controller.currentAction = BEHAVIOR_ACTION_FIGHTING;
				break;
			}
			//break;
		}
	}//---for: playercount

	// check other creatures in range
	if(type == 0) return;
	for(sint32 i2= 0; i2 < gridCount; i2++)
	{
		//---dont check yourself
		if(creature->actor.entityId == entityPosGrid[i2][1])
		{
			continue;
		}
		// dont check dead
		if(creature->actor.stats.healthCurrent <= 0 )
			continue;
		//--check if in same map
		if(mapChannel->mapInfo->contextId == entityPosGrid[i2][0])
		{
			//difX = (sint32)creature ->actor.posX-entityPosGrid[i2][2];
			//difZ = (sint32)creature ->actor.posZ-entityPosGrid[i2][3];	

			difX = entityPosGrid[i2][2] - mPosX;
			difZ = entityPosGrid[i2][3] - mPosZ;
			dist = difX*difX + difZ*difZ;

			if(dist <= minimumRange)
			{
				if( creature->faction != entityPosGrid[i2][4] )
				{
					creature->controller.targetEntityId = entityPosGrid[i2][1];
					creature->controller.currentAction = BEHAVIOR_ACTION_FIGHTING;
					break;
				}
			}
		}
	}
	/*
	for(sint32 i2=0; i2<tCount; i2++)
	{
	if(creatureList == NULL) break; // no creatures found
	creature_t  *crea = creatureList[i2];
	if( tCount == i2) break;
	if(crea->currentHealth<=0) continue;
	if(crea->actor.entityId == creature->actor.entityId) continue; //skip if same creature
	difX = crea->actor.posX - mPosX;
	difZ = crea->actor.posZ - mPosZ;
	dist = difX*difX + difZ*difZ;

	if( creature->currentHealth >0 )
	{
	//__debugbreak();
	// attack only other factional creatures
	if( crea->faction != creature->faction )
	{
	printf("found enemy creature entity: %d  class: %d \n",
	crea->actor.entityId, crea->actor.entityClassId);
	creature->controller.targetEntityId = crea->actor.entityId;
	creature->controller.currentAction = BEHAVIOR_ACTION_FIGHTING;
	break;
	}
	//break;
	}//---if
	}//---for:creatureCount
	*/
}

/*
* Called every 250 milliseconds for every creature on the map
*/

//todo: make walkingspeed, rotationspeed variable(fromd or textfile) 
void controller_creatureThink(mapChannel_t *mapChannel, creature_t *creature)
{
	//---dahrkael @ 8046b75
	if (creature->actor.stats.healthCurrent <= 0) 	
	{ return; } 	

	//---update entity position grid
	for(sint32 i= 0; i < gridCount; i++)
	{
		if(creature->actor.entityId == entityPosGrid[i][1])
		{
			entityPosGrid[i][2] = (sint32)creature->actor.posX;  
			entityPosGrid[i][3] = (sint32)creature->actor.posZ;   
		}
	}

	if(creature->controller.currentAction == BEHAVIOR_ACTION_WANDER )
	{
		//calc new target location
		if(creature->wanderstate == 0)
		{		
			// calculate next node
			mapCell_t *mapCell = cellMgr_tryGetCell(mapChannel, creature->actor.cellLocation.x, creature->actor.cellLocation.z);
			if( mapCell )
			{
				//__debugbreak();
				checkForEntityInRange(mapChannel,creature,mapCell,1); //creature
				checkForEntityInRange(mapChannel,creature,mapCell,0); //player
				//---skip wandering if enemy found
				if(creature->controller.currentAction == BEHAVIOR_ACTION_FIGHTING)
				{
					creature->lastagression = GetTickCount();
					return;
				}
			}//--cellwide search

			//--- idle for int time before get new wander position
			uint32 resttime = GetTickCount();
			if( (resttime-creature->lastresttime) > 3500 )
			{
				//__debugbreak();
				/*printf("entity: %d x:%f y:%f z:%f \n",
				creature->actor.entityId,creature->actor.posX,creature->actor.posY,creature->actor.posZ); */
				creature->lastresttime = resttime;
				//calc target
				srand(GetTickCount());
				// creature->wander_dist = 40; // test
				sint32 srndx = rand() % (sint32)creature->wander_dist;
				sint32 srndz = rand() % (sint32)creature->wander_dist;
				creature->wx = creature->homePos.x + (float)srndx;
				creature->wy = creature->homePos.y+1.0f;
				creature->wz = creature->homePos.z + (float)srndz;			
				//next step approaching
				creature->wanderstate = 1;
			}

		}	
		if(creature->wanderstate == 1)
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
				endPos[0] = creature->wx;
				endPos[1] = creature->wy;
				endPos[2] = creature->wz;	
				creature->controller.pathIndex = 0;
				creature->controller.pathLength = navmesh_getPath(mapChannel, startPos, endPos, creature->controller.path, false);
				// printf("creature: Calculated path with %d nodes\n", creature->controller.pathLength);
				if( creature->controller.pathLength == 0 )
				{
					// path could not be generated or too short
					// leave state and go idle mode
					creature->wanderstate = 0;
					return;
				}
				
			}
			mapCell_t *mapCell = cellMgr_tryGetCell(mapChannel, creature->actor.cellLocation.x, creature->actor.cellLocation.z);
			if( mapCell )
			{
				//__debugbreak();
				checkForEntityInRange(mapChannel,creature,mapCell,1); //creature
				checkForEntityInRange(mapChannel,creature,mapCell,0); //player
				//---skip wandering if enemy found
				if(creature->controller.currentAction == BEHAVIOR_ACTION_FIGHTING)
				{
					creature->lastagression = GetTickCount();
					creature->controller.pathLength = 0; // remove path
					return;
				}


			}//--cellwide search

			// get distance
			float* nextPathNodePos = creature->controller.path+(creature->controller.pathIndex*3);
			float difX = nextPathNodePos[0] - creature->actor.posX;
			float difY = nextPathNodePos[1] - creature->actor.posY;
			float difZ = nextPathNodePos[2] - creature->actor.posZ;
			float dist = difX*difX + difZ*difZ;

			//wander targetlocation reached

			if( dist > 0.01f ) // to avoid division by zero
				updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.29f,true);
			if(dist < 2.0f) 
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

		sint32 difX;
		sint32 difY;
		sint32 difZ; 
		//__debugbreak();
		if ( creature->actor.stats.healthCurrent <=0)
			return;
		// get target
		void *target = entityMgr_get(creature->controller.targetEntityId);

		if( target )
		{

			//--get out of combat
			sint32 aggtime = GetTickCount();
			if(aggtime - creature->lastagression > creature->agression)
			{
				creature->controller.currentAction =  BEHAVIOR_ACTION_WANDER;
				creature->wanderstate = 0;
				return;
			}			

			// get position of target
			float targetX = 0.0f;
			float targetY = 0.0f;
			float targetZ = 0.0f;
			if( entityMgr_getEntityType(creature->controller.targetEntityId) == ENTITYTYPE_CLIENT )
			{
				mapChannelClient_t *client = (mapChannelClient_t*)target;
				// if target dead, set wander state
				if( client->player->actor->stats.healthCurrent <= 0 )
				{
					creature->wanderstate = 0;
					creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
					return;
				}
				targetX = client->player->actor->posX;
				targetY = client->player->actor->posY;
				targetZ = client->player->actor->posZ;
			}
			else if( entityMgr_getEntityType(creature->controller.targetEntityId) == ENTITYTYPE_CREATURE )
			{  
				creature_t  *targetCreature = (creature_t*)target;
				if( (targetCreature->actor.stats.healthCurrent <= 0) )
				{
					creature->wanderstate = 0;
					creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
					return;
				}
				targetX = targetCreature->actor.posX;
				targetY = targetCreature->actor.posY;
				targetZ = targetCreature->actor.posZ;
			}
			else
				__debugbreak(); // todo


			//########### update rotation ###############	
			float difXR = targetX - creature->actor.posX;
			float difYR = targetY - creature->actor.posY;
			float difZR = targetZ - creature->actor.posZ;
			float distR = difXR*difXR + difZR*difZR;	
			if( distR > 0.01f ) // to avoid division by zero
				updateEntityMovementW2(difXR,difYR,difZR,creature,mapChannel,0.0f,false);


			//########### distance check #################
			uint32 resttime = GetTickCount();

			if( (creature->movestate == 0) && ((resttime-creature->lastresttime) > 1500 ) )
			{

				creature->lastresttime = resttime;	
				//---check distance to other entitys
				for(sint32 i= 0; i < gridCount; i++)
				{
					//---dont check yourself
					if(creature->actor.entityId == entityPosGrid[i][1])
					{
						continue;
					}

					//--check if in same map

					if(mapChannel->mapInfo->contextId == entityPosGrid[i][0])
					{
						difX = (sint32)creature ->actor.posX-entityPosGrid[i][2];
						difZ = (sint32)creature ->actor.posZ-entityPosGrid[i][3];	

						if( (abs(difX) <2) && (abs(difZ) < 2) )
						{
							srand(GetTickCount());
							sint32 sgnrnd = rand() % 2;
							sint32 sign = (sgnrnd == 0) ? -1: 1;
							sint32 srnd1 = rand() % 7;
							creature->wx = creature->actor.posX + (float)(srnd1 * sign);
							sgnrnd = rand() % 2;
							sign = (sgnrnd == 0) ? -1: 1;
							srnd1 = rand() % 7;
							creature->wz = creature->actor.posZ + (float)(srnd1 * sign);	
							creature->wy = targetY;
							creature->movestate = 1;
						}//---if entity to close

					}//---if: same mapid
				}//--- check whole grid
			}//---movestate == 0
			if(creature-> movestate == 1)
			{

				float difX1 = creature->wx - creature->actor.posX;
				float difZ1 = creature->wz - creature->actor.posZ;
				float dist1 = difX1*difX1 + difZ1*difZ1;
				if( dist1 > 0.01f )
					updateEntityMovementW2(difX1,0.0f,difZ1,creature,mapChannel,0.85f,true);
				if(dist1 < 1.0f) 
				{

					creature->movestate = 0;
				}
				return;
			}//---movestate == 1

			// get distance			
			float difX = targetX - creature->actor.posX;
			float difY = targetY - creature->actor.posY;
			float difZ = targetZ - creature->actor.posZ;
			float dist = difX*difX + difZ*difZ;	

			float tpx,tpy,tpz;

			float meeleRange = 1.5f;
			meeleRange *= meeleRange;		 

			//---if player inside attack range update aggressiontimer
			if( (dist < meeleRange) || (dist < creature -> range) )
				creature->lastagression = GetTickCount();

			if( dist < meeleRange )
			{
				//rangeattack (every x sec)
				uint32 time2 = GetTickCount();
				if( (time2-creature->lastattack) > creature->attackspeed && (dist < creature-> range) )
				{
					//__debugbreak();
					creature->lastattack = time2;			
					missile_launch(mapChannel, &creature->actor, creature->controller.targetEntityId, creature->type->MeleeMissile, 2);
				}
			}
			else
			{

				if( (creature->attack_habbit == 0 && dist > creature -> range) || (creature->attack_habbit == 1 ))
				{
					updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.91f,true);
				}
				//rangeattack (every x sec)
				uint32 time = GetTickCount();
				if( ((time-creature->lastattack) > creature->attackspeed) && (dist <= creature-> range) )
				{
					//__debugbreak();
					creature->lastattack = time;
					//if( creature->controller.targetEntityId )

					if(creature->attack_habbit == 0)
						missile_launch(mapChannel, &creature->actor, creature->controller.targetEntityId, creature->type->RangeMissile, 3);
				}
			}//---range attack and movement
		}
		else
		{
			// target disappeared, leave combat mode
			creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
		}//---nor target: -> wandering
	}//---fighting
}


/*
 * Called every 250 milliseconds
 */
void controller_mapChannelThink(mapChannel_t *mapChannel)
{
	for(sint32 i=0; i<mapChannel->mapCellInfo.loadedCellCount; i++)
	{
		mapCell_t *mapCell = mapChannel->mapCellInfo.loadedCellList[i];
		if( mapCell == NULL ) // should never happen, but still do a check for saftey
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
				controller_creatureThink(mapChannel, creatureList[f]);
		}
	}
}

void controller_initForMapChannel(mapChannel_t *mapChannel)
{

}

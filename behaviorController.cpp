#include "Global.h"

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

void updateEntityMovementW2 (float difX,float difY,float difZ,creature_t *creature,mapChannel_t *mapChannel,float speeddiv)
{

	float length = 1.0f / sqrt(difX*difX + difY*difY + difZ*difZ);
	difX *= length;
	difY *= length;
	difZ *= length;

	float vX = atan2(-difX, -difZ);
	// multiplicate with speed
	float velocity = creature->velocity * speeddiv;
	float speed = velocity * (250.0f / 1000.0f);
	float rspeed = creature->rotspeed * (250.0f / 1000.0f);
	difX *= speed;
	difY *= speed;
	difZ *= speed;
	// move unit
	creature->actor.posX += difX;
	creature->actor.posY += difY;
	creature->actor.posZ += difZ;
	// send movement update
	netCompressedMovement_t movement;
	movement.entityId = creature->actor.entityId;
	movement.posX24b = (int)(creature->actor.posX*256.0f);
	movement.posY24b = (int)(creature->actor.posY*256.0f);
	movement.posZ24b = (int)(creature->actor.posZ*256.0f);
	movement.flag = 0x08;
	movement.viewX = (unsigned short)(short)(vX*10240.0f);
	movement.viewY = 0;//(unsigned short)(short)(vX*10240.0f);//(int)(calcAngle(difX, difZ)*1024.0f);
	movement.velocity = (int)(velocity * 1024.0f);
	netMgr_cellDomain_sendEntityMovement(mapChannel, &creature->actor, &movement);
				

}


void checkForEntityInRange(mapChannel_t *mapChannel, creature_t *creature,mapCell_t *mapCell,int type)
{
  
	int tCount =0;
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
		tCount = hashTable_getCount(&mapCell->ht_playerNotifyList);
		playerList = (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList);
	   
	}
	if (type == 1)
	{
		tCount = hashTable_getCount(&mapCell->ht_creatureList);
		creatureList = (creature_t **)hashTable_getValueArray(&mapCell->ht_creatureList);
	
	}	

	// check players in range
	for(int i=0; i<tCount; i++)
	{
		if( playerList == NULL) break; //no player found
		mapChannelClient_t *player = playerList[i];
		if(player->player->actor->stats.healthCurrent<=0) break;
		difX = (int)(player->player->actor->posX) - mPosX;
		difZ = (int)(player->player->actor->posZ) - mPosZ;
		dist = difX*difX + difZ*difZ;
		if( dist <= minimumRange && creature->currentHealth > 0 )
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
	for(int i2=0; i2<tCount; i2++)
	{
		if(creatureList == NULL) break; // no creatures found
		creature_t  *crea = creatureList[i2];
		if( tCount == i2) break;
		if(crea->currentHealth<=0) continue;
		if(crea->actor.entityId == creature->actor.entityId) continue; //skip if same creature
		difX = (int)(crea->actor.posX) - mPosX;
		difZ = (int)(crea->actor.posZ) - mPosZ;
		dist = difX*difX + difZ*difZ;
	
		if( dist <= minimumRange && creature->currentHealth >0 )
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
}

/*
 * Called every 250 milliseconds for every creature on the map
 */

//todo: make walkingspeed, rotationspeed variable(fromd or textfile) 
void controller_creatureThink(mapChannel_t *mapChannel, creature_t *creature)
{
	 	

	if(creature->controller.currentAction == BEHAVIOR_ACTION_WANDER )
	{
	   //__debugbreak();
           //calc new target location
	       if(creature->wanderstate == 0)
		   {		    
	
				mapCell_t *mapCell = cellMgr_tryGetCell(mapChannel, creature->actor.cellLocation.x, creature->actor.cellLocation.z);
				if( mapCell )
				{
					//__debugbreak();
					checkForEntityInRange(mapChannel,creature,mapCell,1); //creature
					checkForEntityInRange(mapChannel,creature,mapCell,0); //player
					//---skip wandering if enemy found
	                if(creature->controller.currentAction == BEHAVIOR_ACTION_FIGHTING)
						return;
				}//--cellwide search
			   
			    //--- idle for short time before get new wander position
				unsigned int resttime = GetTickCount();
				if( (resttime-creature->lastresttime) > 4500 )
				{
					//__debugbreak();
					/*printf("entity: %d x:%f y:%f z:%f \n",
					creature->actor.entityId,creature->actor.posX,creature->actor.posY,creature->actor.posZ); */
					creature->lastresttime = resttime;
					//calc target
					srand(GetTickCount());
					int srnd = rand() % (int)creature->wander_dist;
					creature->wx = creature->homePos.x + (float)srnd;
					creature->wy = creature->homePos.y;
					creature->wz = creature->homePos.z + (float)srnd;			
					//next step approaching
					creature->wanderstate = 1;
				}
				
		   }	
		  if(creature->wanderstate == 1)
		  {   
				
			    mapCell_t *mapCell = cellMgr_tryGetCell(mapChannel, creature->actor.cellLocation.x, creature->actor.cellLocation.z);
				if( mapCell )
				{
					//__debugbreak();
					checkForEntityInRange(mapChannel,creature,mapCell,1); //creature
					checkForEntityInRange(mapChannel,creature,mapCell,0); //player
					//---skip wandering if enemy found
					if(creature->controller.currentAction == BEHAVIOR_ACTION_FIGHTING)
						return;
					
				}//--cellwide search
			  
			    // get distance
				float difX = creature->wx - creature->actor.posX;
				float difY = creature->wy - creature->actor.posY;
				float difZ = creature->wz - creature->actor.posZ;
				float dist = difX*difX + difZ*difZ;
			    
				//wander targetlocation reached
			    // if(creature->actor.isRooted != 1) //next release
		        updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.29f);
				if(dist < 2.0f) 
				{
					creature->actor.posX = creature->wx;
					creature->actor.posY = creature->wy;
					creature->actor.posZ = creature->wz;
					creature->wanderstate = 0;
				}
		  }

	}
	else if(creature->controller.currentAction == BEHAVIOR_ACTION_FIGHTING )
	{
		
    	//__debugbreak();
		if ( creature->currentHealth <=0)
			return;
		// get target
		void *target = entityMgr_get(creature->controller.targetEntityId);
		
		if( target )
		{
			// get poisition of target
			float targetX = 0.0f;
			float targetY = 0.0f;
			float targetZ = 0.0f;
			if( entityMgr_getEntityType(creature->controller.targetEntityId) == ENTITYTYPE_CLIENT )
			{
				mapChannelClient_t *client = (mapChannelClient_t*)target;
	            //if target dead, set wander state
				if( client->player->actor->stats.healthCurrent<= 0 )
				{
				   creature->wanderstate = 0;
                   creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
				}
				targetX = client->player->actor->posX;
				targetY = client->player->actor->posY;
				targetZ = client->player->actor->posZ;

			}
			else if( entityMgr_getEntityType(creature->controller.targetEntityId) == ENTITYTYPE_CREATURE )
			{  
				creature_t  *creat = (creature_t*)target;
				if( (creat->currentHealth <= 0) )
				{
				   creature->wanderstate = 0;
                   creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
				}
				targetX = creat->actor.posX;
				targetY = creat->actor.posY;
				targetZ = creat->actor.posZ;
			}
			else
				__debugbreak(); // todo
			// get distance
			float difX = targetX - creature->actor.posX;
			float difY = targetY - creature->actor.posY;
			float difZ = targetZ - creature->actor.posZ;
			float dist = difX*difX + difZ*difZ;
			
			//---move entity
			//if(creature->actor.isRooted != 1) //next release
			updateEntityMovementW2(difX,difY,difZ,creature,mapChannel,0.91f);

			float meeleRange = 1.5f;
			meeleRange *= meeleRange;
			if( dist < meeleRange )
			{
				//rangeattack (every 2 sec)
				unsigned int time2 = GetTickCount();
				if( (time2-creature->lastattack) > creature->attackspeed && (dist < creature-> range) )
				{
					//__debugbreak();
				    creature->lastattack = time2;
					//if( creature->controller.targetEntityId )
					

					missile_launch(mapChannel, 
				                  &creature->actor, 
								  creature->controller.targetEntityId, 
								  MISSILE_PISTOL, 
								  1);
				}
			}
			else if ( creature->attack_habbit == 2 )
			{
								
				//rangeattack (every 2 sec)
				unsigned int time = GetTickCount();
				if( (time-creature->lastattack) > creature->attackspeed && (dist < creature-> range) )
				{
					//__debugbreak();
				    creature->lastattack = time;
					//if( creature->controller.targetEntityId )
					

					missile_launch(mapChannel, 
				                  &creature->actor, 
								  creature->controller.targetEntityId, 
								  MISSILE_PISTOL, 
								  1);
				}
			}
		}
		else
		{
			// target disappeared, leave combat mode (todo: immideatly search for new target)
			creature->controller.currentAction = BEHAVIOR_ACTION_WANDER;
		}
	}
}


/*
 * Called every 250 milliseconds
 */
void controller_mapChannelThink(mapChannel_t *mapChannel)
{
	for(int i=0; i<mapChannel->mapCellInfo.loadedCellCount; i++)
	{
		mapCell_t *mapCell = mapChannel->mapCellInfo.loadedCellList[i];
		if( mapCell == NULL ) // should never happen, but still do a check for saftey
			continue;
		// npcs
		npc_t **npcList = (npc_t**)hashTable_getValueArray(&mapCell->ht_npcList);
		int npcCount = hashTable_getCount(&mapCell->ht_npcList);
		for(int f=0; f<npcCount; f++)
			controller_npcThink(mapChannel, npcList[f]);
		// creatures
		creature_t **creatureList = (creature_t**)hashTable_getValueArray(&mapCell->ht_creatureList);
		int creatureCount = hashTable_getCount(&mapCell->ht_creatureList);
		for(int f=0; f<creatureCount; f++)
			controller_creatureThink(mapChannel, creatureList[f]);
	}
}

void controller_initForMapChannel(mapChannel_t *mapChannel)
{
	
}

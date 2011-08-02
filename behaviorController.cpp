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

/*
 * Called every 250 milliseconds for every creature on the map
 */
void controller_creatureThink(mapChannel_t *mapChannel, creature_t *creature)
{
	if( creature->controller.currentAction == BEHAVIOR_ACTION_IDLE )
	{
		mapCell_t *mapCell = cellMgr_tryGetCell(mapChannel, creature->actor.cellLocation.x, creature->actor.cellLocation.z);
		int mPosX = creature->actor.posX;
		int mPosZ = creature->actor.posZ;
		if( mapCell )
		{
			// check players in range
			int playerCount = hashTable_getCount(&mapCell->ht_playerNotifyList);
			mapChannelClient_t **playerList = (mapChannelClient_t**)hashTable_getValueArray(&mapCell->ht_playerNotifyList);
			int minimumRange = 15;
			minimumRange *= minimumRange;
			for(int i=0; i<playerCount; i++)
			{
				mapChannelClient_t *player = playerList[i];
				int difX = (int)(player->player->actor->posX) - mPosX;
				int difY = (int)(player->player->actor->posZ) - mPosZ;
				int dist = difX*difX + difY*difY;
				if( dist <= minimumRange )
				{
					//printf("Found player to attack\n");
					// set target and change state
					creature->controller.targetEntityId = player->clientEntityId;
					creature->controller.currentAction = BEHAVIOR_ACTION_FIGHTING;
					break;
				}
			}
		}
	}
	else if(creature->controller.currentAction == BEHAVIOR_ACTION_FIGHTING )
	{
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
				targetX = client->player->actor->posX;
				targetY = client->player->actor->posY;
				targetZ = client->player->actor->posZ;
			}
			else
				__debugbreak(); // todo
			// get distance
			float difX = targetX - creature->actor.posX;
			float difY = targetY - creature->actor.posY;
			float difZ = targetZ - creature->actor.posZ;
			float dist = difX*difX + difZ*difZ;
			float meeleRange = 1.5f;
			meeleRange *= meeleRange;
			if( dist < meeleRange )
			{
				// can do meele attack
			}
			else
			{
				// to far away, move
				// movefactor is 250 / 1000 * 6
				// normalize difference vector
				float length = 1.0f / sqrt(difX*difX + difY*difY + difZ*difZ);
				difX *= length;
				difY *= length;
				difZ *= length;

				float vX = atan2(-difX, -difZ);
				// multiplicate with speed
				float velocity = 6.0f;
				float speed = velocity * (250.0f / 1000.0f);
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
		}
		else
		{
			// target disappeared, leave combat mode (todo: immideatly search for new target)
			creature->controller.currentAction = BEHAVIOR_ACTION_IDLE;
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

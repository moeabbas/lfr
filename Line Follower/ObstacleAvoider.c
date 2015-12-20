/*
 * ObstacleAvoider.c
 *
 * Created: 20/12/2015 14:24:37
 *  Author: Mohamed
 */ 
#include "ObstacleAvoider.h"
#include "ADC.h"
#include "Logic.h"

bool hasLineBeenFound();
bool obstacleDetectedInFront();
void alignWithObstacle();
bool isTooFarFromObstacle();
bool isTooNearToObstacle();
void arcTowardsObstacle();
void arcAwayFromObstacle();
void moveForward();


void runObstacle()
{
	do 
	{
		if(obstacleDetectedInFront())
		{
			alignWithObstacle();
		}
		else if(isTooFarFromObstacle())
		{
			arcTowardsObstacle();
		}
		else if(isTooNearToObstacle())
		{
			arcAwayFromObstacle();
		}
		else
		{
			moveForward();
		}

	} while (!hasLineBeenFound());
}

bool hasLineBeenFound()
{
	// TODO: scan for line, return true if found
	return false;
}

bool obstacleDetectedInFront()
{
	uint16_t distance = AdcConvert(1);

	if (distance > (uint16_t)400)
	{
		return true;
	}

	return false;
}

void alignWithObstacle()
{
	
}

bool isTooFarFromObstacle()
{
	uint16_t distance = AdcConvert(2);

	if (distance < (uint16_t)400)
	{	
		return true;
	}

	return false;
}

bool isTooNearToObstacle()
{
	uint16_t distance = AdcConvert(2);

	if (distance > (uint16_t)500)
	{
		return true;
	}

	return false;
}

void arcTowardsObstacle()
{
	go(5,DIRECTION_LEFT,SPEED_CREEP);
}

void arcAwayFromObstacle()
{
	go(5,DIRECTION_RIGHT,SPEED_CREEP);
}

void moveForward()
{
	go(5,DIRECTION_STRAIGHT,SPEED_CREEP);
}
/*
 * ObstacleAvoider.c
 *
 * Created: 20/12/2015 14:24:37
 *  Author: Mohamed
 */ 
#include "ObstacleAvoider.h"
#include "ADC.h"
#include "Floor_sensor.h"
#include "AdvancedMotorControl.h"

bool hasLineBeenFound();
bool obstacleDetectedInFront();
void alignWithObstacle();
bool isTooFarFromObstacle();
bool isTooNearToObstacle();
void arcTowardsObstacle();
void arcAwayFromObstacle();
void moveForward();
bool hasAlignedWithObstacle();
bool isExtremelyNearToObstacle();
void arcSharplyAwayFromObstacle();

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
		else if(isExtremelyNearToObstacle())
		{
			arcSharplyAwayFromObstacle();
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
	// Currently working on run obstacle only, do not want to exit whiles developing.
	return false;

	// TODO: make reading line more reliable.
	uint8_t floorSensor = readFloorSensors();

	switch (floorSensor)
	{
		case 0b01111111:
			return true;
		default:
			return false;
	}
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
	do 
	{
		driveArcOnAxis(50,DIRECTION_ARC_RIGHT, 65);
	} while (hasAlignedWithObstacle() ==  false);
}

bool isTooFarFromObstacle()
{
	uint16_t distance = AdcConvert(2);

	if (distance < (uint16_t)350)
	{	
		return true;
	}

	return false;
}

bool isTooNearToObstacle()
{
	uint16_t distance = AdcConvert(2);

	if (distance > (uint16_t)400)
	{
		return true;
	}

	return false;
}

void arcTowardsObstacle()
{
	driveArc(5, DIRECTION_ARC_LEFT, 70, 80);
}

void arcSharplyAwayFromObstacle()
{
	driveArcOnAxis(5,DIRECTION_ARC_RIGHT,70);
}

void arcAwayFromObstacle()
{
	driveArc(5, DIRECTION_ARC_RIGHT, 70, 90);
}

void moveForward()
{
	driveArc(5, DIRECTION_ARC_RIGHT, 70, 0);
}

bool hasAlignedWithObstacle()
{
	uint16_t distance = AdcConvert(2);

	if (distance > 250)
	{
		return true;
	}

	return false;
}

bool isExtremelyNearToObstacle()
{
	uint16_t distance = AdcConvert(2);

	if (distance > (uint16_t)500)
	{
		return true;
	}

	return false;
}
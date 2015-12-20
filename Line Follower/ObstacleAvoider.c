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
	return false;

	uint8_t floorSensor = readFloorSensors();

	switch (floorSensor)
	{
		case 0b01111000:
		case 0b00111100:
		case 0b00011110:
		case 0b00001111:
		case 0b01111100:
		case 0b00111110:
		case 0b00011111:
		case 0b01111110:
		case 0b00111111:
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
		driveArcOnAxis(50,DIRECTION_ARC_RIGHT, 50);
	} while (hasAlignedWithObstacle() ==  false);
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

	if (distance > (uint16_t)450)
	{
		return true;
	}

	return false;
}

void arcTowardsObstacle()
{
	driveArc(5, DIRECTION_ARC_LEFT, 70, 80);
}

void arcAwayFromObstacle()
{
	driveArc(5, DIRECTION_ARC_RIGHT, 70, 80);
}

void moveForward()
{
	driveArc(5, DIRECTION_ARC_RIGHT, 70, 0);
}

bool hasAlignedWithObstacle()
{
	uint16_t distance = AdcConvert(2);

	if (distance > 350)
	{
		return true;
	}

	return false;
}
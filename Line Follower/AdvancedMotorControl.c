/*
 * AdvancedMotorControl.c
 *
 * Created: 20/12/2015 15:35:45
 *  Author: Mohamed
 */ 
#include "AdvancedMotorControl.h"
#include "Motor_Control.h"
#include "defines.h"

uint8_t calculateExpectedTurnWheelDistance(uint8_t distance, uint8_t degree);
bool haveMotorsTravelledRequiredDistance(uint8_t currentLeft, uint8_t currentRight, uint8_t targetLeft, uint8_t targetRight);

// distance:100, direction: right, speed ?, degree: 10
void driveArc(uint8_t distance, uint8_t direction, uint8_t speed, uint8_t degree)
{
	// initial state
	int currentTravelledLeft = 0;
	int currentTravelledRight = 0;
	
	// target state
	uint8_t targetTravelledLeft;
	uint8_t targetTravelledRight;

	if (direction == DIRECTION_ARC_LEFT)
	{	
		targetTravelledLeft = calculateExpectedTurnWheelDistance(distance, degree);
		targetTravelledRight = distance;
	}

	else if (direction == DIRECTION_ARC_RIGHT)
	{	
		targetTravelledLeft = distance;
		targetTravelledRight = calculateExpectedTurnWheelDistance(distance, degree);
	}
	else 
	{
		return;
	}

	setDirectionMotorL(MOTOR_FORWARD);
	setDirectionMotorR(MOTOR_FORWARD);

	while (haveMotorsTravelledRequiredDistance(currentTravelledLeft, currentTravelledRight, targetTravelledLeft, targetTravelledRight) == false)
	{	
		currentTravelledLeft  += getSensorMotorL();
		currentTravelledRight += getSensorMotorR();
	
		uint8_t turnWheelError;

		if (direction == DIRECTION_ARC_LEFT)
		{
			uint8_t expectedTurnWheelDistance = calculateExpectedTurnWheelDistance(currentTravelledRight, degree);
			turnWheelError = expectedTurnWheelDistance - currentTravelledLeft;
		}
		else if (direction == DIRECTION_ARC_RIGHT)
		{
			uint8_t expectedTurnWheelDistance = calculateExpectedTurnWheelDistance(currentTravelledLeft, degree);
			turnWheelError = expectedTurnWheelDistance - currentTravelledRight;
		}
		else
		{
			return; // Should never come here...
		}

		
	}
}

uint8_t calculateExpectedTurnWheelDistance(uint8_t distance, uint8_t degree)
{
	uint8_t result = (distance*(90-degree)/90);
	return result;
}

bool haveMotorsTravelledRequiredDistance(uint8_t currentLeft, uint8_t currentRight, uint8_t targetLeft, uint8_t targetRight)
{
	if (currentLeft>=targetLeft && currentRight>=targetRight)
	{
		return true;
	}
	else
	{
		return false;
	}
}

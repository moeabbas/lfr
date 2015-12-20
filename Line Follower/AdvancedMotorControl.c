/*
 * AdvancedMotorControl.c
 *
 * Created: 20/12/2015 15:35:45
 *  Author: Mohamed
 */ 
#include "AdvancedMotorControl.h"
#include "Motor_Control.h"
#include "defines.h"

uint8_t calculateTurnWheelSpeed(uint8_t distance, uint8_t degree);
bool hasMotorTravelledRequiredDistance(uint8_t currentLeft, uint8_t targetLeft);

void driveArcOnAxis(uint8_t distance, uint8_t direction, uint8_t speed)
{
	// initial state
	uint8_t currentTravelled = 0;

	// target state
	uint8_t targetTravelled = distance;

	uint8_t outsideWheelSpeed = speed;
	uint8_t insideWheelSpeed = 45;

	if (direction == DIRECTION_ARC_LEFT)
	{
		setDirectionMotorL(MOTOR_REVERSE);
		setDirectionMotorR(MOTOR_FORWARD);
		setDutyCycleMotorL(insideWheelSpeed);
		setDutyCycleMotorR(outsideWheelSpeed);
	}
	else if (direction == DIRECTION_ARC_RIGHT)
	{
		setDirectionMotorL(MOTOR_FORWARD);
		setDirectionMotorR(MOTOR_REVERSE);
		setDutyCycleMotorL(outsideWheelSpeed);
		setDutyCycleMotorR(insideWheelSpeed);
	}

	while (hasMotorTravelledRequiredDistance(currentTravelled, targetTravelled) == false)
	{
		if (!getSensorUpdateFlag())
		{
			continue;
		}
		
		if (direction == DIRECTION_ARC_LEFT)
		{
			currentTravelled += getSensorMotorR();
		}
		else if (direction == DIRECTION_ARC_RIGHT)
		{
			currentTravelled += getSensorMotorL();
		}
		else
		{
			return;
		}

		clearSensorUpdateFlag();
	}

	setDutyCycleMotorL(0);
	setDutyCycleMotorR(0);
}

/*
 * distance:	units of distance to travel (0 -> unlimited)
 * direction:	direction to arc in (DIRECTION_ARC_LEFT / DIRECTION_ARC_RIGHT)
 * speed:		speed of movement (0 -> 250)
 * degree:		sharpness of arc (0 [straight line] -> 90 [sharpest turn])
 */

void driveArc(uint8_t distance, uint8_t direction, uint8_t speed, uint8_t degree)
{
	if (degree > 90 || degree < 0)
	{
		return;
	}

	// initial state
	uint8_t currentTravelled = 0;
	
	// target state
	uint8_t targetTravelled = distance;

	uint8_t outsideWheelSpeed = speed;
	uint8_t insideWheelSpeed = calculateTurnWheelSpeed(speed, degree);

	setDirectionMotorL(MOTOR_FORWARD);
	setDirectionMotorR(MOTOR_FORWARD);

	if (direction == DIRECTION_ARC_LEFT)
	{	
		setDutyCycleMotorL(insideWheelSpeed);
		setDutyCycleMotorR(outsideWheelSpeed);
	}

	else if (direction == DIRECTION_ARC_RIGHT)
	{	
		setDutyCycleMotorL(outsideWheelSpeed);
		setDutyCycleMotorR(insideWheelSpeed);
	}
	else 
	{
		// Invalid directional input.
		return;
	}

	while (hasMotorTravelledRequiredDistance(currentTravelled, targetTravelled) == false)
	{
		if (!getSensorUpdateFlag())
		{
			continue;
		}
		
		if (direction == DIRECTION_ARC_LEFT)
		{
			currentTravelled += getSensorMotorR();
		}
		else if (direction == DIRECTION_ARC_RIGHT)
		{
			currentTravelled += getSensorMotorL();
		}
		else
		{
			return;
		}

		clearSensorUpdateFlag();
	}

	setDutyCycleMotorL(0);
	setDutyCycleMotorR(0);
}

uint8_t calculateTurnWheelSpeed(uint8_t speed, uint8_t degree)
{
	uint8_t result = speed*(90-degree)/90;

	if (result < 0)
	{
		result = 0;
	}

	return result;
}

bool hasMotorTravelledRequiredDistance(uint8_t currentDistance, uint8_t targetDistance)
{
	if (currentDistance >= targetDistance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

#include <avr/io.h>
#include <stdio.h>
#include "defines.h"
#include "motor_Control.h"
#include "lcd.h"
#include "Floor_sensor.h"
#include "LED_control.h"
#include "Logic.h"
#include "ADC.h"
#include "buttonMenu.h"
#include "ObstacleAvoider.h"
#include "AdvancedMotorControl.h"


#include <util/delay.h>

/*
Update 19.12.12
Floor_sensor.c changed so error calculation is inactive before finding line
In the beginning, and changed pattern of how line is found in the beginning.
ButtonMenu.c changed little bit.
Wall_sensor changed, forwardWallLeftStop(void) decreased distance from 50 to 30.
And added a missing function in runWall() function.
MotorControl.c added a function to clear the integer sum error.
*/

void Initialize();

int main(void) {

	Initialize();
	LEDVoltage();
	buttonMenu();

	while(1)
	{
		// Update voltage LED's
		LEDVoltage();
	
		if(getRunLineFlag())
		{
			runLine();

			setRunLineFlag(true);
			setStopFlag(FLAG_NOT_SET);

			for( int i = 0; i < 1000; i++)
			{
				_delay_ms(3);
			}
		}

		runObstacle();
	}
	
	return 0;
}

void Initialize()
{
	initMotorControl();
	initFloorSensor();
	LCDInit(16);
	LEDInit();
	AdcInit();
	LEDEnable();
	enableHBridge();
	SetInterrupt();
}


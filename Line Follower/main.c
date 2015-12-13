#include <avr/io.h>
#include <stdio.h>
#include "defines.h"
#include "motor_Control.h"
#include "lcd.h"
#include "Floor_sensor.h"
#include "LED_control.h"
#include "Logic.h"
#include "ADC.h"
#include "Wall_sensor.h"
#include "bluetooth.h"
#include "buttonMenu.h"

#define F_CPU 10000000UL
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

/*
Update 28.12.12
Added rainbow effect to the LED controller, uses 4 bit PWM.
*/

int main(void) {

	initMotorControl();
	initFloorSensor();
	LCDInit(16);
	LEDInit();
	AdcInit();
	USARTInit(31);

	LEDEnable();
	
	enableHBridge();

	SetInterrupt();
	
	while(1)
	{
		// Update voltage LED's
		LEDVoltage();
		
		//test();
		
		buttonMenu();

		if(getRunLineFlag())
		{
			runLine();
			setRunLineFlag(false);

			for( int i = 0; i < 1000; i++)
			{
				_delay_ms(3);
			}
			
			setRunWallFlag(true);
		}

		if(getRunWallFlag())
		{
			rainBowOn();
			runWall();
			setRunWallFlag(false);
			rainBowOff();
		}
	}
	return 0;
}

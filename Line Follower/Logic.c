/////////////////////////////////////////////////////////////////////////
//                        Logic.C 		                               //
//                                                                     //
//Functions for the Robot and IHK Track						           //
//                                                                     //
//Version         : 1.0                                                //
//Date            : 12.12.2012                                         //
//Author          : David Sigurdsson // dabbis@gmail.com               //
//Chip type       : ATmega32                                           //
//This code may not be used by anyone unless otherwise agreed upon.    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "Logic.h"
#include "Motor_Control.h"
#include "LED_control.h"
#include "Floor_sensor.h"
#include "lcd.h"
#include "ADC.h"
#include <stdio.h>
 
/////////////////////////////////////////////////////////////////////////
//                       Local variables                               //
/////////////////////////////////////////////////////////////////////////

uint8_t stopFlag = FLAG_NOT_SET;
uint8_t foundLineFlag = FLAG_NOT_SET;
uint8_t lostLineFlag = FLAG_NOT_SET;

/////////////////////////////////////////////////////////////////////////
//                       Public functions                              //
/////////////////////////////////////////////////////////////////////////

void setStopFlag(uint8_t flag){
	stopFlag = flag;
}

void setFoundLineFlag(uint8_t flag){
	foundLineFlag = flag;
}

void setLostLineFlag(uint8_t flag){
	lostLineFlag = flag;
}	

uint8_t getStopFlag(){
	return stopFlag;
}

uint8_t getFoundLineFlag(){
	return foundLineFlag;
}


void searchMode(){
	
	uint8_t a = 0;
	
	while(readFloorSensors() == 0)
	{
		switch(a)
		{
			case 0:
			// Drive a little bit forward.
			go(150,DIRECTION_STRAIGHT,SPEED_CREEP);
			a = 1;
			break;

			// Scan Right.
			case 1:
			go(100,DIRECTION_RIGHT,SPEED_CREEP);
			a = 2;
			break;

			// Scan left
			case 2:
			go(200,DIRECTION_LEFT,SPEED_CREEP);
			a = 3;
			break;

			// Center again.
			case 3:
			go(105,DIRECTION_RIGHT,SPEED_CREEP);
			a = 0;
			break;
		}	
	}
	
	setDirectionMotorL(0);
	setDirectionMotorR(0);
}

// Run the IHK line
void runLine(){

	LCDClear();

	static const uint8_t forwardDirection = 0;
	
	// Start speed, current speed.
	uint8_t currentSpeed = SPEED_CREEP;

	setDirectionMotorL(forwardDirection);
	setDirectionMotorR(forwardDirection);

	// Run until end of track.
	while(stopFlag == FLAG_NOT_SET)
	{
		if(getSensorUpdateFlag())
		{
			// When a line is lost after driving straight, go in search mode
			if(lostLineFlag == FLAG_SET)
			{
				searchMode();
				lostLineFlag = FLAG_NOT_SET;
			}

			// When line is found, make a Right turn
			// Increase speed and follow the line.
			if(foundLineFlag == FLAG_SET)
			{
				go(150,DIRECTION_RIGHT,SPEED_CREEP);
				setDirectionMotorL(forwardDirection);
				setDirectionMotorR(forwardDirection);
				currentSpeed = SPEED_CREEP;
				foundLineFlag = FLAG_USED;
			}
		
			// When following the line,
			// if front sensor detects obstacle,
			// exit line following loop
			if (foundLineFlag == FLAG_USED)
			{
				uint16_t distance = AdcConvert(1);
				
				if (distance > (uint16_t)500)
				{
					setStopFlag(FLAG_SET);
				}
			}

			// Calculate and set new duty cycle.
			calcDuty(currentSpeed, calcFloorErrorAndFlagControl());

			clearSensorUpdateFlag();
		}
	}

	// Stop the robot at end mark.
	setDutyCycleMotorL(0);
	setDutyCycleMotorR(0);
}

// Drive the Robot certain distance or turn left/right
// dist = 1100 approx 360°
// direction = 0 Straight, 1 Left, 2 Right
// ref = 0-17 Speed.
// Does not compensate for overshoot, slow speed better.
void go(signed int distance, uint8_t direction, uint8_t currentSpeed){

	// Total amount of pulses traveled.
	int pulsesTravelledLeft = 0;
	int pulsesTravelledRight = 0;
	signed char differenceBetweenLeftAndRightPulses = 0;

	// Boolean
	uint8_t hasLeftFinished  = false;
	uint8_t hasRightFinished = false;

	// Change direction.
	if(direction == DIRECTION_STRAIGHT)
	{
		setDirectionMotorL(MOTOR_FORWARD);
		setDirectionMotorR(MOTOR_FORWARD);
	}
	if(direction == DIRECTION_LEFT)
	{
		setDirectionMotorL(MOTOR_REVERSE);
		setDirectionMotorR(MOTOR_FORWARD);
	}
	if(direction == DIRECTION_RIGHT)
	{
		setDirectionMotorL(MOTOR_FORWARD);
		setDirectionMotorR(MOTOR_REVERSE);
	}
		
	// Go the distance until finished.
	while(hasLeftFinished == false || hasRightFinished == false)
	{
		if(getSensorUpdateFlag())
		{
			pulsesTravelledRight += getSensorMotorR();
			pulsesTravelledLeft  += getSensorMotorL();

			// Difference between Wheels
			differenceBetweenLeftAndRightPulses = pulsesTravelledLeft - pulsesTravelledRight;

			// Calculate and set new dutyCycle.
			calcDuty(currentSpeed, differenceBetweenLeftAndRightPulses);
			
			// Finished ?
			if(pulsesTravelledLeft >= distance){
				hasLeftFinished = true;
				setDutyCycleMotorL(0);
			}

			if(pulsesTravelledRight >= distance){
				hasRightFinished = true;
				setDutyCycleMotorR(0);
			}

			clearSensorUpdateFlag();
		}
	}
}

// Count distance traveled in pulses and show on display.
void test(){

	char buffer[20];	
	int pulseCountL = 0;
	int pulseCountR = 0;

	while(1){
		if(getSensorUpdateFlag()){

			pulseCountL += getSensorMotorL();
			pulseCountR += getSensorMotorR();
		
			LCDGotoXY(0,0);
			sprintf(buffer, "%04i %04i %04i",pulseCountL,pulseCountR, (pulseCountL-pulseCountR));
			LCDPutString(buffer);
			
			readFloorSensors();
			clearSensorUpdateFlag();
		}
	}
}
 

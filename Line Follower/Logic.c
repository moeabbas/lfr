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
#include <stdio.h>
 
/////////////////////////////////////////////////////////////////////////
//                       Local variables                               //
/////////////////////////////////////////////////////////////////////////

// Flags for driving the IHK Line.
// 0 = flag not set
// 1 = flag set.
// 2 = flag used
uint8_t stopFlag = 0;
uint8_t threeSixtyFlag = 0;
uint8_t highSpeedFlag = 0;
uint8_t foundLineFlag = 0;
uint8_t lostLineFlag = 0;


/////////////////////////////////////////////////////////////////////////
//                       Public functions                              //
/////////////////////////////////////////////////////////////////////////

void setStopFlag(uint8_t flag){
	stopFlag = flag;
}

void setThreeSixtyFlag(uint8_t flag){
	threeSixtyFlag = flag;
}

void setHighSpeedFlag(uint8_t flag){
	highSpeedFlag = flag;
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

uint8_t getThreeSixtyFlag(){
	return threeSixtyFlag;
}

uint8_t getHighSpeedFlag(){
	return highSpeedFlag;
}

uint8_t getFoundLineFlag(){
	return foundLineFlag;
}


void searchMode(){
	
	uint8_t a = 0;
	
	while(1)
	{
		// Check if robot found line again.
		if(readFloorSensors() != 0)
		{
			setDirectionMotorL(0);
			setDirectionMotorR(0);
			break;
		}

		switch(a)
		{
			case 0:
			// Drive a little bit forward.
			go(150,0,2);
			a = 1;
			break;

			// Scan Right.
			case 1:
			go(100,2,2);
			a = 2;
			break;

			// Scan left
			case 2:
			go(200,1,2);
			a = 3;
			break;

			// Center again.
			case 3:
			go(105,2,2);
			a = 0;
			break;
		}	
	}
}

// Run the IHK line
void runLine(){

	LCDClear();

	// speed MAX ~17-18.
	static const uint8_t highSpeed = 16;
	static const uint8_t normalSpeed = 12;
	static const uint8_t lowSpeed = 6;
	
	static const uint8_t forwardDirection = 0;
	
	// Start speed, current speed.
	uint8_t currentSpeed = lowSpeed;

	setDirectionMotorL(forwardDirection);
	setDirectionMotorR(forwardDirection);

	// Run until end of track.
	while(stopFlag == 0)
	{
		if(getSensorUpdateFlag())
		{
			// When a line is lost after driving straight, go in search mode
			if(lostLineFlag == 1)
			{
				searchMode();
				lostLineFlag = 0;
			}

			// When line is found, make a Right turn
			// Increase speed and follow the line.
			if(foundLineFlag == 1)
			{
				go(150,2,lowSpeed);
				setDirectionMotorL(forwardDirection);
				setDirectionMotorR(forwardDirection);
				currentSpeed = normalSpeed;
				foundLineFlag = 2;
			}

			// 360° mark
			if(threeSixtyFlag == 1)
			{
				go(1180,1,lowSpeed);
				setDirectionMotorL(forwardDirection);
				setDirectionMotorR(forwardDirection);
				threeSixtyFlag = 2;
			}
	
			// High speed mark.
			if(highSpeedFlag == 1)
			{
				currentSpeed = highSpeed;
				highSpeedFlag = 2;
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
// dist = 1100 aprox 360°
// dir = 0 Straight, 1 Left, 2 Right
// ref = 0-17 Speed.
// Does not compensate for overshoot, slow speed better.
void go(signed int dist, uint8_t dir, uint8_t currentSpeed){

	// Total amount of pulses traveled.
	int pulseCountL = 0;
	int pulseCountR = 0;
	signed char error = 0;

	// Boolean
	uint8_t finishedL = 0;
	uint8_t finishedR = 0;

	// Change direction.
	if(dir == 0){
		setDirectionMotorL(0);
		setDirectionMotorR(0);
	}
	if(dir == 1){
		setDirectionMotorL(1);
		setDirectionMotorR(0);
		
	}
	if(dir == 2){
		setDirectionMotorL(0);
		setDirectionMotorR(1);
	}
		
	// Go the distance until finished.
	while((finishedL == 0) || (finishedR == 0)){
	
		if(getSensorUpdateFlag()){
			
			pulseCountR += getSensorMotorR();
			pulseCountL += getSensorMotorL();

			// Difference between Wheels
			error = (pulseCountL - pulseCountR)/2;

			// Calculate and set new dutyCycle.
			calcDuty(currentSpeed, error);
			
			// Finished ?
			if(pulseCountL >= dist){
				finishedL = 1;
				setDutyCycleMotorL(0);
			}

			if(pulseCountR >= dist){
				finishedR = 1;
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
 

/////////////////////////////////////////////////////////////////////////
//                        Wall_sensor.C                                //
//                                                                     //
//Functions for GP2D12 - Distance Measuring Sensors using ADC          //
//                                                                     //
//Version         : 1.0                                                //
//Date            : 30.10.2012                                         //
//Author          : Morten/David Sigurdsson // dabbis@gmail.com        //
//Chip type       : ATmega32                                           //
//This code may not be used by anyone unless otherwise agreed upon.    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#define F_CPU 10000000UL
#include <avr/interrupt.h>
#include "Motor_Control.h"
#include <util/delay.h>
#include "adc.h"
#include "Floor_sensor.h"
#include <stdlib.h>
#include "lcd.h"
#include "logic.h"
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////
//                       Local variables                               //
/////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////
//                       Public functions                              //
/////////////////////////////////////////////////////////////////////////

// Write on display wall function number, 2nd line on LCD
void showPlace(uint8_t value){
	
	char buffer[20];
	LCDGotoXY(0,1);
	sprintf(buffer, "%02i", value);
	LCDPutString(buffer);
}


// Calculates the error from wall sensor
float calcWallError(void){

	uint16_t adc=0;	
	adc=AdcConvert(2);

	// Check Position from wall

	if(adc<=150){ 
		return 2;
	}		
	if(adc>150){
		return -2; 
	}
	return 0;
}

//Stops the robot by driving backwards for 80 ms
void fullStop(void){

	setDirectionMotorL(1);
	setDirectionMotorR(1);
	setDutyCycleMotorL(100);
	setDutyCycleMotorR(100);
	_delay_ms(80);
	setDutyCycleMotorL(0);
	setDutyCycleMotorR(0);
}

//Makes the robot go straight forward and stop at the wall. 
//Parameter: stop: distance (to the wall): 0-620, speed: 0-250 
void forward_stop(uint16_t distance){

	setDirectionMotorL(0);
	setDirectionMotorR(0);

	while (distance > AdcConvert(1)){
		showPlace(1);
		if(getSensorUpdateFlag()){
			// Calculate and set new dutyCycle.
			calcDuty(6, 0);
			clearSensorUpdateFlag();
		}
	}
	fullStop();
}


//Makes the robot turn 90 degrees. 
//Parameter: direction,- 1:left, 2:right 
void turn_90(uint16_t LR){

		if(LR=='L'){
		go(260, 1, 6);
		setDirectionMotorL(0);
		setDirectionMotorR(1);
		setDutyCycleMotorL(100);
		setDutyCycleMotorR(100);
		_delay_ms(60);
		setDutyCycleMotorL(0);
		setDutyCycleMotorR(0);
		}
		
		if(LR=='R'){
		go(240, 2, 6);
		setDirectionMotorL(1);
		setDirectionMotorR(0);
		setDutyCycleMotorL(100);
		setDutyCycleMotorR(100);
		_delay_ms(60);
		setDutyCycleMotorL(0);
		setDutyCycleMotorR(0);
		}
	}


//Makes the robot go forward until a wall on left side.  
void forwardWallLeftStop(void){

	setDirectionMotorL(0);
	setDirectionMotorR(0);

	while (40 < AdcConvert(2)){
		showPlace(2);
		if(getSensorUpdateFlag()){
			calcDuty(6, 0);	
			clearSensorUpdateFlag();
		}
	}
	go(560,0,6);
	showPlace(3);
}


//Makes the robot go forward until a wall on left side.  
void forwardFindWallLeft(void){

	setDirectionMotorL(0);
	setDirectionMotorR(0);

	while (100 > AdcConvert(2)){
		showPlace(4);
		if(getSensorUpdateFlag()){
			calcDuty(6, 0);	
			clearSensorUpdateFlag();
		}
	}
	setDutyCycleMotorL(0);
	setDutyCycleMotorR(0);
}


//Makes the robot go parallel with the wall until the wall ends. 
//Parameter: speed: 0-250 
void parallelWall(void){

	setDirectionMotorL(0);
	setDirectionMotorR(0);
	
	while (250 > AdcConvert(1)){
		showPlace(5);
		if(getSensorUpdateFlag()){
			calcDuty(6, calcWallError());
			clearSensorUpdateFlag();
		}
	}
	fullStop();
}


void runWall(void){
	
	setKp(1.5);
	setKi(0);
	LCDClear();
	clearIntSumError();

	forward_stop(250);
	turn_90('R');
	forwardFindWallLeft();
	forwardWallLeftStop();
	turn_90('L');
	forwardFindWallLeft();
	forwardWallLeftStop();
	turn_90('L');
	forwardFindWallLeft();
		
	parallelWall();
	turn_90('R');
	parallelWall();	

}

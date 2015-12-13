/////////////////////////////////////////////////////////////////////////
//                        Floor_sensor.C                               //
//                                                                     //
//Functions for 7 output Infrared sensor board			               //
//                                                                     //
//Version         : 1.1                                                //
//Date            : 12.12.2012                                         //
//Author          : David Sigurdsson // dabbis@gmail.com               //
//Chip type       : ATmega32                                           //
//This code may not be used by anyone unless otherwise agreed upon.    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "defines.h"
#include "Floor_Sensor.h"
#include "Logic.h"
#include "lcd.h"
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////
//                       Public functions                              //
/////////////////////////////////////////////////////////////////////////

// Initialize the Floor Sensor.
void initFloorSensor(){

	// Set the directions.
	setInput(DDRB, L3);
	setInput(DDRB, L2);
	setInput(DDRB, L1);
	setInput(DDRB, C1);
	setInput(DDRA, R1);
	setInput(DDRA, R2);
	setInput(DDRA, R3);
}

// Read the current value from the floor sensors.
uint8_t readFloorSensors(){	
	uint8_t data = ((PINA & 240) >> 1) | (PINB & 7);

	// Write Floor sensor values on display.
	showFloorInfo(data);
	return data;
}

// Calculates the error from floor sensor and change various flags.
signed char calcFloorError(){

	static uint8_t oldData = 0;
	uint8_t data = 0;
	
	data = readFloorSensors();

	// Check if on line, use new data. otherwise oldData
	// usefull if the robot undershots a line in a turn.
	// Checks if it was going straight before loosing the line
	// then indicate with a LostLine flag.
	
	if(data != 0){
		oldData = data;
	}
	else{
		if((oldData == 8) || (oldData == 12) || (oldData == 24)){
			setLostLineFlag(1);
			return 0;
		}
		data = oldData;
	}

	// Check for Line beginning, only once
	if(getFoundLineFlag() != 2){
		if((data & 0b0001000) && (data & 0b0010000) && (data & 0b0000100)){
			setFoundLineFlag(1);
			return 0;
		}
	}

	// Check for 360° mark, only once and only after Line beginning
	if((getThreeSixtyFlag() != 2) && (getFoundLineFlag() != 0)){
		if((data == 127) || (data == 107)){
			setThreeSixtyFlag(1);
		}
	}

	// Check for High speed mark, only after 360°
	if(getThreeSixtyFlag() == 2){
		if((data == 124) || (data == 120)){
			setHighSpeedFlag(1);
		}
	}

	// Check for end line mark, only after highSpeed.
	if(getHighSpeedFlag() == 2){
		if((data == 127) || (data == 119)){
			setStopFlag(1);
		}
	}
	
	if(getFoundLineFlag()){

		// Check Position on line.
		// Going Right returns positive
		// Going left returns negative.
		if((data == 96) || (data == 64)){
			return 15;
		}
		if(data == 32){
			return 13;
		}
		if(data == 48){
			return 8;
		}
		if(data == 16){
			return 6;
		}		
		if(data == 24){
			return 1;
		}
		if(data == 8){
			return 0;
		}
		if(data == 12){
			return -1;
		}
		if(data == 4){
			return -6;
		}
		if(data == 6){
			return -8;
		}
		if(data == 2){
			return -13;
		}
		if((data == 3) || (data == 1)){
			return -15;
		}
	}
	return 0;
}


// Writes on display floor sensor values in Binary and dec
void showFloorInfo(uint8_t data){

	char buffer[20];
	uint8_t i = 0;
	uint8_t value = 0;
	uint8_t dataTemp = data;
	LCDGotoXY(0,1);

	while(i <=6){
			value = ((dataTemp & (64>>i)) && 1);
			sprintf(buffer, "%d", value);
			LCDPutString(buffer);
			i++;
	}
	sprintf(buffer, " %02d", data);
	LCDPutString(buffer);
}

// Deliang/David Sigurdsson // dabbis@gmail.com
// This code may not be used by anyone unless otherwise agreed upon.

#include "adc.h"
#include "lcd.h"
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "defines.h"
#include "Logic.h"

char  menuStr1[14];
char  menuStr2[14];
uint8_t kp=5;
uint8_t ki=15;
uint8_t k=100;
bool	runLineFlag=false;
bool    runWallFlag=false;

// User decided codes for directions
#define rightButton '0'
#define upButton '1'
#define downButton '2'
#define leftButton '3'
#define centralButton '4'
#define noButton '5'

bool getRunLineFlag(){
	return runLineFlag;
}

bool getRunWallFlag(){
	return runWallFlag;
}

void setRunLineFlag(bool value){
	runLineFlag = value;
}

void setRunWallFlag(bool value){
	runWallFlag = value;
}


/******************************************************************************/
// Function name: AdcReadButton
// Function task: set a char for each button been pushed or not
// Input:
// Return: char
/******************************************************************************/
char ADCReadButton(void)
{
	uint16_t keyboardVoltage = (int16_t)AdcConvert(0);	// get ADC value from channel ADC0
	
	// Voltages attributed to button on the keyboard
	const int maxRightButtonVoltage = 100;
	const int maxUpButtonVoltage = 300;
	const int maxDownButtonVoltage = 500;
	const int maxLeftButtonVoltage = 700;
	const int maxCentralButtonVoltage = 900;
	
	if (keyboardVoltage < maxRightButtonVoltage)
	{
		return rightButton;
	}
	else if (keyboardVoltage < maxUpButtonVoltage)
	{
		return upButton;
	}
	else if (keyboardVoltage < maxDownButtonVoltage)
	{
		return downButton;
	}
	else if (keyboardVoltage < maxLeftButtonVoltage)
	{
		return leftButton;
	}
	else if (keyboardVoltage < maxCentralButtonVoltage)
	{
		return centralButton;
	}
	else
	{
		return noButton;
	}
}

/******************************************************************************/
// Function name: getButton
// Function task: read the button, wait for stable input (twice same input)
// Input:
// Return: ASCII value of active button, '5' for no button pushed
/******************************************************************************/
char getButton(void)
{
	static char last = noButton;
	char temp, now;

	now = ADCReadButton();          // read the present input
	if (now == last)                // test for two subsequent equal inputs
	{
		return now;
	}
	else
	{
		temp = last;                 // if new: store new and return the last stable
		last = now;
		return temp;
	}
}
/******************************************************************************/
// Function name: buttonMenu
// Function task: create a button menu sequence, in order to control the robot manually
// 				  The function is taking action on relevant buttons
// Button functions:
//		'0':
//  	'1':
//		'2':
//		'3':
//		'4':
/******************************************************************************/
void buttonMenu(void)
{
	char lastButton = noButton;
	char button = getButton();
	sprintf(menuStr1, "Run");
	sprintf(menuStr2, "Parameters");
	LcdTask();
	LCDGotoXY(15,0);
	LCDPutChar(1);	// display the up arrow
	LCDGotoXY(14,1);
	LCDPutChar(0x7F);	// display the left arrow
	LCDGotoXY(15,1);
	LCDPutChar(0x7E);	// display the right arrow
	while(button != centralButton)
	{
		button=getButton();
		if(button != lastButton)
		{
			switch (button)
			{
				case rightButton:
				subMenuRight();
				break;
				
				case upButton:
				subMenuUp();
				break;

				case downButton:
				subMenuDown();
				break;

				case leftButton :
				subMenuLeft();
				break;
				
				default:
				break;
			}
			
		}
	}
	
	lastButton = button;

}
bool buttonReleased(void)
{
	char button = getButton();
	if(button==noButton)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void subMenuRight(void)
{
	char subButton = getButton();
	
	sprintf(menuStr1, "Adjust kp");
	sprintf(menuStr2,"kp = %03d", kp);
	
	LcdTask();
	while(subButton!= centralButton)
	{
		subButton = getButton();
		if(subButton == upButton)
		{
			while(!buttonReleased());
			kp+=1;
			sprintf(menuStr2,"kp = %03d", kp);
			LcdTask();
		}
		else if(subButton == downButton)
		{
			while(!buttonReleased());
			kp-=1;
			sprintf(menuStr2,"kp = %03d", kp);
			LcdTask();
		}
	}
}
void subMenuLeft(void)
{
	char subButton = getButton();
	sprintf(menuStr1, "Adjust ki");
	sprintf(menuStr2,"ki = %03d", ki);
	LcdTask();
	while(subButton!=centralButton)
	{
		subButton=getButton();
		if(subButton == upButton)
		{
			while(!buttonReleased());
			ki+=1;
			sprintf(menuStr2,"ki = %03d", ki);
			LcdTask();
		}
		else if(subButton == downButton)
		{
			while(!buttonReleased());
			ki-=1;
			sprintf(menuStr2,"ki = %03d", ki);
			LcdTask();
		}
	}
}
void subMenuUp(void)
{
	char subButton = getButton();
	sprintf(menuStr1, "Run selection");
	LcdTask();
	while(subButton != centralButton)
	{
		subButton = getButton();
		if(subButton == upButton)
		{
			while(!buttonReleased())
			{
				// do nothing
			};
			setRunLineFlag(true);
			setRunWallFlag(false);
			sprintf(menuStr2,"RunLine()");
			LcdTask();
			LCDGotoXY(15,0);
			LCDPutChar(' ');	// 0x20 using empty char blanks the up arrow
		}
	}
}
void subMenuDown(void)
{
	while(1)
	{
		test();
	}
}

void LcdTask (void)
{
	LCDClear();
	LCDGotoXY(0,0);
	LCDPutString(menuStr1);
	LCDGotoXY(0,1);
	LCDPutString(menuStr2);
	LCDGotoXY(15,0);
	LCDPutChar(1);	// display the up arrow
	LCDGotoXY(15,1);
	LCDPutChar(0);	// display the down arrow
	/*LCDGotoXY(14,0);
	LCDPutChar(0x7F);	// display the left arrow
	LCDGotoXY(14,1);
	LCDPutChar(0x7E);	// display the right arrow
	*/
}

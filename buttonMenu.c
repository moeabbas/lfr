
#include "adc.h"
#include "lcd.h"
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "defines.h"

char  menuStr1[14];
char  menuStr2[14];
uint8_t kp=5;
uint8_t ki=15;
uint8_t k=100;
bool	runLineFlag=false;
bool    runWallFlag=false;


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
	 uint16_t x0 = (int16_t)AdcConvert(0);	// get ADC value from channel ADC0
	 
	if (x0<100)
	{
	 	return '0';	// right button
     }
	 else
	 if (x0<300)
	 {
	 	return '1';	// up button
     }
	 else
	 if (x0<500)
	 {
	 	return '2';	// down button
     }
	 else	
	 if (x0<700)
	 {
	 	return '3';	// left button
     }
	 else
	 if (x0<900)
	 {
	 	return '4';	// central button
     }
	 else
	 if (x0>=900)
	 {
	 	return '5';	// no button pressed
     }	
     else         
	
	 return 0; // return 0 on illegal input combination
}

/******************************************************************************/
// Function name: getButton
// Function task: read the button, wait for stable input (twice same input)
// Input: 
// Return: ASCII value of active button, '5' for no button pushed
/******************************************************************************/
char getButton(void)
{
   static char last= '5';
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
	char button;
	char lastButton='5';
	button=getButton();
		sprintf(menuStr1, "Run");
			sprintf(menuStr2, "Parameters");
			LcdTask();
			LCDGotoXY(15,0);
			LCDPutChar(1);	// display the up arrow
			LCDGotoXY(14,1);
			LCDPutChar(0x7F);	// display the left arrow
			LCDGotoXY(15,1);
			LCDPutChar(0x7E);	// display the right arrow
	while(button!='4')
	{
		button=getButton();
		if(button != lastButton)
		{
			switch (button)
      		{
         		case '0' : 
		    	
					subMenuRight();
            	break;
         		case '1' :
            		subMenuUp();
            	break;

         		case '2' :
		 			subMenuDown();
		  		break;

         		case '3' :
            		subMenuLeft();
            	break;
              	default:  
					
		 		break;
			}
          
		} 
	}
	 
	lastButton = button;

}
bool buttonPressed(void)
{
	char button;
	button=getButton();
	if(button!='5')
	{
		return true;
	}
	else
	return false;
}
bool buttonReleased(void)
{
	char button;
	button=getButton();
	if(button=='5')
	{
		return true;
	}
	else
	return false;
}

void subMenuRight(void)
{
	char subButton;
	char lastButton='5';
	subButton=getButton();
	sprintf(menuStr1, "Adjust kp");
	sprintf(menuStr2,"kp = %03d", kp);
	LcdTask();
	while(subButton!='4')
	{
		subButton=getButton();
		if(subButton =='1')
		{
			while(!buttonReleased());
				kp+=1;
            	sprintf(menuStr2,"kp = %03d", kp);
				LcdTask();
		 }
		else
		if(subButton =='2')
		{
			while(!buttonReleased());
				kp-=1;
            	sprintf(menuStr2,"kp = %03d", kp);
				LcdTask();
		 }
	}
	lastButton = subButton; 	
}
void subMenuLeft(void)
{
	char subButton;
	char lastButton='5';
	subButton=getButton();
	sprintf(menuStr1, "Adjust ki");
	sprintf(menuStr2,"ki = %03d", ki);
	LcdTask();
	while(subButton!='4')
	{
		subButton=getButton();
		if(subButton =='1')
		{
			while(!buttonReleased());
				ki+=1;
            	sprintf(menuStr2,"ki = %03d", ki);
				LcdTask();
		 }
		else
		if(subButton =='2')
		{
			while(!buttonReleased());
				ki-=1;
            	sprintf(menuStr2,"ki = %03d", ki);
				LcdTask();
		 }
	}
	lastButton = subButton; 	
}
void subMenuUp(void)
{
	char subButton;
	char lastButton='5';
	subButton=getButton();
	sprintf(menuStr1, "Run selection");
	LcdTask();
	while(subButton!='4')
	{
		subButton=getButton();
		if(subButton =='1')
		{
			while(!buttonReleased());
				runLineFlag=true;
				runWallFlag=false;
            	sprintf(menuStr2,"RunLine()");
				LcdTask();
				LCDGotoXY(15,0);
				LCDPutChar(' ');	// 0x20 using empty char blanks the up arrow
		 }
		else
		if(subButton =='2')
		{
			while(!buttonReleased());
				runWallFlag=true;
				runLineFlag=false;
            	sprintf(menuStr2,"RunWall()");
				LcdTask();
				LCDGotoXY(15,1);
				LCDPutChar(' ');	// 0x20 using empty char blanks the down arrow
		 }   
	}
	lastButton = subButton; 	
}
void subMenuDown(void)
{
	char subButton;
	char lastButton='5';
	subButton=getButton();
	 sprintf(menuStr1, "En_Bluetooth");
			LcdTask();
	while(subButton!='4')
	{
		subButton=getButton();
		if(subButton =='1')
		{
			while(!buttonReleased());
				k+=5;
            	sprintf(menuStr2,"kp = %03d", k);
				LcdTask();
		 }
		else
		if(subButton =='2')
		{
			while(!buttonReleased());
				k-=5;
            	sprintf(menuStr2,"kp = %03d", k);
				LcdTask();
		 }
	}
	lastButton = subButton; 	
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
/*
void menu(void)
{
	char x;
	x=getButton();


	while(x!='4')
	{
		x=getButton();
		
		if(x!='5')
		{
        	buttonMenu();
				
		}
		else
		{
			sprintf(menuStr1, "Run");
			sprintf(menuStr2, "Parameters");
			LcdTask();
			LCDGotoXY(15,0);
			LCDPutChar(1);	// display the up arrow
			LCDGotoXY(14,1);
			LCDPutChar(0x7F);	// display the left arrow
			LCDGotoXY(15,1);
			LCDPutChar(0x7E);	// display the right arrow
		}
		
	}
		
}

int main(void)
{
	LCDInit(16);
	AdcInit();
	while(1){
		buttonMenu();
		if(runLine){
			LCDBackLight(true);
	
		}
		if(runWall){
			LCDBackLight(false);
	
		}
	}
		
}

*/

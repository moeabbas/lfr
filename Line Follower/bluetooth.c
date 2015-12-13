// Morten/David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>			// for itoa() call
#include "lcd.h"
#include <stdio.h>			// define function sprintf
#include "serialio.h"		//USART
#include "Motor_Control.h"
#include "Floor_sensor.h"
#include "lcd.h"

char ReceivedByte;

//Parring code for the Bluetooth module is 1234//

/////////////////////////////////////////////////////////////
////          Initialize the USART and give Baudrate     ////
/////////////////////////////////////////////////////////////

void USARTInit(uint16_t ubrr_value)
{
   //Set Baud rate
   UBRRL = ubrr_value;
   UBRRH = (ubrr_value>>8);

   //Don't remember what this does
   UCSRC=(1<<URSEL)|(3<<UCSZ0);

   //Enable The receiver and transmitter
   UCSRB=(1<<RXEN)|(1<<TXEN);

   //Enable the USART Recieve Complete interrupt (USART_RXC) 
   UCSRB |= (1 << RXCIE); 

   //Enable the Global Interrupt Enable flag so that interrupts can be processed 
   sei(); 
}

void USARTWriteChar(char data)
{
   //Wait untill the transmitter is ready

   while(!(UCSRA & (1<<UDRE)))
   {
      //Do nothing
   }

   //Now write the data to USART buffer

   UDR=data;
}

void bluetoothScan(void){

	uint8_t floorSensors=0;

	if(getSensorUpdateFlag()){

	floorSensors = readFloorSensors();

	showFloorInfo(floorSensors);

	uint8_t i = 0;
	uint8_t value = 0;
	uint8_t dataTemp = floorSensors;
	
		while(i <=6){
			value = ((dataTemp & (64>>i)) && 1);
			if(value==0){
			USARTWriteChar('-');
			}
			if(value==1){
			USARTWriteChar('+');
			}
			i++;
		}
		USARTWriteChar(10);
		clearSensorUpdateFlag();
	}

}



void bluetoothControl(void){
	

	uint8_t bluetoothFlag = 1;
	
	setDirectionMotorL(0);
	setDirectionMotorR(0);
	setDutyCycleMotorL(0);
	setDutyCycleMotorR(0);

	while(bluetoothFlag){	

			while((UCSRA & (1 << RXC)) == 0){};

			if(ReceivedByte=='i'){	
				setKp(1.5);
				setKi(0);
				setDirectionMotorL(0);
				setDirectionMotorR(0);
				calcDuty(4, 0);	
			}

			if(ReceivedByte=='k'){	
				setKp(1.5);
				setKi(0);
				setDirectionMotorL(1);
				setDirectionMotorR(1);
				calcDuty(4, 0);	
			}

			if(ReceivedByte=='s'){
				setDutyCycleMotorL(0);
				setDutyCycleMotorR(0);
			}	
		
			if(ReceivedByte=='q'){
				setDutyCycleMotorL(0);
				setDutyCycleMotorR(0);
				bluetoothFlag=0;
			}	
	}

	LCDGotoXY(0,0);
	LCDPutString("Bluetooth test");
}


/////////////////////////////////////////////////////////////
////          Interrupt Service  Rutine                  ////
////           Received Bluetooth Data                   ////
/////////////////////////////////////////////////////////////

ISR(USART_RXC_vect)
	{
	// Fetch the received byte value into the variable " ByteReceived "
	ReceivedByte = UDR;
	//UDR = ReceivedByte;
	//UDR = 13;
	}


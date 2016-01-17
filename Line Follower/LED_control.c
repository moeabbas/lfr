/////////////////////////////////////////////////////////////////////////
//                        LED_control.C                                //
//                                                                     //
//Functions for 74HC256 Shift register						           //
//                                                                     //
//Version         : 1.2                                                //
//Date            : 28.12.2012                                         //
//Author          : David Sigurdsson // dabbis@gmail.com               //
//Chip type       : ATmega32                                           //
//This code may not be used by anyone unless otherwise agreed upon.    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "defines.h"
#include "LED_control.h"
#include "ADC.h"

/////////////////////////////////////////////////////////////////////////
//                       Public functions                              //
/////////////////////////////////////////////////////////////////////////

// Initialize the LED_control
void LEDInit(void){

	// SPI Control register SPCR
	// bit7 SPIE 	0 = Interupt disabled.
	// bit6 SPE		1 = SPI Enable.
	// bit5 DORD	0 = MSB first.
	// bit4 MSTR	1 = Set to Master.
	// bit3 CPOL	1 = SCK low when idle
	// bit2 CPHA	1 = setup falling, sample rising (mode3)
	// bit1	SPR1	0 = fosc/4 = 2.5 MHz
	// bit0 SPR0 	0
	SPCR = 0b01011100;

	// SPI Status Register SPSR
	SPSR = 0b00000000;

	//set directions
	setOutput(DDRB, SPI_CLK);
	setOutput(DDRB, SPI_MOSI);
	setOutput(DDRB, SPI_SS);
	setOutput(DDRC, LED_OE);

	// Set the initial output values for outputs
	setLow(PORTB, SPI_MOSI);
	setLow(PORTB, SPI_SS);
	setHigh(PORTC, LED_OE);		//default off

	// Initilize Timer/Counter 2		
	TCCR2 = 0b00001101;
	// WGM CTC, OCR2 is TOP
	// clk/128 prescaler = 78.125 Hz
	
	TCNT2 = 0;	// reset counter0
	OCR2 = 25;	// Counter divisor 78.125 Hz/25 = 3005 Hz 

	// Update, set all Led's off
	LEDUpdate(0);
}

//Send data through SPI
void LEDWrite(uint8_t data){
	SPDR = data;
	while(!(SPSR & 0b10000000)){}
}

// Send new values to shift registers.
void LEDUpdate(uint8_t ledBank0){
	setLow(PORTB, SPI_SS);
	LEDWrite(ledBank0);
	setHigh(PORTB, SPI_SS);
}

// Enable All Leds
void LEDEnable(){
	setLow(PORTC,LED_OE);
}

// Disable all Leds
void LEDDisable(){
	setHigh(PORTC, LED_OE);
}

// Get a ADC value for battery and decode for Led's 
void LEDVoltage(){
	
	int ADCValue = AdcConvert(3);

	if(ADCValue >= 845){
		LEDUpdate(255);
	}
	else if(815<=ADCValue){
		LEDUpdate(127);
	}
	else if(779<=ADCValue){
		LEDUpdate(63);
	}
	else if(745<=ADCValue){
		LEDUpdate(31);
	}
	else if(710<=ADCValue){
		LEDUpdate(15);
	}
	else if(680<=ADCValue){
		LEDUpdate(7);
	}
	else if(644<=ADCValue){
		LEDUpdate(3);
	}
	else if(607<=ADCValue){
		LEDUpdate(1);
	}
	else{
		LEDUpdate(0);
	}
}
/////////////////////////////////////////////////////////////////////////
//                        LED_control.C                                //
//                                                                     //
//Functions for 74HC256 Shift register						           //
//                                                                     //
//Version         : 1.2                                                //
//Date            : 01.12.2015                                         //
//Author          : Mohamed Abbas                                      //
//Chip type       : ATmega32                                           //
//This code may not be used by anyone unless otherwise agreed upon.    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "defines.h"
#include "LED_control.h"
#include "ADC.h"

/////////////////////////////////////////////////////////////////////////
//                       Local variables                               //
/////////////////////////////////////////////////////////////////////////

uint8_t ledBank0 = 0;
uint8_t ledBank1 = 254;
uint8_t RGB[2][3] = {{0}};

// Counter for the PWM 0-15
volatile uint8_t pwm = 0;

// Counter for the update interval for the LED's
volatile uint8_t counter = 0;

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
	LEDUpdate();
}

//Send data through SPI
void LEDWrite(uint8_t data){
	SPDR = data;
	while(!(SPSR & 0b10000000)){}
}

// Send new values to shift registers.
void LEDUpdate(){
	setLow(PORTB, SPI_SS);
	LEDWrite(ledBank1);
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

// Turn LCD backlight on
void LEDLcdBackLightOn(){
	ledBank1 |= 0b00000001;
	LEDUpdate();
}

// Turn LCD backlight off
void LEDLcdBackLightOff(){
	ledBank1 &= 0b11111110;
	LEDUpdate();
}

/*
Change LED on/off. 
ledBank0 is LED voltage meter (red1,red2,green1...green6)
ledBank1 is LCD backlight(0) extra outputs(1-7)

Color		dec		bit
-----------------------
Red left	126		1
Red right	252		7
Green left	250		2
Green right	190		6
Blue left	246		3
blue right	222		5 */

void LEDChangeLed(uint8_t bank, uint8_t data){
	if(bank == 0){
		ledBank0 = data;
	}
	if(bank == 1){
		ledBank1 = data;
	}
	LEDUpdate();
}

// Set all Red LED on
void setRed(){
	LEDChangeLed(1,124);
}

// Set all Green LED on
void setGreen(){
	LEDChangeLed(1,186);
}

// Set all Blue LED on
void setBlue(){
	LEDChangeLed(1,214);
}

// Set all LED off
void allOff(){
	LEDChangeLed(1,254);
}

// Get a ADC value for battery and decode for Led's 
void LEDVoltage(){
	
	int ADCValue = AdcConvert(3);

	if(ADCValue >= 845){
		LEDChangeLed(0,255);
	}
	else if(815<=ADCValue){
		LEDChangeLed(0,127);
	}
	else if(779<=ADCValue){
		LEDChangeLed(0,63);
	}
	else if(745<=ADCValue){
		LEDChangeLed(0,31);
	}
	else if(710<=ADCValue){
		LEDChangeLed(0,15);
	}
	else if(680<=ADCValue){
		LEDChangeLed(0,7);
	}
	else if(644<=ADCValue){
		LEDChangeLed(0,3);
	}
	else if(607<=ADCValue){
		LEDChangeLed(0,1);
	}
	else{
		LEDChangeLed(0,0);
	}
	LEDUpdate();
}

// Rainbow effect, fades from red to green to blue and back to red and loop
// Uses the Timer/counter2, 4 bit PWM. Runs until rainBowOff().
// Uses the changeColor() and calcPWM() functions.
void rainBowOn(){
	// Enable Compare Match interrupt.
	TIMSK |= 128;
}

void rainBowOff(){
	TIMSK &= ~128;
	allOff();
}

void changeColor(){

	static volatile uint8_t red = 0;
	static volatile uint8_t green = 0;
	static volatile uint8_t blue = 0;
	static volatile uint8_t color = 0;

	switch(color){

		// First step, add Red, only once.
		case 0:
		if(red != 15){
			red++;
			break;
		}else{
			color = 1;	
		}
			
		// Add Green
		case 1:
		if(green != 15){
			green++;
			break;
		}else{
			color = 2;
			break;
		}
		
		// Remove Red
		case 2:
		if(red != 0){
			red--;
			break;
		}else{
			color = 3;
			break;
		}

		// Add blue
		case 3:
		if(blue != 15){
			blue++;
			break;
		}else{
			color = 4;
			break;
		}

		// Remove Green
		case 4:
		if(green != 0){
			green--;
			break;
		}else{
			color = 5;
			break;
		}
		
		// Add Red
		case 5:
		if(red != 15){
			red++;
			break;
		}else{
			color = 6;
			break;
		}
		
		// Remove blue
		case 6:
		if(blue != 0){
			blue--;
			break;
		}else{
			color = 1;
			break;
		}
	}

	RGB[0][0] = red;
	RGB[1][0] = red;
	RGB[0][1] = green;
	RGB[1][1] = green;
	RGB[0][2] = blue;
	RGB[1][2] = blue;
}

// Set the duty cycle for the leds
void calcPWM(){

	// Red Left
	if(RGB[0][0] <= pwm){
		ledBank1 |= (1<<1); //off
	}else{
		ledBank1 &= ~(1<<1); //on
	}
	
	// Green Left
	if(RGB[0][1] <= pwm){
		ledBank1 |= (1<<2);
	}else{
		ledBank1 &= ~(1<<2);
	}

	// Blue Left
	if(RGB[0][2] <= pwm){
		ledBank1 |= (1<<3);
	}else{
		ledBank1 &= ~(1<<3);
	}
	
	// Red Right
	if(RGB[1][0] <= pwm){
		ledBank1 |= (1<<7);
	}else{
		ledBank1 &= ~(1<<7);
	}

	// Green Right
	if(RGB[1][1] <= pwm){
		ledBank1 |= (1<<6);
	}else{
		ledBank1 &= ~(1<<6);
	}

	// Blue Right
	if(RGB[1][2] <= pwm){
		ledBank1 |= (1<<5);
	}else{
		ledBank1 &= ~(1<<5);
	}
}

/////////////////////////////////////////////////////////////////////////
//                       Interrupt service routines                    //
/////////////////////////////////////////////////////////////////////////

ISR(TIMER2_COMP_vect){

	// PWM  count 0-15
	pwm++;
	if(pwm > 15){
		pwm = 0;
	}
	
	// Set new values for LED's
	counter++;
	if(counter == 100){
		counter = 0;
		changeColor();
	}	

	// Update all LED's
	calcPWM();
	LEDUpdate();
}

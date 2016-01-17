// David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

#include <avr/interrupt.h>

/////////////////////////////////////////////////////////////////////////
//                       Port and pin definitions                      //
/////////////////////////////////////////////////////////////////////////

#define SPI_CLK		7
#define SPI_MOSI	5
#define SPI_SS		4
#define LED_OE		1

/////////////////////////////////////////////////////////////////////////
//                       Function prototypes:        	               //
/////////////////////////////////////////////////////////////////////////

// Initialize the LED_control
void LEDInit(void);

//Send data through SPI
void LEDWrite(uint8_t data);

// Change LED on/off for LED voltage meter.
// Send new values to shift registers.
void LEDUpdate(uint8_t data);

// Enable All Leds
void LEDEnable();

// Disable All Leds
void LEDDisable();

// Get a ADC value for battery and decode for Led's 
void LEDVoltage();
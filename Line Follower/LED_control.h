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

// Send new values to shift registers.
void LEDUpdate();

// Enable All Leds
void LEDEnable();

// Disable All Leds
void LEDDisable();

// Turn LCD backlight on
void LEDLcdBackLightOn();

// Turn LCD backlight off
void LEDLcdBackLightOff();

// Change LED on/off. 
// ledBank0 is LED voltage meter 
// ledBank1 is LCD backlight(0) extra outputs(1-7)
void LEDChangeLed(uint8_t bank, uint8_t data);

// Set all Red LED on
void setRed();

// Set all Green LED on
void setGreen();

// Set all Blue LED on	
void setBlue();

// Set all LED off
void allOff();

// Get a ADC value for battery and decode for Led's 
void LEDVoltage();

//Rainbow effect
void rainBowOn();
void rainBowOff();
void changeColor();
void calcPWM();

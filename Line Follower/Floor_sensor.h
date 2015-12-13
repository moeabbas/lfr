// David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

#include <avr/interrupt.h>

/////////////////////////////////////////////////////////////////////////
//                       Port and pin definitions                      //
/////////////////////////////////////////////////////////////////////////

#define L3	7 	// L4 on schematic
#define L2	6	// L3
#define L1	5	// L2
#define C1	4	// L1
#define R1	2	// R2
#define R2	1	// R3
#define R3	0	// R4

// Initialize the Floor Sensor.
void initFloorSensor();

// Read the current value from the floor sensors.
uint8_t readFloorSensors();

// Calculates the error from floor sensor and change various flags.
signed char calcFloorErrorAndFlagControl();

// Write on display duty cycle for Both Motors
// Current floor sensor values in Binary and dec
void showFloorInfo(uint8_t data);

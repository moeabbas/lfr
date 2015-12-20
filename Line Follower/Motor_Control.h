// David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

#include <avr/interrupt.h>

/////////////////////////////////////////////////////////////////////////
//                       Port and pin definitions                      //
/////////////////////////////////////////////////////////////////////////

#define BRIDGE_A1	5 	// Left Motor PWM
#define BRIDGE_A2	7	// Left Motor DIR
#define BRIDGE_A3	4	// Right Motor PWM
#define BRIDGE_A4	6	// Right Motor DIR
#define BRIDGE_EN	0	// H-bridge Enable
#define MOTOR_L_A	3	// Left Motor Sensor A
#define MOTOR_R_A	2	// Right Motor sensor A


#define MOTOR_FORWARD 0
#define MOTOR_REVERSE 1

/////////////////////////////////////////////////////////////////////////
//                       Function prototypes:        	               //
/////////////////////////////////////////////////////////////////////////

// Clear the interger sum Error
void clearIntSumError();

// Initialize the motor control.
void initMotorControl(void);

// Enable outputs on the H-bridge
void enableHBridge(void);

// Disable outputs on the H-bridge
void disableHBridge(void);

// Get sensor update Flag
uint8_t getSensorUpdateFlag(void);

// Clear Sensor update Flag
void clearSensorUpdateFlag(void);

// Set duty cycle for Left Motor 0-250
void setDutyCycleMotorL(signed int duty);

// Set duty cycle for Right Motor 0-250
void setDutyCycleMotorR(signed int duty);

// Get the Current dutyCycle for Left Motor
signed int getDutyCycleMotorL();

// Get the Current dutyCycle for Left Motor
signed int getDutyCycleMotorR();

// Limits for the DutyCycle
uint8_t limitDutyCycle(signed int duty);

// Set direction for Left Motor. 0 forward, 1 backward
void setDirectionMotorL(uint8_t dir);

// Set direction for Right Motor. 0 forward, 1 backward
void setDirectionMotorR(uint8_t dir);

// Get the count value from left Motor
uint8_t getSensorMotorL();

// Get the count value from Right Motor
uint8_t getSensorMotorR();

// PI controller. 
// Computes the Duty Cycle for desired speed and bias and set the dutyCycle.
void calcDuty(uint16_t ref, float error);

// Write on display duty cycle for Both Motors, 1st line on LCD
void showMotorInfo();

void setKp(float value);

void setKi(float value);

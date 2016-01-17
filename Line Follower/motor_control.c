/////////////////////////////////////////////////////////////////////////
//                        Motor_Control.C                              //
//                                                                     //
//Functions for L293/298 H-bridge and Tachometer for motor             //
//                                                                     //
//Version         : 1.05                                               //
//Date            : 05.10.2012                                         //
//Author          : David Sigurdsson // dabbis@gmail.com               //
//Chip type       : ATmega32                                           //
//This code may not be used by anyone unless otherwise agreed upon.    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////
/*
Version history
1.0  (23.09.12) Working version
1.01 (05.10.12) Fixed error in counter0, error was the counter resets
				itself when overflow not on OCR0. 
1.02 (09.12.12) Changed PWM range from 0-100 to 0-250
1.05 (12.12.12) integrated PI controller into this module.
*/
#include <stdio.h>
#include "defines.h"
#include "Motor_Control.h"
#include "lcd.h"



/////////////////////////////////////////////////////////////////////////
//                       Local variables                               //
/////////////////////////////////////////////////////////////////////////

// Current duty cycle for the motors.
signed int dutyL = 0;
signed int dutyR = 0;

// Integer sum for the I-controller.
float intL = 0;
float intR = 0;

// Integer anti windup prodection
uint8_t windL = 1;
uint8_t windR = 1;

// PI Parameters.
float Kp = 1.5;
float Ki = 2.0;

// Time constant.
float T = 0.01;

// Error values.
float errorL = 0;	// Left Motor error
float errorR = 0;	// Right Motor error

// Current values (temp) and interval values from the Tachometers.
volatile uint8_t sensorMotorR_temp = 0;
volatile uint8_t sensorMotorL_temp = 0;
volatile uint8_t sensorMotorR = 0;
volatile uint8_t sensorMotorL = 0;
volatile uint8_t sensorUpdateFlag = 0;


/////////////////////////////////////////////////////////////////////////
//                       Public functions                              //
/////////////////////////////////////////////////////////////////////////

// Clear the interger sum Error
void clearIntSumError(){
	intL = 0;
	intR = 0;	
}

// Initialize the motor control.
void initMotorControl(void){
	// Set the directions.
	setOutput(DDRD, BRIDGE_A1);
	setOutput(DDRD, BRIDGE_A2);
	setOutput(DDRD, BRIDGE_A3);
	setOutput(DDRD, BRIDGE_A4);
	setOutput(DDRD, BRIDGE_EN);
	setInput(DDRD, MOTOR_L_A);
	setInput(DDRD, MOTOR_R_A);


	// Set the initial output values for outputs.
	setLow(PORTD, BRIDGE_A1);
	setHigh(PORTD, BRIDGE_A2);
	setLow(PORTD, BRIDGE_A3);
	setHigh(PORTD, BRIDGE_A4);
	setLow(PORTC, BRIDGE_EN);


	// Initilize Timer/counter 1
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
	// Both PMW outputs set at TOP, clear on compare
	// Waveform generation mode (WGM) set to 14 (fast PWM TOP is ICR1)
	// Input capture noise canceler (ICNC1) disabled, Input capture Edge select (ICES1) set to falling edge.
	// NOTE: when ICR1 is set to TOP the input capture function is disabled thus ICES1 and ICNC1 are cleared.
	// no prescaler is used clk/1

	TCNT1 = 0; 		//reset counter1
	OCR1A = 250; 	//speed of Left Motor 0-250
	OCR1B = 250; 	//speed of Right Motor 0-250
	ICR1 = 250; 	// Top value for TCNT1 defines the freq of the PMW (10MHz / 250 = 40 kHz)


	// Initilize Timer/Counter 0		
	TCCR0 = 0b00001101;
	// WGM CTC, OCR0 is TOP
	// OC0 disconnected.
	// clk/1024 prescaler = 9,765.625 Hz
	
	TCNT0 = 0;	// reset counter0
	OCR0 = 98;	// Counter divisor 9,765.625/98 = 99,64923469 Hz ~ 10 ms period

	// Enabled Timer/Counter0 Compare Match interrupt.
	TIMSK |= 2;


	// External interrupts... 
	//General Interrupt Control Register
	GICR |= 0b11000000;
	// bit 6 = 1 = Enable INT1
	// bit 7 = 1 = Enable INT0

	//MCU Control Register
	MCUCR |= 0b00001111;
	// bit1, bit0 = 11 = positive logical change on INT0 generates an interupt request.
	// bit3, bit2 = 11 = positive logical change on INT1 generates an interupt request

	//MSU Control and Status Register
	MCUCSR |= 0b00000000;

	// General Interrupt Flag Register
	GIFR   |= 0b11000000;
}

// Enable outputs on the H-bridge
void enableHBridge(void){
	setHigh(PORTC, BRIDGE_EN);
}

// Disable outputs on the H-bridge
void disableHBridge(void){
	setLow(PORTC, BRIDGE_EN);
}

// Get sensor update Flag
uint8_t getSensorUpdateFlag(){
	return sensorUpdateFlag;
}

// Clear Sensor update Flag
void clearSensorUpdateFlag(){
	sensorUpdateFlag = 0;
}

// Set duty cycle for Left Motor 0-100%
void setDutyCycleMotorL(signed int duty){
	
	OCR1A = limitDutyCycle(duty);
}

// Set duty cycle for Right Motor 0-100%
void setDutyCycleMotorR(signed int duty){
	
	OCR1B = limitDutyCycle(duty);	
}


// Limits for the DutyCycle
uint8_t limitDutyCycle(signed int duty){

	// Limit the range to 0-250 for protection.
	if(duty>250){
		duty = 250;
	}
	if(duty<0){
		duty = 0;
	}
	return duty;
}

// Set direction for Left Motor. 0 forward, 1 backward
void setDirectionMotorL(uint8_t dir){

	if(dir)
	{
		setLow(PORTD, BRIDGE_A2);
		TCCR1A &= ~(1 << 6);
	}
	else
	{
		setHigh(PORTD, BRIDGE_A2);	
		TCCR1A |= 1 << 6;
	}
}

// Set direction for Right Motor 0 forward, 1 backward
void setDirectionMotorR(uint8_t dir){

	if(dir){
		setLow(PORTD, BRIDGE_A4);
		TCCR1A &= ~(1 << 4); 
	}else{
		setHigh(PORTD, BRIDGE_A4);
		TCCR1A |= 1 << 4; 
	}
}

// Get the count value from left Motor
uint8_t getSensorMotorL(){
	return sensorMotorL;
}

// Get the count value from Right Motor
uint8_t getSensorMotorR(){
	return sensorMotorR;
}

// PI controller. 
// Computes the Duty Cycle for desired speed and bias and set the dutyCycle.
void calcDuty(uint16_t ref, float error){
	
	// Find the error values
	errorL = ref - (float)getSensorMotorL() - error / 2;
	errorR = ref - (float)getSensorMotorR() + error / 2;
	
	// Anti windup
	if((dutyL >=250) || (dutyL < 0)){
		windL = 0;
	}
	else{
		windL = 1;
	}			

	if((dutyR >=250) || (dutyR < 0)){
		windR = 0;
	}
	else{
		windR = 1;
	}	

	// Calculate integration sum.
	intL = intL + errorL*T*windL;
	intR = intR + errorR*T*windR;


	// Calculate new DutyCycle values.
	// *12.5 to transform from pulses to DutyCycle
	dutyL = (signed int)((intL*Ki + errorL*Kp)*12.5);
	dutyR = (signed int)((intR*Ki + errorR*Kp)*12.5);

	setDutyCycleMotorL(dutyL);
	setDutyCycleMotorR(dutyR);

	// Show 
	showMotorInfo();

}

void setKp(float value){
	Kp = value;
}

void setKi(float value){
	Ki = value;
}

// Write on display duty cycle for Both Motors, 1st line on LCD
void showMotorInfo(){
	
	char buffer[20];
	LCDGotoXY(0,0);
	sprintf(buffer, "%04i %04i",dutyL, dutyR);
	LCDPutString(buffer);
}


/////////////////////////////////////////////////////////////////////////
//                       Interrupt service routines                    //
/////////////////////////////////////////////////////////////////////////

// Interrupt from SensorA, Right Motor
ISR(INT0_vect){
	sensorMotorR_temp++;
}

// Interrupt from SensorB, Left Motor
ISR(INT1_vect){
	sensorMotorL_temp++;
}

// Safe current value of counts from the sensors every ~10ms and clear the counters.
ISR(TIMER0_COMP_vect){
	sensorMotorR = sensorMotorR_temp;
	sensorMotorR_temp = 0;
	sensorMotorL = sensorMotorL_temp;
	sensorMotorL_temp = 0;
	sensorUpdateFlag = 1;
}

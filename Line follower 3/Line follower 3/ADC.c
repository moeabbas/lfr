//Date            : 01.12.2015                                         
//Author          : Mohamed Abbas                                       
// This code may not be used by anyone unless otherwise agreed upon.

/****************************************************************************
*        
*          Module: Adc.C handles analog-2-digital conversions              *
******************************************************************************/
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>


char ADC0Str[10];

/******************************************************************************/
// Function name: AdcConvert 
// Function task: make a 10 bit conversion from requested channel 0..7
// Input: channel number, 0..7
// Return: 10 bit result, 0..1023 
/******************************************************************************/
uint16_t AdcConvert (uint8_t channel)
{
   ADMUX = 0x40+channel;            // use Avcc+ ref with capacitor, select channel
   ADCSRA |= 0x50;                  // clear EOC-flag, new start conversion
   while ((ADCSRA & 0x10) == 0);    // Waiting for conversion complete
   return ADC;                      // Return 16 bit ADC value
}


/******************************************************************************/
// Function name: AdcInit 
// Function task: start the prescaler, set clock div 128
// Input: 
// Return: 
/******************************************************************************/
void AdcInit (void)
{
   ADCSRA = 0x87;                   // enable ADC, clear EOC-flag, set clock div.
}

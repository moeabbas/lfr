//Date            : 01.12.2015                                         
//Author          : Mohamed Abbas                                       
// This code may not be used by anyone unless otherwise agreed upon.

/******************************************************************************
*                                                                             *   
*               Module: Header file for ADC interface ADC.H                   *
******************************************************************************/

#include <avr/interrupt.h>
#ifndef ADC_H
#define ADC_H


extern void AdcInit (void);
extern uint16_t AdcConvert (uint8_t channel);

// the following definitions are required in order to use the bool type
#define true      1
#define false     0
#define bool      _Bool

#endif



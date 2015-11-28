// Deliang/David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

/******************************************************************************
*                                                                             *   
*               Module: Header file for buttonMenu buttonMenu.H                   *
******************************************************************************/

#include <avr/interrupt.h>
#include "defines.h"

#define buttonMenu_H

void buttonMenu(void);
void menu(void);
void subMenuRight(void);
void subMenuLeft(void);
void subMenuUp(void);
void subMenuDown(void);
void LcdTask (void);
char ADCReadButton(void);
char getButton(void);

bool getRunLineFlag(void);
bool getRunWallFlag(void);
void setRunLineFlag(bool value);
void setRunWallFlag(bool value);

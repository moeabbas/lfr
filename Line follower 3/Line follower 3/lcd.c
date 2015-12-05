/////////////////////////////////////////////////////////////////////////
//                        LCD.C                         2010-02-05 AgF //
//                                                                     //
// Functions for LCD display. For AVR microcontroller.                 //
//                                                                     //
// Instructions:                                                       //
// Connect LCD display to the ports and pins defined in lcd.h          //
// Call LCDInit() first with the number of columns on the display.     //
//                                                                     //
// This version has no handshaking. You may connect LCD_RD to ground   //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "lcd.h"

#include "buttonMenu.h"
/////////////////////////////////////////////////////////////////////////
//                       Local variables                               //
/////////////////////////////////////////////////////////////////////////
static unsigned char LCD_base_y[4] = {0x80, 0xC0, 0, 0};
static unsigned char LCD_x, LCD_y, LCD_maxx;


/////////////////////////////////////////////////////////////////////////
//                    Functions used internally                        //
/////////////////////////////////////////////////////////////////////////

// Delay. Circa 200 us at 10 MHz
// The data sheet says that 40 us is sufficient, but the displays we have
// require more than 100 us to work properly.
void LCD_delay(void) __attribute__ ((noinline)); // noinline prevents problems when optimizing with option -O3
void LCD_delay(void)  {
   __asm__ __volatile__("ldi r31,65 \n 1:\n dec r31 \n brne 1b" ::: "r31");
}

// Long delay. Circa 26 ms at 10 MHz
void LCD_long_delay(void) __attribute__ ((noinline));
void LCD_long_delay(void) {
   __asm__ __volatile__("clr r26 \n clr r27 \n 2:\n sbiw r26,1\n brne 2b \n" ::: "r26", "r27");
}

// Write 4 bits
static void LCD_write_nibble(char data) {
   const char DataMask = 0x0F << LCD_DATA4;
   LCD_PORT = (LCD_PORT & ~DataMask) | (data & DataMask);
   LCD_PORT |= (1<<LCD_ENABLE); // EN=1
   LCD_delay();
   LCD_PORT &= ~(1<<LCD_ENABLE); // EN=0
   LCD_delay();
}

// Write 8 bits. RS = 0 or 1
static void LCD_write_data(unsigned char data) {
   LCD_write_nibble(data);            // RD=0, write MSN
   LCD_write_nibble(data << 4);       // write LSN
}

// Write a byte to the LCD character generator or display RAM
void LCD_write_byte(unsigned char addr, unsigned char data) {
   LCD_delay();
   LCD_PORT &= ~(1<<LCD_RS);       // RS=0
   LCD_write_data(addr);
   LCD_delay();
   LCD_PORT |= (1<<LCD_RS);        // RS=1
   LCD_write_data(data);
}
/******************************************************************************/
// Function name: LcdDrvCgramWr
// Function task: tx array of 8 bytes to CG RAM addr 0..7
// Input: char with address in CG-RAM [0..7] + array with 8 bytes to the CG-RAM
// Return:
/******************************************************************************/
static void LcdCgramWr (uint8_t addr, const uint8_t *d)
{                        
   uint8_t i;
   LCD_write_data(0x40 + 8*addr);
        // cmd to LCD to receive data to CG-Ram
   for (i = 0; i <= 7; i++) {
    
      LCDPutChar(*d);               // tx 8 char to CG-Ram
      d++;                          // point to next value
   }
   LCD_write_data(0x80);                 // back to DD ram
}

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//                       Public functions                              //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

// Initialize the LCD controller. Specify the number of columns
void LCDInit(unsigned char lcd_columns) {
	const uint8_t cUp[] = {0x04,0x0E,0x15,0x04,0x04,0x00,0x00,0x00,};
	const uint8_t cDown[] = {0x04,0x04,0x15,0x0E,0x04,0x00,0x00,0x00};

   LCD_PORT &= ~((1<<LCD_ENABLE) | (1<<LCD_RS)); // EN=0, RS=0
   LCD_PORT &= ~(1<<LCD_RD); // Set RD = 0 in case it is connected
   LCD_DIRECTION |= (0xF << LCD_DATA4) | (1<<LCD_RS) | (1<<LCD_ENABLE) ; // set all as output
   //LCD_DIRECTION |= (1<<LCD_RD); // set RD as output if it is connected
   LCD_maxx = lcd_columns;
   LCD_base_y[2] = lcd_columns+0x80;
   LCD_base_y[3] = lcd_columns+0xc0;
   LCD_long_delay();
   LCD_write_nibble(0x30);
   LCD_long_delay();
   LCD_write_nibble(0x30);
   LCD_long_delay();
   LCD_write_nibble(0x30);
   LCD_long_delay();
   LCD_write_nibble(0x20);
   LCD_long_delay();
   LCD_write_data(0x28);
   LCD_long_delay();
   LCD_write_data(4);
   LCD_long_delay();
   LCD_write_data(0x85);
   LCD_long_delay();
   LCD_PORT &= ~(1<<LCD_RS);
   LCD_write_data(6);
   
   LcdCgramWr(0,cDown);
   LcdCgramWr(1,cUp);
   LCDClear();
   
}

// Clear the LCD display
void LCDClear(void) {
   LCD_long_delay();
   LCD_PORT &= ~(1<<LCD_RS);          // RS=0
   LCD_write_data(2);                 // cursor home
   LCD_long_delay();
   LCD_write_data(0xC);               // cursor off
   LCD_long_delay();
   LCD_write_data(1);                 // clear
   LCD_long_delay();
   LCD_x = LCD_y = 0;
}

// Set the LCD display position
void LCDGotoXY(unsigned char x, unsigned char y) {
   LCD_delay();
   LCD_PORT &= ~(1<<LCD_RS);       // RS=0
   LCD_write_data(LCD_base_y[y]+x);
   LCD_x = x;
   LCD_y = y;
   LCD_delay();
   //LCD_long_delay();
}

// write a zero-terminated ASCII string to the display
void LCDPutString(const char *str) {
   char c;
   for (; (c = *str) != 0; str++) LCDPutChar(c);
}

// write a single ASCII character to the display
void LCDPutChar(char c) {
   if (c == '\n') {
      // newline character goes to next line
      ++LCD_y;
      LCDGotoXY(0,LCD_y);
   }
   else {
      // any other character
      ++LCD_x;
      if (LCD_x>LCD_maxx) {
         // end of line. go to next line
         ++LCD_y;
         LCDGotoXY(0,LCD_y);
      }
      LCD_delay();
      LCD_PORT |= (1<<LCD_RS);  // RS = 1
      LCD_write_data(c);
      LCD_delay();
   }
}

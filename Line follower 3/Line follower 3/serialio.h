//                        SERIALIO.C                          2010-02-02 AgF
//
// Functions for serial communication. For AVR microcontroller.
//
// The default communications settings are 19200 baud, 8 data bits, no
// parity, 1 stop bit.
//
// A driver for USB to serial converter is needed when connecting to a
// PC through a USB cable.
//
// RXD is connected to port D bit 0.
// TXD is connected to port D bit 1.
/////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stdio.h>


//////////////////////////////////////////////////////////////////////
//
//            functions
//
//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus  // Use C-style linking when compiled as C++
extern "C" {
#endif

// initialize UART
void SerialInitialize();


// Write one character to serial connection
int SerialPutChar(int c);


// Read one character from serial connection.
// Will wait if no character ready
char SerialGetChar(void);


// Tell if input is ready for reading.
// Returns 1 if there is input, 0 if no input.
char SerialInputReady();


// Write a zero-terminated ASCII string followed by a newline ("\r\n")
int SerialPutString(const char *str);


//	Reads a character string terminated by the newline character ('\r' or '\n')
// The newline character will be replaced with 0.
// The maximum length of the string is len. If len characters were read
// without encountering the newline character, then the string is terminated
// with 0 and the function returns.
// str must be a character array of at least len characters.
char *SerialGetString(char *str, unsigned char len);

// Write formatted output. See any C language textbook for details.
// Warning: Will fail if string longer than 81 characters!
// Floating point not supported.
int SerialPrintf(const char *format, ...)
__attribute__ ((format (printf,1,2))); // additional syntax check

#ifdef __cplusplus
}
#endif

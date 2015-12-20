// David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

// set interrupt
#define SetInterrupt()   __asm__ __volatile__ ("sei" ::);

// clear interrupt
#define ClearInterrupt() __asm__ __volatile__ ("cli" ::);

// Sets pin as output
#define setOutput(ddr, pin) ((ddr) |= (1 << (pin)))

// Sets pin as input
#define setInput(ddr, pin) ((ddr) &= ~(1 << (pin)))

// Sets pin low on port
#define setLow(port, pin) ((port) &= ~(1 << (pin)))

// Sets pin high on port
#define setHigh(port, pin) ((port) |= (1 << (pin)))

#define F_CPU 10000000UL

// the following definitions are required in order to use the bool type
#define true      1
#define false     0
#define bool      _Bool



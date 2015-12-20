// David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

#include <avr/interrupt.h>

void searchMode();

void runLine();

void go(signed int dist, uint8_t dir, uint8_t currentSpeed);

void setStopFlag(uint8_t flag);

void setThreeSixtyFlag(uint8_t flag);

void setHighSpeedFlag(uint8_t flag);

void setFoundLineFlag(uint8_t flag);

void setLostLineFlag(uint8_t flag);

uint8_t getStopFlag();

uint8_t getThreeSixtyFlag();

uint8_t getHighSpeedFlag();

uint8_t getFoundLineFlag();

void test();

#define DIRECTION_STRAIGHT 0
#define DIRECTION_LEFT     1
#define DIRECTION_RIGHT    2

// speed MAX ~17-18.
#define SPEED_HIGH   16
#define SPEED_NORMAL 12
#define SPEED_LOW    6
#define SPEED_CREEP  2
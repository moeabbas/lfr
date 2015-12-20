/*
 * AdvancedMotorControl.h
 *
 * Created: 20/12/2015 15:36:13
 *  Author: Mohamed
 */ 


#ifndef ADVANCEDMOTORCONTROL_H_
#define ADVANCEDMOTORCONTROL_H_

#include <stdint.h>

#define DIRECTION_ARC_LEFT  0
#define DIRECTION_ARC_RIGHT 1

void driveArc(uint8_t distance, uint8_t direction, uint8_t speed, uint8_t degree);

#endif /* ADVANCEDMOTORCONTROL_H_ */
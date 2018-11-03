/*
 * joystick.h
 *
 * Created: 26-05-2018 16:29:43
 *  Author: amavin
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>

#define JOYSTICK_UP _BV(PINC6)
#define JOYSTICK_RIGHT _BV(PINC1)
#define JOYSTICK_DOWN _BV(PINC0)
#define JOYSTICK_LEFT _BV(PINC7)
#define JOYSTICK_PUSH _BV(PIND3)


#define JOYSTICK_TASK_PERIOD 50

void joystick_task(void *pvParameters);
uint8_t read_joystick();


#endif /* JOYSTICK_H_ */
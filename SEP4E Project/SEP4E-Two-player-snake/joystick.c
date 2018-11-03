/*
 * joystick.c
 *
 * Created: 26-05-2018 16:29:55
 *  Author: amavin
 */ 
#include "game.h"
#include "joystick.h"

#include <FreeRTOS.h>

uint8_t read_joystick()
{
	return (PINC & _BV(PINC7)) | (PINC & _BV(PINC1)) | (PINC & _BV(PINC6)) | (PINC & _BV(PINC0)) | (PIND & _BV(PIND3));
}


void joystick_task(void *pvParameters)
{
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	//vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );
	#endif

	TickType_t joystick_task_lastwake = xTaskGetTickCount();
	
	while(1)
	{
		//Set task period
		vTaskDelayUntil(&joystick_task_lastwake, JOYSTICK_TASK_PERIOD);
		
		//Action
		if((read_joystick() & JOYSTICK_LEFT) == 0)
		{
			snake_change_direction(PL_ONE, D_LEFT);
		}
		if((read_joystick() & JOYSTICK_RIGHT) == 0)
		{
			snake_change_direction(PL_ONE, D_RIGHT);
		}
		if((read_joystick() & JOYSTICK_UP) == 0)
		{
			snake_change_direction(PL_ONE, D_UP);
		}
		if((read_joystick() & JOYSTICK_DOWN) == 0)
		{
			snake_change_direction(PL_ONE, D_DOWN);
		}
		if((read_joystick() & JOYSTICK_PUSH) == 0)
		{
			restart_game();
		}
	}
	
}
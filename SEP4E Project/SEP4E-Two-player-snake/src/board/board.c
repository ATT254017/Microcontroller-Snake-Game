/*
* board.c
*
* Created: 27/11/2016 12:40:26
*  Author: IHA
*/

#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "../buffer/buffer.h"
#include "../serial/serial.h"
#include "board.h"

// Handle to serial COM instance
static serial_p _com_serial_instance = 0;

// Handler for COM RX queue
static QueueHandle_t _x_rx_com_queue = NULL;

// Pointer to application display handler
static display_callback_ptr_t cb_ptr = NULL;

// ----------------------------------------------------------------------------------------------------------------------
static void _com_call_back(serial_p _com_serial_instance, uint8_t serial_last_received_byte) {
	if (_x_rx_com_queue) {
		signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

		xQueueSendFromISR( _x_rx_com_queue, &serial_last_received_byte, &xHigherPriorityTaskWoken );

		if( xHigherPriorityTaskWoken != pdFALSE )
		{
			taskYIELD();
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------------
void init_com(QueueHandle_t x_rx_queue) {
	_x_rx_com_queue = x_rx_queue;
}

// ----------------------------------------------------------------------------------------------------------------------
void init_display_timer(display_callback_ptr_t cb)
{
	cb_ptr = cb;
	
	// Setup timer T0 to CTC, ~1309 Hz
	OCR0A = 10;
	TCCR0A |= _BV(WGM01); // Mode 2
	TIMSK0 |= _BV(OCIE0A); // Enable interrupt
	TCCR0B |= _BV(CS00) | _BV(CS02); // Prescaler 1024 - and start timer
}

// ----------------------------------------------------------------------------------------------------------------------
//ISR for display timer
ISR(TIMER0_COMPA_vect)
{
	if (cb_ptr)
	{
		cb_ptr();
	}
}

// ----------------------------------------------------------------------------------------------------------------------
uint8_t com_send_bytes(uint8_t *bytes, uint8_t len) {
	return serial_send_bytes(_com_serial_instance, bytes, len);
}
uint8_t com_send_byte(uint8_t byte) {
	return serial_send_byte(_com_serial_instance, byte);
}

// ----------------------------------------------------------------------------------------------------------------------
void init_board(void) {
	static buffer_struct_t _com_rx_buffer;
	static buffer_struct_t _com_tx_buffer;
	buffer_init(&_com_rx_buffer);
	buffer_init(&_com_tx_buffer);
	_com_serial_instance = serial_new_instance(ser_USART0, 115200UL, ser_BITS_8, ser_STOP_1, ser_NO_PARITY, &_com_rx_buffer, &_com_tx_buffer, _com_call_back);
	
	// Set Joystick pins as inputs:
	DDRC &= ~(_BV(PINC0) | _BV(PINC1) | _BV(PINC6) | _BV(PINC7)); 
	DDRD &= ~_BV(PIND3);

	// Pull up on Joystick inputs
	PORTC |= _BV(PORTC0) | _BV(PORTC1) | _BV(PORTC6) | _BV(PORTC7);
	PORTD |= _BV(PORTD3);
	
	// Shift register control pins to output
	// SER, RCK, SCK, G (enable output)
	DDRD |= _BV(DDD2) | _BV(DDD4) | _BV(DDD5) | _BV(DDD6);
	// G (enable output) high
	PORTD |= _BV(PORTD6);
	
	// Column pins to output
	DDRA |= 0xFF;
	DDRB |= _BV(DDB0) | _BV(DDB1);
	
	// Trace pins Task Monitor (R2R)
	DDRB |= _BV(DDB2) | _BV(DDB3) | _BV(DDB4) | _BV(DDB5);
}

#if (configUSE_TRACE_FACILITY == 1)
	/**********************************************************************//**
	 @ingroup trace
	 @brief Set PORTB bit 2-5, to the task switched into running by the operating system.
	
	 Called by the the traceTASK_SWITCHED_IN() macro in FreeRTOS.
	 Are enabled in FreeRTOSConfig.h
	 **********************************************************************/
	void switch_in(uint8_t task_no) {
		PORTB &= 0b11000011;
		PORTB |= task_no << 2;
	}
	/**********************************************************************//**
	 @ingroup trace
	 @brief Set PORTB bit 2-5, to zero when a task is switched out of running by the operating system.
	
	 Called by the the traceTASK_SWITCHED_OUT() macro in FreeRTOS.
	 Are enabled in FreeRTOSConfig.h
	 **********************************************************************/
	void switch_out(uint8_t task_no) {
		PORTB &= 0b11000011;
	}
#endif


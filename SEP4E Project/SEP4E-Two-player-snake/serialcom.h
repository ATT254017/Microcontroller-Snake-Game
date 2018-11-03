/*
 * serialcom.h
 *
 * Created: 26-05-2018 15:05:49
 *  Author: amavin
 */ 


#ifndef SERIALCOM_H_
#define SERIALCOM_H_
#include <FreeRTOS.h>
#include <queue.h>

#define SERIAL_COM_TASK_PERIOD 50

void serial_com_task(void *pvParameters);
bool send_bytes(uint8_t *byte_buffer, uint8_t buffer_size);
#endif /* SERIALCOM_H_ */
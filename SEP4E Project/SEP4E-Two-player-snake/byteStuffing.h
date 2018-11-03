#pragma once
#include <stdint.h>
#define LINK_FLAG 0xff
#define LINK_ESC 0
uint8_t byte_stuff(uint8_t *data_buff, uint8_t data_buff_size, uint8_t *frame_buff,	uint8_t	frame_buff_size);
uint8_t byte_unstuff(uint8_t *dst_buffer, uint8_t dst_buffer_size, uint8_t *link_data_buffer, uint8_t link_data_buffer_size);
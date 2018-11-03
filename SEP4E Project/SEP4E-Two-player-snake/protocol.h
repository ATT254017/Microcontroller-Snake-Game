#pragma once
#include <stdint.h>
#include "byteStuffing.h"
#include <stdbool.h>
#include <FreeRTOS.h>
#include <queue.h>

/*
SIMPLE STOP AND WAIT PROTOCOL:
Send one frame -> wait for ack or timeout and send new or same frame
*/
typedef uint8_t FLAG;
typedef uint8_t TOTAL_LENGTH;
typedef uint8_t CRC;


#define ACK_TIMEOUT 100
#define MAX_CONSECUTIVE_ACK_TIMEOUTS 5


#define LINK_FLAG_SIZE sizeof(uint8_t)

//PROTOCOL HEADER FIELD SIZES (in bytes)
#define FLAGS_SIZE sizeof(FLAG)
#define TOTAL_LENGTH_SIZE sizeof(TOTAL_LENGTH)

#define CRC_SIZE sizeof(CRC)

#define HEADER_SIZE FLAGS_SIZE + TOTAL_LENGTH_SIZE
#define FRAME_OVERHEAD LINK_FLAG_SIZE + HEADER_SIZE + CRC_SIZE
#define MAX_PAYLOAD_SIZE UINT8_MAX - FRAME_OVERHEAD

#define MAX_FRAME_SIZE 100
#define MIN_FRAME_SIZE FRAME_OVERHEAD 

//Flags
#define PLACEHOLDER_FLAG 0b00000001
#define ACK_FLAG 0b00000010

#define CRC8_POLYNOMIAL 0x07

typedef enum
{
	FrameInvalid = 0,
	ACK = ACK_FLAG,
	DATA = PLACEHOLDER_FLAG,
} Frame_type_t;

CRC compute_crc8(uint8_t *data, uint8_t data_size); //returns the CRC8 of message

/*packs data_buffer_size bytes from data_buffer in a frame:
	Computes the header:
		Set length field as HEADER_SIZE + data (post-stuff) + CRC_SIZE
		Add passed seq_num and data_buffer_size (post-stuff) is added
		Ack_num is added as-is.
		Flags is set
	Header is prepended to data_buffer
	CRC is computed of header+data_buffer
	CRC is appended to header+data_buffer

	returns the size of the frame in the frame_buffer
*/
uint8_t get_data_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size, uint8_t *data_buffer, uint8_t data_buffer_size);//, uint16_t *seq_num, uint16_t ack_num)

/*
Destructively unpacks the frame (overwrites the packed frame buffer), if the format is valid and returns the frame type.
*/
Frame_type_t unpack_data_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size, uint8_t *data_length);

uint8_t get_acknowledge_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size); //returns size of frame in frame_buffer
bool is_start_of_frame(uint8_t byte);
bool is_ack_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size);
bool is_CRC_valid(uint8_t *frame_buffer, uint8_t bytes);
uint8_t get_frame_size(uint8_t *byte_buffer, uint8_t byte_buffer_size);
bool get_frame_from_queue(QueueHandle_t queue, uint8_t *buffer, uint8_t buffer_size, uint8_t *frame_size);


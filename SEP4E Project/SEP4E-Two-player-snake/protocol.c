#include "protocol.h"
#include <FreeRTOS.h>
#include <queue.h>

CRC compute_crc8(uint8_t data[], uint8_t data_size)
{
	CRC crc = 0;

	for (uint8_t i = 0; i < data_size; i++)
	{
		crc ^= data[i];
		for (uint8_t j = 0; j < 8; j++)
		{
			uint8_t msb = (crc >> 7) & 1;
			crc <<= 1;
			if (msb) {
				crc ^= CRC8_POLYNOMIAL;
			}
		}
	}
	return crc;
}

Frame_type_t unpack_data_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size, uint8_t *data_length)
{
	if (frame_buffer_size < MIN_FRAME_SIZE)
		return FrameInvalid;
	if (!is_start_of_frame(*frame_buffer))
		return FrameInvalid;

	uint8_t frame_size = get_frame_size(frame_buffer, frame_buffer_size);
	if (!is_CRC_valid(frame_buffer, frame_size))
		return FrameInvalid;

	Frame_type_t type = *(frame_buffer + LINK_FLAG_SIZE);

	uint8_t data_offset = LINK_FLAG_SIZE + FLAGS_SIZE + TOTAL_LENGTH_SIZE;

	*data_length = byte_unstuff(frame_buffer, frame_buffer_size, frame_buffer + data_offset, frame_size - (data_offset + CRC_SIZE));

	return type;
}
uint8_t get_data_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size, uint8_t *data_buffer, uint8_t data_buffer_size)//, uint16_t *seq_num, uint16_t ack_num)
{
	uint8_t const *frame_buffer_start = frame_buffer;
	uint8_t *stuffed_bytes = pvPortMalloc(frame_buffer_size);
	if(!stuffed_bytes)
		return 0;
	uint8_t payload_bytes = 0;
	if ((payload_bytes = byte_stuff(data_buffer, data_buffer_size, stuffed_bytes, data_buffer_size * 2)))
	{
		if ((payload_bytes <= MAX_PAYLOAD_SIZE) && (frame_buffer_size >= (payload_bytes + FRAME_OVERHEAD)))
		{
			*frame_buffer++ = LINK_FLAG;
			*frame_buffer++ = PLACEHOLDER_FLAG;
			*frame_buffer++ = payload_bytes + FRAME_OVERHEAD;
			for (uint8_t i = 0; i < payload_bytes; i++) //maybe I should use memcpy..
				*frame_buffer++ = stuffed_bytes[i];

			vPortFree(stuffed_bytes);

			*frame_buffer = compute_crc8(frame_buffer_start, frame_buffer - frame_buffer_start);
			frame_buffer++;

			return frame_buffer - frame_buffer_start;
		}
	}
	vPortFree(stuffed_bytes);
	return 0;
}
uint8_t get_acknowledge_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size)
{
	uint8_t const *frame_buffer_start = frame_buffer;
	if (frame_buffer_size >= LINK_FLAG_SIZE + HEADER_SIZE + CRC_SIZE)
	{
		*frame_buffer++ = LINK_FLAG;
		*frame_buffer++ = ACK_FLAG;
		*frame_buffer++ = FRAME_OVERHEAD;

		*frame_buffer = compute_crc8(frame_buffer_start, frame_buffer - frame_buffer_start);
		frame_buffer++;
		return frame_buffer - frame_buffer_start;
	}
	return 0;
}
bool is_start_of_frame(uint8_t byte)
{
	return byte == LINK_FLAG;
}
bool is_CRC_valid(uint8_t *frame_buffer, uint8_t bytes)
{
	return compute_crc8(frame_buffer, bytes) == 0;
}
uint8_t get_frame_size(uint8_t *byte_buffer, uint8_t byte_buffer_size)
{
	if (byte_buffer_size < (HEADER_SIZE + LINK_FLAG_SIZE))
		return 0;
	if (*byte_buffer != LINK_FLAG)
		return 0;
	byte_buffer += LINK_FLAG_SIZE;
	byte_buffer += FLAGS_SIZE;

	return *byte_buffer;
}

bool is_ack_frame(uint8_t *frame_buffer, uint8_t frame_buffer_size)
{
	if (frame_buffer_size < (HEADER_SIZE + LINK_FLAG_SIZE))
		return false;

	frame_buffer += LINK_FLAG_SIZE;
	return (*frame_buffer & ACK_FLAG);
}

bool get_frame_from_queue(QueueHandle_t queue, uint8_t *buffer, uint8_t buffer_size, uint8_t *frame_size)
{
	if (buffer_size < MIN_FRAME_SIZE)
	return false;

	while(*frame_size <= buffer_size)
	{
		if(!xQueueReceive(queue, (buffer + *frame_size), 0))
		return false;

		if (((*frame_size) == 0) && (!is_start_of_frame(*buffer)))
		continue; //Seek the start of a new frame

		(*frame_size)++;
		
		if (*frame_size >= (LINK_FLAG_SIZE + HEADER_SIZE))
		{
			//frame is big enough to have the header -> size should be available.
			uint8_t f_size = get_frame_size(buffer, LINK_FLAG_SIZE + HEADER_SIZE);
			if (buffer_size < f_size)
			return false;

			//try to get the rest
			while ((*frame_size < f_size))
			{
				if(!xQueueReceive(queue, (buffer + *frame_size), 0))
				return false;

				(*frame_size)++;
			}

			//Survived the loop! must be at the end of the frame
			return true;
		}
	}
}
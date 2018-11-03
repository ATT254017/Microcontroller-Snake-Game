#include "byteStuffing.h"

uint8_t byte_stuff(uint8_t *data_buff, uint8_t data_buff_size, uint8_t *frame_buff, uint8_t	frame_buff_size)
{
	uint8_t const *frame_buff_start = frame_buff;
	uint8_t const *frame_buff_end = frame_buff + frame_buff_size;
	uint8_t const *data_buff_end = data_buff + data_buff_size;
	while (data_buff < data_buff_end && frame_buff < frame_buff_end)
	{
		switch (*data_buff)
		{
		case LINK_ESC:
		{
			if (frame_buff < (frame_buff_end - 1))
			{
				*frame_buff++ = *data_buff;
				*frame_buff++ = *data_buff;
			}
			else
			{
				//no room
				return 0;
			}
		}
		break;
		case LINK_FLAG:
		{
			if (frame_buff < (frame_buff_end - 1))
			{
				*frame_buff++ = LINK_ESC;
				*frame_buff++ = *data_buff;
			}
			else
			{
				//no room
				return 0;
			}
		}
		break;
		default:
		{
			*frame_buff++ = *data_buff;
		}
		break;
		}
		data_buff++;

	}
	return frame_buff - frame_buff_start;
}
uint8_t byte_unstuff(uint8_t *dst_buffer, uint8_t dst_buffer_size, uint8_t *link_data_buffer, uint8_t link_data_buffer_size)
{
	uint8_t const *dst_buffer_start = dst_buffer;
	uint8_t const *dst_buffer_end = dst_buffer + dst_buffer_size;
	uint8_t const *link_data_buffer_end = link_data_buffer + link_data_buffer_size;
	while (dst_buffer < dst_buffer_end && link_data_buffer < link_data_buffer_end)
	{
		if (*link_data_buffer == LINK_ESC)
		{
			link_data_buffer++;
			if ((*link_data_buffer == LINK_ESC) || (*link_data_buffer == LINK_FLAG))
				*dst_buffer++ = *link_data_buffer;
			else
				return 0;
		}
		else
			*dst_buffer++ = *link_data_buffer;

		link_data_buffer++;
	}
	return dst_buffer - dst_buffer_start;
}
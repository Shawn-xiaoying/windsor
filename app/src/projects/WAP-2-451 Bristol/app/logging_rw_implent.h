#ifndef _LOGGING_RW_IMPLENT_H_
#define _LOGGING_RW_IMPLENT_H_

#include "session_log.h"
#include "implemention_cfg_cmd.h"

#define USE_LED_AMBIENT_LIGHT_ADJ 0

typedef union
{
	struct
	{
		unsigned int minute : 9;
		unsigned int second : 6;
		unsigned int bit_data : 1;
	} bit_data;
	unsigned char buf[2];
} logging_bit_data;

typedef struct
{
	uint8_t pulse_output[SAVE_BUF_MAX_LENGTH];
	uint8_t current_temperature[2][SAVE_BUF_MAX_LENGTH];
	uint16_t write_index;
	uint16_t read_index;
	uint16_t count;

} logging_save_ring_buf_t; // 以固定间隔保存状态数据

extern logging_save_ring_buf_t m_logging_save_ring_buf;
void logging_save_ring_buf_in_queue(Get_Response_Pack_t *data_pack);

#endif

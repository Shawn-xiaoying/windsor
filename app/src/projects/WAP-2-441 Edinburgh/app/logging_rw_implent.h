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
	uint8_t power_status[SAVE_BUF_MAX_LENGTH];
	uint8_t celsius_temp_set[SAVE_BUF_MAX_LENGTH];
	uint8_t temp_scale_switch[SAVE_BUF_MAX_LENGTH];
	uint8_t fahrenheit_temp_set[SAVE_BUF_MAX_LENGTH];
	uint16_t fahrenheit_temp_display[SAVE_BUF_MAX_LENGTH];
	uint16_t celsius_temp_display[SAVE_BUF_MAX_LENGTH];
	uint8_t ozone_valve_opening_time[SAVE_BUF_MAX_LENGTH];
	uint8_t atuo_turn_on_after_power_on[SAVE_BUF_MAX_LENGTH];
	uint8_t water_pumb_flow_switch[SAVE_BUF_MAX_LENGTH];
	uint16_t water_flow_rate[SAVE_BUF_MAX_LENGTH];
	uint8_t vacuum_pump_water_flow_switch[SAVE_BUF_MAX_LENGTH];
	uint16_t write_index;
	uint16_t read_index;
	uint16_t count;

} logging_save_ring_buf_t; // 以固定间隔保存状态数据

extern logging_save_ring_buf_t m_logging_save_ring_buf;
void logging_save_ring_buf_in_queue(Get_Response_Pack_t *data_pack);

#endif

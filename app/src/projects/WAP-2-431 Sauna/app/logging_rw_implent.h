#ifndef _LOGGING_RW_IMPLENT_H_
#define _LOGGING_RW_IMPLENT_H_

#include "session_log_def.h"
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
	uint8_t heat_status[SAVE_BUF_MAX_LENGTH];
	uint8_t heat_time_set[SAVE_BUF_MAX_LENGTH];
	uint8_t celsius_temp_set[SAVE_BUF_MAX_LENGTH];
	uint8_t fahrenheit_temp_set[SAVE_BUF_MAX_LENGTH];
	uint8_t temp_scale_switch[SAVE_BUF_MAX_LENGTH];
	uint16_t plan_heat_time_set[SAVE_BUF_MAX_LENGTH];
	uint8_t internal_read_light_switch[SAVE_BUF_MAX_LENGTH];
	uint8_t external_read_light_switch[SAVE_BUF_MAX_LENGTH];

#if USE_LED_AMBIENT_LIGHT_ADJ
	uint8_t led_ambient_light_adjustment[SAVE_BUF_MAX_LENGTH][12]; // Light_Color_ID
#endif

	uint8_t led_ambient_light_color_change_switch[SAVE_BUF_MAX_LENGTH];
	uint8_t color_light_switch[SAVE_BUF_MAX_LENGTH];
	uint8_t heat_reservation_switch[SAVE_BUF_MAX_LENGTH];
	uint16_t fahrenheit_temp_display[SAVE_BUF_MAX_LENGTH];
	uint16_t celsius_temp_display[SAVE_BUF_MAX_LENGTH];
	uint8_t heat1_power[SAVE_BUF_MAX_LENGTH];
	uint8_t heat2_power[SAVE_BUF_MAX_LENGTH];
	uint8_t heat3_power[SAVE_BUF_MAX_LENGTH];

	uint16_t write_index;
	uint16_t read_index;
	uint16_t count;

} logging_save_ring_buf_t; // 以固定间隔保存状态数据

extern logging_save_ring_buf_t m_logging_save_ring_buf;
void logging_save_ring_buf_in_queue(Get_Response_Pack_t *data_pack);

#endif

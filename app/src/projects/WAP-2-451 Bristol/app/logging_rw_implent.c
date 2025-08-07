#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "logging_rw_implent.h"
#include "app_cfg_cmd.h"
#include "session.h"

#define NRF_LOG_MODULE_NAME logging_rw_implent_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

logging_save_ring_buf_t m_logging_save_ring_buf = {.write_index = 0, .read_index = 0, .count = 0};

void logging_save_ring_buf_in_queue(Get_Response_Pack_t *data_pack)
{
	if (m_logging_save_ring_buf.count >= SAVE_BUF_MAX_LENGTH)
	{
		// 队列已经满，需要移动读指针
		m_logging_save_ring_buf.read_index = (m_logging_save_ring_buf.read_index + 1) % SAVE_BUF_MAX_LENGTH;
		m_logging_save_ring_buf.count--;
		//		m_first_logging_time += m_record_cycle_time ;
	}

	for (unsigned char i = 0; i < m_logging_need_to_record_feature_num; i++)
	{
		switch (m_logging_feature_id_list[i])
		{

		case pulse_output_ID: // 101 1byte
			m_logging_save_ring_buf.pulse_output[m_logging_save_ring_buf.write_index] = data_pack->pulse_output.attribute;
			break;
		case red_light_bed_device_current_temperature_ID: // 102 2byte
			m_logging_save_ring_buf.current_temperature[0][m_logging_save_ring_buf.write_index] = data_pack->red_light_bed_device_current_temperature.attribute1;
			m_logging_save_ring_buf.current_temperature[1][m_logging_save_ring_buf.write_index] = data_pack->red_light_bed_device_current_temperature.attribute2;
			break;
		default:
			break;
		}
	}

	m_logging_save_ring_buf.write_index = (m_logging_save_ring_buf.write_index + 1) % SAVE_BUF_MAX_LENGTH;
	m_logging_save_ring_buf.count++;
}

uint8_t get_feature_log_data(uint16_t actual_reading_index, uint8_t *out_data_buf)
{
	uint8_t len = 0, i;
	for (i = 0; i < m_logging_need_to_record_feature_num; i++)
	{
		out_data_buf[len++] = m_logging_feature_id_list[i];
		NRF_LOG_INFO("feature_id_list id=%0x", m_logging_feature_id_list[i]);
		switch (m_logging_feature_id_list[i])
		{

		case pulse_output_ID:												  // 101 1byte
			out_data_buf[len++] = Get_Response_Pack.pulse_output.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.pulse_output[actual_reading_index];
			break;
		case red_light_bed_device_current_temperature_ID:												  // 102 2byte
			out_data_buf[len++] = Get_Response_Pack.red_light_bed_device_current_temperature.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.current_temperature[0][actual_reading_index];
			out_data_buf[len++] = m_logging_save_ring_buf.current_temperature[1][actual_reading_index];
			break;

		default:
			break;
		}
	}

	return len;
}

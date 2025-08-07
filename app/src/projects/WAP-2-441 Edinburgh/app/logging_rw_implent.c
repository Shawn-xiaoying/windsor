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

		case Power_Switch_ID: // 101 1byte
			m_logging_save_ring_buf.power_status[m_logging_save_ring_buf.write_index] = data_pack->power_switch.attribute;
			break;
		case Celsius_Temperature_Setting_ID: // 104 1byte
			m_logging_save_ring_buf.celsius_temp_set[m_logging_save_ring_buf.write_index] = data_pack->celsius_temp_setting.attribute;
			break;
		case Temperature_Scale_Switching_ID: // 105  1byte
			m_logging_save_ring_buf.temp_scale_switch[m_logging_save_ring_buf.write_index] = data_pack->temp_scale_switching.attribute;
			break;
		case Fahrenheit_Temperature_Setting_ID: // 111  1byte
			m_logging_save_ring_buf.fahrenheit_temp_set[m_logging_save_ring_buf.write_index] = data_pack->fahrenheit_temperature_setting.attribute;
			break;
		case Real_Display_Of_Fahrenheit_Temp_ID:																										   // 119 2byte
			m_logging_save_ring_buf.fahrenheit_temp_display[m_logging_save_ring_buf.write_index] = data_pack->real_time_display_fahrenheit_temp.attribute; //
			break;
		case Real_Display_Of_Celsius_Temp_ID: // 114 3byte
			m_logging_save_ring_buf.celsius_temp_display[m_logging_save_ring_buf.write_index] = data_pack->real_display_of_celsius_temp.attribute;
			break;
		case Ozone_valve_opening_time_ID:
			m_logging_save_ring_buf.ozone_valve_opening_time[m_logging_save_ring_buf.write_index] = data_pack->ozone_valve_opening_time.attribute;
			break;
		case Auto_Turn_on_After_PowerOn_ID:
			m_logging_save_ring_buf.atuo_turn_on_after_power_on[m_logging_save_ring_buf.write_index] = data_pack->auto_turn_on_after_powerOn.attribute;
			break;
		case Water_Pump_Flow_Switch_ID:
			m_logging_save_ring_buf.water_pumb_flow_switch[m_logging_save_ring_buf.write_index] = data_pack->water_pump_flow_switch.attribute;
			break;
		case Water_Flow_Rate_ID:
			m_logging_save_ring_buf.water_flow_rate[m_logging_save_ring_buf.write_index] = data_pack->water_flow_rate.attribute;
			break;
		case Vacuum_Pump_Water_Flow_Switch_ID:
			m_logging_save_ring_buf.vacuum_pump_water_flow_switch[m_logging_save_ring_buf.write_index] = data_pack->vacuum_pump_water_flow_switch.attribute;
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

		case Power_Switch_ID:												  // 101 1byte
			out_data_buf[len++] = Get_Response_Pack.power_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.power_status[actual_reading_index];

			break;

		case Celsius_Temperature_Setting_ID: // 104 1byte

			out_data_buf[len++] = Get_Response_Pack.celsius_temp_setting.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.celsius_temp_set[actual_reading_index];

			break;
		case Temperature_Scale_Switching_ID: // 105  1byte

			out_data_buf[len++] = Get_Response_Pack.temp_scale_switching.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.temp_scale_switch[actual_reading_index];

			break;

		case Fahrenheit_Temperature_Setting_ID:													// 111  1byte
			out_data_buf[len++] = Get_Response_Pack.fahrenheit_temperature_setting.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.fahrenheit_temp_set[actual_reading_index];

			break;

		case Real_Display_Of_Fahrenheit_Temp_ID:															  // 119 2byte
			out_data_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.packet_size;			  // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.fahrenheit_temp_display[actual_reading_index] >> 8; //??????
			out_data_buf[len++] = m_logging_save_ring_buf.fahrenheit_temp_display[actual_reading_index];

			break;
		case Real_Display_Of_Celsius_Temp_ID:
			out_data_buf[len++] = Get_Response_Pack.real_display_of_celsius_temp.packet_size;
			//	out_data_buf[len++] = Get_Response_Pack.real_display_of_celsius_temp.attribute1;
			out_data_buf[len++] = m_logging_save_ring_buf.celsius_temp_display[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.celsius_temp_display[actual_reading_index];
			break;

		case Ozone_valve_opening_time_ID:
			out_data_buf[len++] = Get_Response_Pack.ozone_valve_opening_time.packet_size;
			out_data_buf[len++] = m_logging_save_ring_buf.ozone_valve_opening_time[actual_reading_index];
			break;
		case Auto_Turn_on_After_PowerOn_ID:
			out_data_buf[len++] = Get_Response_Pack.auto_turn_on_after_powerOn.packet_size;
			out_data_buf[len++] = m_logging_save_ring_buf.atuo_turn_on_after_power_on[actual_reading_index];
			break;
		case Water_Pump_Flow_Switch_ID:
			out_data_buf[len++] = Get_Response_Pack.water_pump_flow_switch.packet_size;
			out_data_buf[len++] = m_logging_save_ring_buf.water_pumb_flow_switch[actual_reading_index];
			break;
		case Water_Flow_Rate_ID:
			out_data_buf[len++] = Get_Response_Pack.water_flow_rate.packet_size;
			out_data_buf[len++] = m_logging_save_ring_buf.water_flow_rate[actual_reading_index] >> 8; //??????
			out_data_buf[len++] = m_logging_save_ring_buf.water_flow_rate[actual_reading_index];

			break;
		case Vacuum_Pump_Water_Flow_Switch_ID:
			out_data_buf[len++] = Get_Response_Pack.vacuum_pump_water_flow_switch.packet_size;
			out_data_buf[len++] = m_logging_save_ring_buf.vacuum_pump_water_flow_switch[actual_reading_index];
			break;

		default:
			break;
		}
	}

	return len;
}

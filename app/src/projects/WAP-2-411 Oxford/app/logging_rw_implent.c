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

		case Air_Compressor_ID:
			m_logging_save_ring_buf.air_Compressor[m_logging_save_ring_buf.write_index] = data_pack->Air_Compressor.attribute;

			break;
		case Oxygen_Concentrator_ID:

			m_logging_save_ring_buf.oxygen_Concent[m_logging_save_ring_buf.write_index] = data_pack->Oxygen_Concent.attribute;

			break;
		case Air_Conditioner_ID:

			m_logging_save_ring_buf.air_Conditi[m_logging_save_ring_buf.write_index] = data_pack->Air_Conditi.attribute;
			break;

		case Remaining_Minutes_Timer_ID:

			m_logging_save_ring_buf.remaining_minutes_timert[m_logging_save_ring_buf.write_index] = data_pack->Remaining_Minutes_Timer.attribute;

			break;
		case Measured_Temper_ID:

			m_logging_save_ring_buf.airConditi_measured_temper[m_logging_save_ring_buf.write_index] = data_pack->AirConditi_MeasuredTemper.attribute;

			break;

		case Humidity_ID:

			m_logging_save_ring_buf.humidi_sensor[m_logging_save_ring_buf.write_index] = data_pack->Humidi_sensor.attribute;

			break;

		case Temprature_ID:

			m_logging_save_ring_buf.airConditi_target_temper[m_logging_save_ring_buf.write_index] = data_pack->AirConditi_Target_Temper.attribute;

			break;
		case CO2_ID:

			m_logging_save_ring_buf.co2_sensor[m_logging_save_ring_buf.write_index] = data_pack->CO2_sensor.attribute;

			break;

		case Noise_ID:

			m_logging_save_ring_buf.noise_sensor[m_logging_save_ring_buf.write_index] = data_pack->noise_sensor.attribute;

			break;

		case TVOC_ID:

			m_logging_save_ring_buf.tvoc_sensor[m_logging_save_ring_buf.write_index] = data_pack->TVOC_sensor.attribute;

			break;

		case PM_ID:

			m_logging_save_ring_buf.pm1_value[m_logging_save_ring_buf.write_index] = data_pack->PM_Sensor.attribute1; // PM1_value
			m_logging_save_ring_buf.pm25_value[m_logging_save_ring_buf.write_index] = data_pack->PM_Sensor.attribute2;
			m_logging_save_ring_buf.pm10_value[m_logging_save_ring_buf.write_index] = data_pack->PM_Sensor.attribute3;

			break;
		case AirPressure_ID:

			m_logging_save_ring_buf.airpress_sensor[m_logging_save_ring_buf.write_index] = data_pack->airpress_sensor.attribute;

			break;

		case O2_ID:

			m_logging_save_ring_buf.o2_sensor[m_logging_save_ring_buf.write_index] = data_pack->O2_sensor.attribute;

			break;

		case O3_ID:

			m_logging_save_ring_buf.o3_sensor[m_logging_save_ring_buf.write_index] = data_pack->O3_sensor.attribute; //

			break;

		case HCHO_ID:

			m_logging_save_ring_buf.hcho_sensor[m_logging_save_ring_buf.write_index] = data_pack->HCHO_sensor.attribute; //

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

		case Connect_Status_ID:													  //  1byte
			out_data_buf[len++] = Get_Response_Pack.connction_status.packet_size; // packet size
			out_data_buf[len++] = Get_Response_Pack.connction_status.attribute;

			break;
		case Air_Compressor_ID:													// 101 1byte
			out_data_buf[len++] = Get_Response_Pack.Air_Compressor.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.air_Compressor[actual_reading_index];

			break;
		case Oxygen_Concentrator_ID: // 102  1byte

			out_data_buf[len++] = Get_Response_Pack.Oxygen_Concent.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.oxygen_Concent[actual_reading_index];

			break;
		case Air_Conditioner_ID: // 103 1byte

			out_data_buf[len++] = Get_Response_Pack.Air_Conditi.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.air_Conditi[actual_reading_index];

			break;

		case Remaining_Minutes_Timer_ID: // 104 1byte

			out_data_buf[len++] = Get_Response_Pack.Remaining_Minutes_Timer.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.remaining_minutes_timert[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.remaining_minutes_timert[actual_reading_index];

			break;
		case Target_Temperature_ID: // 105  1byte

			out_data_buf[len++] = Get_Response_Pack.AirConditi_Target_Temper.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.airConditi_target_temper[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.airConditi_target_temper[actual_reading_index];
			break;

		case Measured_Temper_ID: // 106 2byte

			out_data_buf[len++] = Get_Response_Pack.AirConditi_MeasuredTemper.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.airConditi_measured_temper[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.airConditi_measured_temper[actual_reading_index];

			break;

		case Humidity_ID:													   // 107  1byte
			out_data_buf[len++] = Get_Response_Pack.Humidi_sensor.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.humidi_sensor[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.humidi_sensor[actual_reading_index];
			break;
		case Temprature_ID: // 108  1byte

			out_data_buf[len++] = Get_Response_Pack.temper_sensor.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.temper_sensor[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.temper_sensor[actual_reading_index];
			break;

		case CO2_ID:														// 111  1byte
			out_data_buf[len++] = Get_Response_Pack.CO2_sensor.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.co2_sensor[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.co2_sensor[actual_reading_index];
			break;

		case Noise_ID: // 114 3byte
			out_data_buf[len++] = Get_Response_Pack.noise_sensor.packet_size;
			out_data_buf[len++] = m_logging_save_ring_buf.noise_sensor[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.noise_sensor[actual_reading_index];

			break;

		case TVOC_ID: // 115 1byte

			out_data_buf[len++] = Get_Response_Pack.TVOC_sensor.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.tvoc_sensor[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.tvoc_sensor[actual_reading_index];
			break;
		case PM_ID: // 116  1byte

			out_data_buf[len++] = Get_Response_Pack.PM_Sensor.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.pm1_value[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.pm1_value[actual_reading_index];
			out_data_buf[len++] = m_logging_save_ring_buf.pm25_value[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.pm25_value[actual_reading_index];
			out_data_buf[len++] = m_logging_save_ring_buf.pm10_value[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.pm10_value[actual_reading_index];
			break;

		case AirPressure_ID: // 117  1byte

			out_data_buf[len++] = Get_Response_Pack.airpress_sensor.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.airpress_sensor[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.airpress_sensor[actual_reading_index];
			break;

		case O2_ID:																				// 119 2byte
			out_data_buf[len++] = Get_Response_Pack.O2_sensor.packet_size;						// packet size
			out_data_buf[len++] = m_logging_save_ring_buf.o2_sensor[actual_reading_index] >> 8; //??????
			out_data_buf[len++] = m_logging_save_ring_buf.o2_sensor[actual_reading_index];

			break;
		case O3_ID:																				// 119 2byte
			out_data_buf[len++] = Get_Response_Pack.O3_sensor.packet_size;						// packet size
			out_data_buf[len++] = m_logging_save_ring_buf.o3_sensor[actual_reading_index] >> 8; //??????
			out_data_buf[len++] = m_logging_save_ring_buf.o3_sensor[actual_reading_index];

			break;
		case HCHO_ID:																			  // 119 2byte
			out_data_buf[len++] = Get_Response_Pack.HCHO_sensor.packet_size;					  // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.hcho_sensor[actual_reading_index] >> 8; //??????
			out_data_buf[len++] = m_logging_save_ring_buf.hcho_sensor[actual_reading_index];

			break;

		default:
			break;
		}
	}

	return len;
}

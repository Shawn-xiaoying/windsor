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

// void enqueue(int data) {
//     if (count >= QUEUE_SIZE) {
//         // ????,???????
//         front = (front + 1) % QUEUE_SIZE;
//         count--; // ???????,?????????
//     }
//     queue[rear] = data;
//     rear = (rear + 1) % QUEUE_SIZE;
//     count++;
// }
//
// int dequeue() {
//     if (count == 0) {
//         // ????
//         printf("Queue is empty\n");
//         exit(1);
//     }
//     int data = queue[front];
//     front = (front + 1) % QUEUE_SIZE;
//     count--;
//     return data;
// }
//
// int main() {
//     // ????,?????????????
//     for (int i = 0; i < QUEUE_SIZE * 2; ++i) {
//         enqueue(i);
//     }
//
//     // ??????,???????
//     while (count > 0) {
//         int data = dequeue();
//         printf("Dequeued: %d\n", data);
//     }
//
//     return 0;
// }

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
		case Heat_Switch_ID: // 102  1byte

			m_logging_save_ring_buf.heat_status[m_logging_save_ring_buf.write_index] = data_pack->heat_switch.attribute;

			break;
		case Heat_Time_ID: // 103 1byte

			m_logging_save_ring_buf.heat_time_set[m_logging_save_ring_buf.write_index] = data_pack->heatint_time.attribute;
			break;

		case Celsius_Temperature_Setting_ID: // 104 1byte

			m_logging_save_ring_buf.celsius_temp_set[m_logging_save_ring_buf.write_index] = data_pack->celsius_temp_setting.attribute;

			break;
		case Temperature_Scale_Switching_ID: // 105  1byte

			m_logging_save_ring_buf.temp_scale_switch[m_logging_save_ring_buf.write_index] = data_pack->temp_scale_switching.attribute;

			break;

		case Timer_ON_OFF_ID: // 106 2byte

			m_logging_save_ring_buf.plan_heat_time_set[m_logging_save_ring_buf.write_index] = data_pack->timer_on_off.attribute;

			break;

		case Internal_Reading_Light_Switch_ID: // 107  1byte

			m_logging_save_ring_buf.internal_read_light_switch[m_logging_save_ring_buf.write_index] = data_pack->internal_reading_light_switch.attribute;

			break;
		case External_Reading_Light_Switch_ID: // 108  1byte

			m_logging_save_ring_buf.external_read_light_switch[m_logging_save_ring_buf.write_index] = data_pack->external_reading_light_switch.attribute;

			break;

//				case  Heating_Display_ID:  //109  1byte
//        m_logging_save_ring_buf. = data_pack.heating_display.attribute	;
//
//				break ;
#if USE_LED_AMBIENT_LIGHT_ADJ
		case Light_Color_ID: // 110  12byte

			for (int j = 0; j < 12; j++)
				m_logging_save_ring_buf.led_ambient_light_adjustment[m_logging_save_ring_buf.write_index][j] = data_pack->led_ambient_light_adjustment.attribute[j];

			break;
#endif
		case Temp_Set_ID: // 111  1byte

			m_logging_save_ring_buf.fahrenheit_temp_set[m_logging_save_ring_buf.write_index] = data_pack->fahrenheit_temperature_setting.attribute;

			break;
			//				case  Media_Switch_ID:     // 112  1byte
			//
			//        m_logging_save_ring_buf. = data_pack.multimedia_switch.attribute	;
			//
			//				break ;
			//
			//				case Fan_Switch_ID:    //113  1byte
			//
			//        m_logging_save_ring_buf. = data_pack.fan_switch.attribute	;
			//
			//				break ;

		case Indoor_Temp_ID: // 114 3byte

			m_logging_save_ring_buf.celsius_temp_display[m_logging_save_ring_buf.write_index] = data_pack->indoor_temp.attribute;

			break;

		case Color_Light_Switch_ID: // 115 1byte

			m_logging_save_ring_buf.color_light_switch[m_logging_save_ring_buf.write_index] = data_pack->color_light_switch.attribute;

			break;
		case Color_Change_Switch_ID: // 116  1byte

			m_logging_save_ring_buf.led_ambient_light_color_change_switch[m_logging_save_ring_buf.write_index] = data_pack->led_ambient_light_color_change_switch.attribute;

			break;

		case Heat_Reservation_Switch_ID: // 117  1byte

			m_logging_save_ring_buf.heat_reservation_switch[m_logging_save_ring_buf.write_index] = data_pack->heating_reservation_switch.attribute;

			break;

		case Real_Display_Of_Fahrenheit_Temp_ID: // 119 2byte

			m_logging_save_ring_buf.fahrenheit_temp_display[m_logging_save_ring_buf.write_index] = data_pack->real_time_display_fahrenheit_temp.attribute; //

			break;
		case heat1_power_ID:
			m_logging_save_ring_buf.heat1_power[m_logging_save_ring_buf.write_index] = data_pack->heat1_power.attribute;
			break;

		case heat2_power_ID:
			m_logging_save_ring_buf.heat2_power[m_logging_save_ring_buf.write_index] = data_pack->heat2_power.attribute;
			break;

		case heat3_power_ID:
			m_logging_save_ring_buf.heat3_power[m_logging_save_ring_buf.write_index] = data_pack->heat3_power.attribute;
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

			//				case 	Connect_Status_ID : //  1byte
			//				 buf[len++] = Get_Response_Pack.connction_status.packet_size ; //packet size
			//         buf[len++] = Get_Response_Pack.connction_status.attribute	;
			//				break ;
		case Power_Switch_ID:												  // 101 1byte
			out_data_buf[len++] = Get_Response_Pack.power_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.power_status[actual_reading_index];

			break;
		case Heat_Switch_ID: // 102  1byte

			out_data_buf[len++] = Get_Response_Pack.heat_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.heat_status[actual_reading_index];

			break;
		case Heat_Time_ID: // 103 1byte

			out_data_buf[len++] = Get_Response_Pack.heatint_time.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.heat_time_set[actual_reading_index];

			break;

		case Celsius_Temperature_Setting_ID: // 104 1byte

			out_data_buf[len++] = Get_Response_Pack.celsius_temp_setting.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.celsius_temp_set[actual_reading_index];

			break;
		case Temperature_Scale_Switching_ID: // 105  1byte

			out_data_buf[len++] = Get_Response_Pack.temp_scale_switching.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.temp_scale_switch[actual_reading_index];

			break;

		case Timer_ON_OFF_ID: // 106 2byte

			out_data_buf[len++] = Get_Response_Pack.timer_on_off.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.plan_heat_time_set[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.plan_heat_time_set[actual_reading_index];

			break;

		case Internal_Reading_Light_Switch_ID:												   // 107  1byte
			out_data_buf[len++] = Get_Response_Pack.internal_reading_light_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.internal_read_light_switch[actual_reading_index];

			break;
		case External_Reading_Light_Switch_ID: // 108  1byte

			out_data_buf[len++] = Get_Response_Pack.external_reading_light_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.external_read_light_switch[actual_reading_index];

			break;

//				case  Heating_Display_ID:  //109  1byte
//
//				out_data_buf[len++] = Get_Response_Pack.heating_display.packet_size ; //packet size
//        	    out_data_buf[len++] = Get_Response_Pack.heating_display.attribute	;
//
//				break ;
#if USE_LED_AMBIENT_LIGHT_ADJ
		case Light_Color_ID: // 110  12byte

			out_data_buf[len++] = Get_Response_Pack.led_ambient_light_adjustment.packet_size; // packet size
			for (int j = 0; j < 12; j++)
				out_data_buf[len++] = m_logging_save_ring_buf.led_ambient_light_adjustment[actual_reading_index][j];
			break;
#endif

		case Temp_Set_ID:																		// 111  1byte
			out_data_buf[len++] = Get_Response_Pack.fahrenheit_temperature_setting.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.fahrenheit_temp_set[actual_reading_index];

			break;
			//				case  Media_Switch_ID:     // 112  1byte
			//				out_data_buf[len++] = Get_Response_Pack.multimedia_switch.packet_size ; //packet size
			//        		out_data_buf[len++] = Get_Response_Pack.multimedia_switch.attribute	;
			//				out_data_buf[len++] = index_pos>>8 ;
			//				out_data_buf[len++] = index_pos ;
			//
			//				break ;
			//
			//				case Fan_Switch_ID:    //113  1byte
			//				out_data_buf[len++] = Get_Response_Pack.fan_switch.packet_size ; //packet size
			//        out_data_buf[len++] = Get_Response_Pack.fan_switch.attribute	;
			//				out_data_buf[len++] = index_pos>>8 ;
			//				out_data_buf[len++] = index_pos ;
			//
			//				break ;

		case Indoor_Temp_ID: // 114 3byte
			out_data_buf[len++] = Get_Response_Pack.indoor_temp.packet_size;
			//	out_data_buf[len++] = Get_Response_Pack.indoor_temp.attribute1;
			out_data_buf[len++] = m_logging_save_ring_buf.celsius_temp_display[actual_reading_index] >> 8;
			out_data_buf[len++] = m_logging_save_ring_buf.celsius_temp_display[actual_reading_index];

			break;

		case Color_Light_Switch_ID: // 115 1byte

			out_data_buf[len++] = Get_Response_Pack.color_light_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.color_light_switch[actual_reading_index];

			break;
		case Color_Change_Switch_ID: // 116  1byte

			out_data_buf[len++] = Get_Response_Pack.led_ambient_light_color_change_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.led_ambient_light_color_change_switch[actual_reading_index];

			break;

		case Heat_Reservation_Switch_ID: // 117  1byte

			out_data_buf[len++] = Get_Response_Pack.heating_reservation_switch.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.heat_reservation_switch[actual_reading_index];

			break;

		case Real_Display_Of_Fahrenheit_Temp_ID:															  // 119 2byte
			out_data_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.packet_size;			  // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.fahrenheit_temp_display[actual_reading_index] >> 8; //??????
			out_data_buf[len++] = m_logging_save_ring_buf.fahrenheit_temp_display[actual_reading_index];

			break;
		case heat1_power_ID:
			out_data_buf[len++] = Get_Response_Pack.heat1_power.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.heat1_power[actual_reading_index];
			break;

		case heat2_power_ID:
			out_data_buf[len++] = Get_Response_Pack.heat2_power.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.heat2_power[actual_reading_index];
			break;

		case heat3_power_ID:
			out_data_buf[len++] = Get_Response_Pack.heat3_power.packet_size; // packet size
			out_data_buf[len++] = m_logging_save_ring_buf.heat3_power[actual_reading_index];
			break;

		default:
			break;
		}
	}

	return len;
}

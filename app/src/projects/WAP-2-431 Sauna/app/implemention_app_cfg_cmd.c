/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "app_cfg_cmd.h"
#include "mt_cst_protocol.h"
#include "mt_ble_service.h"
#include "mcu.h"
#define NRF_LOG_MODULE_NAME implemention_app_cfg_cmd
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT

extern void start_status_query(void);
extern void start_dpid_set(void);

const feature_value_length_map_t features_supported_by_the_device[] = {
	{Power_Switch_ID, 1},
	{Heat_Switch_ID, 1},
	{Heat_Time_ID, 1},
	{Celsius_Temperature_Setting_ID, 1},
	{Temperature_Scale_Switching_ID, 1},
	{Timer_ON_OFF_ID, 2},
	{Internal_Reading_Light_Switch_ID, 1},
	{External_Reading_Light_Switch_ID, 1},
	{Heating_Display_ID, 1},
	{Light_Color_ID, 12},
	{Temp_Set_ID, 1},
	{Media_Switch_ID, 1},
	{Fan_Switch_ID, 1},
	{Color_Light_Switch_ID, 1},
	{Color_Change_Switch_ID, 1},
	{Heat_Reservation_Switch_ID, 1},
	{Real_Display_Of_Fahrenheit_Temp_ID, 2},
	{Indoor_Temp_ID, 2},
	{heat1_power_ID, 1},
	{heat2_power_ID, 1},
	{heat3_power_ID, 1}
};

const uint8_t features_length_supported_by_the_device = sizeof(features_supported_by_the_device) / sizeof(feature_value_length_map_t);

const uint8_t log_features_supported_by_the_device[] = {
	{Power_Switch_ID},
	{Heat_Switch_ID},
	//	{Heat_Time_ID},
	//	{Celsius_Temperature_Setting_ID},
	{Temperature_Scale_Switching_ID},
	//	{Timer_ON_OFF_ID},
	{Internal_Reading_Light_Switch_ID},
	{External_Reading_Light_Switch_ID},
	//	{Heating_Display_ID},
	{Light_Color_ID},
	//	{Temp_Set_ID},
	//	{Media_Switch_ID},
	//	{Fan_Switch_ID},
	{Color_Light_Switch_ID},
	{Color_Change_Switch_ID},
	{Heat_Reservation_Switch_ID},
	{Real_Display_Of_Fahrenheit_Temp_ID},
	{Indoor_Temp_ID}};

const uint8_t log_features_length_supported_by_the_device = sizeof(log_features_supported_by_the_device);

Get_Response_Pack_t Last_Get_Response_Pack = {0};

Get_Response_Pack_t Get_Response_Pack = {

	.connction_status.Feature_ID = Connect_Status_ID,
	.connction_status.packet_size = 1,
	.connction_status.attribute = 0,

	.power_switch.Feature_ID = Power_Switch_ID,
	.power_switch.packet_size = 1,

	.heat_switch.Feature_ID = Heat_Switch_ID,
	.heat_switch.packet_size = 1,

	.heatint_time.Feature_ID = Heat_Time_ID,
	.heatint_time.packet_size = 1,

	.celsius_temp_setting.Feature_ID = Celsius_Temperature_Setting_ID,
	.celsius_temp_setting.packet_size = 1,

	.temp_scale_switching.Feature_ID = Temperature_Scale_Switching_ID,
	.temp_scale_switching.packet_size = 1,

	.timer_on_off.Feature_ID = Timer_ON_OFF_ID,
	.timer_on_off.packet_size = 2,

	.internal_reading_light_switch.Feature_ID = Internal_Reading_Light_Switch_ID,
	.internal_reading_light_switch.packet_size = 1,

	.external_reading_light_switch.Feature_ID = External_Reading_Light_Switch_ID,
	.external_reading_light_switch.packet_size = 1,

	.heating_display.Feature_ID = Heating_Display_ID,
	.heating_display.packet_size = 1,

	.led_ambient_light_adjustment.Feature_ID = Light_Color_ID,
	.led_ambient_light_adjustment.packet_size = 12,

	.fahrenheit_temperature_setting.Feature_ID = Temp_Set_ID,
	.fahrenheit_temperature_setting.packet_size = 1,

	.multimedia_switch.Feature_ID = Media_Switch_ID,
	.multimedia_switch.packet_size = 1,

	.fan_switch.Feature_ID = Fan_Switch_ID,
	.fan_switch.packet_size = 1,

	.color_light_switch.Feature_ID = Color_Light_Switch_ID,
	.color_light_switch.packet_size = 1,

	.led_ambient_light_color_change_switch.Feature_ID = Color_Change_Switch_ID,
	.led_ambient_light_color_change_switch.packet_size = 1,

	.heating_reservation_switch.Feature_ID = Heat_Reservation_Switch_ID,
	.heating_reservation_switch.packet_size = 1,

	.real_time_display_fahrenheit_temp.Feature_ID = Real_Display_Of_Fahrenheit_Temp_ID,
	.real_time_display_fahrenheit_temp.packet_size = 2,

	.indoor_temp.Feature_ID = Indoor_Temp_ID,
	.indoor_temp.packet_size = 2,

	.heat1_power.Feature_ID = heat1_power_ID,
	.heat1_power.packet_size = 1,

	.heat2_power.Feature_ID = heat2_power_ID,
	.heat2_power.packet_size = 1,

	.heat3_power.Feature_ID = heat3_power_ID,
	.heat3_power.packet_size = 1,

}; // Get_Response_Pack 数据数组

// 不发命令时控制器主动上传
Feature_Id_List Update_Feature_Id_list = {
	.update_feature_num = 0

};

// 发送命令时的响应feature_id_list
Feature_Id_List Response_Feature_Id_list = {
	.update_feature_num = 0

};
APP_REQUEST_DATA_TYPE App_Request_Data_Type = REQUEST_ALL_FEATURE_ID;

Set_Request_Data_Pack_t SettingData; // 用来保存Set Reuset 的FeatureID数据

Feature_Id_List Set_Feature_Id_list = {
	.update_feature_num = 0

};

Set_Request_Data_Pack_t Session_SettingData; // 用来保存Set Reuset 的FeatureID数据

Feature_Id_List Session_Set_Feature_Id_list = {
	.update_feature_num = 0

};


uint8_t set_all_upload_feature_id(void)
{

	uint8_t index = 0;
	Get_Request_id[index++] = Connect_Status_ID;
	Get_Request_id[index++] = Power_Switch_ID;
	Get_Request_id[index++] = Heat_Switch_ID;
	Get_Request_id[index++] = Heat_Time_ID;
	Get_Request_id[index++] = Celsius_Temperature_Setting_ID;
	Get_Request_id[index++] = Temperature_Scale_Switching_ID;
	Get_Request_id[index++] = Timer_ON_OFF_ID;
	Get_Request_id[index++] = Internal_Reading_Light_Switch_ID;
	Get_Request_id[index++] = External_Reading_Light_Switch_ID;
	Get_Request_id[index++] = Heating_Display_ID;
	Get_Request_id[index++] = Light_Color_ID;
	Get_Request_id[index++] = Temp_Set_ID;
	Get_Request_id[index++] = Media_Switch_ID;
	Get_Request_id[index++] = Fan_Switch_ID;
	Get_Request_id[index++] = Color_Light_Switch_ID;
	Get_Request_id[index++] = Color_Change_Switch_ID;
	Get_Request_id[index++] = Heat_Reservation_Switch_ID;
	Get_Request_id[index++] = Real_Display_Of_Fahrenheit_Temp_ID;
	Get_Request_id[index++] = Indoor_Temp_ID;
	Get_Request_id[index++] = heat1_power_ID;
	Get_Request_id[index++] = heat2_power_ID;
	Get_Request_id[index++] = heat3_power_ID;

	return index;
}

//#define REFRES_UPDATE_FEATURE_ID_LIST(OBJECT)  if(memcmp(&Last_Get_Response_Pack.OBJECT,&Get_Response_Pack.OBJECT,sizeof(Get_Response_Pack.OBJECT)) !=0) \
//																													{  																																																					\
//																													Update_Feature_Id_list.feature_id[Update_Feature_Id_list.update_feature_num++] = Get_Response_Pack.OBJECT.Feature_ID ; \
//																													memcpy(&Last_Get_Response_Pack.OBJECT, &Get_Response_Pack.OBJECT, sizeof(Get_Response_Pack.OBJECT)); \
//																													}

void refresh_change_data_feature_id_list(void)
{
	Update_Feature_Id_list.update_feature_num = 0;

	REFRES_UPDATE_FEATURE_ID_LIST(connction_status)
	REFRES_UPDATE_FEATURE_ID_LIST(power_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(heat_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(heatint_time)
	REFRES_UPDATE_FEATURE_ID_LIST(celsius_temp_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(temp_scale_switching)
	REFRES_UPDATE_FEATURE_ID_LIST(timer_on_off)
	REFRES_UPDATE_FEATURE_ID_LIST(internal_reading_light_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(external_reading_light_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(heating_display)
	REFRES_UPDATE_FEATURE_ID_LIST(led_ambient_light_adjustment)
	REFRES_UPDATE_FEATURE_ID_LIST(fahrenheit_temperature_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(multimedia_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(fan_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(color_light_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(led_ambient_light_color_change_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(heating_reservation_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(real_time_display_fahrenheit_temp)
	REFRES_UPDATE_FEATURE_ID_LIST(indoor_temp)
	REFRES_UPDATE_FEATURE_ID_LIST(heat1_power)
	REFRES_UPDATE_FEATURE_ID_LIST(heat2_power)
	REFRES_UPDATE_FEATURE_ID_LIST(heat3_power)

	bluetooth_all_connection_set_update_feature_num(Update_Feature_Id_list.update_feature_num);
}

/***********************************************************
 * 将feature_id_list 列表中的数据发送给app
 ***********************************/

// static uint8_t buf[sizeof(Get_Response_Pack)];

/*
将Payload Size 设置为 0 作为通讯数据有错误的应答发送给app Get Request  的应答 Get Response
*/

void send_failed_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	uint8_t len = 0;

	/**********************************************/
	bluetooth_connection_status_task->send_buf[len++] = 0x5A;
	bluetooth_connection_status_task->send_buf[len++] = 0x51;
	bluetooth_connection_status_task->send_buf[len++] = bluetooth_connection_status_task->rv_MsgID;

	// device id
	memcpy(bluetooth_connection_status_task->send_buf + len, device_infor.ChipID, 8);
	len += 8;

	// syetem  m_system_clock
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 24;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 16;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 8;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock;

	// Payload Size
	bluetooth_connection_status_task->send_buf[len++] = 0; //
	bluetooth_connection_status_task->send_buf[len++] = 0; // byte 16

	uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
	bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;				//
	bluetooth_connection_status_task->send_buf[len++] = temp_crc;					//

#ifdef IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT
	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
#endif

  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);



}

void send_feature_id_data_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *feature_id_list, uint8_t id_num, unsigned char ack_cmd)
{

	uint8_t len = 0, i;

	/**********************************************/
	bluetooth_connection_status_task->send_buf[len++] = 0x5A;
	//   buf[len++] = 0x51;
	bluetooth_connection_status_task->send_buf[len++] = ack_cmd;
	
	if(ack_cmd != ACTIVE_UPLOAD_CMD)
	bluetooth_connection_status_task->send_buf[len++] = bluetooth_connection_status_task->rv_MsgID;
	else 
	bluetooth_connection_status_task->send_buf[len++] = 1;
	
	// device id
	memcpy(bluetooth_connection_status_task->send_buf + len, device_infor.ChipID, 8);
	len += 8;

	// syetem  m_system_clock
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 24;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 16;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 8;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock;

	// Payload Size
	bluetooth_connection_status_task->send_buf[len++] = 0;	  //
	bluetooth_connection_status_task->send_buf[len++] = 0x3f; // byte 16
	//	NRF_LOG_INFO("id_num=%d",id_num);
	for (i = 0; i < id_num; i++)
	{
		bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
		//  NRF_LOG_INFO("feature_id_list id=%0x",feature_id_list[i]);
		switch (feature_id_list[i])
		{

		case Connect_Status_ID:																					//  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.attribute;
			break;
		case Power_Switch_ID:																				// 101 1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.power_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.power_switch.attribute;

			break;
		case Heat_Switch_ID: // 102  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat_switch.attribute;

			break;
		case Heat_Time_ID: // 103 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heatint_time.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heatint_time.attribute;
			break;

		case Celsius_Temperature_Setting_ID: // 104 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.celsius_temp_setting.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.celsius_temp_setting.attribute;

			break;
		case Temperature_Scale_Switching_ID: // 105  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.temp_scale_switching.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.temp_scale_switching.attribute;

			break;

		case Timer_ON_OFF_ID: // 106 2byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.timer_on_off.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.timer_on_off.attribute >> 8;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.timer_on_off.attribute;

			break;

		case Internal_Reading_Light_Switch_ID:																				 // 107  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.internal_reading_light_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.internal_reading_light_switch.attribute;

			break;
		case External_Reading_Light_Switch_ID: // 108  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.external_reading_light_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.external_reading_light_switch.attribute;

			break;

		case Heating_Display_ID: // 109  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heating_display.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heating_display.attribute;

			break;
		case Light_Color_ID: // 110  12byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.led_ambient_light_adjustment.packet_size; // packet size
			for (int j = 0; j < 12; j++)
				bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.led_ambient_light_adjustment.attribute[j];

			break;
		case Temp_Set_ID:																									  // 111  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fahrenheit_temperature_setting.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fahrenheit_temperature_setting.attribute;

			break;
		case Media_Switch_ID:																					 // 112  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.multimedia_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.multimedia_switch.attribute;

			break;

		case Fan_Switch_ID:																				  // 113  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_switch.attribute;

			break;

		case Indoor_Temp_ID: // 114 3byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.indoor_temp.packet_size;
			//		 buf[len++] = Get_Response_Pack.indoor_temp.attribute1;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.indoor_temp.attribute >> 8;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.indoor_temp.attribute;

			break;

		case Color_Light_Switch_ID: // 115 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.color_light_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.color_light_switch.attribute;

			break;
		case Color_Change_Switch_ID: // 116  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.led_ambient_light_color_change_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.led_ambient_light_color_change_switch.attribute;

			break;

		case Heat_Reservation_Switch_ID: // 117  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heating_reservation_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heating_reservation_switch.attribute;

			break;

		case Real_Display_Of_Fahrenheit_Temp_ID:																					// 119 2byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.packet_size;	// packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.attribute >> 8; // 高字节放低位
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.attribute;
			break;

		case heat1_power_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat1_power.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat1_power.attribute;
			break;

		case heat2_power_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat2_power.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat2_power.attribute;
			break;

		case heat3_power_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat3_power.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.heat3_power.attribute;
			break;

		default:
			break;
		}
	}

	// Payload Size 16 byte
	bluetooth_connection_status_task->send_buf[16] = len - 17; // 总长度-前面字节长度

	uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
	bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;				//
	bluetooth_connection_status_task->send_buf[len++] = temp_crc;					//

#ifdef IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT
	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
#endif

  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);

	

}

void device_Get_Response_ids(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t id_len)
{
	send_feature_id_data_to_app(bluetooth_connection_status_task, Get_Request_id, id_len, GET_RESPONSE);
}

uint8_t analysis_set_request_features_data_packet(uint8_t *id_data, int16_t temp_Payload_Size,Feature_Id_List* Set_Feature_Id_list,Set_Request_Data_Pack_t* SettingData)
{
	uint8_t id_len_offset = 0;

	//	Set_Feature_Id_list.update_feature_num = 0;

	// for(i = 0;i<temp_Payload_Size;i++)/*根据数据，查找id*/
	do
	{
		switch (id_data[id_len_offset])
		{
#ifdef ADV_USER_DEVICE_CODE

		case DEVICE_CODE_FEATURE_ID:
			if (id_data[id_len_offset + 1] != 2)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->device_code = id_data[id_len_offset + 2] << 8 | id_data[id_len_offset + 3];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
															   //	if(fstorge_write_device_code(device_code)==false )
															   //	return SET_ERROR ;
			break;
#endif
		case Connect_Status_ID:
			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->app_connect_state = id_data[id_len_offset + 2];

#ifdef IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT
			NRF_LOG_INFO("SettingData.app_connect_state=%d", SettingData->app_connect_state);
#endif

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case Power_Switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->power_switch = id_data[id_len_offset + 2];

			NRF_LOG_INFO("Power_Switch_ID=%d,value =%d,update_feature_num=%d ", id_data[id_len_offset], SettingData->power_switch, Set_Feature_Id_list->update_feature_num);

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case Heat_Switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->heat_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Heat_Time_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->heatint_time = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Celsius_Temperature_Setting_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			//					if(Get_Response_Pack.temp_scale_switching.attribute == 0)
			//					{
			// The temperature scale is in Celsius
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->celsius_temp_setting = id_data[id_len_offset + 2];

			//					}else
			//					{
			//						//The temperature scale is in Fahrenheit format
			//						Set_Feature_Id_list.feature_id[Set_Feature_Id_list.update_feature_num++] = Temp_Set_ID;
			//						SettingData.celsius_temp_setting = id_data[id_len_offset + 2];
			//						SettingData.fahrenheit_temperature_setting = celsius_to_fahrenheit(SettingData.celsius_temp_setting);
			//						NRF_LOG_INFO("Celsius_Temperature_Setting_ID convert celsius_temp=%d,fahrenheit_temp=%d",SettingData.celsius_temp_setting,SettingData.fahrenheit_temperature_setting);
			//					}

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Temperature_Scale_Switching_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->temp_scale_switching = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Timer_ON_OFF_ID:

			if (id_data[id_len_offset + 1] != 2)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->timer_on_off = id_data[id_len_offset + 2] << 8 | id_data[id_len_offset + 3];

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Internal_Reading_Light_Switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->internal_reading_light_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case External_Reading_Light_Switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->external_reading_light_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Heating_Display_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->heating_display = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Light_Color_ID: // led_ambient_light_adjustment

			if (id_data[id_len_offset + 1] != 12)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			memcpy(SettingData->led_ambient_light_adjustment, &id_data[id_len_offset + 2], 12);

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Temp_Set_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			//					if(Get_Response_Pack.temp_scale_switching.attribute == 1)
			//					{
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->fahrenheit_temperature_setting = id_data[id_len_offset + 2];
			//					NRF_LOG_INFO("temp_set_id no convert celsius_temp=%d,fahrenheit_temp=%d",SettingData.celsius_temp_setting,SettingData.fahrenheit_temperature_setting);
			//
			//					}else
			//					{
			//					 Set_Feature_Id_list.feature_id[Set_Feature_Id_list.update_feature_num++] = Celsius_Temperature_Setting_ID ;
			//					 SettingData.fahrenheit_temperature_setting = id_data[id_len_offset + 2];
			//					 SettingData.celsius_temp_setting  = fahrenheit_to_celsius(SettingData.fahrenheit_temperature_setting);
			//					 NRF_LOG_INFO("temp_set_id convert celsius_temp=%d,fahrenheit_temp=%d",SettingData.celsius_temp_setting,SettingData.fahrenheit_temperature_setting);
			//
			//					}

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Media_Switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->multimedia_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Fan_Switch_ID:
			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->fan_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Color_Light_Switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->color_light_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Color_Change_Switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->led_ambient_light_color_change_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Heat_Reservation_Switch_ID: //=0x20

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->heating_reservation_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case heat1_power_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->heat1_power = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case heat2_power_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->heat2_power = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case heat3_power_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->heat3_power = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		}

	} while (id_len_offset < temp_Payload_Size);

	return id_len_offset;
}

/********************************************************************
 * Set Request Payload Structure (To set specific writtable features)
 * app 设置请求处理函数
返回 0 ，数据包有错误
返回 1， 数据包正确
 ******************************/
// extern uint8_t uart_set_valu[8] ;

uint8_t app_Set_Request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{

	int16_t temp_Payload_Size = 0; // temp_i16 = 0;
	temp_Payload_Size = check_the_data_packet(bluetooth_connection_status_task, p_data, len);
	if (temp_Payload_Size == -1)
		return 0;
	Set_Request_Pack *Set_pack_t = (Set_Request_Pack *)p_data;
	if (analysis_set_request_features_data_packet(Set_pack_t->id_data, temp_Payload_Size,&Set_Feature_Id_list,&SettingData) == 0)
		return 0;
	return SET_OK;
}

/******************************************************
 * device_Info_Response
 * */
void device_Info_Response(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	uint8_t len = 0;
	// uint8_t buf[30];

	uint16_t temp_crc = 0;

	bluetooth_connection_status_task->send_buf[len++] = 0x5A;
	bluetooth_connection_status_task->send_buf[len++] = INFOR_RESPONSE; // 0x21
	bluetooth_connection_status_task->send_buf[len++] = bluetooth_connection_status_task->rv_MsgID;			// MsgId

	// device id
	memcpy(bluetooth_connection_status_task->send_buf + len, device_infor.ChipID, 8);
	len += 8;

	// syetem  m_system_clock
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 24;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 16;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 8;
	bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock;

	// Payload Size
	bluetooth_connection_status_task->send_buf[len++] = 0; //
	bluetooth_connection_status_task->send_buf[len++] = 0x05;

	// Product  ID
	bluetooth_connection_status_task->send_buf[len++] = DEF_PRODUCT_ID; //
	bluetooth_connection_status_task->send_buf[len++] = DEF_HARDWARE_VER;
	bluetooth_connection_status_task->send_buf[len++] = DEF_FIRWARE_VER_H; //
	bluetooth_connection_status_task->send_buf[len++] = DEF_FIRWARE_VER_L; //
	bluetooth_connection_status_task->send_buf[len++] = DEF_FIRWARE_VER_B; //

	temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
	bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;	   //
	bluetooth_connection_status_task->send_buf[len++] = temp_crc;		   //

  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);


}

/*
temp_i8 =0 使用08命令查询所有的状态，并返回08命令的返回的所有的feature id 数据
temp_id !=0 返回APP 请求的feature id数据
*/

void supplier_get_request_cmd_process(bluetooth_connection_status_task_t *bluetooth_connection_status_task, int16_t temp_i16)
{

	if (temp_i16 < 0){
//		send_failed_to_app(bluetooth_connection_status_task);
		 bluetooth_connection_status_task->rv_lock = 0 ;
		return ;
	}

	if (temp_i16 == 0)
		temp_i16 = set_all_upload_feature_id();

	device_Get_Response_ids(bluetooth_connection_status_task, temp_i16);
	 bluetooth_connection_status_task->rv_lock = 0 ;
}

void supplier_set_request_cmd_process(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
	if (app_Set_Request(bluetooth_connection_status_task, p_data, len) == 0) // 报文有错误
	{
		 device_Set_Response(bluetooth_connection_status_task,false);
		 bluetooth_connection_status_task->rv_lock = 0 ;
		NRF_LOG_INFO("set request data packet error");
	//	dpid_setting_status = DPID_SET_ACK_DATA_ERROR;
	}
	else
	{
		NRF_LOG_INFO("call start_dpid_set");
		start_dpid_set();
	}
}

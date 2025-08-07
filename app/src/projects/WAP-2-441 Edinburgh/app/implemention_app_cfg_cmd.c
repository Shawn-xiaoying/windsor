/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include <string.h>
#include "app_cfg_cmd.h"
#include "mt_cst_protocol.h"
#include "mt_ble_service.h"
#include "mcu.h"

#define NRF_LOG_MODULE_NAME implemention_app_cfg_cmd
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();


#define  IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT

extern void start_status_query(void);
extern void start_dpid_set(void);

const feature_value_length_map_t features_supported_by_the_device[] = {
	{Connect_Status_ID, 1},
	{Power_Switch_ID, 1},
	{Celsius_Temperature_Setting_ID, 1},
	{Temperature_Scale_Switching_ID, 1},
	{Fahrenheit_Temperature_Setting_ID, 1},
	{Lock_Screen_ID, 1},
	{Touch_Screen_Button_Sound_ID, 1},
	{Ozone_valve_opening_time_ID, 1},
	{Auto_Turn_on_After_PowerOn_ID, 1},
	{Real_Display_Of_Fahrenheit_Temp_ID, 2},
	{Water_Pump_Flow_Switch_ID, 1},
	{Water_Flow_Rate_ID, 2},
	{Vacuum_Pump_Water_Flow_Switch_ID, 1},
	{Real_Display_Of_Celsius_Temp_ID, 2}

};

const uint8_t features_length_supported_by_the_device = sizeof(features_supported_by_the_device) / sizeof(feature_value_length_map_t);

const uint8_t log_features_supported_by_the_device[] = {
	//	{Connect_Status_ID},
	{Power_Switch_ID},
	//	{Celsius_Temperature_Setting_ID},
	//	{Temperature_Scale_Switching_ID},
	//	{Fahrenheit_Temperature_Setting_ID},
	//	{Lock_Screen_ID},
	//	{Touch_Screen_Button_Sound_ID},
	//	{Ozone_valve_opening_time_ID},
	//	{Auto_Turn_on_After_PowerOn_ID, 1},
	{Real_Display_Of_Fahrenheit_Temp_ID},
	{Water_Pump_Flow_Switch_ID},
	{Water_Flow_Rate_ID},
	{Vacuum_Pump_Water_Flow_Switch_ID},
	{Real_Display_Of_Celsius_Temp_ID}};

const uint8_t log_features_length_supported_by_the_device = sizeof(log_features_supported_by_the_device);

Get_Response_Pack_t Last_Get_Response_Pack = {0};

Get_Response_Pack_t Get_Response_Pack = {

	.connction_status.Feature_ID = Connect_Status_ID,
	.connction_status.packet_size = 1,

	.power_switch.Feature_ID = Power_Switch_ID,
	.power_switch.packet_size = 1,

	.celsius_temp_setting.Feature_ID = Celsius_Temperature_Setting_ID,
	.celsius_temp_setting.packet_size = 1,

	.temp_scale_switching.Feature_ID = Temperature_Scale_Switching_ID,
	.temp_scale_switching.packet_size = 1,

	.fahrenheit_temperature_setting.Feature_ID = Fahrenheit_Temperature_Setting_ID,
	.fahrenheit_temperature_setting.packet_size = 1,

	.real_time_display_fahrenheit_temp.Feature_ID = Real_Display_Of_Fahrenheit_Temp_ID,
	.real_time_display_fahrenheit_temp.packet_size = 2,

	.real_display_of_celsius_temp.Feature_ID = Real_Display_Of_Celsius_Temp_ID,
	.real_display_of_celsius_temp.packet_size = 2,

	.lock_screen.Feature_ID = Lock_Screen_ID,
	.lock_screen.packet_size = 1,

	.water_pump_flow_switch.Feature_ID = Water_Pump_Flow_Switch_ID,
	.water_pump_flow_switch.packet_size = 1,

	.water_flow_rate.Feature_ID = Water_Flow_Rate_ID,
	.water_flow_rate.packet_size = 2,

	.touch_screen_button_sound.Feature_ID = Touch_Screen_Button_Sound_ID,
	.touch_screen_button_sound.packet_size = 1,

	.ozone_valve_opening_time.Feature_ID = Ozone_valve_opening_time_ID,
	.ozone_valve_opening_time.packet_size = 1,

	.auto_turn_on_after_powerOn.Feature_ID = Auto_Turn_on_After_PowerOn_ID,
	.auto_turn_on_after_powerOn.packet_size = 1,

	.vacuum_pump_water_flow_switch.Feature_ID = Vacuum_Pump_Water_Flow_Switch_ID,
	.vacuum_pump_water_flow_switch.packet_size = 1,

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
	Get_Request_id[index++] = Celsius_Temperature_Setting_ID;
	Get_Request_id[index++] = Temperature_Scale_Switching_ID;
	Get_Request_id[index++] = Fahrenheit_Temperature_Setting_ID;
	Get_Request_id[index++] = Lock_Screen_ID;
	Get_Request_id[index++] = Touch_Screen_Button_Sound_ID;
	Get_Request_id[index++] = Ozone_valve_opening_time_ID;
	Get_Request_id[index++] = Auto_Turn_on_After_PowerOn_ID;
	Get_Request_id[index++] = Real_Display_Of_Fahrenheit_Temp_ID;
	Get_Request_id[index++] = Water_Pump_Flow_Switch_ID;
	Get_Request_id[index++] = Water_Flow_Rate_ID;
	Get_Request_id[index++] = Vacuum_Pump_Water_Flow_Switch_ID;
	Get_Request_id[index++] = Real_Display_Of_Celsius_Temp_ID;

	return index;
}

void refresh_change_data_feature_id_list(void)
{
	Update_Feature_Id_list.update_feature_num = 0;

	REFRES_UPDATE_FEATURE_ID_LIST(connction_status)
	REFRES_UPDATE_FEATURE_ID_LIST(power_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(celsius_temp_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(temp_scale_switching)
	REFRES_UPDATE_FEATURE_ID_LIST(fahrenheit_temperature_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(real_time_display_fahrenheit_temp)
	REFRES_UPDATE_FEATURE_ID_LIST(lock_screen)
	REFRES_UPDATE_FEATURE_ID_LIST(water_pump_flow_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(water_flow_rate)
	REFRES_UPDATE_FEATURE_ID_LIST(touch_screen_button_sound)
	REFRES_UPDATE_FEATURE_ID_LIST(ozone_valve_opening_time)
	REFRES_UPDATE_FEATURE_ID_LIST(auto_turn_on_after_powerOn)
	REFRES_UPDATE_FEATURE_ID_LIST(vacuum_pump_water_flow_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(real_display_of_celsius_temp)
	bluetooth_all_connection_set_update_feature_num(Update_Feature_Id_list.update_feature_num);
}

/***********************************************************
 * 将feature_id_list 列表中的数据发送给app
 ***********************************/

//static uint8_t buf[sizeof(Get_Response_Pack)];

/*
将Payload Size 设置为 0 作为通讯数据有错误的应答发送给app Get Request  的应答 Get Response
*/

void send_failed_to_app(bluetooth_connection_status_task_t* bluetooth_connection_status_task)
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
	bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;				 //
	bluetooth_connection_status_task->send_buf[len++] = temp_crc;					 //



#ifdef IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT
	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
#endif
  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void send_feature_id_data_to_app(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t *feature_id_list, uint8_t id_num, unsigned char ack_cmd)
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
	bluetooth_connection_status_task->send_buf[len++] = 0;	   //
	bluetooth_connection_status_task->send_buf[len++] = 0x3f; // byte 16
	//	NRF_LOG_INFO("id_num=%d",id_num);
	for (i = 0; i < id_num; i++)
	{
		bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
		//  NRF_LOG_INFO("feature_id_list id=%0x",feature_id_list[i]);
		switch (feature_id_list[i])
		{

		case Connect_Status_ID:											 //  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.attribute;
			break;
		case Power_Switch_ID:										 // 101 1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.power_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.power_switch.attribute;

			break;

		case Celsius_Temperature_Setting_ID: // 104 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.celsius_temp_setting.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.celsius_temp_setting.attribute;

			break;
		case Temperature_Scale_Switching_ID: // 105  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.temp_scale_switching.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.temp_scale_switching.attribute;

			break;

		case Fahrenheit_Temperature_Setting_ID:										   // 111  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fahrenheit_temperature_setting.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fahrenheit_temperature_setting.attribute;

			break;

		case Real_Display_Of_Celsius_Temp_ID: // 114 3byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_display_of_celsius_temp.packet_size;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_display_of_celsius_temp.attribute >> 8;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_display_of_celsius_temp.attribute;

			break;

		case Real_Display_Of_Fahrenheit_Temp_ID:											 // 119 2byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.packet_size;	 // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.attribute >> 8; // 高字节放低位
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.real_time_display_fahrenheit_temp.attribute;
			break;

		case Lock_Screen_ID:

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.lock_screen.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.lock_screen.attribute;

			break;

		case Touch_Screen_Button_Sound_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.touch_screen_button_sound.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.touch_screen_button_sound.attribute;

			break;

		case Ozone_valve_opening_time_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.ozone_valve_opening_time.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.ozone_valve_opening_time.attribute;

			break;

		case Auto_Turn_on_After_PowerOn_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.auto_turn_on_after_powerOn.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.auto_turn_on_after_powerOn.attribute;

			break;

		case Water_Pump_Flow_Switch_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.water_pump_flow_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.water_pump_flow_switch.attribute;
			break;

		case Water_Flow_Rate_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.water_flow_rate.packet_size;	   // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.water_flow_rate.attribute >> 8; // 高字节放低位
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.water_flow_rate.attribute;
			break;

		case Vacuum_Pump_Water_Flow_Switch_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.vacuum_pump_water_flow_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.vacuum_pump_water_flow_switch.attribute;
			break;

		default:
			break;
		}
	}

	// Payload Size 16 byte
	bluetooth_connection_status_task->send_buf[16] = len - 17; // 总长度-前面字节长度

	uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
	bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;				 //
	bluetooth_connection_status_task->send_buf[len++] = temp_crc;					 //



#ifdef IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT
	//NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
#endif
  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void device_Get_Response_ids(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t id_len)
{
	send_feature_id_data_to_app(bluetooth_connection_status_task, Get_Request_id, id_len, GET_RESPONSE);
}

uint8_t analysis_set_request_features_data_packet(uint8_t *id_data, int16_t temp_Payload_Size,Feature_Id_List* Set_Feature_Id_list,Set_Request_Data_Pack_t* SettingData)
{
	uint8_t id_len_offset = 0;

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
			//						Set_Feature_Id_list.feature_id[Set_Feature_Id_list.update_feature_num++] = Fahrenheit_Temperature_Setting_ID;
			//						SettingData.celsius_temp_setting = id_data[id_len_offset + 2];
			//						SettingData.fahrenheit_temperature_setting = celsius_to_fahrenheit(SettingData.celsius_temp_setting);
			//
			//					}
			//				  NRF_LOG_INFO("celsius_temp=%d,fahrenheit_temp=%d",SettingData.celsius_temp_setting,SettingData.fahrenheit_temperature_setting);
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Fahrenheit_Temperature_Setting_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;

			//					if(Get_Response_Pack.temp_scale_switching.attribute == 1)
			//					{
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->fahrenheit_temperature_setting = id_data[id_len_offset + 2];
			//					}else
			//					{
			//					 Set_Feature_Id_list.feature_id[Set_Feature_Id_list.update_feature_num++] = Celsius_Temperature_Setting_ID ;
			//					 SettingData.fahrenheit_temperature_setting = id_data[id_len_offset + 2];
			//					 SettingData.celsius_temp_setting  = fahrenheit_to_celsius(SettingData.fahrenheit_temperature_setting);
			//
			//
			//					}
			//					 NRF_LOG_INFO("celsius_temp=%d,fahrenheit_temp=%d",SettingData.celsius_temp_setting,SettingData.fahrenheit_temperature_setting);
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case Temperature_Scale_Switching_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->temp_scale_switching = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case Lock_Screen_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->lock_screen = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case Touch_Screen_Button_Sound_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->touch_screen_button_sound = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case Ozone_valve_opening_time_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->qzone_valve_opening_time = id_data[id_len_offset + 2];

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case Auto_Turn_on_After_PowerOn_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->auto_turn_on_after_powerOn = id_data[id_len_offset + 2];
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

uint8_t app_Set_Request(bluetooth_connection_status_task_t* bluetooth_connection_status_task,uint8_t *p_data, uint8_t len)
{

	int16_t temp_Payload_Size = 0; // temp_i16 = 0;
	temp_Payload_Size = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
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
void device_Info_Response(bluetooth_connection_status_task_t* bluetooth_connection_status_task)
{
	uint8_t len = 0;
//	uint8_t buf[30];

	uint16_t temp_crc = 0;

	bluetooth_connection_status_task->send_buf[len++] = 0x5A;
	bluetooth_connection_status_task->send_buf[len++] = INFOR_RESPONSE; // 0x21
	bluetooth_connection_status_task->send_buf[len++] = bluetooth_connection_status_task->rv_MsgID;			 // MsgId

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
	bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;		//
	bluetooth_connection_status_task->send_buf[len++] = temp_crc;			//



#ifdef IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT
	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
#endif
  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

/*
temp_i8 =0 使用08命令查询所有的状态，并返回08命令的返回的所有的feature id 数据
temp_id !=0 返回APP 请求的feature id数据
*/

void supplier_get_request_cmd_process(bluetooth_connection_status_task_t* bluetooth_connection_status_task, int16_t temp_i16)
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

void supplier_set_request_cmd_process(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
	if (app_Set_Request(bluetooth_connection_status_task,p_data, len) == 0) // 报文有错误
	{
		 device_Set_Response(bluetooth_connection_status_task,false);
		 bluetooth_connection_status_task->rv_lock = 0 ;
		//dpid_setting_status = DPID_SET_ACK_DATA_ERROR;
	}
	else
	{
		start_dpid_set();
	}
}

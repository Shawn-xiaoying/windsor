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

//#define IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT

extern void start_status_query(void);
extern void start_dpid_set(void);

const feature_value_length_map_t features_supported_by_the_device[] = {
	{connect_status_ID, 1},
	{pulse_output_ID, 1},
	{pulse_selection_ID, 6},
	{pulse_frequency_setting_ID, 12},
	{pulse_duty_cycle_setting_ID, 6},
	{working_time_setting_ID, 2},
	{music_bluetooth_connection_status_ID, 1},
	{music_control_ID, 1},
	{music_volume_ID, 1},
	{fan_gear_ID, 1},
	{equipment_time_setting_ID, 7},
	{fan_temperature_setting_ID, 2},
	{fan_control_ID, 2},
	{red_light_bed_dev_name_ID, 12},
	{red_light_bed_device_current_temperature_ID, 2},
	{red_light_bed_device_warn_ID, 1}};

const uint8_t features_length_supported_by_the_device = sizeof(features_supported_by_the_device) / sizeof(feature_value_length_map_t);

const uint8_t log_features_supported_by_the_device[] = {
	{pulse_output_ID},
	{red_light_bed_device_current_temperature_ID}};

const uint8_t log_features_length_supported_by_the_device = sizeof(log_features_supported_by_the_device);

Get_Response_Pack_t Last_Get_Response_Pack = {0};

Get_Response_Pack_t Get_Response_Pack = {

	.connction_status.Feature_ID = connect_status_ID,
	.connction_status.packet_size = 1,

	.pulse_output.Feature_ID = pulse_output_ID,
	.pulse_output.packet_size = 1,

	.pulse_selection.Feature_ID = pulse_selection_ID,
	.pulse_selection.packet_size = 6,

	.pulse_frequency_setting.Feature_ID = pulse_frequency_setting_ID,
	.pulse_frequency_setting.packet_size = 12,

	.pulse_duty_cycle_setting.Feature_ID = pulse_duty_cycle_setting_ID,
	.pulse_duty_cycle_setting.packet_size = 6,

	.working_time_setting.Feature_ID = working_time_setting_ID,
	.working_time_setting.packet_size = 2,

	.music_bluetooth_connection_status.Feature_ID = music_bluetooth_connection_status_ID,
	.music_bluetooth_connection_status.packet_size = 1,

	.music_control.Feature_ID = music_control_ID,
	.music_control.packet_size = 1,

	.music_volume.Feature_ID = music_volume_ID,
	.music_volume.packet_size = 1,

	.fan_gear.Feature_ID = fan_gear_ID,
	.fan_gear.packet_size = 1,

	.equipment_time_setting.Feature_ID = equipment_time_setting_ID,
	.equipment_time_setting.packet_size = 7,

	.fan_ctrl_temperature_setting.Feature_ID = fan_temperature_setting_ID,
	.fan_ctrl_temperature_setting.packet_size = 2,

	.fan_ctrl.Feature_ID = fan_control_ID,
	.fan_ctrl.packet_size = 2,

	.red_light_bed_device_name.Feature_ID = red_light_bed_dev_name_ID,
	.red_light_bed_device_name.packet_size = 12,

	.red_light_bed_device_current_temperature.Feature_ID = red_light_bed_device_current_temperature_ID,
	.red_light_bed_device_current_temperature.packet_size = 2,

	.red_light_bed_device_warn.Feature_ID = red_light_bed_device_warn_ID,
	.red_light_bed_device_warn.packet_size = 1,

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

uint8_t
set_all_upload_feature_id(void)
{

	uint8_t index = 0;
	Get_Request_id[index++] = connect_status_ID;
	Get_Request_id[index++] = working_time_setting_ID;
	Get_Request_id[index++] = pulse_output_ID;
	Get_Request_id[index++] = pulse_selection_ID;
	Get_Request_id[index++] = pulse_frequency_setting_ID;
	Get_Request_id[index++] = pulse_duty_cycle_setting_ID;
	Get_Request_id[index++] = music_bluetooth_connection_status_ID;
	Get_Request_id[index++] = music_control_ID;
	Get_Request_id[index++] = music_volume_ID;
	Get_Request_id[index++] = fan_gear_ID;
	Get_Request_id[index++] = equipment_time_setting_ID;
	Get_Request_id[index++] = fan_temperature_setting_ID;
	Get_Request_id[index++] = fan_control_ID;
	Get_Request_id[index++] = red_light_bed_dev_name_ID;
	Get_Request_id[index++] = red_light_bed_device_current_temperature_ID;
	Get_Request_id[index++] = red_light_bed_device_warn_ID;

	return index;
}

void refresh_change_data_feature_id_list(void)
{
	Update_Feature_Id_list.update_feature_num = 0;

	REFRES_UPDATE_FEATURE_ID_LIST(connction_status)
	REFRES_UPDATE_FEATURE_ID_LIST(working_time_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(pulse_output)
	REFRES_UPDATE_FEATURE_ID_LIST(pulse_selection)
	REFRES_UPDATE_FEATURE_ID_LIST(pulse_frequency_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(pulse_duty_cycle_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(music_bluetooth_connection_status)
	REFRES_UPDATE_FEATURE_ID_LIST(music_control)
	REFRES_UPDATE_FEATURE_ID_LIST(music_volume)
	REFRES_UPDATE_FEATURE_ID_LIST(fan_gear)
	//	REFRES_UPDATE_FEATURE_ID_LIST(equipment_time_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(fan_ctrl_temperature_setting)
	REFRES_UPDATE_FEATURE_ID_LIST(fan_ctrl)
	REFRES_UPDATE_FEATURE_ID_LIST(red_light_bed_device_name)
	REFRES_UPDATE_FEATURE_ID_LIST(red_light_bed_device_current_temperature)
	REFRES_UPDATE_FEATURE_ID_LIST(red_light_bed_device_warn)
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
	uint8_t array_index = 0;

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

		case connect_status_ID:											 //  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.attribute;
			break;
		case working_time_setting_ID:										 // 101 1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.working_time_setting.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.working_time_setting.attribute >> 8;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.working_time_setting.attribute;
			break;

		case pulse_output_ID: // 104 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_output.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_output.attribute;

			break;
		case pulse_selection_ID: // 105  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_selection.packet_size; // packet size
			for (array_index = 0; array_index < 6; array_index++)
			{
				bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_selection.attribute[array_index];
			}

			break;

		case pulse_frequency_setting_ID:										// 111  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_frequency_setting.packet_size; // packet size
			for (array_index = 0; array_index < 6; array_index++)
			{
				bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_frequency_setting.attribute[array_index] >> 8;
				bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_frequency_setting.attribute[array_index];
			}

			break;

		case pulse_duty_cycle_setting_ID: // 114 3byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_duty_cycle_setting.packet_size;
			for (array_index = 0; array_index < 6; array_index++)
			{
				bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.pulse_duty_cycle_setting.attribute[array_index];
			}

			break;

		case music_bluetooth_connection_status_ID:										  //
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.music_bluetooth_connection_status.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.music_bluetooth_connection_status.attribute;
			break;

		case music_control_ID: // 120 1byte:

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.music_control.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.music_control.attribute;

			break;

		case music_volume_ID:										 // 121 1byte:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.music_volume.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.music_volume.attribute;

			break;

		case fan_gear_ID:										 // 122 1byte:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_gear.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_gear.attribute;

			break;

		case equipment_time_setting_ID:										   // 123 7byte:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.equipment_time_setting.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.equipment_time_setting.attribute1 >> 8;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.equipment_time_setting.attribute1;
			for (array_index = 0; array_index < 5; array_index++)
				bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.equipment_time_setting.attribute2[array_index];

			break;

		case fan_temperature_setting_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_ctrl_temperature_setting.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_ctrl_temperature_setting.attribute1;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_ctrl_temperature_setting.attribute2;
			break;

		case fan_control_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_ctrl.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_ctrl.attribute1;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fan_ctrl.attribute2;
			break;

		case red_light_bed_dev_name_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.red_light_bed_device_name.packet_size;
			memcpy(&bluetooth_connection_status_task->send_buf[len], Get_Response_Pack.red_light_bed_device_name.attribute, Get_Response_Pack.red_light_bed_device_name.packet_size); // packet size
			len += Get_Response_Pack.red_light_bed_device_name.packet_size;
			break;
		case red_light_bed_device_current_temperature_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.red_light_bed_device_current_temperature.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.red_light_bed_device_current_temperature.attribute1;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.red_light_bed_device_current_temperature.attribute2;
			break;
		case red_light_bed_device_warn_ID:										  // 123 7byte:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.red_light_bed_device_warn.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.red_light_bed_device_warn.attribute;
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
	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
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
//	uint8_t array_index = 0;
	uint16_t temp_int_value;

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

		case connect_status_ID:
			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->app_connect_state = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			break;

		case working_time_setting_ID:
			if (id_data[id_len_offset + 1] != 2)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			temp_int_value = id_data[id_len_offset + 2] << 8 | id_data[id_len_offset + 3];

			SettingData->working_time_setting[0] = temp_int_value / 60;
			SettingData->working_time_setting[1] = temp_int_value % 60;
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case pulse_output_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->pulse_output = id_data[id_len_offset + 2];

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case pulse_selection_ID:

			if (id_data[id_len_offset + 1] != 6)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			memcpy(SettingData->pulse_selection, &id_data[id_len_offset + 2], sizeof(SettingData->pulse_selection));
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case pulse_frequency_setting_ID:

			if (id_data[id_len_offset + 1] != 12)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			memcpy(SettingData->pulse_frequency_setting, &id_data[id_len_offset + 2], sizeof(SettingData->pulse_frequency_setting));
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case pulse_duty_cycle_setting_ID:

			if (id_data[id_len_offset + 1] != 6)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			memcpy(SettingData->pulse_duty_cycle_setting, &id_data[id_len_offset + 2], sizeof(SettingData->pulse_duty_cycle_setting));
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case music_bluetooth_connection_status_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->music_bluetooth_connection_status = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case music_control_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->music_control = id_data[id_len_offset + 2];

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case music_volume_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->music_volume = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			break;

		case fan_gear_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->fan_gear = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case equipment_time_setting_ID:

			if (id_data[id_len_offset + 1] != 7)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			memcpy(SettingData->equipment_time_setting, &id_data[id_len_offset + 2], sizeof(SettingData->equipment_time_setting));
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case fan_temperature_setting_ID:
			if (id_data[id_len_offset + 1] != 2)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			memcpy(&SettingData->fan_ctrl_temperature_setting, &id_data[id_len_offset + 2], sizeof(SettingData->fan_ctrl_temperature_setting)); // 2 byte 数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			id_len_offset += (id_data[id_len_offset + 1] + 2);																				  // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			break;
		case fan_control_ID:
			if (id_data[id_len_offset + 1] != 2)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset]; // 2 byte 数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			memcpy(&SettingData->fan_ctrl, &id_data[id_len_offset + 2], sizeof(SettingData->fan_ctrl));		   // 2 byte 数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			id_len_offset += (id_data[id_len_offset + 1] + 2);												   // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case red_light_bed_dev_name_ID:
			if (id_data[id_len_offset + 1] != 12)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];					 // 2 byte 数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			memcpy(SettingData->red_light_bed_dev_name, &id_data[id_len_offset + 2], sizeof(SettingData->red_light_bed_dev_name)); // 2 byte 数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
			id_len_offset += (id_data[id_len_offset + 1] + 2);																	 // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
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

//#if OPEN_ASE
//	for (uint8_t i = 0; i < len; i += 16)
//	{
//		AES128_ECB_encrypt(&buf[i], AESKey, &m_send_buf[i]);
//	}
//#else
//	memcpy(m_send_buf, buf, len);
//#endif

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

		//dpid_setting_status = DPID_SET_ACK_DATA_ERROR;
		 bluetooth_connection_status_task->rv_lock = 0 ;
		 device_Set_Response(bluetooth_connection_status_task,false);
	}
	else
	{
		start_dpid_set();
	}
}

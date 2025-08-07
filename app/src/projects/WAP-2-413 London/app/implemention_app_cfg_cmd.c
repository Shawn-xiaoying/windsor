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
	{air_conditioning_temp_set_ID, 2},
	{air_conditioning_water_temp_ID, 2},
	{air_conditioning_remote_switch_ID, 1},
	{air_conditioning_waring_code_ID, 1},
	{ac_power_switch_ID, 1},
	{ac_onoff_switch_ID, 1},
	{select_ata_ID, 1},
	{bibs1_air_switch_ID, 1},
	{bibs1_o2_switch_ID, 1},
	{bibs2_air_switch_ID, 1},
	{bibs2_o2_switch_ID, 1},
	{door_power_switch_ID, 1},
	{door_detection_ID, 1},
	{value_chamber_switch_ID, 1},
	{i_sense_a_ID, 2},
	{i_sense_b_ID, 2},
	{top_light_switch_ID, 1},
	{bottom_light_switch_ID, 1},
	{top_light_color_ID, 4},
	{bottom_light_color_ID, 4},
	{fcv_exit_set_value_ID, 2},
	{fcv_entry_set_value_ID, 2},
	{acp1_state_1_ID, 1},
	{acp1_state_2_ID, 1},
	{acp1_remote_cmd_ID, 1},
	{acp1_waring_code_ID, 1},
	{acp1_run_state_ID, 1},
	{acp1_fault_code_ID, 1},
	{acp2_state_1_ID, 1},
	{acp2_state_2_ID, 1},
	{acp2_remote_cmd_ID, 1},
	{acp2_waring_code_ID, 1},
	{acp2_run_state_ID, 1},
	{acp2_fault_code_ID, 1},
	{o2_generator_1_run_state_ID, 1},
	{o2_generator_1_alarm_state_ID, 1},
	{o2_generator_2_run_state_ID, 1},
	{o2_generator_2_alarm_state_ID, 1}
};

const uint8_t features_length_supported_by_the_device = sizeof(features_supported_by_the_device) / sizeof(feature_value_length_map_t);

const uint8_t log_features_supported_by_the_device[] = {
	{air_conditioning_water_temp_ID},
	{door_detection_ID}
};

const uint8_t log_features_length_supported_by_the_device = sizeof(log_features_supported_by_the_device);

Get_Response_Pack_t Last_Get_Response_Pack = {0};

Get_Response_Pack_t Get_Response_Pack = {

	.connction_status.Feature_ID = connect_status_ID,
	.connction_status.packet_size = 1,
	.connction_status.attribute = 0,
	
	.air_conditioning_temp_set.Feature_ID = air_conditioning_temp_set_ID,		 
	.air_conditioning_temp_set.packet_size = 2, 
	.air_conditioning_temp_set.attribute = 0, 
	
	.air_conditioning_water_temp.Feature_ID = air_conditioning_water_temp_ID,		 
	.air_conditioning_water_temp.packet_size = 2, 
	.air_conditioning_water_temp.attribute = 0, 
	
	.air_conditioning_remote_switch.Feature_ID = air_conditioning_remote_switch_ID,		
	.air_conditioning_remote_switch.packet_size = 1,
	.air_conditioning_remote_switch.attribute = 0,
	
	.air_conditioning_waring_code.Feature_ID = air_conditioning_waring_code_ID,		
  .air_conditioning_waring_code.packet_size = 1,
	.air_conditioning_waring_code.attribute = 0,
	
	.ac_power_switch.Feature_ID = ac_power_switch_ID,		
  .ac_power_switch.packet_size = 1,
	.ac_power_switch.attribute = 0,	
	
	.ac_onoff_switch.Feature_ID = ac_onoff_switch_ID,			
	.ac_onoff_switch.packet_size = 1,
	.ac_onoff_switch.attribute = 0,	
	
	.select_ata.Feature_ID = select_ata_ID,	
	.select_ata.packet_size = 1,
	.select_ata.attribute = 0,		
	
	.bibs1_air_switch.Feature_ID = bibs1_air_switch_ID,		
	.bibs1_air_switch.packet_size = 1,	
	.bibs1_air_switch.attribute = 0,				
	
	.bibs1_o2_switch.Feature_ID = bibs1_o2_switch_ID,				
	.bibs1_o2_switch.packet_size = 1,	
	.bibs1_o2_switch.attribute = 0,		
	
	.bibs2_air_switch.Feature_ID = bibs2_air_switch_ID,			
  .bibs2_air_switch.packet_size = 1,		
	.bibs2_air_switch.attribute = 0,				

	.bibs2_o2_switch.Feature_ID = bibs2_o2_switch_ID,				
  .bibs2_o2_switch.packet_size = 1,		
  .bibs2_o2_switch.attribute = 0,				

	.door_power_switch.Feature_ID = door_power_switch_ID,		
  .door_power_switch.packet_size = 1,	
	.door_power_switch.attribute = 0,		

	.door_detection.Feature_ID = door_detection_ID,			
  .door_detection.packet_size = 1,	
  .door_detection.attribute = 0,				  
	
	.value_chamber_switch.Feature_ID = value_chamber_switch_ID,			
  .value_chamber_switch.packet_size = 1,
	.value_chamber_switch.attribute = 0,			

	.i_sense_a.Feature_ID = i_sense_a_ID,		 
	.i_sense_a.packet_size = 2,
	.i_sense_a.attribute = 0,		  
	
	.i_sense_b.Feature_ID = i_sense_b_ID,		 
	.i_sense_b.packet_size = 2, 
	.i_sense_b.attribute = 0,		   
	
	.top_light_switch.Feature_ID = top_light_switch_ID,		 
	.top_light_switch.packet_size = 1,
	.top_light_switch.attribute = 0,		 
	
	.bottom_light_switch.Feature_ID = bottom_light_switch_ID,		 
	.bottom_light_switch.packet_size = 1,
	.bottom_light_switch.attribute = 0,
	
	.top_light_color.Feature_ID = top_light_color_ID,		
	.top_light_color.packet_size = 4,
	.top_light_color.attribute = 0,
	
	.bottom_light_color.Feature_ID = bottom_light_color_ID,		
	.bottom_light_color.packet_size = 4,
	.bottom_light_color.attribute = 0,
	
	.fcv_exit_set_value.Feature_ID = fcv_exit_set_value_ID,		
	.fcv_exit_set_value.packet_size = 2,
	.fcv_exit_set_value.attribute = 0,
	
	.fcv_entry_set_value.Feature_ID = fcv_entry_set_value_ID,		
	.fcv_entry_set_value.packet_size = 2,
	.fcv_entry_set_value.attribute = 0,
	
	
	
	.acp1_state_1.Feature_ID = acp1_state_1_ID,		 
	.acp1_state_1.packet_size = 1,  
	.acp1_state_1.attribute = 0,
	
	.acp1_state_2.Feature_ID = acp1_state_2_ID,		  
	.acp1_state_2.packet_size = 1,  
	.acp1_state_2.attribute = 0,  
	
	.acp1_remote_cmd.Feature_ID = acp1_remote_cmd_ID,		 
	.acp1_remote_cmd.packet_size = 1, 
	.acp1_remote_cmd.attribute = 0,  
	
	.acp1_waring_code.Feature_ID = acp1_waring_code_ID,		 
	.acp1_waring_code.packet_size = 1, 
	.acp1_waring_code.attribute = 0, 
	
	.acp1_run_state.Feature_ID = acp1_run_state_ID,		  
	.acp1_run_state.packet_size = 1,  
	.acp1_run_state.attribute = 0,   
	
	.acp1_fault_code.Feature_ID = acp1_fault_code_ID,		  
	.acp1_fault_code.packet_size = 1, 
	.acp1_fault_code.attribute = 0, 
	
	.acp2_state_1.Feature_ID = acp2_state_1_ID,		  
	.acp2_state_1.packet_size = 1,  
	.acp2_state_1.attribute = 0,  
	
	.acp2_state_2.Feature_ID = acp2_state_2_ID,		 
	.acp2_state_2.packet_size = 1, 
	.acp2_state_2.attribute = 0, 

	.acp2_remote_cmd.Feature_ID = acp2_remote_cmd_ID,		
	.acp2_remote_cmd.packet_size = 1, 
	.acp2_remote_cmd.attribute = 0, 
	
	.acp2_waring_code.Feature_ID = acp2_waring_code_ID,		 
	.acp2_waring_code.packet_size = 1,  
	.acp2_waring_code.attribute = 0,
	
	.acp2_run_state.Feature_ID = acp2_run_state_ID,		  
	.acp2_run_state.packet_size = 1,
	.acp2_run_state.attribute = 0,  
	
	.acp2_fault_code.Feature_ID = acp2_fault_code_ID,		
	.acp2_fault_code.packet_size = 1, 
	.acp2_fault_code.attribute = 0, 
	
	.o2_generator_1_run_state.Feature_ID = o2_generator_1_run_state_ID,		 
	.o2_generator_1_run_state.packet_size = 1,  
	.o2_generator_1_run_state.attribute = 0, 
	
	.o2_generator_1_alarm_state.Feature_ID = o2_generator_1_alarm_state_ID,		 
	.o2_generator_1_alarm_state.packet_size = 1, 
	.o2_generator_1_alarm_state.attribute = 0, 
	
	.o2_generator_2_run_state.Feature_ID = o2_generator_2_run_state_ID,		 
	.o2_generator_2_run_state.packet_size = 1,  
	.o2_generator_2_run_state.attribute = 0,
	
	.o2_generator_2_alarm_state.Feature_ID = o2_generator_2_alarm_state_ID,		 
	.o2_generator_2_alarm_state.packet_size = 1,
	.o2_generator_2_alarm_state.attribute = 0,
	

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
	Get_Request_id[index++] = connect_status_ID;
	Get_Request_id[index++] = air_conditioning_temp_set_ID;
	Get_Request_id[index++] = air_conditioning_water_temp_ID;
	Get_Request_id[index++] = air_conditioning_remote_switch_ID;
	
	Get_Request_id[index++] = ac_power_switch_ID;
	Get_Request_id[index++] = ac_onoff_switch_ID;
	Get_Request_id[index++] = select_ata_ID;
	Get_Request_id[index++] = bibs1_air_switch_ID;
	Get_Request_id[index++] = bibs1_o2_switch_ID;
	Get_Request_id[index++] = bibs2_air_switch_ID;
	Get_Request_id[index++] = bibs2_o2_switch_ID;
	Get_Request_id[index++] = door_power_switch_ID;
	Get_Request_id[index++] = door_detection_ID;
	Get_Request_id[index++] = value_chamber_switch_ID;
//	Get_Request_id[index++] = i_sense_a_ID;
//	Get_Request_id[index++] = i_sense_b_ID;
	Get_Request_id[index++] = top_light_switch_ID;
	Get_Request_id[index++] = bottom_light_switch_ID;
	Get_Request_id[index++] = top_light_color_ID;
	Get_Request_id[index++] = bottom_light_color_ID;
	Get_Request_id[index++] = fcv_exit_set_value_ID;
	Get_Request_id[index++] = fcv_entry_set_value_ID;
	
	Get_Request_id[index++] = acp1_state_1_ID;
	Get_Request_id[index++] = acp1_state_2_ID;
	Get_Request_id[index++] = acp1_remote_cmd_ID;
	Get_Request_id[index++] = acp1_waring_code_ID;
	Get_Request_id[index++] = acp1_run_state_ID;
	Get_Request_id[index++] = acp1_fault_code_ID;
	
	Get_Request_id[index++] = acp2_state_1_ID;
	Get_Request_id[index++] = acp2_state_2_ID;
	Get_Request_id[index++] = acp2_remote_cmd_ID;
	Get_Request_id[index++] = acp2_waring_code_ID;
	Get_Request_id[index++] = acp2_run_state_ID;
	Get_Request_id[index++] = acp2_fault_code_ID;
	
	Get_Request_id[index++] = o2_generator_1_run_state_ID;
	Get_Request_id[index++] = o2_generator_1_alarm_state_ID;
	Get_Request_id[index++] = o2_generator_2_run_state_ID;
	Get_Request_id[index++] = o2_generator_2_alarm_state_ID;

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
	REFRES_UPDATE_FEATURE_ID_LIST(ac_power_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(ac_onoff_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(select_ata)
	REFRES_UPDATE_FEATURE_ID_LIST(bibs1_air_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(bibs1_o2_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(bibs2_air_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(bibs2_o2_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(door_power_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(door_detection)
	REFRES_UPDATE_FEATURE_ID_LIST(value_chamber_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(top_light_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(bottom_light_switch)
	REFRES_UPDATE_FEATURE_ID_LIST(top_light_color)
	REFRES_UPDATE_FEATURE_ID_LIST(bottom_light_color)
	REFRES_UPDATE_FEATURE_ID_LIST(fcv_exit_set_value)
	REFRES_UPDATE_FEATURE_ID_LIST(fcv_entry_set_value)
	REFRES_UPDATE_FEATURE_ID_LIST(air_conditioning_temp_set)
	REFRES_UPDATE_FEATURE_ID_LIST(air_conditioning_water_temp)
	REFRES_UPDATE_FEATURE_ID_LIST(air_conditioning_remote_switch)
	
	REFRES_UPDATE_FEATURE_ID_LIST(acp1_state_1)
	REFRES_UPDATE_FEATURE_ID_LIST(acp1_state_2)
  REFRES_UPDATE_FEATURE_ID_LIST(acp1_remote_cmd)
	REFRES_UPDATE_FEATURE_ID_LIST(acp1_waring_code)
	REFRES_UPDATE_FEATURE_ID_LIST(acp1_run_state)
	REFRES_UPDATE_FEATURE_ID_LIST(acp1_fault_code)
	
	REFRES_UPDATE_FEATURE_ID_LIST(acp2_state_1)
	REFRES_UPDATE_FEATURE_ID_LIST(acp2_state_2)
  REFRES_UPDATE_FEATURE_ID_LIST(acp2_remote_cmd)
	REFRES_UPDATE_FEATURE_ID_LIST(acp2_waring_code)
	REFRES_UPDATE_FEATURE_ID_LIST(acp2_run_state)
	REFRES_UPDATE_FEATURE_ID_LIST(acp2_fault_code)
	
	REFRES_UPDATE_FEATURE_ID_LIST(o2_generator_1_run_state)
	REFRES_UPDATE_FEATURE_ID_LIST(o2_generator_1_alarm_state)
	REFRES_UPDATE_FEATURE_ID_LIST(o2_generator_2_run_state)
	REFRES_UPDATE_FEATURE_ID_LIST(o2_generator_2_alarm_state)
	
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
  unsigned short j =0;

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

		case connect_status_ID:																					//  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.connction_status.attribute;
			break;
		case air_conditioning_temp_set_ID:																				// 101 1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_temp_set.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_temp_set.attribute>>8;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_temp_set.attribute;

			break;
		case air_conditioning_water_temp_ID: // 102  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_water_temp.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_water_temp.attribute>>8;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_water_temp.attribute;
		  NRF_LOG_INFO("air_conditioning_water_temp=%d",Get_Response_Pack.air_conditioning_water_temp.attribute);
			break;
		case air_conditioning_remote_switch_ID: // 103 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_remote_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.air_conditioning_remote_switch.attribute;
			break;

		case ac_power_switch_ID: // 104 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.ac_power_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.ac_power_switch.attribute;
		  NRF_LOG_INFO("ac_power_switch=%d",Get_Response_Pack.ac_power_switch.attribute);

			break;
		case ac_onoff_switch_ID: // 105  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.ac_onoff_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.ac_onoff_switch.attribute;

			break;

		case select_ata_ID: // 106 2byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.select_ata.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.select_ata.attribute ;
	

			break;

		case bibs1_air_switch_ID:																				 // 107  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs1_air_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs1_air_switch.attribute;

			break;
		case bibs1_o2_switch_ID: // 108  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs1_o2_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs1_o2_switch.attribute;

			break;

		case bibs2_air_switch_ID: // 109  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs2_air_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs2_air_switch.attribute;

			break;
		case bibs2_o2_switch_ID: // 110  12byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs2_o2_switch.packet_size; // packet size
		 	bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bibs2_o2_switch.attribute;

			break;
		case door_power_switch_ID:																									  // 111  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.door_power_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.door_power_switch.attribute;

			break;
		case door_detection_ID:																					 // 112  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.door_detection.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.door_detection.attribute;

			break;

		case value_chamber_switch_ID:																				  // 113  1byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.value_chamber_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.value_chamber_switch.attribute;

			break;

		case i_sense_a_ID: // 114 3byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.i_sense_a.packet_size;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.i_sense_a.attribute >> 8;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.i_sense_a.attribute;

			break;

		case i_sense_b_ID: // 115 1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.i_sense_b.packet_size;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.i_sense_b.attribute >> 8;
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.i_sense_b.attribute;

			break;
		case top_light_switch_ID: // 116  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.top_light_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.top_light_switch.attribute;

			break;

		case bottom_light_switch_ID: // 117  1byte

			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bottom_light_switch.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bottom_light_switch.attribute;

			break;

		case top_light_color_ID:																					// 119 2byte
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.top_light_color.packet_size;	// packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.top_light_color.attribute >> 24; // 高字节放低位
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.top_light_color.attribute >>16;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.top_light_color.attribute >>8;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.top_light_color.attribute ;
			break;

		case bottom_light_color_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bottom_light_color.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bottom_light_color.attribute >> 24; // 高字节放低位
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bottom_light_color.attribute >>16;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bottom_light_color.attribute >>8;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.bottom_light_color.attribute ;
			break;

		case fcv_exit_set_value_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fcv_exit_set_value.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fcv_exit_set_value.attribute>>8;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fcv_exit_set_value.attribute;
			break;

		case fcv_entry_set_value_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fcv_entry_set_value.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fcv_entry_set_value.attribute>>8;
		  bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.fcv_entry_set_value.attribute;
			break;

		case acp1_state_1_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_state_1.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_state_1.attribute;
			break;

		case acp1_state_2_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_state_2.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_state_2.attribute;
			break;
		
		case acp1_remote_cmd_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_remote_cmd.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_remote_cmd.attribute;
			break;

		case acp1_waring_code_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_waring_code.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_waring_code.attribute;
			break;
		
		case acp1_run_state_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_run_state.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_run_state.attribute;
			break;

		case acp1_fault_code_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_fault_code.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_fault_code.attribute;
			break;
		
		case acp2_state_1_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_state_1.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp1_state_1.attribute;
			break;

		case acp2_state_2_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_state_2.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_state_2.attribute;
			break;
		
		case acp2_remote_cmd_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_remote_cmd.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_remote_cmd.attribute;
			break;

		case acp2_waring_code_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_waring_code.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_waring_code.attribute;
			break;
		
		case acp2_run_state_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_run_state.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_run_state.attribute;
			break;

		case acp2_fault_code_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_fault_code.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.acp2_fault_code.attribute;
			break;
		
			case o2_generator_1_run_state_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_1_run_state.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_1_run_state.attribute;
			break;

		case o2_generator_1_alarm_state_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_1_alarm_state.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_1_alarm_state.attribute;
			break;
		
		case o2_generator_2_run_state_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_2_run_state.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_2_run_state.attribute;
			break;

		case o2_generator_2_alarm_state_ID:
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_2_alarm_state.packet_size; // packet size
			bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.o2_generator_2_alarm_state.attribute;
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
	//NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	if(len<=80)
    {
			NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
			
		}else {
			for(j=0;j<len/80;j++)
			NRF_LOG_HEXDUMP_INFO(&bluetooth_connection_status_task->send_buf[j*80], 80);
			
			NRF_LOG_HEXDUMP_INFO(&bluetooth_connection_status_task->send_buf[j*80], len%80);
			
		}
	
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
		case connect_status_ID:
			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->app_connect_state = id_data[id_len_offset + 2];

#ifdef IMPLEMENTION_APP_CFG_CMD_DEBUG_OUT
			NRF_LOG_INFO("SettingData.app_connect_state=%d", SettingData->app_connect_state);
#endif

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case air_conditioning_temp_set_ID:

			if (id_data[id_len_offset + 1] != 2)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->air_conditioning_temp_set = id_data[id_len_offset + 2] << 8 | id_data[id_len_offset + 3];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case air_conditioning_remote_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->air_conditioning_remote_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case ac_power_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->ac_power_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case ac_onoff_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->ac_onoff_switch = id_data[id_len_offset + 2];
		  id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case select_ata_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->select_ata = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case bibs1_air_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->bibs1_air_switch = id_data[id_len_offset + 2] ;
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case bibs1_o2_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->bibs1_o2_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case bibs2_air_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;

			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->bibs2_air_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case bibs2_o2_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->bibs2_o2_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case door_power_switch_ID: // led_ambient_light_adjustment

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->door_power_switch = id_data[id_len_offset + 2];

			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case value_chamber_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->value_chamber_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case top_light_switch_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->top_light_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case bottom_light_switch_ID:
			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->bottom_light_switch = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case top_light_color_ID:

			if (id_data[id_len_offset + 1] != 4)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->top_light_color = (id_data[id_len_offset + 2]<<24) | (id_data[id_len_offset + 3]<<16) | (id_data[id_len_offset + 4]<<8) | (id_data[id_len_offset + 5]);
			NRF_LOG_INFO("top_light_color =%x",SettingData->top_light_color);
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case bottom_light_color_ID:

			if (id_data[id_len_offset + 1] != 4)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

			SettingData->bottom_light_color = id_data[id_len_offset + 2]<<24 | id_data[id_len_offset + 3]<<16 | id_data[id_len_offset + 4]<<8 | id_data[id_len_offset + 5];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case fcv_exit_set_value_ID: //=0x20

			if (id_data[id_len_offset + 1] != 2)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->fcv_exit_set_value = id_data[id_len_offset + 2]<<8 | id_data[id_len_offset + 3] ;
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
		case fcv_entry_set_value_ID:

			if (id_data[id_len_offset + 1] != 2)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->fcv_entry_set_value =  id_data[id_len_offset + 2]<<8 | id_data[id_len_offset + 3] ;
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case acp1_remote_cmd_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->acp1_remote_cmd = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;

		case acp2_remote_cmd_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->acp2_remote_cmd = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
			
		case o2_generator_1_run_state_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->o2_generator_1_run_state = id_data[id_len_offset + 2];
			id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)

			break;
			
			
		case o2_generator_2_run_state_ID:

			if (id_data[id_len_offset + 1] != 1)
				return 0;
			Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
			SettingData->o2_generator_2_run_state = id_data[id_len_offset + 2];
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

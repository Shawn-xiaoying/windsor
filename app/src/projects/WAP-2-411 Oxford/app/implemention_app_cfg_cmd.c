/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "wap-1-100-windsor-master-protocol.h"
#include <string.h>
#include "app_cfg_cmd.h"
#include "fstorage.h"
#include "mt_cst_protocol.h"
#include "mt_ble_service.h"
#include "app_set_cmd_management.h"

#include "mt_gpio.h"
#include "HBOT_uart_cmd.h"
#define NRF_LOG_MODULE_NAME send_hbot_app
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

extern uint8_t m_send_buf[300]; /*传输给app数据*/
extern uint8_t m_rev_buf[300];  /*接收app数据*/

const feature_value_length_map_t features_supported_by_the_device[] = {
    {Air_Compressor_ID, 1},
    {Oxygen_Concentrator_ID, 1},
    {Air_Conditioner_ID, 1},
    {Remaining_Minutes_Timer_ID, 2},
    {Target_Temperature_ID, 2},
    {Measured_Temper_ID, 2},
//    {Humidity_ID, 2},
//    {Temprature_ID, 2},
//    {CO2_ID, 2},
//    {Noise_ID, 2},
//    {TVOC_ID, 2},
//    {PM_ID, 6},
//    {AirPressure_ID, 2},
//    {O2_ID, 2},
//    {O3_ID, 2},
//    {HCHO_ID, 2}

};

const uint8_t features_length_supported_by_the_device = sizeof(features_supported_by_the_device) / sizeof(feature_value_length_map_t);

const uint8_t log_features_supported_by_the_device[] = {
    {Air_Compressor_ID},
    {Oxygen_Concentrator_ID},
    {Air_Conditioner_ID},
    //   {Remaining_Minutes_Timer_ID, 2},
    //   {Target_Temperature_ID, 2},
    {Measured_Temper_ID},
//    {Humidity_ID},
//    {Temprature_ID},
//    {CO2_ID},
//    {Noise_ID},
//    {TVOC_ID},
//    {PM_ID},
//    {AirPressure_ID},
//    {O2_ID},
//    {O3_ID},
//    {HCHO_ID}
		};

const uint8_t log_features_length_supported_by_the_device = sizeof(log_features_supported_by_the_device);

// extern  uint8_t Get_Request_id[20];
Get_Response_Pack_t Last_Get_Response_Pack = {0};
		
Get_Response_Pack_t Get_Response_Pack = {
    .Air_Compressor.Feature_ID = Air_Compressor_ID,
    .Air_Compressor.packet_size = 1,

    .Oxygen_Concent.Feature_ID = Oxygen_Concentrator_ID,
    .Oxygen_Concent.packet_size = 1,

    .Air_Conditi.Feature_ID = Air_Conditioner_ID,
    .Air_Conditi.packet_size = 1,

    .Remaining_Minutes_Timer.Feature_ID = Remaining_Minutes_Timer_ID,
    .Remaining_Minutes_Timer.packet_size = 2,

    .AirConditi_Target_Temper.Feature_ID = Target_Temperature_ID,
    .AirConditi_Target_Temper.packet_size = 2,

    .AirConditi_MeasuredTemper.Feature_ID = Measured_Temper_ID,
    .AirConditi_MeasuredTemper.packet_size = 2,

    .Humidi_sensor.Feature_ID = Humidity_ID,
    .Humidi_sensor.packet_size = 2,

    .temper_sensor.Feature_ID = Temprature_ID,
    .temper_sensor.packet_size = 2,

    .CO2_sensor.Feature_ID = CO2_ID,
    .CO2_sensor.packet_size = 2,

    .noise_sensor.Feature_ID = Noise_ID,
    .noise_sensor.packet_size = 2,

    .TVOC_sensor.Feature_ID = TVOC_ID,
    .TVOC_sensor.packet_size = 2,

    .PM_Sensor.Feature_ID = PM_ID,
    .PM_Sensor.packet_size = 6,

    .airpress_sensor.Feature_ID = AirPressure_ID,
    .airpress_sensor.packet_size = 2,

    .O2_sensor.Feature_ID = O2_ID,
    .O2_sensor.packet_size = 2,

    .O3_sensor.Feature_ID = O3_ID,
    .O3_sensor.packet_size = 2,

    .HCHO_sensor.Feature_ID = HCHO_ID,
    .HCHO_sensor.packet_size = 2,

}; // Get_Response_Pack 数据数组
// extern Get_Response_Pack   Get_Response_Pack_t; // Get_Response_Pack 数据数组

// 不发命令时控制器主动上传
Feature_Id_List Update_Feature_Id_list = {
	.update_feature_num = 0

};

uint8_t set_all_upload_feature_id(void)
{

  uint8_t index = 0;
  Get_Request_id[index++] = Air_Compressor_ID;
  Get_Request_id[index++] = Oxygen_Concentrator_ID;
  Get_Request_id[index++] = Air_Conditioner_ID;
  Get_Request_id[index++] = Remaining_Minutes_Timer_ID;
  Get_Request_id[index++] = Target_Temperature_ID;
  Get_Request_id[index++] = Measured_Temper_ID;

  return index;
}

void refresh_change_data_feature_id_list(void)
{
	Update_Feature_Id_list.update_feature_num = 0;

	REFRES_UPDATE_FEATURE_ID_LIST(Air_Compressor)
	REFRES_UPDATE_FEATURE_ID_LIST(Oxygen_Concent)
	REFRES_UPDATE_FEATURE_ID_LIST(Air_Conditi)
	REFRES_UPDATE_FEATURE_ID_LIST(AirConditi_MeasuredTemper)
#if 0
	REFRES_UPDATE_FEATURE_ID_LIST(Humidi_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(temper_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(CO2_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(noise_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(TVOC_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(PM_Sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(airpress_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(O2_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(O3_sensor)
	REFRES_UPDATE_FEATURE_ID_LIST(HCHO_sensor)
#endif 
	
	bluetooth_all_connection_set_update_feature_num(Update_Feature_Id_list.update_feature_num);
}


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


	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);

	bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}



void send_feature_id_data_to_app(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t *feature_id_list, uint8_t id_num, unsigned char ack_cmd)
{
	// uint8_t buf[100];
  uint8_t len = 0, i;

  /**********************************************/
  bluetooth_connection_status_task->send_buf[len++] = 0x5A;
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
  bluetooth_connection_status_task->send_buf[len++] = 0;    //
  bluetooth_connection_status_task->send_buf[len++] = 0x3f; // byte 16

  /*根据features，获取id数据*/
  for (i = 0; i < id_num; i++)
  {
    switch (feature_id_list[i])
    {
    case Air_Compressor_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Air_Compressor.packet_size; // packet size
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Air_Compressor.attribute;
      break;
    case Oxygen_Concentrator_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Oxygen_Concent.packet_size; // packet size
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Oxygen_Concent.attribute;
      break;
    case Air_Conditioner_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Air_Conditi.packet_size; // packet size
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Air_Conditi.attribute;
      break;

    case Remaining_Minutes_Timer_ID:
      bluetooth_connection_status_task->send_buf[len++] =feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Remaining_Minutes_Timer.packet_size; // packet size
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Remaining_Minutes_Timer.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Remaining_Minutes_Timer.attribute;
      break;
    case Target_Temperature_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.AirConditi_Target_Temper.packet_size; // packet size
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.AirConditi_Target_Temper.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.AirConditi_Target_Temper.attribute;
      break;

    case Measured_Temper_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.AirConditi_MeasuredTemper.packet_size; // packet size
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.AirConditi_MeasuredTemper.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.AirConditi_MeasuredTemper.attribute;
      break;

    case Humidity_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Humidi_sensor.packet_size; // packet size
                         //    buf[len++] = Get_Response_Pack.Humidi_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Humidi_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.Humidi_sensor.attribute;
      break;
    case Temprature_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.temper_sensor.packet_size; // packet size
                         //   buf[len++] = Get_Response_Pack.temper_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.temper_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.temper_sensor.attribute;
      break;
    case CO2_ID:
      bluetooth_connection_status_task->send_buf[len++] = 0x32;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.CO2_sensor.packet_size; // packet size
                         // buf[len++] = Get_Response_Pack.CO2_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.CO2_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.CO2_sensor.attribute;
      break;
    case Noise_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.noise_sensor.packet_size; // packet size
                         // buf[len++] = Get_Response_Pack.noise_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.noise_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.noise_sensor.attribute;
      break;
    case TVOC_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.TVOC_sensor.packet_size; // packet size
                         //  buf[len++] = Get_Response_Pack.TVOC_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.TVOC_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.TVOC_sensor.attribute;
      break;
    case PM_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.PM_Sensor.packet_size; // packet size
                         //  buf[len++] = Get_Response_Pack.PM_Sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.PM_Sensor.attribute1 >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.PM_Sensor.attribute1;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.PM_Sensor.attribute2 >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.PM_Sensor.attribute2;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.PM_Sensor.attribute3 >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.PM_Sensor.attribute3;
      break;
    case AirPressure_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.airpress_sensor.packet_size; // packet size
                         //   buf[len++] = Get_Response_Pack.airpress_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.airpress_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.airpress_sensor.attribute;
      break;
    case O2_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.O2_sensor.packet_size; // packet size
                         //   buf[len++] = Get_Response_Pack.O2_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.O2_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.O2_sensor.attribute;
      break;
    case O3_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.O3_sensor.packet_size; // packet size
                         //  buf[len++] = Get_Response_Pack.O3_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.O3_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.O3_sensor.attribute;
      break;
    case HCHO_ID:
      bluetooth_connection_status_task->send_buf[len++] = feature_id_list[i];
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.HCHO_sensor.packet_size; // packet size
                         //   buf[len++] = Get_Response_Pack.HCHO_sensor.is_working;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.HCHO_sensor.attribute >> 8;
      bluetooth_connection_status_task->send_buf[len++] = Get_Response_Pack.HCHO_sensor.attribute;
      break;

    default:
      break;
    }
  }

  // Payload Size 16 byte
  bluetooth_connection_status_task->send_buf[16] = len - 17; // 总长度-前面字节长度

  uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
  bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
  bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //


  NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);

	
}



/******************************************************
 * device_Info_Response
 * */
void device_Info_Response(bluetooth_connection_status_task_t* bluetooth_connection_status_task)
{
  uint8_t len = 0;
  //uint8_t buf[30];

  uint16_t temp_crc = 0;

  bluetooth_connection_status_task->send_buf[len++] = 0x5A;
  bluetooth_connection_status_task->send_buf[len++] = INFOR_RESPONSE; // 0x21
  bluetooth_connection_status_task->send_buf[len++] = bluetooth_connection_status_task->rv_MsgID ;           // MsgId

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
  bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;     //
  bluetooth_connection_status_task->send_buf[len++] = temp_crc;          //



  NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	
  bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);


}

/***********************************************************
 * Get_Response 全部信息
 ***********************************/
void device_Get_Response_ids(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t id_len)
{
 send_feature_id_data_to_app(bluetooth_connection_status_task,Get_Request_id, id_len, GET_RESPONSE);
}

/********************************************************************
 * Set Request Payload Structure (To set specific writtable features)
 * app 设置请求处理函数
 ******************************/
extern uint8_t uart_set_valu[8];

Set_Request_Data_Pack_t SettingData; // 用来保存Set Reuset 的FeatureID数据

Feature_Id_List Set_Feature_Id_list = {
    .update_feature_num = 0

};

Set_Request_Data_Pack_t Session_SettingData; // 用来保存Set Reuset 的FeatureID数据

Feature_Id_List Session_Set_Feature_Id_list = {
	.update_feature_num = 0

};

extern void start_oxygen_ctrl_task(void);
extern void start_air_comp_ctrl_task(void);

uint8_t analysis_set_request_features_data_packet(uint8_t *id_data, int16_t temp_Payload_Size,Feature_Id_List* Set_Feature_Id_list,Set_Request_Data_Pack_t* SettingData)
{
  uint8_t id_len_offset = 0;
  uint16_t temp_i16 = 0;
  uint8_t temp_ui8 = 0;
  do
  {

    switch (id_data[id_len_offset])
    {

#ifdef ADV_USER_DEVICE_CODE

    case DEVICE_CODE_FEATURE_ID:
      if (id_data[id_len_offset + 1] != 2)
        return 0;
      //		device_code =  id_data[id_len_offset + 2]<<8|  id_data[id_len_offset + 3] ;
      Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];
      SettingData->device_code = id_data[id_len_offset + 2] << 8 | id_data[id_len_offset + 3];
      id_len_offset += (id_data[id_len_offset + 1] + 2); // 当前Feature ID 总的数据长度+ 2 (Feature ID占一个字节,长度本身占一个字节)
                                                         //		if(fstorge_write_device_code(device_code)==false )
                                                         //		return SET_ERROR ;
      break;

#endif

    case Air_Compressor_ID:
      id_len_offset += 1; // packet size

      temp_ui8 = id_data[id_len_offset + 1];
      SettingData->Air_Compressor_Pwr_on = temp_ui8;
      // Get_Response_Pack->Air_Compressor.Pwr_on = temp_ui8;
      if (Get_Response_Pack.Air_Compressor.attribute != temp_ui8)
      {
				//start_air_comp_ctrl_task();
				Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset-1];
			}
      //  NRF_LOG_INFO("[app_Set_Request] Air_Compressor.Pwr_on =%d",Get_Response_Pack.Air_Compressor.Pwr_on);
      id_len_offset += 2; // 当前的设置值长度 + 下一个id偏移数据地址长度

      break;

    case Oxygen_Concentrator_ID:
      id_len_offset += 1; // packet size

      //           Get_Response_Pack.Oxygen_Concent.Pwr_on
      temp_ui8 = id_data[id_len_offset + 1];

      //     Get_Response_Pack.Oxygen_Concent.Pwr_on = temp_ui8;
      SettingData->Oxygen_Concent_Pwr_on = temp_ui8;
      if (Get_Response_Pack.Oxygen_Concent.attribute != temp_ui8)
      {  
				//start_oxygen_ctrl_task();
				Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset-1];
			}

      //     NRF_LOG_INFO("[app_Set_Request] Oxygen_Concent.Pwr_on =%d",Get_Response_Pack.Oxygen_Concent.Pwr_on);

      id_len_offset += 2; // 当前的设置值长度 + 下一个id偏移数据地址长度
      break;

    case Air_Conditioner_ID:
      Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

      id_len_offset += 1; // packet size
      Get_Response_Pack.Air_Conditi.attribute = id_data[id_len_offset + 1];
      SettingData->Air_Conditi_Pwr_on = id_data[id_len_offset + 1];
      id_len_offset += 2; ////当前的设置值长度 + 下一个id偏移数据地址长度
      break;

    case Remaining_Minutes_Timer_ID:
      Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

      id_len_offset += 1; // packet size
      temp_i16 = id_data[id_len_offset + 1];
      temp_i16 = (temp_i16 << 8) | id_data[id_len_offset + 2];
      SettingData->Remaining_Minutes_Timer_ID = temp_i16;
      Get_Response_Pack.Remaining_Minutes_Timer.attribute = temp_i16;
      id_len_offset += 3; // 当前的设置值长度 + 下一个id偏移数据地址长度
      break;

    case Target_Temperature_ID:
      Set_Feature_Id_list->feature_id[Set_Feature_Id_list->update_feature_num++] = id_data[id_len_offset];

      id_len_offset += 1; // packet size
      temp_i16 = id_data[id_len_offset + 1];
      temp_i16 = (temp_i16 << 8) | id_data[id_len_offset + 2];

      if (temp_i16 < 10)
        temp_i16 = 10;
      if (temp_i16 > 250)
        temp_i16 = 250;

    //  Get_Response_Pack.AirConditi_Target_Temper.attribute = temp_i16 * 10; //*10 App使用一个小数点，所以会放大10倍发送下来，这边再放大10倍发送发送给空调，总的倍数就是100
      Get_Response_Pack.AirConditi_Target_Temper.attribute = temp_i16;
	  SettingData->AirConditi_Target_Temper = Get_Response_Pack.AirConditi_Target_Temper.attribute*10;
      NRF_LOG_INFO("[app_Set_Request] Air_Compressor.AirConditi_Target_Temper =%d", Get_Response_Pack.AirConditi_Target_Temper.attribute);

      id_len_offset += 3; // 当前的设置值长度 + 下一个id偏移数据地址长度

      break;

    default:
      break;
    }

  } while (id_len_offset < temp_Payload_Size);

  return id_len_offset;
}

uint8_t app_Set_Request(bluetooth_connection_status_task_t* bluetooth_connection_status_task,uint8_t *p_data, uint8_t len)
{
  int16_t temp_Payload_Size = 0; // temp_i16 = 0;
  temp_Payload_Size = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
  if (temp_Payload_Size == -1)
    return 0;
  Set_Request_Pack_t *Set_pack = (Set_Request_Pack_t *)p_data;
  if (analysis_set_request_features_data_packet(Set_pack->id_data, temp_Payload_Size,&Set_Feature_Id_list,&SettingData) == 0)
    return 0;
  return SET_OK;
}

void supplier_get_request_cmd_process(bluetooth_connection_status_task_t* bluetooth_connection_status_task, int16_t temp_i16)
{
	if (temp_i16 < 0){
	//send_failed_to_app(bluetooth_connection_status_task);
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

  if (app_Set_Request(bluetooth_connection_status_task,p_data, len) == SET_OK) /*后面2 个字节是crc检验*/
  {
  //  uart_switch_cmd_state(SET_AIR_CONDITI_STATE); // 切换到uart send cmd---由于空调需要
		start_dpid_set();
    return;
  }
  device_Set_Response(bluetooth_connection_status_task, false);
	bluetooth_connection_status_task->rv_lock = 0 ;
}

#ifndef IMPLEMENTION_CFG_CMD_H
#define IMPLEMENTION_CFG_CMD_H

#include "stdint.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "bluetooth_connection_management.h"
#define DEF_PRODUCT_ID 0x03	  //  Unknown = 0,HBOT = 1,Sauna = 2,RedLight = 3,ColdPlunge = 4
#define DEF_HARDWARE_VER 0x02 /* 默认蓝牙硬件信息 */

#define DEF_FIRWARE_VER_H 0X04 // Firmware Version
#define DEF_FIRWARE_VER_L 0X01
#define DEF_FIRWARE_VER_B 0X00 // Firmware Build

/*功能特性 id======可读写*/
enum
{
	connect_status_ID = 0x00,
	pulse_output_ID = 0x60,
	pulse_selection_ID = 0x61,
	pulse_frequency_setting_ID = 0x62,
	pulse_duty_cycle_setting_ID = 0x63,
	working_time_setting_ID = 0x64,
	music_bluetooth_connection_status_ID = 0x65,
	music_control_ID = 0x66,
	music_volume_ID = 0x67,
	fan_gear_ID = 0x68,
	equipment_time_setting_ID = 0x69,
	fan_temperature_setting_ID = 0x6a,
	fan_control_ID = 0x6b,
	red_light_bed_dev_name_ID = 0x6c,
	red_light_bed_device_current_temperature_ID = 0x6d,
	red_light_bed_device_warn_ID = 0x6e

};

/*********************************************************
 * @brief app读取Request_Pack 的数据包结构
 */
typedef struct
{
	response_header_packet_t response_header_packet;
	Valu_1Bytes_1Attribute_Pack connction_status;
	Valu_1Bytes_1Attribute_Pack pulse_output;
	Valu_6Bytes_6Attribute_Pack pulse_selection;
	Valu_12Bytes_6Attribute_Pack pulse_frequency_setting;
	Valu_6Bytes_6Attribute_Pack pulse_duty_cycle_setting;
	Valu_2Bytes_1Attribute_Pack working_time_setting;
	Valu_1Bytes_1Attribute_Pack music_bluetooth_connection_status;
	Valu_1Bytes_1Attribute_Pack music_control;
	Valu_1Bytes_1Attribute_Pack music_volume;
	Valu_1Bytes_1Attribute_Pack fan_gear;
	Valu_7Bytes_6Attribute_Pack equipment_time_setting;
	Valu_2Bytes_2Attribute_Pack fan_ctrl_temperature_setting;
	Valu_2Bytes_2Attribute_Pack fan_ctrl;
	Valu_12Bytes_1Attribute_Pack red_light_bed_device_name;
	Valu_2Bytes_2Attribute_Pack red_light_bed_device_current_temperature;
	Valu_1Bytes_1Attribute_Pack red_light_bed_device_warn;

	uint16_t crc_vule;

} __attribute__((packed, aligned(1))) Get_Response_Pack_t;

/*********************************************************
 * @brief Set Request  设置命令类
 */
typedef struct
{
	header_packet_t header_packet;
	uint8_t id_data[sizeof(Get_Response_Pack_t) - 14]; // 按最多feature来开闭空间（包括两个字节CRC)

} __attribute__((packed, aligned(1))) Set_Request_Pack;

typedef struct
{
	unsigned char app_connect_state;

	uint8_t pulse_output;
	uint8_t pulse_selection[6];
	uint8_t pulse_frequency_setting[12];
	uint8_t pulse_duty_cycle_setting[6];
	uint8_t working_time_setting[2];
	uint8_t music_bluetooth_connection_status;
	uint8_t music_control;
	uint8_t music_volume;
	uint8_t fan_gear;
	uint8_t equipment_time_setting[7];
	uint8_t fan_ctrl_temperature_setting[2]; // byte0 : Fan opening temperature，byte1: Fan alarm temperature
	uint8_t fan_ctrl[2];					 // byte0 :Default fan gear ,byte1: End delay time
	uint8_t red_light_bed_dev_name[12];
	unsigned short device_code;

} __attribute__((packed, aligned(1))) Set_Request_Data_Pack_t;

typedef enum
{
	BLUETOOTH_CON_AFTER_SEND_ALL_ID, // 蓝牙连接后，主动使用一次08命令，数据返回后将所有的FEATURE ID 数据上传
	REQUEST_ALL_FEATURE_ID,			 // 使用08命令查询08命令能够返回的所有数据
	REQUEST_APPOINT_FEATURE_ID,		 // 返回指定的FEATURE ID 数据

} APP_REQUEST_DATA_TYPE;

extern Get_Response_Pack_t Get_Response_Pack; // Get_Response_Pack 数据数组
extern Get_Response_Pack_t Last_Get_Response_Pack;
extern Feature_Id_List Update_Feature_Id_list;
extern Feature_Id_List Response_Feature_Id_list;
extern APP_REQUEST_DATA_TYPE App_Request_Data_Type;

extern Set_Request_Data_Pack_t SettingData; // 用来保存Set Reuset 的FeatureID数据

extern Feature_Id_List Set_Feature_Id_list;

extern Set_Request_Data_Pack_t Session_SettingData; // 用来保存Set Reuset 的FeatureID数据
extern Feature_Id_List Session_Set_Feature_Id_list;

void refresh_change_data_feature_id_list(void);
void send_feature_id_data_to_app(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t *feature_id_list, uint8_t id_num, unsigned char ack_cmd);
//uint8_t analysis_set_request_features_data_packet(uint8_t *id_data, int16_t temp_Payload_Size);
uint8_t set_all_upload_feature_id(void);
void send_failed_to_app(bluetooth_connection_status_task_t* bluetooth_connection_status_task);
#endif

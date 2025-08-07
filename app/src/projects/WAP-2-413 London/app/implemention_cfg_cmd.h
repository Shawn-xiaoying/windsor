#ifndef IMPLEMENTION_CFG_CMD_H
#define IMPLEMENTION_CFG_CMD_H

#include "stdint.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "bluetooth_connection_management.h"

#define DEF_PRODUCT_ID 0x02	  //  Unknown = 0,HBOT = 1,Sauna = 2,RedLight = 3,ColdPlunge = 4
#define DEF_HARDWARE_VER 0x01 /* 默认蓝牙硬件信息 */

#define DEF_FIRWARE_VER_H 0X04 // Firmware Version
#define DEF_FIRWARE_VER_L 0X01
#define DEF_FIRWARE_VER_B 0X00 // Firmware Build

#define GET_REQUEST_ACK_CMD 0x51

/*功能特性 id======可读写*/
enum
{
	connect_status_ID = 0x00,
	air_conditioning_temp_set_ID=0x14, //rw
	air_conditioning_water_temp_ID=0x15, //r
	air_conditioning_remote_switch_ID=0x12, //rw
	air_conditioning_waring_code_ID=0x5a, //ro
	ac_power_switch_ID=0x3a,						   // rw
	ac_onoff_switch_ID=0x3b,						   // rw
	select_ata_ID=0x3c,						   // rw
	bibs1_air_switch_ID=0x3d,				   // rw
	bibs1_o2_switch_ID=0x3e,				   // rw
	bibs2_air_switch_ID=0x3f,				   // rw
	bibs2_o2_switch_ID=0x40,				   // rw
	door_power_switch_ID=0x41,				   // rw
	door_detection_ID=0x42,				   // r
	value_chamber_switch_ID=0x43,				   // rw
	i_sense_a_ID=0x44,  //r
	i_sense_b_ID=0x45,  //r
	top_light_switch_ID=0x46, //rw
	bottom_light_switch_ID=0x47, //rw
	top_light_color_ID=0x48,  //rw
	bottom_light_color_ID=0x49, //rw
	fcv_exit_set_value_ID=0x4a, //rw
	fcv_entry_set_value_ID=0x4b, //rw
	acp1_state_1_ID=0x4c, //ro
	acp1_state_2_ID=0x4d,  //ro
	acp1_remote_cmd_ID=0x10, //rw
	acp1_waring_code_ID=0x4e, //ro
	acp1_run_state_ID=0x4f, //ro
	acp1_fault_code_ID=0x50,//ro
	acp2_state_1_ID=0x51,  //ro
	acp2_state_2_ID=0x52,  //ro
	acp2_remote_cmd_ID=0x53, //rw
	acp2_waring_code_ID=0x54, //ro
	acp2_run_state_ID=0x55,  //ro
	acp2_fault_code_ID=0x56, //ro
	
	o2_generator_1_run_state_ID=0x11, //rw
	o2_generator_1_alarm_state_ID=0x57,//ro
	
	o2_generator_2_run_state_ID=0x58, //rw
	o2_generator_2_alarm_state_ID=0x59 //ro
	
	

};



/*********************************************************
 * @brief app读取Request_Pack 的数据包结构
 */
typedef struct
{
	//    uint8_t  Head;
	//    uint8_t  cmd;
	//    uint8_t  Attr_MsgId;
	//    uint8_t  chipID[8];
	//    uint8_t  TimeStamp[4];
	//    uint16_t Payload_Size;
	response_header_packet_t    response_header_packet;
	Valu_1Bytes_1Attribute_Pack connction_status;					   //
	Valu_1Bytes_1Attribute_Pack ac_power_switch;						   // rw
	Valu_1Bytes_1Attribute_Pack ac_onoff_switch;						   // rw
	Valu_1Bytes_1Attribute_Pack select_ata;						   // rw
	Valu_1Bytes_1Attribute_Pack bibs1_air_switch;				   // rw
	Valu_1Bytes_1Attribute_Pack bibs1_o2_switch;				   // rw
	Valu_1Bytes_1Attribute_Pack bibs2_air_switch;				   // rw
	Valu_1Bytes_1Attribute_Pack bibs2_o2_switch;				   // rw
	Valu_1Bytes_1Attribute_Pack door_power_switch;				   // rw
	Valu_1Bytes_1Attribute_Pack door_detection;				   // r
	Valu_1Bytes_1Attribute_Pack value_chamber_switch;				   // rw
	Valu_2Bytes_1Attribute_Pack i_sense_a ;  //r
	Valu_2Bytes_1Attribute_Pack i_sense_b ;  //r
	Valu_1Bytes_1Attribute_Pack top_light_switch ; //rw
	Valu_1Bytes_1Attribute_Pack bottom_light_switch ; //rw
	Valu_4Bytes_1Attribute_Pack top_light_color;  //rw
	Valu_4Bytes_1Attribute_Pack bottom_light_color; //rw
	Valu_2Bytes_1Attribute_Pack fcv_exit_set_value; //rw
	Valu_2Bytes_1Attribute_Pack fcv_entry_set_value; //rw
	Valu_2Bytes_1Attribute_Pack air_conditioning_temp_set ;
	Valu_2Bytes_1Attribute_Pack air_conditioning_water_temp ;
	Valu_1Bytes_1Attribute_Pack air_conditioning_waring_code;
	Valu_1Bytes_1Attribute_Pack air_conditioning_remote_switch;
	Valu_1Bytes_1Attribute_Pack acp1_state_1 ;  //ro
	Valu_1Bytes_1Attribute_Pack acp1_state_2 ;  //ro
	Valu_1Bytes_1Attribute_Pack acp1_remote_cmd ; //rw
	Valu_1Bytes_1Attribute_Pack acp1_waring_code ; //ro
	Valu_1Bytes_1Attribute_Pack acp1_run_state ;  //ro
	Valu_1Bytes_1Attribute_Pack acp1_fault_code ; //ro
	Valu_1Bytes_1Attribute_Pack acp2_state_1 ;  //ro
	Valu_1Bytes_1Attribute_Pack acp2_state_2 ;  //ro
	Valu_1Bytes_1Attribute_Pack acp2_remote_cmd ; //rw
	Valu_1Bytes_1Attribute_Pack acp2_waring_code ; //ro
	Valu_1Bytes_1Attribute_Pack acp2_run_state ;  //ro
	Valu_1Bytes_1Attribute_Pack acp2_fault_code ; //ro
	
	Valu_1Bytes_1Attribute_Pack  o2_generator_1_run_state ; //rw
	Valu_1Bytes_1Attribute_Pack  o2_generator_1_alarm_state ;//ro
	Valu_1Bytes_1Attribute_Pack  o2_generator_1_acp_run_state ;//rw
	Valu_1Bytes_1Attribute_Pack  o2_generator_1_drain_value_state ;//rw
	Valu_2Bytes_1Attribute_Pack  o2_generator_1_pressure; //ro
	Valu_2Bytes_1Attribute_Pack  o2_generator_1_concentration; //r0
	Valu_2Bytes_1Attribute_Pack  o2_generator_1_temperature;  //r0
	
	Valu_1Bytes_1Attribute_Pack  o2_generator_2_run_state ; //rw
	Valu_1Bytes_1Attribute_Pack  o2_generator_2_alarm_state ; //ro
	Valu_1Bytes_1Attribute_Pack  o2_generator_2_acp_run_state ;//rw
	Valu_1Bytes_1Attribute_Pack  o2_generator_2_drain_value_state ;//rw
	Valu_2Bytes_1Attribute_Pack  o2_generator_2_pressure; //ro
	Valu_2Bytes_1Attribute_Pack  o2_generator_2_concentration; //r0
	Valu_2Bytes_1Attribute_Pack  o2_generator_2_temperature;  //r0
	
	
	uint16_t crc_vule;

} __attribute__((packed, aligned(1))) Get_Response_Pack_t;

/*********************************************************
 * @brief Set Request  设置命令类
 */
typedef struct
{
	//    uint8_t  Head;
	//    uint8_t  cmd;
	//    uint8_t  Attr_MsgId;
	//    uint8_t  ShortUserID[5];
	//    uint8_t  TimeStamp[4];
	//    uint16_t Payload_Size;
	header_packet_t header_packet;
	uint8_t id_data[sizeof(Get_Response_Pack_t) - sizeof(response_header_packet_t)]; // 按最多feature来开闭空间（包括两个字节CRC)

} __attribute__((packed, aligned(1))) Set_Request_Pack;


typedef struct
{
	unsigned char  app_connect_state;
	unsigned char  ac_power_switch;						   // rw
	unsigned char  ac_onoff_switch;						   // rw
	unsigned char  select_ata;						   // rw
	unsigned char  bibs1_air_switch;				   // rw
	unsigned char  bibs1_o2_switch;				   // rw
	unsigned char  bibs2_air_switch;				   // rw
	unsigned char  bibs2_o2_switch;				   // rw
	unsigned char  door_power_switch;				   // rw
	unsigned char  value_chamber_switch;				   // rw
	unsigned char  top_light_switch ; //rw
	unsigned char  bottom_light_switch ; //rw
	unsigned int   top_light_color;  //rw
	unsigned int   bottom_light_color; //rw
	unsigned short fcv_exit_set_value; //rw
	unsigned short fcv_entry_set_value; //rw
	unsigned short air_conditioning_temp_set ; //rw
	unsigned char  air_conditioning_remote_switch; //rw
	unsigned char  acp1_remote_cmd ; //rw
	unsigned char  acp2_remote_cmd ; //rw
	unsigned char  o2_generator_1_run_state ; //rw
	unsigned char  o2_generator_2_run_state ; //rw
	unsigned short device_code;
	
} __attribute__((packed, aligned(1))) Set_Request_Data_Pack_t;

/*
主动上传的feature_id 列表, update_feature_num 为要更新的feature_num ;
*/
// typedef struct {
// uint8_t update_feature_num ;
// uint8_t feature_id[MAX_FEATURE_NUM];
//
// }__attribute__((packed, aligned(1))) Feature_Id_List;

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

void   refresh_change_data_feature_id_list(void);

void send_feature_id_data_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *feature_id_list, uint8_t id_num, unsigned char ack_cmd);
//uint8_t analysis_set_request_features_data_packet(uint8_t *id_data, int16_t temp_Payload_Size,Feature_Id_List* Set_Feature_Id_list);
uint8_t set_all_upload_feature_id(void);
void send_failed_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
#endif

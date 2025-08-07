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
	Connect_Status_ID = 0x00,
	Power_Switch_ID = 0x01,
	Heat_Switch_ID,					  // 0x02
	Heat_Time_ID,					  // 0x03
	Celsius_Temperature_Setting_ID,	  // 0x04
	Temperature_Scale_Switching_ID,	  // 0x05
	Timer_ON_OFF_ID,				  // 0x06
	Internal_Reading_Light_Switch_ID, // 0x07
	External_Reading_Light_Switch_ID, // 0x08
	Heating_Display_ID,				  // 0x09
	Light_Color_ID,					  // led_ambient_light_adjustment 0x0A
	Temp_Set_ID,					  // 0X0B
	Media_Switch_ID,				  // 0X0C
	Fan_Switch_ID,					  // 0X0D
	Color_Light_Switch_ID,			  // 0X0E
	Color_Change_Switch_ID,			  // 0X0F
	heat1_power_ID = 0x16,
	heat2_power_ID = 0x17,
	heat3_power_ID = 0x18,
	Heat_Reservation_Switch_ID = 0x20

};

// Indoor_Temp      =  Temprature_ID = 0x31
/*只是可读设备状态 ID*/
enum
{
	// Connect_Status_ID   App 读取该值时，是表示蓝牙板与控制器的连接状态，App 写入该值时，表示的是App与蓝牙板的连接状态，写入1 表示App在线，写入0，表示App将要退出
	// 收到App退出时，就可以关闭蓝牙灯，清除user_connect_state 状态，写入1时，蓝牙板会将与APP的连接状态使用命令GET_BLUETOOTH_STATUS_CMD 发送给桑拿控制器，桑拿控制器会主动上传状态
	// 引入该ID的写入值，是因为苹果手机蓝牙（或者电脑）与篮板板一直处于绑定连接状态时，APP连接上蓝牙板后，蓝牙板是得不到App的连接状态的，退出时也得不到状态

	Real_Display_Of_Fahrenheit_Temp_ID = 0x21,
	Indoor_Temp_ID = 0x31,

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
	response_header_packet_t response_header_packet;
	Valu_1Bytes_1Attribute_Pack connction_status;					   //
	Valu_1Bytes_1Attribute_Pack power_switch;						   // 开关101
	Valu_1Bytes_1Attribute_Pack heat_switch;						   // 加热器开关102
	Valu_1Bytes_1Attribute_Pack heatint_time;						   // 加热时间设置103
	Valu_1Bytes_1Attribute_Pack celsius_temp_setting;				   // 摄氏温度设置104
	Valu_1Bytes_1Attribute_Pack temp_scale_switching;				   // 温标切换105
	Valu_2Bytes_1Attribute_Pack timer_on_off;						   // 定时开关机106
	Valu_1Bytes_1Attribute_Pack internal_reading_light_switch;		   // 内阅读灯开关107
	Valu_1Bytes_1Attribute_Pack external_reading_light_switch;		   // 外阅读灯开关108
	Valu_1Bytes_1Attribute_Pack heating_display;					   // 加热显示109
	Valu_12Bytes_1Attribute_Pack led_ambient_light_adjustment;		   // LED氛围灯调节110
	Valu_1Bytes_1Attribute_Pack fahrenheit_temperature_setting;		   // 华氏温度设置 111
	Valu_1Bytes_1Attribute_Pack multimedia_switch;					   // 多媒体开关 112
	Valu_1Bytes_1Attribute_Pack fan_switch;							   // 风扇开关 113
	Valu_1Bytes_1Attribute_Pack color_light_switch;					   // 彩灯开关 115
	Valu_1Bytes_1Attribute_Pack led_ambient_light_color_change_switch; // LED氛围灯彩色变幻开关 116
	Valu_1Bytes_1Attribute_Pack heating_reservation_switch;			   // 加热预约开关117
	Valu_2Bytes_1Attribute_Pack real_time_display_fahrenheit_temp;	   // 华氏温度实时显示119
	Valu_2Bytes_1Attribute_Pack indoor_temp;						   // 室内摄氏温度实时显示 114
	Valu_1Bytes_1Attribute_Pack heat1_power;						   // 加热功率 120
	Valu_1Bytes_1Attribute_Pack heat2_power;						   // 加热功率 121
	Valu_1Bytes_1Attribute_Pack heat3_power;						   // 加热功率 122

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
	unsigned char app_connect_state;
	unsigned char power_switch;							 // 开关101
	unsigned char heat_switch;							 // 加热器开关102
	unsigned char heatint_time;							 // 加热时间设置103
	unsigned char celsius_temp_setting;					 // 摄氏温度设置104
	unsigned char temp_scale_switching;					 // 温标切换105
	unsigned short timer_on_off;						 // 定时开关机106
	unsigned char internal_reading_light_switch;		 // 内阅读灯开关107
	unsigned char external_reading_light_switch;		 // 外阅读灯开关108
	unsigned char heating_display;						 // 加热显示109
	unsigned char led_ambient_light_adjustment[12];		 // LED氛围灯调节110
	unsigned char fahrenheit_temperature_setting;		 // 华氏温度设置 111
	unsigned char multimedia_switch;					 // 多媒体开关 112
	unsigned char fan_switch;							 // 风扇开关 113
	unsigned char color_light_switch;					 // 彩灯开关 115
	unsigned char led_ambient_light_color_change_switch; // LED氛围灯彩色变幻开关 116
	unsigned char heating_reservation_switch;			 // 加热预约开关117
	unsigned char heat1_power;
	unsigned char heat2_power;
	unsigned char heat3_power;
	unsigned short device_code;
	//	Valu_2Bytes_1Attribute_Pack		real_time_display_fahrenheit_temp ; //华氏温度实时显示119
	// Valu_3Bytes_2Attribute_Pack		indoor_temp; //室内摄氏温度实时显示 114

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

void refresh_change_data_feature_id_list(void);

void send_feature_id_data_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *feature_id_list, uint8_t id_num, unsigned char ack_cmd);
//uint8_t analysis_set_request_features_data_packet(uint8_t *id_data, int16_t temp_Payload_Size);
uint8_t set_all_upload_feature_id(void);
void send_failed_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
#endif

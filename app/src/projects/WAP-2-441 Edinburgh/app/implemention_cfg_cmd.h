#ifndef IMPLEMENTION_CFG_CMD_H
#define IMPLEMENTION_CFG_CMD_H

#include "stdint.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "bluetooth_connection_management.h"
#define DEF_PRODUCT_ID 0x04	  //  Unknown = 0,HBOT = 1,Sauna = 2,RedLight = 3,ColdPlunge = 4
#define DEF_HARDWARE_VER 0x02 /* 默认蓝牙硬件信息 */

#define DEF_FIRWARE_VER_H 0X04 // Firmware Version
#define DEF_FIRWARE_VER_L 0X01
#define DEF_FIRWARE_VER_B 0X00 // Firmware Build

/*功能特性 id======可读写*/
enum
{
	Connect_Status_ID = 0x00,
	Power_Switch_ID = 0x01,
	Celsius_Temperature_Setting_ID = 0x04,	  // 0x04
	Temperature_Scale_Switching_ID = 0x05,	  // 0x05
	Fahrenheit_Temperature_Setting_ID = 0x0B, // 0X0B Fahrenheit temperature set
	Lock_Screen_ID = 0x22,
	Touch_Screen_Button_Sound_ID = 0x25,
	Ozone_valve_opening_time_ID = 0x26,
	Auto_Turn_on_After_PowerOn_ID = 0x27
};

// Indoor_Temp      =  Temprature_ID = 0x31
/*只是可读设备状态 ID*/
enum
{
	// Connect_Status_ID   App 读取该值时，是表示蓝牙板与控制器的连接状态，App 写入该值时，表示的是App与蓝牙板的连接状态，写入1 表示App在线，写入0，表示App将要退出
	// 收到App退出时，就可以关闭蓝牙灯，清除user_connect_state 状态，写入1时，蓝牙板会将与APP的连接状态使用命令GET_BLUETOOTH_STATUS_CMD 发送给桑拿控制器，桑拿控制器会主动上传状态
	// 引入该ID的写入值，是因为苹果手机蓝牙（或者电脑）与篮板板一直处于绑定连接状态时，APP连接上蓝牙板后，蓝牙板是得不到App的连接状态的，退出时也得不到状态

	Real_Display_Of_Fahrenheit_Temp_ID = 0x21,
	Water_Pump_Flow_Switch_ID = 0x23,
	Water_Flow_Rate_ID = 0x24,
	Vacuum_Pump_Water_Flow_Switch_ID = 0x28,
	Real_Display_Of_Celsius_Temp_ID = 0x31,

};

/*********************************************************
 * @brief app读取Request_Pack 的数据包结构
 */
typedef struct
{
	uint8_t Head;
	uint8_t cmd;
	uint8_t Attr_MsgId;
	// uint8_t  ShortUserID[5];
	uint8_t chipID[8];
	uint8_t TimeStamp[4];
	uint16_t Payload_Size;
	Valu_1Bytes_1Attribute_Pack connction_status;				   // fun id 100                feature id   0x00
	Valu_1Bytes_1Attribute_Pack power_switch;					   // 开关fun id 108            feature id   0x01
	Valu_1Bytes_1Attribute_Pack celsius_temp_setting;			   // 摄氏温度设置 fun id 2     feature id   0x04
	Valu_1Bytes_1Attribute_Pack temp_scale_switching;			   // 温标切换111        			 feature id   0x05
	Valu_1Bytes_1Attribute_Pack fahrenheit_temperature_setting;	   // 华氏温度设置 111          feature id   0x0b
	Valu_2Bytes_1Attribute_Pack real_time_display_fahrenheit_temp; // 华氏温度实时显示137    	 feature id   0x21
	Valu_1Bytes_1Attribute_Pack lock_screen;					   // 锁屏 7										 feature id   0x22,
	Valu_1Bytes_1Attribute_Pack water_pump_flow_switch;			   // 水泵水流开关信号 101			 feature id   0x23,
	Valu_2Bytes_1Attribute_Pack water_flow_rate;				   // 水流量	106								 				   feature id   0x24,
	Valu_1Bytes_1Attribute_Pack touch_screen_button_sound;		   // 点击触摸屏幕时是否需要声音选项 110  feature id   0x25,
	Valu_1Bytes_1Attribute_Pack ozone_valve_opening_time;		   // 臭氧阀开启时间	112				 feature id   0x26,
	Valu_1Bytes_1Attribute_Pack auto_turn_on_after_powerOn;		   // 上电开机	116							 feature id   0x27
	Valu_1Bytes_1Attribute_Pack vacuum_pump_water_flow_switch;	   // 真空泵水流开关信号	129 	 feature id   0x28,
	Valu_2Bytes_1Attribute_Pack real_display_of_celsius_temp;	   // 摄氏温度实时显示 114  	   feature id   0x31

	uint16_t crc_vule;

} __attribute__((packed, aligned(1))) Get_Response_Pack_t;

/*********************************************************
 * @brief Set Request  设置命令类
 */
typedef struct
{
	uint8_t Head;
	uint8_t cmd;
	uint8_t Attr_MsgId;
	uint8_t ShortUserID[5];
	uint8_t TimeStamp[4];
	uint16_t Payload_Size;

	uint8_t id_data[sizeof(Get_Response_Pack_t) - 14]; // 按最多feature来开闭空间（包括两个字节CRC)

} __attribute__((packed, aligned(1))) Set_Request_Pack;

typedef struct
{
	unsigned char app_connect_state;
	unsigned char power_switch;					  // 开关101
	unsigned char celsius_temp_setting;			  // 摄氏温度设置104
	unsigned char temp_scale_switching;			  // 温标切换105
	unsigned char fahrenheit_temperature_setting; // 华氏温度设置 111
	unsigned char lock_screen;
	unsigned char touch_screen_button_sound;
	unsigned char qzone_valve_opening_time;
	unsigned char auto_turn_on_after_powerOn;
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

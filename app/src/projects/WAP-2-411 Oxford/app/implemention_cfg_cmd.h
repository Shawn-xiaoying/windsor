#ifndef IMPLEMENTION_CFG_CMD_H
#define IMPLEMENTION_CFG_CMD_H
#include "wap-1-100-windsor-master-protocol.h"
#include "bluetooth_connection_management.h"
#define DEF_PRODUCT_ID 0x01   //  Unknown = 0,HBOT = 1,Sauna = 2,RedLight = 3,ColdPlunge = 4
#define DEF_HARDWARE_VER 0x02 /* 默认蓝牙硬件信息 */

#define DEF_FIRWARE_VER_H 0X04    // Firmware Version
#define DEF_FIRWARE_VER_L 0X02
#define DEF_FIRWARE_VER_B 0X00// Firmware Build

/*功能特性 id======可读写*/
enum
{
  Connect_Status_ID = 0x00,
  Air_Compressor_ID = 0x10,
  Oxygen_Concentrator_ID,
  Air_Conditioner_ID,
  Remaining_Minutes_Timer_ID,
  Target_Temperature_ID,
  Measured_Temper_ID,

  // LED_bit_field_ID
};

// Indoor_Temp      =  Temprature_ID = 0x31
/*只是可读设备状态 ID*/
enum
{
  Humidity_ID = 0x30,
  Temprature_ID,
  CO2_ID,
  Noise_ID,
  TVOC_ID,
  PM_ID,
  AirPressure_ID,
  O2_ID,
  O3_ID,
  HCHO_ID

};

#if 0

/*********************************************************
 * @brief 2 字节结构数据包
 */
typedef struct
{
    uint8_t  Feature_ID;
    uint8_t  packet_size;
    uint8_t  Pwr_on;
}__attribute__((packed, aligned(1)))Valu_2Bytes_Pack;


/*********************************************************
 * @brief  4 字节结构数据包
 */
typedef struct
{
#if 0   
    uint8_t  Feature_ID;
    uint8_t  packet_size;
#endif

 //   uint8_t  is_working;
    uint16_t value;
}__attribute__((packed, aligned(1)))Valu_4Bytes_Pack;

/*********************************************************
 * @brief PM Sensor   的数据包结构-----
 */
typedef struct
{
#if 0   
    uint8_t  Feature_ID;
    uint8_t  packet_size;
#endif

 //   uint8_t  is_working;
    uint16_t PM1_value;
    uint16_t PM25_value;
    uint16_t PM10_value;
}__attribute__((packed, aligned(1))) PM_Sensor_Pack;

#endif

/*********************************************************
 * @brief app读取Request_Pack 的数据包结构
 */
typedef struct
{
  response_header_packet_t response_header_packet;
  Valu_1Bytes_1Attribute_Pack connction_status; //
  Valu_1Bytes_1Attribute_Pack Air_Compressor;   /*气压传感器*/
  Valu_1Bytes_1Attribute_Pack Oxygen_Concent;   /*Oxygen Concentrator ON*/
  Valu_1Bytes_1Attribute_Pack Air_Conditi;      /*Air Conditioner ON*/

  Valu_2Bytes_1Attribute_Pack Remaining_Minutes_Timer;
  Valu_2Bytes_1Attribute_Pack AirConditi_Target_Temper;
  Valu_2Bytes_1Attribute_Pack AirConditi_MeasuredTemper; // LED_bit_field;//空调测量温度，实际温度

  Valu_2Bytes_1Attribute_Pack Humidi_sensor; // the Humidity sensor
  Valu_2Bytes_1Attribute_Pack temper_sensor; // the temperature sensor
  Valu_2Bytes_1Attribute_Pack CO2_sensor;    // the CO2 sensor
  Valu_2Bytes_1Attribute_Pack noise_sensor;  // the noise sensor
  Valu_2Bytes_1Attribute_Pack TVOC_sensor;   // the TVOC sensor

  Valu_6Bytes_3Attribute_Pack PM_Sensor;

  Valu_2Bytes_1Attribute_Pack airpress_sensor; // the airpressure sensor
  Valu_2Bytes_1Attribute_Pack O2_sensor;       // the O2 sensor
  Valu_2Bytes_1Attribute_Pack O3_sensor;       // the O3 sensor
  Valu_2Bytes_1Attribute_Pack HCHO_sensor;     // the HCHO  sensor

  uint16_t crc_vule;

} __attribute__((packed, aligned(1))) Get_Response_Pack_t;

typedef struct
{
  unsigned char app_connect_state;
  unsigned char Air_Compressor_Pwr_on;
  unsigned char Oxygen_Concent_Pwr_on;
  unsigned char Air_Conditi_Pwr_on;
  unsigned short Remaining_Minutes_Timer_ID;
  unsigned short AirConditi_Target_Temper;
  unsigned short device_code;

} __attribute__((packed, aligned(1))) Set_Request_Data_Pack_t;

extern Get_Response_Pack_t Get_Response_Pack; // Get_Response_Pack 数据数组
extern Get_Response_Pack_t Last_Get_Response_Pack;
extern Feature_Id_List Update_Feature_Id_list;

extern Feature_Id_List Set_Feature_Id_list;
extern Set_Request_Data_Pack_t SettingData;   // 用来保存Set Reuset 的FeatureID数据


extern Set_Request_Data_Pack_t Session_SettingData; // 用来保存Set Reuset 的FeatureID数据
extern Feature_Id_List Session_Set_Feature_Id_list;

void refresh_change_data_feature_id_list(void);
void send_feature_id_data_to_app(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t *feature_id_list, uint8_t id_num, unsigned char ack_cmd);


#endif

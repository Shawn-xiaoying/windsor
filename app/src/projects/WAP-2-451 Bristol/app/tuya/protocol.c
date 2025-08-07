/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020
**
**
**
*********************************************************************************/
/**
 * @file    protocol.c
 * @author  shawn
 * @version v1.0.0
 * @date    2020.12.16
 * @brief
 *                       *******非常重要，一定要看哦！！！********
 *          1. 用户在此文件中实现数据下发/上报功能
 *          2. DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
 *          3. 当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的bluetooth_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/

#include "mcu.h"
#include "app_cfg_cmd.h"
#include "comm_lib.h"

#define NRF_LOG_MODULE_NAME protocol_log
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

//#define PROTOCOL_DEBUG_OUT

/******************************************************************************
                              第一步:初始化
1:在需要使用到bluetooth相关文件的文件中include "bluetooth.h"
2:在MCU初始化中调用mcu_api.c文件中的bluetooth_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的bluetooth_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**
******************************************************************************/

const DOWNLOAD_CMD_S upload_cmd[] =
    {
        {DPID_PULSE_OUTPUT, DP_TYPE_ENUM},
        {DPID_PULSE_SELECTION, DP_TYPE_BOOL},
        {DPID_PULSE_FREQUENCY_SETTING, DP_TYPE_VALUE},
        {DPID_PULSE_DUTY_CYCLE_SETTING, DP_TYPE_VALUE},
        {DPID_WORKING_TIME_SETTING, DP_TYPE_VALUE},
        {DPID_MUSIC_BLUETOOTH_CONNECT_STATUS, DP_TYPE_BOOL},
        {DPID_MUSIC_CONTROL, DP_TYPE_ENUM},
        {DPID_MUSIC_VOLUME, DP_TYPE_VALUE},
        {DPID_FAN_GEAR, DP_TYPE_VALUE},
        {DPID_EQUIMENT_DATE_TIME_SETTING, DP_TYPE_VALUE},
        {DPID_TEMPERATURE_SET, DP_TYPE_VALUE},
        {DPID_FAN_CONTROL, DP_TYPE_VALUE},
        {DPID_DEV_NAME, DP_TYPE_STRING},
        {DPID_DEV_SN, DP_TYPE_STRING},
        {DPID_DEV_CURRENT_TEMPERATURE, DP_TYPE_STRING},
        {DPID_DEV_WARN, DP_TYPE_ENUM}

};

/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/

/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart_transmit_output(unsigned char value)
{
  // #error "请将MCU串口发送函数填入该函数,并删除该行"
  uint32_t mt_serial_put(uint8_t *p_data, uint16_t len);

  mt_serial_put(&value, 1);
  /*
      //Example:
      extern void Uart_PutChar(unsigned char value);
      Uart_PutChar(value);	                                //串口发送函数
  */
}

/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/

// 自动化生成数据上报函数

/**
 * @brief  系统所有dp点信息上传,实现APP和muc数据同步
 * @param  Null
 * @return Null
 * @note   此函数SDK内部需调用，MCU必须实现该函数内数据上报功能，包括只上报和可上报可下发型数据
 */
void all_data_update(void)
{
}

static unsigned char dp_upload_pulse_output_handle(const unsigned char value[], unsigned short length)
{
  if (length != 1)
    return ERROR;
  Get_Response_Pack.pulse_output.attribute = value[0];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("pulse_output=%d", Get_Response_Pack.pulse_output.attribute);
#endif
  return SUCCESS;
}

static unsigned char dp_upload_pulse_selection_handle(const unsigned char value[], unsigned short length)
{
  uint8_t index = 0;
  if (length != 6)
    return ERROR;

  for (index = 0; index < length; index++)
  {
    Get_Response_Pack.pulse_selection.attribute[index] = value[index];
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("pulse_selection[%d]=%d", index, Get_Response_Pack.pulse_selection.attribute[index]);
#endif
  }

  return SUCCESS;
}

static unsigned char dp_upload_pulse_frequency_setting_handle(const unsigned char value[], unsigned short length)
{

  uint8_t index = 0;
  if (length != 12)
    return ERROR;
  for (index = 0; index < length / 2; index++)
  {
    Get_Response_Pack.pulse_frequency_setting.attribute[index] = value[index * 2] << 8 | value[index * 2 + 1];
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("pulse_frequency[%d]=%d", index, Get_Response_Pack.pulse_frequency_setting.attribute[index]);
#endif
  }

  return SUCCESS;
}

static unsigned char dp_upload_pulse_duty_cycle_setting_handle(const unsigned char value[], unsigned short length)
{

  uint8_t index = 0;
  if (length != 6)
    return ERROR;
  for (index = 0; index < length; index++)
  {
    Get_Response_Pack.pulse_duty_cycle_setting.attribute[index] = value[index];
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("pulse_duty_cycle[%d]=%d", index, value[index]);
#endif
  }

  return SUCCESS;
}

static unsigned char dp_upload_working_time_setting_handle(const unsigned char value[], unsigned short length)
{

  if (length != 2)
    return ERROR;
  Get_Response_Pack.working_time_setting.attribute = value[0] * 60 + value[1];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("working_time=%d", Get_Response_Pack.working_time_setting.attribute);
#endif

  return SUCCESS;
}

static unsigned char dp_upload_music_bluetooth_connection_status_handle(const unsigned char value[], unsigned short length)
{
  if (length != 1)
    return ERROR;
  Get_Response_Pack.music_bluetooth_connection_status.attribute = value[0];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("music_bluetooth_connection_status=%d", Get_Response_Pack.music_bluetooth_connection_status.attribute);
#endif
  return SUCCESS;
}

static unsigned char dp_upload_music_control_handle(const unsigned char value[], unsigned short length)
{
  if (length != 1)
    return ERROR;
  Get_Response_Pack.music_control.attribute = value[0];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("music_controlt=%d", Get_Response_Pack.music_control.attribute);
#endif
  return SUCCESS;
}

static unsigned char dp_upload_music_volume_handle(const unsigned char value[], unsigned short length)
{
  if (length != 1)
    return ERROR;
  Get_Response_Pack.music_volume.attribute = value[0];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("music_volume=%d", Get_Response_Pack.music_volume.attribute);
#endif

  return SUCCESS;
}

static unsigned char dp_upload_fan_gear_handle(const unsigned char value[], unsigned short length)
{
  if (length != 1)
    return ERROR;
  Get_Response_Pack.fan_gear.attribute = value[0];
#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("fan_gear=%d", Get_Response_Pack.fan_gear.attribute);
#endif

  return SUCCESS;
}

static unsigned char dp_upload_equipment_time_setting_handle(const unsigned char value[], unsigned short length)
{
  uint8_t index = 0;

  if (length != 7)
    return ERROR;
  Get_Response_Pack.equipment_time_setting.attribute1 = value[0] << 8 | value[1];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("equipment_time year=%d", Get_Response_Pack.equipment_time_setting.attribute1);
#endif

  for (index = 2; index < 7; index++)
  {
    Get_Response_Pack.equipment_time_setting.attribute2[index - 2] = value[index];
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("equipment_time_other[%d]=%d", index - 2, Get_Response_Pack.equipment_time_setting.attribute2[index - 2]);
#endif
  }

  return SUCCESS;
}

static unsigned char dp_upload_fan_ctrl_temperature_setting_handle(const unsigned char value[], unsigned short length)
{
  if (length != 2)
    return ERROR;

  Get_Response_Pack.fan_ctrl_temperature_setting.attribute1 = value[0];
  Get_Response_Pack.fan_ctrl_temperature_setting.attribute2 = value[1];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("fan_ctrl_temperature_setting.attribute1=%d", Get_Response_Pack.fan_ctrl_temperature_setting.attribute1);
  NRF_LOG_INFO("fan_ctrl_temperature_setting.attribute2=%d", Get_Response_Pack.fan_ctrl_temperature_setting.attribute2);
#endif
  return SUCCESS;
}

static unsigned char dp_upload_fan_ctrl_handle(const unsigned char value[], unsigned short length)
{
  if (length != 2)
    return ERROR;

  Get_Response_Pack.fan_ctrl.attribute1 = value[0];
  Get_Response_Pack.fan_ctrl.attribute2 = value[1];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("fan_ctrl.attribute1=%d", Get_Response_Pack.fan_ctrl.attribute1);
  NRF_LOG_INFO("fan_ctrl.attribute2 =%d", Get_Response_Pack.fan_ctrl.attribute2);
#endif
  return SUCCESS;
}

static unsigned char dp_upload_dev_name_handle(const unsigned char value[], unsigned short length)
{
//  uint8_t index = 0;
  char buffer[64]={0};
  uint8_t device_name_length = sizeof(Get_Response_Pack.red_light_bed_device_name.attribute);
  NRF_LOG_INFO("red_light_bed_device_name=%s", value);
  json_error_t result = json_extract_value((const char*)value, (const char*)"name", buffer, sizeof(buffer));
  if (result == JSON_PARSE_OK)
  {
    NRF_LOG_INFO("Extracted value: %s ", buffer);
    memset(Get_Response_Pack.red_light_bed_device_name.attribute, 0, device_name_length);
    if (strlen(buffer) <= device_name_length)
    memcpy(Get_Response_Pack.red_light_bed_device_name.attribute, buffer, strlen(buffer));
    else
    memcpy(Get_Response_Pack.red_light_bed_device_name.attribute, buffer, device_name_length);
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("red_light_bed_device_name=%s", Get_Response_Pack.red_light_bed_device_name.attribute);
#endif
  }
  else
  {
    NRF_LOG_INFO("Error code: %d ", result);
  }

  //  memset(Get_Response_Pack.red_light_bed_device_name.attribute, 0, 12);
  //  memcpy(Get_Response_Pack.red_light_bed_device_name.attribute, value, length);

  return SUCCESS;
}

#define JSON_KEY_T1 "t1"
#define JSON_KEY_T2 "t2"

static int extract_temperatures(const char *json_str, int8_t *t1, int8_t *t2)
{
  if (!json_str || !t1 || !t2)
  {
    return -1;
  }

  int ret;

  ret = parse_value(json_str, JSON_KEY_T1, t1);
  if (ret != 0)
  {
    return ret;
  }

  ret = parse_value(json_str, JSON_KEY_T2, t2);
  if (ret != 0)
  {
    return ret;
  }

  return 0;
}

static unsigned char dp_upload_red_light_bed_device_current_temperature_handle(const unsigned char value[], unsigned short length)
{
  uint8_t temp_str[32] = {0};
  int8_t temp1, temp2;
  memcpy(temp_str, value, length);
#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("current temperature=%s", temp_str);
#endif
  if (extract_temperatures((const char*)temp_str, &temp1, &temp2) == 0)
  {
    Get_Response_Pack.red_light_bed_device_current_temperature.attribute1 = temp1;
    Get_Response_Pack.red_light_bed_device_current_temperature.attribute2 = temp2;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("current temperature t1=%d t2=%d", temp1, temp2);
#endif

    return SUCCESS;
  }
  else
    return ERROR;
}
static unsigned char dp_upload_red_light_bed_device_warn_handle(const unsigned char value[], unsigned short length)
{
  if (length != 1)
    return ERROR;

  Get_Response_Pack.red_light_bed_device_warn.attribute = value[0];

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("red_light_bed_device_warn=%d", Get_Response_Pack.red_light_bed_device_warn.attribute);
#endif
  return SUCCESS;
}

/**
 * @brief  发送蓝牙连接状态到MCU
 * @param[in] NULL
 * @return Null
 * @note   Null
 */
extern bool bluetooth_connect_status;
void send_bluetooth_status_to_mcu(void)
{
  unsigned short send_len = 0;
  unsigned char conn_state = 0;

  if (bluetooth_connect_status)
    conn_state = 4;
  else
    conn_state = 0;
  //  NRF_LOG_INFO("conn_state==%d",conn_state);
  send_len = set_bluetooth_uart_byte(send_len, conn_state);
  bluetooth_uart_write_frame(GET_BLUETOOTH_STATUS_CMD, BLUETOOTH_TX_VER, send_len);
}

/**
 * @brief  发送心跳到MCU
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void send_heart_to_mcu(void)
{
  bluetooth_uart_write_frame(HEAT_BEAT_CMD, BLUETOOTH_TX_VER, 0);
}

/**
 * @brief  发送状态查询命令
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void send_status_query(void)
{

  bluetooth_uart_write_frame(STATE_QUERY_CMD, BLUETOOTH_TX_VER, 0);
}

/**
 * @brief  得到产品信息
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void get_product_info_cmd(void)
{

  bluetooth_uart_write_frame(PRODUCT_INFO_CMD, BLUETOOTH_TX_VER, 0);
}

/**
 * @brief  重置蓝牙返回命令
 * @param[in] NULL
 * @return Null
 * @note   Null
 */
void ack_bluetooth_reset_cmd(void)
{
}

/******************************************************************************
                                WARNING!!!
此部分函数用户请勿修改!!
******************************************************************************/

/**
 * @brief  dp 上传处理函数
 * @param[in] {dpid} dpid 序号
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {length} dp数据长度
 * @return dp处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   该函数用户不能修改
 */

typedef unsigned char (*dp_upload_fun_handle)(const unsigned char value[], unsigned short length);

typedef struct
{
  unsigned char dpid;
  dp_upload_fun_handle fun;
  unsigned char feature_id;
} dp_upload_hand_st;

dp_upload_hand_st Dp_Upload_Hand_List[] = {
    {DPID_PULSE_OUTPUT, dp_upload_pulse_output_handle, pulse_output_ID},
    {DPID_PULSE_SELECTION, dp_upload_pulse_selection_handle, pulse_selection_ID},
    {DPID_PULSE_FREQUENCY_SETTING, dp_upload_pulse_frequency_setting_handle, pulse_frequency_setting_ID},
    {DPID_PULSE_DUTY_CYCLE_SETTING, dp_upload_pulse_duty_cycle_setting_handle, pulse_duty_cycle_setting_ID},
    {DPID_WORKING_TIME_SETTING, dp_upload_working_time_setting_handle, working_time_setting_ID},
    {DPID_MUSIC_BLUETOOTH_CONNECT_STATUS, dp_upload_music_bluetooth_connection_status_handle, music_bluetooth_connection_status_ID},
    {DPID_MUSIC_CONTROL, dp_upload_music_control_handle, music_control_ID},
    {DPID_MUSIC_VOLUME, dp_upload_music_volume_handle, music_volume_ID},
    {DPID_FAN_GEAR, dp_upload_fan_gear_handle, fan_gear_ID},
    {DPID_EQUIMENT_DATE_TIME_SETTING, dp_upload_equipment_time_setting_handle, equipment_time_setting_ID},
    {DPID_DEV_NAME, dp_upload_dev_name_handle, red_light_bed_dev_name_ID},
    // {DPID_DEV_SN, dp_upload_dev_name_handle, red_light_bed_dev_sn_ID},
    {DPID_DEV_CURRENT_TEMPERATURE, dp_upload_red_light_bed_device_current_temperature_handle, red_light_bed_device_current_temperature_ID},
    {DPID_DEV_WARN, dp_upload_red_light_bed_device_warn_handle, red_light_bed_device_warn_ID},
    {DPID_TEMPERATURE_SET, dp_upload_fan_ctrl_temperature_setting_handle, fan_temperature_setting_ID},
    {DPID_FAN_CONTROL, dp_upload_fan_ctrl_handle, fan_control_ID}

};

static bool feature_id_is_exist(Feature_Id_List *list, unsigned char feature_id)
{
  unsigned char i = 0;
  for (i = 0; i < list->update_feature_num; i++)
  {
    if (list->feature_id[i] == feature_id)
      return true;
  }
  return false;
}
/*
if find fun_id  return i+1 ;
else return 0 ;

*/

static unsigned char find_upload_hand_list_index(unsigned char fun_id)
{
  for (unsigned char i = 0; i < sizeof(Dp_Upload_Hand_List) / sizeof(dp_upload_hand_st); i++)
  {
    if (Dp_Upload_Hand_List[i].dpid == fun_id)
      return i + 1;
  }

  return 0;
}

unsigned char dp_upload_handle_have_cb(unsigned char dpid, const unsigned char value[], unsigned short length)
{
  /*********************************
  当前函数处理可下发/可上报数据调用
  具体函数内需要实现下发数据处理
  完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
  ***********************************/

  unsigned char ret;
  unsigned char index;

  index = find_upload_hand_list_index(dpid);
  if (index == 0)
    return ERROR;
  index = index - 1;

  if (Dp_Upload_Hand_List[index].fun != NULL)
  {
    ret = Dp_Upload_Hand_List[index].fun(value, length);
  }
  else
    return ERROR;

  return ret;
}

unsigned char dp_upload_handle(unsigned char dpid, const unsigned char value[], unsigned short length)
{
  /*********************************
  当前函数处理可下发/可上报数据调用
  具体函数内需要实现下发数据处理
  完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
  ***********************************/
  unsigned char ret;
  unsigned char index;

  if (current_cmd_data.dpID == dpid)
  {
    send_cmd_state = CMD_STATUS_OK;
    // 将返回值再写入current_cmd_data
    memset((void *)current_cmd_data.data.buffer, 0, CMD_DATA_LENGTH);
    memcpy((void *)current_cmd_data.data.buffer, value, length);
    //	NRF_LOG_INFO("end_cmd_state=CMD_STATUS_OK,current_cmd_data.dpID=%d",current_cmd_data.dpID);
  }
  index = find_upload_hand_list_index(dpid);
  if (index == 0)
    return ERROR;
  index = index - 1;

  if (Dp_Upload_Hand_List[index].fun != NULL)
  {

    if (send_cmd_state == CMD_STATUS_NONE) // 主动上传，记录下featchid
    {
      if (!feature_id_is_exist(&Update_Feature_Id_list, Dp_Upload_Hand_List[index].feature_id))
        Update_Feature_Id_list.feature_id[Update_Feature_Id_list.update_feature_num++] = Dp_Upload_Hand_List[index].feature_id;
    }
    else if (current_cmd_data.cmd == STATE_QUERY_CMD) // 状态查询
    {
      if (!feature_id_is_exist(&Response_Feature_Id_list, Dp_Upload_Hand_List[index].feature_id))
        Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] = Dp_Upload_Hand_List[index].feature_id;
    }
    else if (current_cmd_data.cmd == DATA_QUERT_CMD) // 命令下发，设置命令
    {
      if (!feature_id_is_exist(&Response_Feature_Id_list, Dp_Upload_Hand_List[index].feature_id))
        Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] = Dp_Upload_Hand_List[index].feature_id;
    }

    ret = Dp_Upload_Hand_List[index].fun(value, length);
  }
  else
    return ERROR;

  return ret;
}

/**
 * @brief  获取所有dp命令总和
 * @param[in] Null
 * @return 下发命令总和
 * @note   该函数用户不能修改
 */
unsigned char get_upload_cmd_total(void)
{
  return (sizeof(upload_cmd) / sizeof(upload_cmd[0]));
}

void send_dpID_cmd( cmd_data_description *c_d_desc)
{
  uint8_t temp_buf[32] = {0};
  switch (c_d_desc->dpID)
  {
  case DPID_PULSE_OUTPUT:
  case DPID_MUSIC_CONTROL:
    send_dp_enum_to_mcu(c_d_desc->dpID, c_d_desc->data.byte_data);
    break;
  case DPID_MUSIC_VOLUME:
  case DPID_FAN_GEAR:
    send_dp_value_to_mcu(c_d_desc->dpID, (unsigned char*)&c_d_desc->data.byte_data, 1);
    break;

  case DPID_EQUIMENT_DATE_TIME_SETTING:
    send_dp_value_to_mcu(c_d_desc->dpID, c_d_desc->data.buffer, 7);
    break;

  case DPID_PULSE_SELECTION:
    send_dp_bool_to_mcu(c_d_desc->dpID, c_d_desc->data.buffer, 6);
    break;

  case DPID_PULSE_FREQUENCY_SETTING:
    send_dp_value_to_mcu(c_d_desc->dpID, c_d_desc->data.buffer, 12);
    break;

  case DPID_PULSE_DUTY_CYCLE_SETTING:
    send_dp_value_to_mcu(c_d_desc->dpID, c_d_desc->data.buffer, 6);
    break;

  case DPID_WORKING_TIME_SETTING:
  case DPID_TEMPERATURE_SET:
  case DPID_FAN_CONTROL:
    send_dp_value_to_mcu(c_d_desc->dpID, c_d_desc->data.buffer, 2);
    break;
  case DPID_DEV_NAME:
    sprintf((char*)temp_buf, "{\"name\":\"%s\"}", c_d_desc->data.buffer);
    send_dp_string_to_mcu(c_d_desc->dpID, (const unsigned char*)temp_buf, strlen((const char*)temp_buf));
    break;

  default:
    break;
  }
}

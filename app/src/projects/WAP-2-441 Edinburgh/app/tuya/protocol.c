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

#define NRF_LOG_MODULE_NAME protocol_log
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

// #define PROTOCOL_DEBUG_OUT

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
        {DPID_POWER_SWITCH, DP_TYPE_BOOL},
        {DPID_TEMP_C_SET, DP_TYPE_VALUE},
        {DPID_TEMP_C_DISP, DP_TYPE_VALUE},
        {DPID_TEMP_F_SET, DP_TYPE_VALUE},
        {DPID_TEMP_F_DISP, DP_TYPE_VALUE},
        {DPID_TEMP_SCALE_SWITCH, DP_TYPE_ENUM},
        {DPID_LOCK_SCREEN_SWITCH, DP_TYPE_BOOL},
        {DPID_WATER_PUMP_FLOW_SWITCH, DP_TYPE_BOOL},
        {DPID_WATER_FLOW_RATE, DP_TYPE_VALUE},
        {DPID_SCREEN_BUTTON_SOUND_SWITCH, DP_TYPE_BOOL},
        {DPID_OZONE_VALUE_OPEN_TIME, DP_TYPE_VALUE},
        {DPID_AUTO_TURN_ON_AFTER_POWER_ON, DP_TYPE_BOOL},
        {DPID_VACUUM_PUMP_WATER_FLOW_SWITCH, DP_TYPE_BOOL}};

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

static unsigned char dp_upload_lock_screen_switch_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char lock_screen;
  lock_screen = bluetooth_get_dp_upload_bool(value, length);
  Get_Response_Pack.lock_screen.attribute = lock_screen;
#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("lock_screen=%d", lock_screen);
#endif

  ret = mcu_dp_bool_update(DPID_LOCK_SCREEN_SWITCH, lock_screen);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

static unsigned char dp_upload_water_pump_flow_switch_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char water_pump_flow_switch;
  water_pump_flow_switch = bluetooth_get_dp_upload_bool(value, length);
  Get_Response_Pack.water_pump_flow_switch.attribute = water_pump_flow_switch;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("water_pump_flow_switch=%d", water_pump_flow_switch);
#endif

  ret = mcu_dp_bool_update(DPID_WATER_PUMP_FLOW_SWITCH, water_pump_flow_switch);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

static unsigned char dp_upload_water_flow_rate_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned long water_flow_rate;
  water_flow_rate = bluetooth_get_dp_upload_value(value, length);
  Get_Response_Pack.water_flow_rate.attribute = water_flow_rate;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("water_flow_rate=%d", water_flow_rate);
#endif
  ret = mcu_dp_value_update(DPID_WATER_FLOW_RATE, water_flow_rate);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

static unsigned char dp_upload_screen_button_sound_switch_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char touch_screen_button_sound;
  touch_screen_button_sound = bluetooth_get_dp_upload_bool(value, length);
  Get_Response_Pack.touch_screen_button_sound.attribute = touch_screen_button_sound;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("touch_screen_button_sound=%d", touch_screen_button_sound);
#endif

  ret = mcu_dp_bool_update(DPID_SCREEN_BUTTON_SOUND_SWITCH, touch_screen_button_sound);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

static unsigned char dp_upload_ozone_value_open_time_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char ozone_valve_opening_time;
  ozone_valve_opening_time = bluetooth_get_dp_upload_value(value, length);
  Get_Response_Pack.ozone_valve_opening_time.attribute = ozone_valve_opening_time;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("ozone_valve_opening_time=%d", ozone_valve_opening_time);
#endif

  ret = mcu_dp_value_update(DPID_OZONE_VALUE_OPEN_TIME, ozone_valve_opening_time);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

static unsigned char dp_upload_auto_trun_on_after_power_on_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char auto_turn_on_after_powerOn;
  auto_turn_on_after_powerOn = bluetooth_get_dp_upload_bool(value, length);
  Get_Response_Pack.auto_turn_on_after_powerOn.attribute = auto_turn_on_after_powerOn;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("auto_turn_on_after_powerOn=%d", auto_turn_on_after_powerOn);
#endif

  ret = mcu_dp_bool_update(DPID_AUTO_TURN_ON_AFTER_POWER_ON, auto_turn_on_after_powerOn);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

static unsigned char dp_upload_vacuum_pump_water_flow_switch_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char vacuum_pump_water_flow_switch;
  vacuum_pump_water_flow_switch = bluetooth_get_dp_upload_bool(value, length);
  Get_Response_Pack.vacuum_pump_water_flow_switch.attribute = vacuum_pump_water_flow_switch;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("vacuum_pump_water_flow_switch=%d", vacuum_pump_water_flow_switch);
#endif

  ret = mcu_dp_bool_update(DPID_VACUUM_PUMP_WATER_FLOW_SWITCH, vacuum_pump_water_flow_switch);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

/*****************************************************************************
函数名称 : dp_upload_power_switch_handle
功能描述 : 针对DPID_POWER_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
extern void update_adv_data_device_state_of_power(unsigned char state);
static unsigned char dp_upload_power_switch_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char power_switch;
  power_switch = bluetooth_get_dp_upload_bool(value, length);
  Get_Response_Pack.power_switch.attribute = power_switch;
  update_adv_data_device_state_of_power(power_switch);

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("power_switch=%d", power_switch);
#endif

  ret = mcu_dp_bool_update(DPID_POWER_SWITCH, power_switch);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}
/*****************************************************************************
函数名称 : dp_upload_tem_set_handle
功能描述 : 针对DPID_TEMP_C_SET 摄氏温度设置的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_temp_set_C_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为VALUE
  unsigned char ret;
  unsigned long tem_set;
  tem_set = bluetooth_get_dp_upload_value(value, length);
  Get_Response_Pack.celsius_temp_setting.attribute = tem_set;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("temp_c_set=%d", tem_set);
#endif

  ret = mcu_dp_value_update(DPID_TEMP_C_SET, tem_set);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_tem_change_handle
功能描述 : 针对DPID_TEMP_SCALE_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_temp_scale_switch_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  // 0:off/1:on
  unsigned char tem_change;
  tem_change = bluetooth_get_dp_upload_bool(value, length);
  Get_Response_Pack.temp_scale_switching.attribute = tem_change;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("temp_scale_switch=%d", tem_change);
#endif

  ret = mcu_dp_enum_update(DPID_TEMP_SCALE_SWITCH, tem_change);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}
/*****************************************************************************
函数名称 : dp_upload_temp_set_handle
功能描述 : 针对DPID_TEMP_F_SET华氏温度设置处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_temp_set_F_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为ENUM
  unsigned char ret;
  unsigned char temp_set;
  temp_set = bluetooth_get_dp_upload_value(value, length);
  Get_Response_Pack.fahrenheit_temperature_setting.attribute = temp_set;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("temp_set_F=%d", temp_set);
#endif

  ret = mcu_dp_value_update(DPID_TEMP_F_SET, temp_set);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

/*****************************************************************************
函数名称 : dp_upload_indoor_tem_handle
功能描述 : 针对DPID_TEMP_C_DISP 摄氏温度实时显示的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/

void update_adv_data_device_state_of_temperature(unsigned char value);
static unsigned char dp_upload_temp_display_C_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  signed short temp_display_c; // -20 - 120
  unsigned char temp_c;
  // float f_disp=0;
  temp_display_c = (signed short)bluetooth_get_dp_upload_value(value, length);
  //	Get_Response_Pack.real_display_of_celsius_temp.attribute1 = 1 ;
  Get_Response_Pack.real_display_of_celsius_temp.attribute = temp_display_c;
  // f_disp = ((float)(temp_display_c)/(float)10.0);
  temp_c = temp_display_c / 10;
  update_adv_data_device_state_of_temperature(temp_c);
#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("temp_display_c=%d", temp_display_c);
#endif

  ret = mcu_dp_value_update(DPID_TEMP_C_DISP, temp_display_c);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}

/*****************************************************************************
函数名称 : dp_upload_temp_display_handle
功能描述 : 针对DPID_TEMP_F_SET华氏温度实时显示的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/

static unsigned char dp_upload_temp_display_F_handle(const unsigned char value[], unsigned short length)
{
  // 示例:当前DP类型为BOOL
  unsigned char ret;
  signed short temp_display_F; // -20 - 120
  temp_display_F = (signed short)bluetooth_get_dp_upload_value(value, length);
  Get_Response_Pack.real_time_display_fahrenheit_temp.attribute = temp_display_F;

#ifdef PROTOCOL_DEBUG_OUT
  NRF_LOG_INFO("temp_display_f=%d", temp_display_F);
#endif

  ret = mcu_dp_value_update(DPID_TEMP_F_DISP, temp_display_F);
  if (ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
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
    {DPID_TEMP_C_SET, dp_upload_temp_set_C_handle, Celsius_Temperature_Setting_ID},
    {DPID_TEMP_C_DISP, dp_upload_temp_display_C_handle, Real_Display_Of_Celsius_Temp_ID},
    {DPID_LOCK_SCREEN_SWITCH, dp_upload_lock_screen_switch_handle, Lock_Screen_ID},
    {DPID_WATER_PUMP_FLOW_SWITCH, dp_upload_water_pump_flow_switch_handle, Water_Pump_Flow_Switch_ID},
    {DPID_WATER_FLOW_RATE, dp_upload_water_flow_rate_handle, Water_Flow_Rate_ID},
    {DPID_POWER_SWITCH, dp_upload_power_switch_handle, Power_Switch_ID},
    {DPID_TEMP_F_SET, dp_upload_temp_set_F_handle, Fahrenheit_Temperature_Setting_ID},
    {DPID_TEMP_SCALE_SWITCH, dp_upload_temp_scale_switch_handle, Temperature_Scale_Switching_ID},
    {DPID_SCREEN_BUTTON_SOUND_SWITCH, dp_upload_screen_button_sound_switch_handle, Touch_Screen_Button_Sound_ID},
    {DPID_OZONE_VALUE_OPEN_TIME, dp_upload_ozone_value_open_time_handle, Ozone_valve_opening_time_ID},
    {DPID_AUTO_TURN_ON_AFTER_POWER_ON, dp_upload_auto_trun_on_after_power_on_handle, Auto_Turn_on_After_PowerOn_ID},
    {DPID_VACUUM_PUMP_WATER_FLOW_SWITCH, dp_upload_vacuum_pump_water_flow_switch_handle, Vacuum_Pump_Water_Flow_Switch_ID},
    {DPID_TEMP_F_DISP, dp_upload_temp_display_F_handle, Real_Display_Of_Fahrenheit_Temp_ID}

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
    //			      switch(send_cmd_state)
    //						{
    //							case CMD_STATUS_NONE:
    //							 if(!feature_id_is_exist(&Update_Feature_Id_list,Dp_Upload_Hand_List[index].feature_id)){
    //							 if(Update_Feature_Id_list.update_feature_num <MAX_FEATURE_NUM)
    //					      Update_Feature_Id_list.feature_id[Update_Feature_Id_list.update_feature_num++] =Dp_Upload_Hand_List[index].feature_id;
    //
    //						  	}
    //							break ;
    //							case STATE_QUERY_CMD:
    //							case DATA_QUERT_CMD:
    //							if(!feature_id_is_exist(&Response_Feature_Id_list,Dp_Upload_Hand_List[index].feature_id)){
    //								if(Response_Feature_Id_list.update_feature_num <MAX_FEATURE_NUM)
    //								Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] =Dp_Upload_Hand_List[index].feature_id;
    //							}
    //							break ;
    //
    //							default:
    //							break ;
    //						}

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

void send_dpID_cmd(volatile cmd_data_description *c_d_desc)
{
  switch (c_d_desc->dpID)
  {
  // bool  ;
  case DPID_POWER_SWITCH:
  case DPID_LOCK_SCREEN_SWITCH:
  case DPID_SCREEN_BUTTON_SOUND_SWITCH:
  case DPID_AUTO_TURN_ON_AFTER_POWER_ON:

    send_dp_bool_to_mcu(c_d_desc->dpID, c_d_desc->data.byte_data);
    break;
  case DPID_TEMP_SCALE_SWITCH:
    send_dp_enum_to_mcu(c_d_desc->dpID, c_d_desc->data.byte_data);
    break;
  // value
  case DPID_TEMP_C_SET:
  case DPID_TEMP_F_SET:
  case DPID_OZONE_VALUE_OPEN_TIME:

    send_dp_value_to_mcu(c_d_desc->dpID, c_d_desc->data.int_data);
    break;
  }
}

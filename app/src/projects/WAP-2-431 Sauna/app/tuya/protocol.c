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
#include "tuya_cmd_send_task.h"

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
        {DPID_HEAT_SWITCH, DP_TYPE_BOOL},
        {DPID_TIME_SET, DP_TYPE_VALUE},
        {DPID_TEMP_C_SET, DP_TYPE_VALUE},
        {DPID_TEM_CHANGE, DP_TYPE_BOOL},
        {DPID_TIMING_SWITCH, DP_TYPE_VALUE},
        {DPID_LIGHT_SWITCH1, DP_TYPE_BOOL},
        {DPID_LIGHT_SWITCH2, DP_TYPE_BOOL},
        {DPID_HEATING_DISPLAY, DP_TYPE_ENUM},
        {DPID_LIGHT_COLOR, DP_TYPE_STRING},
        {DPID_TEMP_SET, DP_TYPE_VALUE},
        {DPID_MEDIA_SWITCH, DP_TYPE_BOOL},
        {DPID_FAN_SWITCH, DP_TYPE_BOOL},
        {DPID_INDOOR_TEM, DP_TYPE_VALUE},
        {DPID_COLOR_SWITCH, DP_TYPE_BOOL},
        {DPID_COLOR_CHANGE_SWITCH, DP_TYPE_BOOL},
        {DPID_HEATING_RESERVATION, DP_TYPE_BOOL},
        {DPID_TEMP_DISPLAY, DP_TYPE_VALUE},
        {DPID_HEAT1_POWER, DP_TYPE_VALUE},
        {DPID_HEAT2_POWER, DP_TYPE_VALUE},
        {DPID_HEAT3_POWER, DP_TYPE_VALUE},
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

    if (power_switch == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_POWER_SWITCH, power_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_upload_heat_switch_handle
功能描述 : 针对DPID_HEAT_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_heat_switch_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char heat_switch;

    heat_switch = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.heat_switch.attribute = heat_switch;

#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("heat_switch=%d", heat_switch);
#endif

    if (heat_switch == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_HEAT_SWITCH, heat_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_time_set_handle
功能描述 : 针对DPID_TIME_SET的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_time_set_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long time_set;

    time_set = bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.heatint_time.attribute = time_set;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("time_set=%d", time_set);
#endif

    /*
    //VALUE type data processing

    */

    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TIME_SET, time_set);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_upload_tem_set_handle
功能描述 : 针对DPID_TEMP_C_SET的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_tem_set_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long tem_set;

    //	  if(send_cmd_state == CMD_STATUS_NONE) //主动上传，记录下featchid
    //		{
    //			Update_Feature_Id_list.feature_id[Update_Feature_Id_list.update_feature_num++] = Celsius_Temperature_Setting_ID;
    //		}else{
    //
    //		 Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] =Celsius_Temperature_Setting_ID;
    //		}

    tem_set = bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.celsius_temp_setting.attribute = tem_set;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("temp_c_set=%d", tem_set);
#endif
    /*
    //VALUE type data processing

    */

    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TEMP_C_SET, tem_set);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_tem_change_handle
功能描述 : 针对DPID_TEM_CHANGE的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_tem_change_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char tem_change;

    tem_change = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.temp_scale_switching.attribute = tem_change;
    // #ifdef 	PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("tem_change=%d", tem_change);
    // #endif

    if (tem_change == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_TEM_CHANGE, tem_change);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_timing_switch_handle
功能描述 : 针对DPID_TIMING_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_timing_switch_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long timing_switch;

    timing_switch = bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.timer_on_off.attribute = timing_switch;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("timing_switch=%d", timing_switch);
#endif
    /*
    //VALUE type data processing

    */

    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TIMING_SWITCH, timing_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_light_switch1_handle
功能描述 : 针对DPID_LIGHT_SWITCH1的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_light_switch1_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char light_switch1;

    light_switch1 = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.internal_reading_light_switch.attribute = light_switch1;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("light_switch1=%d", light_switch1);
#endif
    if (light_switch1 == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_LIGHT_SWITCH1, light_switch1);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_light_switch2_handle
功能描述 : 针对DPID_LIGHT_SWITCH2的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_light_switch2_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char light_switch2;

    light_switch2 = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.external_reading_light_switch.attribute = light_switch2;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("light_switch2=%d", light_switch2);
#endif
    if (light_switch2 == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_LIGHT_SWITCH2, light_switch2);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_light_control_handle
功能描述 : 针对DPID_LIGHT_CONTROL的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_heating_display_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char heating_display;

    heating_display = bluetooth_get_dp_upload_enum(value, length);
    Get_Response_Pack.heating_display.attribute = heating_display;

#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("heating_display=%d", heating_display);
#endif

    switch (heating_display)
    {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    default:

        break;
    }

    // There should be a report after processing the DP
    ret = mcu_dp_enum_update(DPID_HEATING_DISPLAY, heating_display);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_light_color_handle
功能描述 : 针对DPID_LIGHT_COLOR的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_light_color_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为STRING
    unsigned char ret;

    unsigned char string_data[40] = {0};

    // There should be a report after processing the DP
    memcpy(string_data, value, length);

#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("light_color=%s", string_data);
#endif

    if (length <= 12)
        memcpy(Get_Response_Pack.led_ambient_light_adjustment.attribute, value, length);
    else
        memcpy(Get_Response_Pack.led_ambient_light_adjustment.attribute, value, 12);

    ret = mcu_dp_string_update(DPID_LIGHT_COLOR, value, length);

    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_upload_temp_set_handle
功能描述 : 针对DPID_LIGHT_SCENCE华氏温度设置处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/

static unsigned char dp_upload_temp_set_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char temp_set;

    temp_set = bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.fahrenheit_temperature_setting.attribute = temp_set;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("temp_set=%d", temp_set);
#endif

    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TEMP_SET, temp_set);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_media_switch_handle
功能描述 : 针对DPID_MEDIA_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_media_switch_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char media_switch;

    media_switch = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.multimedia_switch.attribute = media_switch;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("media_switch=%d", media_switch);
#endif

    if (media_switch == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_MEDIA_SWITCH, media_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_upload_fan_switch_handle
功能描述 : 针对DPID_FAN_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_fan_switch_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char fan_switch;

    fan_switch = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.fan_switch.attribute = fan_switch;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("fan_switch=%d", fan_switch);
#endif

    if (fan_switch == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_FAN_SWITCH, fan_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_upload_color_switch_handle
功能描述 : 针对DPID_COLOR_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_upload_color_switch_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char color_switch;

    color_switch = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.color_light_switch.attribute = color_switch;

#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("color_switch=%d", color_switch);
#endif

    if (color_switch == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_COLOR_SWITCH, color_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
函数名称 : dp_upload_indoor_tem_handle
功能描述 : 针对DPID_INDOOR_TEM室内摄氏温度实时显示的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
void update_adv_data_device_state_of_temperature(unsigned char value);

static unsigned char dp_upload_indoor_tem_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    signed char indoor_tem; // -20 - 120

    indoor_tem = (signed char)bluetooth_get_dp_upload_value(value, length);
    //	Get_Response_Pack.indoor_temp.attribute1 = 1 ;
    Get_Response_Pack.indoor_temp.attribute = indoor_tem;
    update_adv_data_device_state_of_temperature(indoor_tem);
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("indoor_temp=%d", indoor_tem);
#endif

    // There should be a report after processing the DP
    // ret = mcu_dp_value_update(DPID_INDOOR_TEM,indoor_tem);

    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
函数名称 : dp_upload_color_change_switch_handle
功能描述 : 针对DPID_COLOR_CHANGE_SWITCH  led氛围灯色彩变幻开关 的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/

static unsigned char dp_upload_color_change_switch_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char color_change_switch;

    color_change_switch = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.led_ambient_light_color_change_switch.attribute = color_change_switch;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("color_change_switch=%d", color_change_switch);
#endif

    if (color_change_switch == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_COLOR_CHANGE_SWITCH, color_change_switch);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
函数名称 : dp_upload_heating_reservation_handle
功能描述 : 针对DPID_HEATING_RESERVATION  加热预约开关 的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/

static unsigned char dp_upload_heating_reservation_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    // 0:off/1:on
    unsigned char heating_reservation;

    heating_reservation = bluetooth_get_dp_upload_bool(value, length);
    Get_Response_Pack.heating_reservation_switch.attribute = heating_reservation;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("heating_reservation=%d", heating_reservation);
#endif

    if (heating_reservation == 0)
    {
        // bool off
    }
    else
    {
        // bool on
    }

    // There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_HEATING_RESERVATION, heating_reservation);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

/*****************************************************************************
函数名称 : dp_upload_temp_display_handle
功能描述 : 针对DDPID_TEMP_DISPLAY华氏温度实时显示的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/

static unsigned char dp_upload_temp_display_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    signed short temp_display; // -20 - 120

    temp_display = (signed short)bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.real_time_display_fahrenheit_temp.attribute = temp_display;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("temp_display=%d", temp_display);
#endif
    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TEMP_DISPLAY, temp_display);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

static unsigned char dp_upload_heat1_power_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    unsigned char heat_power; // 0 - 100

    heat_power = (signed short)bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.heat1_power.attribute = heat_power;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("heat1_power=%d", heat_power);
#endif
    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_HEAT1_POWER, heat_power);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
static unsigned char dp_upload_heat2_power_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    unsigned char heat_power; // 0 - 100

    heat_power = (signed short)bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.heat2_power.attribute = heat_power;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("heat2_power=%d", heat_power);
#endif
    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_HEAT1_POWER, heat_power);
    if (ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

static unsigned char dp_upload_heat3_power_handle(const unsigned char value[], unsigned short length)
{
    // 示例:当前DP类型为BOOL
    unsigned char ret;
    unsigned char heat_power; // 0 - 100

    heat_power = (signed short)bluetooth_get_dp_upload_value(value, length);
    Get_Response_Pack.heat3_power.attribute = heat_power;
#ifdef PROTOCOL_DEBUG_OUT
    NRF_LOG_INFO("heat3_power=%d", heat_power);
#endif
    // There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_HEAT1_POWER, heat_power);
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
    {DPID_POWER_SWITCH /*101*/, dp_upload_power_switch_handle, Power_Switch_ID},
    {DPID_HEAT_SWITCH /*102*/, dp_upload_heat_switch_handle, Heat_Switch_ID},
    {DPID_TIME_SET /*103*/, dp_upload_time_set_handle, Heat_Time_ID},
    {DPID_TEMP_C_SET /*104*/, dp_upload_tem_set_handle, Celsius_Temperature_Setting_ID},
    {DPID_TEM_CHANGE /*105*/, dp_upload_tem_change_handle, Temperature_Scale_Switching_ID},
    {DPID_TIMING_SWITCH /*106*/, dp_upload_timing_switch_handle, Timer_ON_OFF_ID},
    {DPID_LIGHT_SWITCH1 /*107*/, dp_upload_light_switch1_handle, Internal_Reading_Light_Switch_ID},
    {DPID_LIGHT_SWITCH2 /*108*/, dp_upload_light_switch2_handle, External_Reading_Light_Switch_ID},
    {DPID_HEATING_DISPLAY /*109*/, dp_upload_heating_display_handle, Heating_Display_ID},
    {DPID_LIGHT_COLOR /*110*/, dp_upload_light_color_handle, Light_Color_ID},
    {DPID_TEMP_SET /*111*/, dp_upload_temp_set_handle, Temp_Set_ID},
    {DPID_MEDIA_SWITCH /*112*/, dp_upload_media_switch_handle, Media_Switch_ID},
    {DPID_FAN_SWITCH /*113*/, dp_upload_fan_switch_handle, Fan_Switch_ID},
    {DPID_INDOOR_TEM /*114*/, dp_upload_indoor_tem_handle, Indoor_Temp_ID},
    {DPID_COLOR_SWITCH /*115*/, dp_upload_color_switch_handle, Color_Light_Switch_ID},
    {DPID_COLOR_CHANGE_SWITCH /*116*/, dp_upload_color_change_switch_handle, Color_Change_Switch_ID},
    {DPID_HEATING_RESERVATION /*117*/, dp_upload_heating_reservation_handle, Heat_Reservation_Switch_ID},
    {118 /*118*/, NULL, 0},
    {DPID_TEMP_DISPLAY /*119*/, dp_upload_temp_display_handle, Real_Display_Of_Fahrenheit_Temp_ID},
    {DPID_HEAT1_POWER, dp_upload_heat1_power_handle, heat1_power_ID},
    {DPID_HEAT2_POWER, dp_upload_heat2_power_handle, heat2_power_ID},
    {DPID_HEAT3_POWER, dp_upload_heat3_power_handle, heat3_power_ID}

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

unsigned char dp_upload_handle_have_cb(unsigned char dpid, const unsigned char value[], unsigned short length)
{
    /*********************************
    当前函数处理可下发/可上报数据调用
    具体函数内需要实现下发数据处理
    完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
    ***********************************/
    unsigned char ret = ERROR;
    if (dpid >= DPID_POWER_SWITCH && dpid <= DPID_HEAT3_POWER)
    {
        if (Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].fun != NULL)
        {
            ret = Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].fun(value, length);
        }
    }

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
    NRF_LOG_INFO("dp_upload_handle dpid=%d ,current_cmd_data.cmd=%d cmd dpid=%d send_cmd_state =%d", dpid, current_cmd_data.cmd, current_cmd_data.dpID, send_cmd_state);
    if (current_cmd_data.dpID == dpid)
    {
        send_cmd_state = CMD_STATUS_OK;
        // 将返回值再写入current_cmd_data
        memset((void *)current_cmd_data.data.buffer, 0, CMD_DATA_LENGTH);
        memcpy((void *)current_cmd_data.data.buffer, value, length);
        NRF_LOG_INFO("send_cmd_state=CMD_STATUS_OK,current_cmd_data.dpID=%d", current_cmd_data.dpID);
    }
    if (dpid >= DPID_POWER_SWITCH && dpid <= DPID_HEAT3_POWER)
    {

        if (Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].fun != NULL)
        {
            //					switch(send_cmd_state)
            //					{
            //						case CMD_STATUS_NONE:
            //						//						  //NRF_LOG_INFO("upload feature num++");
            //						if(!feature_id_is_exist(&Update_Feature_Id_list,Dp_Upload_Hand_List[dpid-101].feature_id)){
            //						if(Update_Feature_Id_list.update_feature_num <MAX_FEATURE_NUM)
            //						Update_Feature_Id_list.feature_id[Update_Feature_Id_list.update_feature_num++] =Dp_Upload_Hand_List[dpid-101].feature_id;
            //
            //						}
            //						break ;
            //
            //						case STATE_QUERY_CMD:
            //						case DATA_QUERT_CMD:
            //						if(!feature_id_is_exist(&Response_Feature_Id_list,Dp_Upload_Hand_List[dpid-101].feature_id)){
            //						if(Response_Feature_Id_list.update_feature_num <MAX_FEATURE_NUM)
            //						Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] =Dp_Upload_Hand_List[dpid-101].feature_id;
            //						}
            //						break ;
            //
            //						default:
            //						break ;
            //
            //					}

            if (send_cmd_state == CMD_STATUS_NONE) // 主动上传，记录下featchid
            {
                NRF_LOG_INFO("upload feature num++");
                if (!feature_id_is_exist(&Update_Feature_Id_list, Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].feature_id))
                    Update_Feature_Id_list.feature_id[Update_Feature_Id_list.update_feature_num++] = Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].feature_id;
            }
            else if (current_cmd_data.cmd == STATE_QUERY_CMD) // 状态查询
            {
                NRF_LOG_INFO("current_cmd_data.cmd=%d", current_cmd_data.cmd);
                if (!feature_id_is_exist(&Response_Feature_Id_list, Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].feature_id))
                {
                    Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] = Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].feature_id;
                    NRF_LOG_INFO("dp_upload_feature id=%d", Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].feature_id);
                }
            }
            else if (current_cmd_data.cmd == DATA_QUERT_CMD) // 命令下发，设置命令
            {
                NRF_LOG_INFO("current_cmd_data.cmd == DATA_QUERT_CMD");
                if (!feature_id_is_exist(&Response_Feature_Id_list, Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].feature_id))
                    Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] = Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].feature_id;
            }

            ret = Dp_Upload_Hand_List[dpid - DPID_POWER_SWITCH].fun(value, length);
        }
    }

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
    case DPID_HEAT_SWITCH:
    case DPID_TEM_CHANGE:
    case DPID_LIGHT_SWITCH1:
    case DPID_LIGHT_SWITCH2:
    case DPID_MEDIA_SWITCH:
    case DPID_FAN_SWITCH:
    case DPID_COLOR_SWITCH:
    case DPID_COLOR_CHANGE_SWITCH:
    case DPID_HEATING_RESERVATION:
        send_dp_bool_to_mcu(c_d_desc->dpID, c_d_desc->data.byte_data);
        break;

    // value
    case DPID_TIME_SET:
    case DPID_TEMP_C_SET:
    case DPID_TIMING_SWITCH:
    case DPID_TEMP_SET:
    case DPID_INDOOR_TEM:
    case DPID_TEMP_DISPLAY:
    case DPID_HEAT1_POWER:
    case DPID_HEAT2_POWER:
    case DPID_HEAT3_POWER:
        send_dp_value_to_mcu(c_d_desc->dpID, c_d_desc->data.int_data);
        break;

    // enum
    case DPID_HEATING_DISPLAY:
        send_dp_enum_to_mcu(c_d_desc->dpID, c_d_desc->data.byte_data);
        break;

    // string
    case DPID_LIGHT_COLOR:
        send_dp_string_to_mcu(c_d_desc->dpID, (unsigned char *)c_d_desc->data.buffer, strlen((const char *)c_d_desc->data.buffer));
        break;
    }
}

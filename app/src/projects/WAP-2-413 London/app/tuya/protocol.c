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
const DOWNLOAD_CMD_S upload_cmd[] ;
#if 0
=
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
#endif

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
#if 0
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

#endif 

#if 0

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

#endif 


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
    unsigned short dpid;
    dp_upload_fun_handle fun;
    unsigned char feature_id;
} dp_upload_hand_st;

dp_upload_hand_st Dp_Upload_Hand_List[] ;


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

static unsigned char dp_upload_gpio_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
	unsigned char  ucGet_value ;
 
	switch(dp_type)
	{
		case DP_TYPE_BOOL:
		ucGet_value = bluetooth_get_dp_upload_bool(value,length);
		break ;
		
		case DP_TYPE_ENUM:
		ucGet_value = bluetooth_get_dp_upload_enum(value,length);	
		break ;
		
		default:
		break ;
	
		
	}
	
	
	switch(dpid)
	{
	  case DPID_INT_AC_POWER_SWITCH:
  	Get_Response_Pack.ac_power_switch.attribute   = ucGet_value ;
		NRF_LOG_INFO("ac_power_switch=%d",Get_Response_Pack.ac_power_switch.attribute);
		break ;
		case DPID_INT_AC_ONOFF_SWITCH:
    Get_Response_Pack.ac_onoff_switch.attribute   = ucGet_value ;
		break ;
		case DPID_SELECT_ATA:
    Get_Response_Pack.select_ata.attribute 			  = ucGet_value ;
		break ;
		case DPID_BIBS1_AIR_SWITCH:
		Get_Response_Pack.bibs1_air_switch.attribute  = ucGet_value ;
		break ;
		case DPID_BIBS1_O2_SWITCH:
    Get_Response_Pack.bibs1_o2_switch.attribute   = ucGet_value ;
		break ;
		case DPID_BIBS2_AIR_SWITCH:
    Get_Response_Pack.bibs2_air_switch.attribute  = ucGet_value ;
		break ;
		case DPID_BIBS2_O2_SWITCH:
    Get_Response_Pack.bibs2_o2_switch.attribute   = ucGet_value ;
		break ;

		case DPID_DOOR_POWER_SWITCH:
    Get_Response_Pack.door_power_switch.attribute = ucGet_value ;
		break ;

		case DPID_DOOR_DETECTION:
	  Get_Response_Pack.door_detection.attribute    = ucGet_value ;	
		 NRF_LOG_INFO("door_detection=%d", ucGet_value);
		break ;
		case DPID_VALUE_CHAMBER_SWITCH:
    Get_Response_Pack.value_chamber_switch.attribute = ucGet_value ;
		break ;
	
		
		
		
	}
	
	return ret ;
}
static unsigned char dp_upload_light_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
	unsigned char  ucGet_value ;
  unsigned int   ulGet_value ;
	switch(dp_type)
	{
		case DP_TYPE_BOOL:
		ucGet_value = bluetooth_get_dp_upload_bool(value,length);
		break ;
		
		case DP_TYPE_ENUM:
		ucGet_value = bluetooth_get_dp_upload_enum(value,length);	
		break ;
		
		case DP_TYPE_VALUE:
    ulGet_value = bluetooth_get_dp_upload_value(value,length);	
	  break ;		
		
	}
	switch(dpid)
	{
	  case DPID_TOP_LIGHT_SWITCH:
  	Get_Response_Pack.top_light_switch.attribute    = ucGet_value ;
		break ;
		case DPID_BOTTOM_LIGHT_SWITCH:
    Get_Response_Pack.bottom_light_switch.attribute = ucGet_value ;
		break ;
		case DPID_TOP_LIGHT_ADJUST:
    Get_Response_Pack.top_light_color.attribute 		= ulGet_value ;
		break ;
		case DPID_BOTTOM_LIGHT_ADJUST:
		Get_Response_Pack.bottom_light_color.attribute  = ulGet_value ;
		break ;
		
	}
	
	return ret ;
}

static unsigned char dp_upload_air_cond_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
	unsigned char  ucGet_value ;
  unsigned int   ulGet_value ;
	switch(dp_type)
	{
		case DP_TYPE_BOOL:
		ucGet_value = bluetooth_get_dp_upload_bool(value,length);
		break ;
		
		case DP_TYPE_ENUM:
		ucGet_value = bluetooth_get_dp_upload_enum(value,length);	
		break ;
		
		case DP_TYPE_VALUE:
    ulGet_value = bluetooth_get_dp_upload_value(value,length);	
	  break ;	

    default:
    break ;			
		
	}
	switch(dpid)
	{
	  case DPID_AIR_CONDITION_TEMP_SET:
  	Get_Response_Pack.air_conditioning_temp_set.attribute    = ulGet_value;
		NRF_LOG_INFO("air_conditioning_temp_set=%d", Get_Response_Pack.air_conditioning_temp_set.attribute);
		break ;
		case DPID_AIR_CONDITION_WATER_TEMP:
    Get_Response_Pack.air_conditioning_water_temp.attribute   = ulGet_value ;
		NRF_LOG_INFO("air_conditioning_water_temp=%d", Get_Response_Pack.air_conditioning_water_temp.attribute);
		break ;
		case DPID_AIR_CONDITION_WARING_CODE:
		Get_Response_Pack.air_conditioning_waring_code.attribute   = ulGet_value ;	
		NRF_LOG_INFO("air_conditioning_waring_code=%d", Get_Response_Pack.air_conditioning_waring_code.attribute);
		break ;
		case DPID_AIR_CONDITION_REMOET_SWITCH:
    Get_Response_Pack.air_conditioning_remote_switch.attribute 	= ucGet_value  ;
		NRF_LOG_INFO("air_conditioning_remote_switche=%d", Get_Response_Pack.air_conditioning_remote_switch.attribute);
		break ;
		
		default:
		break ;
		
	}
	
	return ret ;
	
}
static unsigned char dp_upload_fcv_exit_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
  unsigned int   ulGet_value ;
	switch(dp_type)
	{
		
		case DP_TYPE_VALUE:
    ulGet_value = bluetooth_get_dp_upload_value(value,length);	
	  break ;		
		
		default:
		break ;
		
	}
	switch(dpid)
	{
	  case DPID_FCV_EXIT_SET_VALUE:
  	Get_Response_Pack.fcv_exit_set_value.attribute    = ulGet_value ;
		NRF_LOG_INFO("fcv_exit_set_value=%d", Get_Response_Pack.fcv_exit_set_value.attribute);
		break ;
		
	  default:
		break ;
		
	}
	
	return ret ;
}
static unsigned char dp_upload_fcv_entry_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
  unsigned int   ulGet_value ;
	switch(dp_type)
	{
		
		case DP_TYPE_VALUE:
    ulGet_value = bluetooth_get_dp_upload_value(value,length);	
	  break ;		
		default:
		break ;
		
	}
	switch(dpid)
	{
	  case DPID_FCV_ENTRY_SET_VALUE:
  	Get_Response_Pack.fcv_entry_set_value.attribute    = ulGet_value ;
		NRF_LOG_INFO("fcv_entry_set_value =%d", Get_Response_Pack.fcv_entry_set_value.attribute);
		break ;
	  default:
		break ;
		
	}
	
	return ret ;
	
}
static unsigned char dp_upload_air_compressor_1_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
	unsigned char  ucGet_value ;
	uint8_t        remote_cmd_value=0;
  
	switch(dp_type)
	{
		case DP_TYPE_ENUM:
		ucGet_value = bluetooth_get_dp_upload_enum(value,length);	
		break ;
				
	}
	switch(dpid)
	{
	  case DPID_ACP1_STATE_1:
  	Get_Response_Pack.acp1_state_1.attribute    	= ucGet_value ;
		NRF_LOG_INFO("acp1_state_1 =%d", Get_Response_Pack.acp1_state_1.attribute);
		break ;
		case DPID_ACP1_STATE_2:
    Get_Response_Pack.acp1_state_2.attribute 			= ucGet_value ;
		NRF_LOG_INFO("acp1_state_2 =%d", Get_Response_Pack.acp1_state_2.attribute);
		break ;
		case DPID_ACP1_REMOTE_CMD:
		switch(ucGet_value)
		{
			case 1: //启动
			remote_cmd_value =1 ;	
			break ;
			
			case 2: //停机
			remote_cmd_value =0 ;		
			break ;
			
			default:
			remote_cmd_value =0 ;		
			break ;
			
		}
    Get_Response_Pack.acp1_remote_cmd.attribute 	= remote_cmd_value ;
		NRF_LOG_INFO("acp1_remote_cmd =%d", Get_Response_Pack.acp1_remote_cmd.attribute);
		break ;
		
		case DPID_ACP1_WARING_CODE:
		Get_Response_Pack.acp1_waring_code.attribute  = ucGet_value ;
		NRF_LOG_INFO("acp1_waring_code =%d", Get_Response_Pack.acp1_waring_code.attribute);
		break ;
		
		case DPID_ACP1_RUN_STATE:
		Get_Response_Pack.acp1_run_state.attribute    = ucGet_value ;	
		NRF_LOG_INFO("acp1_run_state =%d", Get_Response_Pack.acp1_run_state.attribute);
		break;
		
		case DPID_ACP1_FAULT_CODE:
		Get_Response_Pack.acp1_fault_code.attribute   = ucGet_value ;	
		NRF_LOG_INFO("acp1_fault_code =%d", Get_Response_Pack.acp1_fault_code.attribute);
		break ;
		
		default:
		break ;
		
	}
	
	return ret ;
}
static unsigned char dp_upload_air_compressor_2_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
	unsigned char  ucGet_value ;
  uint8_t        remote_cmd_value=0;
	switch(dp_type)
	{
		case DP_TYPE_ENUM:
		ucGet_value = bluetooth_get_dp_upload_enum(value,length);	
		break ;
				
	}
	switch(dpid)
	{
	  case DPID_ACP2_STATE_1:
  	Get_Response_Pack.acp2_state_1.attribute    	= ucGet_value ;
		NRF_LOG_INFO("acp2_state_1 =%d", Get_Response_Pack.acp2_state_1.attribute);
		
		break ;
		case DPID_ACP2_STATE_2:
    Get_Response_Pack.acp2_state_2.attribute 			= ucGet_value ;
		NRF_LOG_INFO("acp2_state_2 =%d", Get_Response_Pack.acp2_state_2.attribute);
		break ;
		case DPID_ACP2_REMOTE_CMD:
   switch(ucGet_value)
		{
			case 1: //启动
			remote_cmd_value =1 ;	
			break ;
			
			case 2: //停机
			remote_cmd_value =0 ;		
			break ;
			
			default:
			remote_cmd_value =0 ;		
			break ;
			
		}
    Get_Response_Pack.acp2_remote_cmd.attribute 	= remote_cmd_value ;
		NRF_LOG_INFO("acp2_remote_cmd =%d", Get_Response_Pack.acp2_remote_cmd.attribute);
		break ;
		
		case DPID_ACP2_WARING_CODE:
		Get_Response_Pack.acp2_waring_code.attribute  = ucGet_value ;
		NRF_LOG_INFO("acp2_waring_code =%d", Get_Response_Pack.acp2_waring_code.attribute);
		break ;
		
		case DPID_ACP2_RUN_STATE:
		Get_Response_Pack.acp2_run_state.attribute    = ucGet_value ;	
		NRF_LOG_INFO("acp2_run_state =%d", Get_Response_Pack.acp2_run_state.attribute);
		break;
		
		case DPID_ACP2_FAULT_CODE:
		Get_Response_Pack.acp2_fault_code.attribute   = ucGet_value ;	
		NRF_LOG_INFO("acp2_fault_code =%d", Get_Response_Pack.acp2_fault_code.attribute);
		break ;
		
		default:
		break ;
		
	}
	
	return ret ;
}


	

unsigned char dp_upload_oxygen_generator_1_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
	unsigned char  ucGet_value ;
	unsigned int   ulGet_value ;
 
	switch(dp_type)
	{
		case DP_TYPE_BOOL:
		ucGet_value = bluetooth_get_dp_upload_bool(value,length);
		break ;
		
		case DP_TYPE_ENUM:
		ucGet_value = bluetooth_get_dp_upload_enum(value,length);	
		break ;
		
		case DP_TYPE_VALUE:
    ulGet_value = bluetooth_get_dp_upload_value(value,length);	
	  
		default:
		break ;
		
		
	}
	
	
	
	switch(dpid)
	{
	  case DPID_O2_GENERATOR_1_RUN_STATE:
  	Get_Response_Pack.o2_generator_1_run_state.attribute    = ucGet_value ;
		NRF_LOG_INFO("o2_generator_1_run_state =%d", Get_Response_Pack.o2_generator_1_run_state.attribute);
		break ;
		case DPID_O2_GENERATOR_1_ALARM_STATE:
    Get_Response_Pack.o2_generator_1_alarm_state.attribute = ucGet_value ;
		NRF_LOG_INFO("o2_generator_1_alarm_state =%d", Get_Response_Pack.o2_generator_1_alarm_state.attribute );
		break ;
		case DPID_O2_GENERATOR_1_ACP_RUN_STATE:
		Get_Response_Pack.o2_generator_1_acp_run_state.attribute    = ucGet_value ;
		NRF_LOG_INFO("o2_generator_1_acp_run_state =%d", Get_Response_Pack.o2_generator_1_acp_run_state.attribute);
		break ;
		
		case DPID_O2_GENERATOR_1_DRAIN_VALUE_STATE:
		Get_Response_Pack.o2_generator_1_drain_value_state.attribute    = ucGet_value ;
		NRF_LOG_INFO("o2_generator_1_drain_value_state =%d", Get_Response_Pack.o2_generator_1_drain_value_state.attribute);
		break ;
		
		case DPID_02_GENERATOR_1_RUN_PRESSURE:
		Get_Response_Pack.o2_generator_1_pressure.attribute    = ulGet_value ;	
		NRF_LOG_INFO("o2_generator_1_pressure =%d", Get_Response_Pack.o2_generator_1_pressure.attribute);
		break ;
		
		case DPID_02_GENERATOR_1_OXYGEN_CONCENTRATION:
		Get_Response_Pack.o2_generator_1_concentration.attribute  = ulGet_value ;	
		NRF_LOG_INFO("o2_generator_1_concentration =%d", Get_Response_Pack.o2_generator_1_concentration.attribute);
		break ;
		
		case DPID_02_GENERATOR_1_TEMPERATURE:
		Get_Response_Pack.o2_generator_1_temperature.attribute    = ulGet_value ;	
		NRF_LOG_INFO("o2_generator_1_temperature =%d", Get_Response_Pack.o2_generator_1_temperature.attribute);
		break ;
		
		default:
		break ;
		
	}
	
	return ret ;
}
unsigned char dp_upload_oxygen_generator_2_handle(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
	unsigned char  ret=SUCCESS; 
	unsigned char  ucGet_value ;
  unsigned int   ulGet_value ;
	switch(dp_type)
	{
		case DP_TYPE_BOOL:
		ucGet_value = bluetooth_get_dp_upload_bool(value,length);
		break ;
		
		case DP_TYPE_ENUM:
		ucGet_value = bluetooth_get_dp_upload_enum(value,length);	
		break ;
		
    case DP_TYPE_VALUE:
    ulGet_value = bluetooth_get_dp_upload_value(value,length);			
	  
		default:
		break ;
		
		
	}
	switch(dpid)
	{
	  case DPID_O2_GENERATOR_2_RUN_STATE:
  	  Get_Response_Pack.o2_generator_2_run_state.attribute    = ucGet_value ;
			NRF_LOG_INFO("o2_generator_2_run_state =%d", Get_Response_Pack.o2_generator_2_run_state.attribute);
		break ;
		case DPID_O2_GENERATOR_2_ALARM_STATE:
      Get_Response_Pack.o2_generator_2_alarm_state.attribute = ucGet_value ;
	  	NRF_LOG_INFO("o2_generator_2_alarm_state =%d", Get_Response_Pack.o2_generator_2_alarm_state.attribute );
		break ;
		case DPID_O2_GENERATOR_2_ACP_RUN_STATE:
		Get_Response_Pack.o2_generator_2_acp_run_state.attribute    = ucGet_value ;
		NRF_LOG_INFO("o2_generator_2_acp_run_state =%d", Get_Response_Pack.o2_generator_2_acp_run_state.attribute);
		break ;
		
		case DPID_O2_GENERATOR_2_DRAIN_VALUE_STATE:
		Get_Response_Pack.o2_generator_2_drain_value_state.attribute    = ucGet_value ;
		NRF_LOG_INFO("o2_generator_2_drain_value_state =%d", Get_Response_Pack.o2_generator_2_drain_value_state.attribute);
		break ;
		
		case DPID_02_GENERATOR_2_RUN_PRESSURE:
		Get_Response_Pack.o2_generator_2_pressure.attribute    = ulGet_value ;	
		NRF_LOG_INFO("o2_generator_2_pressure =%d", Get_Response_Pack.o2_generator_2_pressure.attribute);
		break ;
		
		case DPID_02_GENERATOR_2_OXYGEN_CONCENTRATION:
		Get_Response_Pack.o2_generator_2_concentration.attribute  = ulGet_value ;	
		NRF_LOG_INFO("o2_generator_2_concentration =%d", Get_Response_Pack.o2_generator_2_concentration.attribute);
		break ;
		
		case DPID_02_GENERATOR_2_TEMPERATURE:
		Get_Response_Pack.o2_generator_2_temperature.attribute    = ulGet_value ;	
		NRF_LOG_INFO("o2_generator_2_temperature =%d", Get_Response_Pack.o2_generator_2_temperature.attribute);
		break ;
		
		default:
		break ;
		
	}
	
	return ret ;
	
}

unsigned char dp_upload_handle_have_cb(unsigned short dp_id, unsigned char dp_type,const unsigned char value[], unsigned short length)
{
    /*********************************
    当前函数处理可下发/可上报数据调用
    具体函数内需要实现下发数据处理
    完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
    ***********************************/
	  unsigned char ret = ERROR;

		  if(dp_id>=1 && dp_id<=10)
    {
			   ret = dp_upload_gpio_handle(dp_id,dp_type,value,length);
    }else if(dp_id>=20 && dp_id<= 23)
		{
			
			   ret =dp_upload_light_handle(dp_id,dp_type,value,length);
		}
		
    else if( dp_id>=40 && dp_id<= 59) //空调
    {
			 ret = 	dp_upload_air_cond_handle(dp_id,dp_type,value,length); //
	
    }
		
		
    else if(dp_id>=70 && dp_id<= 86 )		//流量阀EXIT
		{
		
			 ret = 	dp_upload_fcv_exit_handle(dp_id,dp_type,value,length); //
			
		}
	  else if(dp_id>=100 && dp_id<= 116 )		//流量阀ENTRY
		{
	
			ret = 	dp_upload_fcv_entry_handle(dp_id,dp_type,value,length); //
			
		}
		
		
	  else if(dp_id>=130 && dp_id<= 242 )		//空压机1
		{
				 ret = 	dp_upload_air_compressor_1_handle(dp_id,dp_type,value,length); //	
				 
  	}
			
	
		else if(dp_id >=330 && dp_id <= 442 )		//空压机2
		{
		
				 ret = 	dp_upload_air_compressor_2_handle(dp_id,dp_type,value,length); //	
			
		}
		else if(dp_id >=460 && dp_id <= 469 )		//制氧机1
		{
			 ret = 	dp_upload_oxygen_generator_1_handle(dp_id,dp_type,value,length); //
			
		}
		else if(dp_id >=480 && dp_id <= 489 )		//制氧机1
		{
			 ret = 	dp_upload_oxygen_generator_2_handle(dp_id,dp_type,value,length); //
				 
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
#if 0
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
#endif 
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
   // return (sizeof(upload_cmd) / sizeof(upload_cmd[0]));
	return 0;
}


void send_dpID_cmd(volatile cmd_data_description *c_d_desc)
{
	 uint8_t acp_cmd_value ;
    switch (c_d_desc->dpID)
    {
     // bool  ;
			case DPID_INT_AC_POWER_SWITCH:
			case DPID_INT_AC_ONOFF_SWITCH:
			case DPID_BIBS1_AIR_SWITCH:
			case DPID_BIBS1_O2_SWITCH:
			case DPID_BIBS2_AIR_SWITCH:
			case DPID_BIBS2_O2_SWITCH:
			case DPID_DOOR_POWER_SWITCH:
			case DPID_VALUE_CHAMBER_SWITCH:
			case DPID_TOP_LIGHT_SWITCH:
			case DPID_BOTTOM_LIGHT_SWITCH:
			case DPID_AIR_CONDITION_REMOET_SWITCH:
			case DPID_O2_GENERATOR_1_RUN_STATE:
			case DPID_O2_GENERATOR_2_RUN_STATE:
        send_dp_bool_to_mcu(c_d_desc->dpID, c_d_desc->data.byte_data);
        break;
      case DPID_ACP1_REMOTE_CMD:
			case DPID_ACP2_REMOTE_CMD:
				 switch(c_d_desc->data.byte_data)
				 {
					 case 0:
						 acp_cmd_value = 2 ; //停机
						 break ;
					 case 1:
						 acp_cmd_value = 1 ;  //启动
						 break ;
					 default:
						break ;
         }					 
				send_dp_enum_to_mcu(c_d_desc->dpID,acp_cmd_value);
			  break ;
    // value
			case DPID_TOP_LIGHT_ADJUST:
			case DPID_BOTTOM_LIGHT_ADJUST:
      case DPID_AIR_CONDITION_TEMP_SET:
			case DPID_FCV_EXIT_SET_VALUE:
			case DPID_FCV_ENTRY_SET_VALUE:
        send_dp_value_to_mcu(c_d_desc->dpID, c_d_desc->data.int_data);
        break;

    // enum
    case DPID_SELECT_ATA:
        send_dp_enum_to_mcu(c_d_desc->dpID, c_d_desc->data.byte_data);
        break;

    }
	
		
		
}

/* Copyright (C)
   All rights reserved. */

#ifndef UART_CMD_H__
#define UART_CMD_H__

#include "controller_config.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "app_cfg_cmd.h"
#include <stdint.h>
#include "mt_cst_protocol.h"

enum
{
  SET_AIR_CONDITI_STATE = 1,
  WAIT_SET_AIR_CONDITI_STATE,

  SET_AIR_CONDITI_TARGET_TEMP, // 空调目标温度
  WAIT_SET_AIR_CONDITI_TARGET_TEMP,

	WAIT_Air_Compressor_CTRL,
	WAIT_Oxygen_Concentrator_CTRL,
	
  READ_AIR_CONDITI_STATE, // air_conditioner
  WAIT_READ_AIR_CONDITI_STATE,

  READ_AIR_CONDITI_TEMP, // air_conditioner
  WAIT_READ_AIR_CONDITI_TEMP,

  READ_AIR_CONDITI_Measured_TEMP, // 读取空调测量温度
  WAIT_SET_AIR_CONDITI_Measured_TEMP,

  READ_HUM_SENSOR,
  WAIT_READ_HUM_SENSOR,

  READ_ALL_SENSOR,
  WAIT_READ_ALL_SENSOR
	
	
	
};

enum
{
  SET_CMD = 0X06,
  READ_CMD = 0X03,
};

/**
 * @brief 串口接收完成处理函数
 *
 * @param[in] p_data 指向接收的数据.
 * @param[in] len    接收的数据长度.
 */
void uart_receive_handler(uint8_t *p_data, uint16_t len);

void uart_switch_cmd_state(uint8_t set_valu);
void uart_revice_feedback_handle(uint8_t *p_data, uint8_t len);
void task_uart_send_cmd_handle(void);
void set_uart_wirte_sate(uint8_t set_state);
uint8_t get_uart_write_state(void) ;

extern Feature_Id_List* pSet_Feature_Id_list  ;
extern Set_Request_Data_Pack_t* pSettingData  ;


#endif

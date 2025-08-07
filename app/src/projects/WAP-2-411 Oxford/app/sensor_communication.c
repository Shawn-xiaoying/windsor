/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"

#include <string.h>
#include "mt_serial.h"
#include "app_cfg_cmd.h"

#include "HBOT_uart_cmd.h"
#define NRF_LOG_MODULE_NAME sensor_communication_modul
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

extern uint8_t uart_send_buf[50];
extern uint8_t m_cur_read_cmd;

/****************************************** 传感器板子 *********************************************************************
 * uart 读取传感器湿度
 * 主机-相当于蓝牙板子
 * 81 03 00 30 00 01 46 00
 * read_mode = 0x01 ==》对应单独湿度传感器数据
 * read_mode = 0x0a=====》对应10个传感器数据
 *************************************/
bool uart_send_read_sensor_type(uint8_t read_mode)
{
  memset(uart_send_buf, 0, sizeof(uart_send_buf));

  uint8_t len = 0;
  uint16_t temp_crc = 0;

  uart_send_buf[len++] = 0x81;
  uart_send_buf[len++] = 0x03;

  uart_send_buf[len++] = 0x00;
  uart_send_buf[len++] = 0x30;

  // set on off cmd
  uart_send_buf[len++] = 0x00;
  uart_send_buf[len++] = read_mode;

  temp_crc = CalcCrc16(uart_send_buf, len);

  uart_send_buf[len++] = temp_crc;
  uart_send_buf[len++] = temp_crc >> 8;

  mt_serial_put(uart_send_buf, len);

 // NRF_LOG_INFO("[ UART SNED ] read_sensor_type: read_mode =%d", read_mode);
 // NRF_LOG_HEXDUMP_INFO(uart_send_buf, len);
  return true;
}

/*************************************************
 * uart 得到全部传感器数据十个传感器数据
 * 得到全部传感器数据赋值
 ****************************/
void update_adv_data_device_state_of_temperature(unsigned char value);
void update_adv_data_device_state_of_pressure(unsigned char value);
void get_all_sensor_value(uint8_t *pdata, uint8_t len)
{
  uint16_t temp_i16 = 0;

  // 0x30湿度数据
  //    Get_Response_Pack.Humidi_sensor.is_working =  pdata[0];
  temp_i16 = pdata[1];
  temp_i16 = (temp_i16 << 8) | pdata[2];
  Get_Response_Pack.Humidi_sensor.attribute = temp_i16;

  // 0x31 温度
  temp_i16 = 0;
  //  Get_Response_Pack.temper_sensor.is_working =  pdata[3];
  temp_i16 = pdata[4];
  temp_i16 = (temp_i16 << 8) | pdata[5];
  Get_Response_Pack.temper_sensor.attribute = temp_i16;
  update_adv_data_device_state_of_temperature(temp_i16);

  // 0x32二氧化碳数据
  temp_i16 = 0;
  //   Get_Response_Pack.CO2_sensor.is_working =  pdata[6];
  temp_i16 = pdata[7];
  temp_i16 = (temp_i16 << 8) | pdata[8];
  Get_Response_Pack.CO2_sensor.attribute = temp_i16;

  // 0x33噪声数据
  temp_i16 = 0;
  // Get_Response_Pack.noise_sensor.is_working =  pdata[9];
  temp_i16 = pdata[10];
  temp_i16 = (temp_i16 << 8) | pdata[11];
  Get_Response_Pack.noise_sensor.attribute = temp_i16;

  // 0x34 TVOC数据
  temp_i16 = 0;
  //  Get_Response_Pack.TVOC_sensor.is_working =  pdata[12];
  temp_i16 = pdata[13];
  temp_i16 = (temp_i16 << 8) | pdata[14];
  Get_Response_Pack.TVOC_sensor.attribute = temp_i16;

  // 0x35 灰尘数据
  temp_i16 = 0;
  //  Get_Response_Pack.PM_Sensor.is_working =  pdata[15];
  temp_i16 = pdata[16];
  temp_i16 = (temp_i16 << 8) | pdata[17];
  Get_Response_Pack.PM_Sensor.attribute1 = temp_i16;

  temp_i16 = pdata[18];
  temp_i16 = (temp_i16 << 8) | pdata[19];
  Get_Response_Pack.PM_Sensor.attribute2 = temp_i16;

  temp_i16 = pdata[20];
  temp_i16 = (temp_i16 << 8) | pdata[21];
  Get_Response_Pack.PM_Sensor.attribute3 = temp_i16;

  // 0x36 气压数据
  temp_i16 = 0;
  //  Get_Response_Pack.airpress_sensor.is_working =  pdata[22];
  temp_i16 = pdata[23];
  temp_i16 = (temp_i16 << 8) | pdata[24];
  Get_Response_Pack.airpress_sensor.attribute = temp_i16;
  update_adv_data_device_state_of_pressure(temp_i16);

  // 0x37 氧气浓度数据
  temp_i16 = 0;
  //  Get_Response_Pack.O2_sensor.is_working =  pdata[25];
  temp_i16 = pdata[26];
  temp_i16 = (temp_i16 << 8) | pdata[27];
  Get_Response_Pack.O2_sensor.attribute = temp_i16;

  // 0x38 臭氧数据
  temp_i16 = 0;
  //   Get_Response_Pack.O3_sensor.is_working =  pdata[28];
  temp_i16 = pdata[29];
  temp_i16 = (temp_i16 << 8) | pdata[30];
  Get_Response_Pack.O3_sensor.attribute = temp_i16;

  // 0x39 甲醛数据
  temp_i16 = 0;
  //   Get_Response_Pack.HCHO_sensor.is_working =  pdata[31];
  temp_i16 = pdata[32];
  temp_i16 = (temp_i16 << 8) | pdata[33];
  Get_Response_Pack.HCHO_sensor.attribute = temp_i16;
}
/*********************************************************区分命令
 * uart 读取传感器数据
 * 0x01 ==》对应单独湿度传感器数据
 * 0x0a=====》对应10个传感器数据
 **************************************/
void uart_rev_sensor_data_handler(uint8_t *pdata, uint8_t len)
{
  uint16_t rev_len = 0, temp_i16 = 0;

  /*data len*/
  rev_len = pdata[2];

  /*单独读取湿度，由于地址0x01*/
  if (rev_len == 0x03) // 81 03 03 01 00 40 64 46
  {
    //   Get_Response_Pack.Humidi_sensor.is_working =  pdata[3];

    temp_i16 = pdata[4];
    temp_i16 = (temp_i16 << 8) | pdata[5];

    Get_Response_Pack.Humidi_sensor.attribute = temp_i16;

  //  NRF_LOG_INFO("[UART REVICE] humidi sensor_data_handler:");
  //  NRF_LOG_HEXDUMP_INFO(pdata, len);

    return;
  }
  /*读取10个传感器数据*/
  if (rev_len == 0x22) // 81 03  22   01 00 40   01 00 40   01 00 40 01 00 40 01 00 40 01 00 40 00 40 00 40 01 00 40 01 00 40 01 00 40 01 00 40 4E DD
  {
    get_all_sensor_value(pdata + 3, rev_len);

 //   NRF_LOG_INFO("[UART REVICE]  all sensor_data_handler:");
 //   NRF_LOG_HEXDUMP_INFO(pdata, len);
  }
}

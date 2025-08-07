#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"

#include <string.h>

#include "mt_serial.h"
#include "app_cfg_cmd.h"

#include "HBOT_uart_cmd.h"
#define NRF_LOG_MODULE_NAME air_conditione_communication_modul
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

extern uint8_t uart_send_buf[50];
extern uint8_t m_cur_read_cmd;
/*********************************************** �յ�********************************************************
 * �յ�����״̬
 *
 * ��:18 06 00 0C 00 01 8A 00
 *****************************/

bool uart_send_set_air_conditioner_OnOff(bool set_cmd)
{
    memset(uart_send_buf, 0, sizeof(uart_send_buf));

    uint8_t len = 0;
    uint16_t temp_crc = 0;

    uart_send_buf[len++] = 0x18;
    uart_send_buf[len++] = 0x06;
    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x0C;

    // set on off cmd
    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = set_cmd;

    temp_crc = CalcCrc16(uart_send_buf, len);
    uart_send_buf[len++] = temp_crc;
    uart_send_buf[len++] = temp_crc >> 8;

    mt_serial_put(uart_send_buf, len);

  //  NRF_LOG_INFO("[ UART SNED ] set_air_conditioner_OnOff: set_cmd =%d", set_cmd);
  //  NRF_LOG_HEXDUMP_INFO(uart_send_buf, len);
    return true;
}

/******************************************
 * uart ���յ�״̬
 *
 * 18 03 00 0C 00 01 46 00
 *************************************/
void uart_send_read_air_conditioner_state(void)
{
    memset(uart_send_buf, 0, sizeof(uart_send_buf));

    uint8_t len = 0;
    uint16_t temp_crc = 0;

    uart_send_buf[len++] = 0x18;
    uart_send_buf[len++] = 0x03;
    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x0C;

    // set on off cmd
    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x01;

    temp_crc = CalcCrc16(uart_send_buf, len);

    uart_send_buf[len++] = temp_crc;
    uart_send_buf[len++] = temp_crc >> 8;

  //  NRF_LOG_INFO("[ UART SNED ] read_air_conditioner_state:");
  //  NRF_LOG_HEXDUMP_INFO(uart_send_buf, len);
    mt_serial_put(uart_send_buf, len);
}

/******************************************************************
 * ���ÿյ��¶ȡ���Ŀ���¶�
 * 18 06 00 0A 00 64 AA 2A
 * ***********************/
bool uart_send_set_air_conditioner_temper(int16_t set_valu)
{
    memset(uart_send_buf, 0, sizeof(uart_send_buf));

    uint8_t len = 0;
    uint16_t temp_crc = 0;

    uart_send_buf[len++] = 0x18;
    uart_send_buf[len++] = 0x06;
    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x0A;

    // set on off cmd
    uart_send_buf[len++] = set_valu >> 8;
    uart_send_buf[len++] = set_valu;

    temp_crc = CalcCrc16(uart_send_buf, len);
    uart_send_buf[len++] = temp_crc;
    uart_send_buf[len++] = temp_crc >> 8;

    mt_serial_put(uart_send_buf, len);

  //  NRF_LOG_INFO("[ UART SNED ] uart_send_set_air_conditioner_temper: set_cmd =%d", set_valu);
   // NRF_LOG_HEXDUMP_INFO(uart_send_buf, len);
    return true;
}

/******************************************
 * uart ���յ��������¶�(Ŀ���¶�)
 *
 * 18 03 00 0A 00 01 A6 01(????)
 *************************************/
void uart_send_read_air_conditioner_temper(void)
{
    memset(uart_send_buf, 0, sizeof(uart_send_buf));

    uint8_t len = 0;
    uint16_t temp_crc = 0;

    uart_send_buf[len++] = 0x18;
    uart_send_buf[len++] = 0x03;
    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x0A;

    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x01;

    temp_crc = CalcCrc16(uart_send_buf, len);

    uart_send_buf[len++] = temp_crc;
    uart_send_buf[len++] = temp_crc >> 8;

  //  NRF_LOG_INFO("[ UART SNED ] uart_send_read_air_conditioner_temper:");
  //  NRF_LOG_HEXDUMP_INFO(uart_send_buf, len);
    mt_serial_put(uart_send_buf, len);
}

/******************************************
 * uart ��ȡ�յ������¶�
 *
 * 18 03 00 05 00 01 96 02----�̶�
 *************************************/
void uart_send_read_air_conditioner_Measured_temper(void)
{
    memset(uart_send_buf, 0, sizeof(uart_send_buf));

    uint8_t len = 0;
    uint16_t temp_crc = 0;

    uart_send_buf[len++] = 0x18;
    uart_send_buf[len++] = 0x03;
    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x05;

    uart_send_buf[len++] = 0x00;
    uart_send_buf[len++] = 0x01;

    temp_crc = CalcCrc16(uart_send_buf, len);

    uart_send_buf[len++] = temp_crc;
    uart_send_buf[len++] = temp_crc >> 8;

 //   NRF_LOG_INFO("[ UART SNED ] uart_send_read_air_conditioner_Measured_temper=%d", Get_Response_Pack.AirConditi_MeasuredTemper.attribute);
 //   NRF_LOG_HEXDUMP_INFO(uart_send_buf, len);
    mt_serial_put(uart_send_buf, len);
}

/************************************************************************************************************************
 * uart ���տյ��ķ�������-----�ظ�����
 *
 *************************************/
bool air_conditioner_cmd_hanlder(uint8_t *pdata, uint8_t len)
{
    uint16_t data_addr = 0, addr_num = 0;
    uint8_t cmd = 0;

    cmd = pdata[1];
    data_addr = pdata[2]; /*���ݵ�ַ*/
    data_addr = (data_addr << 8) | pdata[3];

 //   NRF_LOG_INFO("[UART REVICE] air_conditioner rev cmd data:");
 //   NRF_LOG_HEXDUMP_INFO(pdata, len);

    switch (cmd)
    {
    case 0x03:
        if (m_cur_read_cmd == READ_AIR_CONDITI_STATE) // ��ȡ���� --18 03 02  00 01  (crc)64 46
        {
            addr_num = pdata[3]; /*��ַ���������ؿյ�״̬ */
            addr_num = (addr_num << 8) | pdata[4];

            Get_Response_Pack.Air_Conditi.attribute = addr_num;
        }
        else if (m_cur_read_cmd == READ_AIR_CONDITI_TEMP) // ��ȡ�յ�Ŀ���¶� 18 03  02 00 64  A4 6D
        {
            /* code */
            addr_num = pdata[3]; /*��ַ���������ؿյ�״̬*/
            addr_num = (addr_num << 8) | pdata[4];

          //  Get_Response_Pack.AirConditi_Target_Temper.attribute = addr_num / 10;
					 Get_Response_Pack.AirConditi_Target_Temper.attribute = addr_num / 100;
        }
        else if (m_cur_read_cmd == READ_AIR_CONDITI_Measured_TEMP) // ��ȡ�յ������¶� 18 03 02 0B A4 A3 0D
        {
            /* code */
            addr_num = pdata[3]; /*��ַ���������ؿյ�״̬*/
            addr_num = (addr_num << 8) | pdata[4];

            Get_Response_Pack.AirConditi_MeasuredTemper.attribute = addr_num / 10;
        }
 //       NRF_LOG_INFO("[UART REVICE] read air_condi CMD : m_cur_read_cmd =%d Pwr_on =%d  AirConditi_Target_Temper=%d MeasuredTemper =%d\r\n", m_cur_read_cmd,
 //                    Get_Response_Pack.Air_Conditi.attribute, Get_Response_Pack.AirConditi_Target_Temper.attribute, Get_Response_Pack.AirConditi_MeasuredTemper.attribute);
        return true;

    case 0x06: // д�յ� ����ؽ��-- 18 06  00 0C  00 00  4B C0
        if (data_addr == 0x000C)
        {
            addr_num = pdata[4]; /*��ַ���������ؿյ�״̬*/
            addr_num = (addr_num << 8) | pdata[5];

            if (addr_num == Get_Response_Pack.Air_Conditi.attribute) /*app���ú�Ͷ�ȡ����ֵ���*/
            {
                return true;
            }
        }
        if (data_addr == 0x000A) // 18 06 00 0A 00 64 AA 2A
        {
            addr_num = pdata[4]; /*��ַ���������ؿյ�״̬*/
            addr_num = (addr_num << 8) | pdata[5];

            if (addr_num == Get_Response_Pack.AirConditi_Target_Temper.attribute)
            {
                return true;
            }
        }
   //     NRF_LOG_INFO("[UART REVICE] air_conditioner write : data_addr =0x%x Pwr_on=%d addr_num=%d",
   //                  data_addr, Get_Response_Pack.Air_Conditi.attribute, addr_num);
        break;
    }
    return false;
}

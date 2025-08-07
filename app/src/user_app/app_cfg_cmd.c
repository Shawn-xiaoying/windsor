/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "wap-1-100-windsor-master-protocol.h"
#include <string.h>
#include "app_cfg_cmd.h"
#include "mt_cst_protocol.h"
#include "mt_ble_service.h"

#define NRF_LOG_MODULE_NAME app_cfg_cmd
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

//extern uint8_t m_send_buf[WINDSOR_MAX_BUF_LENGTH]; /*传输给app数据*/
//extern uint8_t m_rev_buf[WINDSOR_MAX_BUF_LENGTH];  /*接收app数据*/

uint8_t Get_Request_id[MAX_FEATURE_NUM];
uint8_t allRequestID_Num;

uint8_t check_the_featrue_id_is_valid(uint8_t featrue_id)
{
    for (uint8_t i = 0; i < features_length_supported_by_the_device; i++)
    {
        if (featrue_id == features_supported_by_the_device[i].feature_id)
            return 0;
    }

    return 1;
}

uint8_t check_the_log_feature_id_is_valid(uint8_t featrue_id)
{
	   for (uint8_t i = 0; i < log_features_length_supported_by_the_device; i++)
    {
        if (featrue_id == log_features_supported_by_the_device[i])
            return 0;
    }

    return 1;
	
}

uint8_t check_the_feature_id_length_is_valid(uint8_t feature_id, uint8_t length)
{
    for (uint8_t i = 0; i < features_length_supported_by_the_device; i++)
    {
        if (feature_id == features_supported_by_the_device[i].feature_id && length == features_supported_by_the_device[i].value_length)
        {
            return 0;
        }
    }

    return 1;
}

/*
ret value:
  -1  : data data pakcet has error
  other value： data packet length
检查app BLE 数据包校验码

*/
int16_t check_the_data_packet(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *m_rev_buf, uint8_t len)
{
    uint16_t temp_crc = 0, rev_data_crc = 0, temp_id_len = 0;

  //  memcpy(m_rev_buf, p_data, len);
    header_packet_t *head_pack = (header_packet_t *)m_rev_buf;

    temp_crc = CalcCrc16(m_rev_buf, len - 2);

    /*revice data crc 大端表示法 */
    rev_data_crc = m_rev_buf[len - 2];                       // 高位在前，也就是在内存的低地址处
    rev_data_crc = (rev_data_crc << 8) | m_rev_buf[len - 1]; // 低位在后，也就是在内存的高地址处

    /*CRC 检验*/
    if (rev_data_crc != temp_crc)
    {
        return -1;
    }
    
		

    /*系统运行时间*/
    device_infor.m_system_clock = (uint32_t)((head_pack->TimeStamp[0] << 24) |
                                             (head_pack->TimeStamp[1] << 16) |
                                             (head_pack->TimeStamp[2] << 8) |
                                             head_pack->TimeStamp[3]);

    temp_id_len = head_pack->Payload_Size;
    temp_id_len = (temp_id_len << 8) | head_pack->Payload_Size >> 8;

    return temp_id_len;
}

void set_response_packet_head(uint8_t *sned_buf, uint8_t cmd, uint16_t payload_size,uint8_t msg_id)
{
    response_header_packet_t *head_pack = (response_header_packet_t *)sned_buf;
    head_pack->Head = 0x5A;
    head_pack->cmd = cmd;
    head_pack->Attr_MsgId = msg_id;
    // device id
    memcpy(head_pack->ChipID, device_infor.ChipID, 8);

    // syetem  m_system_clock
    head_pack->TimeStamp[0] = device_infor.m_system_clock >> 24;
    head_pack->TimeStamp[1] = device_infor.m_system_clock >> 16;
    head_pack->TimeStamp[2] = device_infor.m_system_clock >> 8;
    head_pack->TimeStamp[3] = device_infor.m_system_clock;

    head_pack->Payload_Size[0] = payload_size >> 8;
    head_pack->Payload_Size[1] = payload_size;
}

void send_unauthorized_response_packet(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t cmd)
{
	  uint8_t len = 0;
	  set_response_packet_head(bluetooth_connection_status_task->send_buf,cmd, 0,bluetooth_connection_status_task->rv_MsgID);
	  len = sizeof(response_header_packet_t);
		uint16_t temp_crc = CalcCrc16( bluetooth_connection_status_task->send_buf, len); // crc
    bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
    bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
    NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	  bluetooth_connection_status_task->send_buf_len = len ;
    ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);

}

static void app_get_features_supported_by_the_device_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
    uint8_t len = 0;
    set_response_packet_head(bluetooth_connection_status_task->send_buf, GET_FEATURES_RESPONSE, features_length_supported_by_the_device + 1,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    bluetooth_connection_status_task->send_buf[len++] = 1; // status  success

    for (uint8_t i = 0; i < features_length_supported_by_the_device; i++)
        bluetooth_connection_status_task->send_buf[len + i] = features_supported_by_the_device[i].feature_id;
    len += features_length_supported_by_the_device;

    uint16_t temp_crc = CalcCrc16( bluetooth_connection_status_task->send_buf, len); // crc
    bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
    bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
    NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	  bluetooth_connection_status_task->send_buf_len = len ;
    ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void app_get_features_supported_by_the_device_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
    if (check_the_data_packet(bluetooth_connection_status_task,p_data, len) == 0)
    {
        app_get_features_supported_by_the_device_response(bluetooth_connection_status_task);
    }
    else
        response_ok_or_failed(bluetooth_connection_status_task, GET_FEATURES_RESPONSE, false);
}

static void app_get_log_features_supported_by_the_device_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
    uint8_t len = 0;
    set_response_packet_head(bluetooth_connection_status_task->send_buf, GET_LOG_FEATURES_RESPONSE, log_features_length_supported_by_the_device + 1,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    bluetooth_connection_status_task->send_buf[len++] = 1; // status  success

    for (uint8_t i = 0; i < log_features_length_supported_by_the_device; i++)
        bluetooth_connection_status_task->send_buf[len + i] = log_features_supported_by_the_device[i];
    len += log_features_length_supported_by_the_device;

    uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
    bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
    bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
    NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	  bluetooth_connection_status_task->send_buf_len = len ;
    ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void app_get_log_features_supported_by_the_device_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
    if (check_the_data_packet(bluetooth_connection_status_task,p_data, len) == 0)
    {
        app_get_log_features_supported_by_the_device_response(bluetooth_connection_status_task);
    }
    else
        response_ok_or_failed(bluetooth_connection_status_task, GET_LOG_FEATURES_RESPONSE, false);
}

/******************************************************
 * app_Info_Request
 * */
bool app_Info_Request(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len)
{
    int temp_id_len = 0;
    temp_id_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
    if (temp_id_len != -1)
    {
        if (temp_id_len == 0)
            return true;
        else
            return false;
    }
    else
        return false;
}

/******************************************************
 * app Get_Request 命令检查函数
 * */
int16_t app_Get_Request_cmd(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len)
{
    int temp_id_len = 0;
    temp_id_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
    if (temp_id_len != -1)
    {
        Get_Request_Pack_t *Get_pack_t = (Get_Request_Pack_t *)p_data;
        if (temp_id_len != 0)
        memcpy(Get_Request_id, Get_pack_t->Feature_ID, temp_id_len);

        return temp_id_len;
    }
    else
        return -1;
}

void response_ok_or_failed(bluetooth_connection_status_task_t *bluetooth_connection_status_task, unsigned char ack_cmd, bool set_value)
{
   // uint8_t buf[100];
    uint8_t len = 0;

    bluetooth_connection_status_task->send_buf[len++] = 0x5A;
    bluetooth_connection_status_task->send_buf[len++] = ack_cmd;
    bluetooth_connection_status_task->send_buf[len++] = bluetooth_connection_status_task->rv_MsgID;

    // device id
    memcpy(bluetooth_connection_status_task->send_buf + len, device_infor.ChipID, 8);
    len += 8;
    // syetem  m_system_clock
    bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 24;
    bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 16;
    bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock >> 8;
    bluetooth_connection_status_task->send_buf[len++] = device_infor.m_system_clock;

    // Payload Size
    bluetooth_connection_status_task->send_buf[len++] = 0; //
    bluetooth_connection_status_task->send_buf[len++] = 0x01;

    if (set_value == true)
        bluetooth_connection_status_task->send_buf[len++] = 0x01;
    else
        bluetooth_connection_status_task->send_buf[len++] = 0x00;

    uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
    bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
    bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //

    NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
		bluetooth_connection_status_task->send_buf_len = len;
    ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

/**************************************************************
 * 设备设置回应
 * Set Response Payload Structure (Requesting all features)
 * 需要等待uart回应后，才返回给app，超时时间为2s,由于空调设置时外接板子
 *************************************/
void device_Set_Response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool set_valu)
{
    response_ok_or_failed(bluetooth_connection_status_task, 0x53, set_valu);
}

void set_log_parames_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool set_valu)
{
    response_ok_or_failed(bluetooth_connection_status_task, 0x57, set_valu);
}

void start_stop_logging_record_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool set_value)
{

    response_ok_or_failed(bluetooth_connection_status_task, 0x5b, set_value);
}

void get_logging_record_response_failed(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{

    response_ok_or_failed(bluetooth_connection_status_task, 0x59, false);
}

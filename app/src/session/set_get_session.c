#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "app_cfg_cmd.h"
#include "mt_ble_service.h"
#include "ble_ctcs.h"
#include "set_get_session.h"
#include "session.h"
#include "session_parameters_check.h"

#define NRF_LOG_MODULE_NAME set_get_session_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define SET_GET_SESSION_DEBUG_OUT

static void set_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool success_flg, uint8_t reason)
{

  uint8_t len = 0;
  if (success_flg == true)
  {
    set_response_packet_head(bluetooth_connection_status_task->send_buf, SET_SESSION_RESPONSE, 1,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    bluetooth_connection_status_task->send_buf[len++] = 0x01;
  }
  else
  {
    set_response_packet_head(bluetooth_connection_status_task->send_buf, SET_SESSION_RESPONSE, 2,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    bluetooth_connection_status_task->send_buf[len++] = 0x00;
    bluetooth_connection_status_task->send_buf[len++] = reason;
  }
  uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
  bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
  bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
  NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	bluetooth_connection_status_task->send_buf_len =len ;
  ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void set_session_requeset(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
  uint8_t error_code;
  int16_t payload_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
  if (payload_len == -1)
  {
    set_session_response(bluetooth_connection_status_task, false, SET_SESSION_PACKET_ERROR);
    return;
  }
  else
  {
    session_config_parames_total_length = payload_len;
    memcpy(session_parames_buf, &p_data[sizeof(header_packet_t)], payload_len);
    /*add parames data check*/

    if (session_parames_check(&error_code) == 1)
    {
      set_session_response(bluetooth_connection_status_task, false, error_code);
      return;
    }

    set_session_response(bluetooth_connection_status_task, true, 0);
    session_status = SESSION_CONFIGURED;
    configre_save_session_logging_parames();
    // session_config_parames_total_length = len-sizeof(header_packet_t)-2) ;
  }
}
static void get_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool success_flg)
{
  uint8_t len = 0;
  if (success_flg == true)
  {
    set_response_packet_head(bluetooth_connection_status_task->send_buf, GET_SESSION_RESPONSE, session_config_parames_total_length,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    memcpy(&bluetooth_connection_status_task->send_buf[len], session_parames_buf, session_config_parames_total_length);
    len += session_config_parames_total_length;
  }
  else
  {
    set_response_packet_head(bluetooth_connection_status_task->send_buf, GET_SESSION_RESPONSE, 1,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    bluetooth_connection_status_task->send_buf[len++] = 0x00;
  }
  uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
  bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
  bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
  NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	bluetooth_connection_status_task->send_buf_len =len ;
  ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void get_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
  int16_t payload_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
  if (payload_len == -1)
    get_session_response(bluetooth_connection_status_task, false);
  else
  {
    get_session_response(bluetooth_connection_status_task, true);
  }
}

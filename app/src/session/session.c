#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "app_cfg_cmd.h"
#include "app_task.h"
#include "mt_cst_protocol.h"
#include "logging_rw_implent.h"
#include "session.h"
#include "session_log.h"
#include "preparation_stage.h"
#include "session_run_stage.h"
#include "mt_ble_service.h"
#include "session_sharing_cmd.h"
#include "mt_ble_adv.h"
#include "session_sharing_user_list.h"

#define NRF_LOG_MODULE_NAME session_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();
/*
                                               0x00, 0x00, 0x30, 0x39, \
                                             //  preparationDuration
                                               0x00, 0x1E,					   \
                                            //  runningDuration
                                               0x00, 0x3C,             \
                                               0x00, 0x02,             \
                                               0x02,                   \
                                               0x31, 0x21,             \
                                               0x03,                   \
                                               0x01,0x01, 0x01,        \
                                               0x15,                   \
                                               0x00, 0x00,             \
                                               0x09,                   \
                                               0x03, 0x01, 0x14,       \
                                               0x04, 0x01, 0x23,       \
                                               0x02, 0x01, 0x01,       \
                                               0x00, 0x3C,             \
                                               0x06,                   \
                                               0x02, 0x01,0x00,         \
                                               0x01, 0x01, 0x00
*/

uint8_t session_parames_buf[PARAMES_BUF_SIZE] = {0x00};
uint8_t session_config_parames_total_length = 0x27; // set session request cmd payload length

session_state_code session_status = FREE;   //SESSION_READY;
session_action_code session_action = SESSION_NONE;
uint32_t session_id = 0;

uint16_t session_run_time_count = 0; // preparation time (in preparation mode), running time (in running mode)
// uint16_t       latest_index=0;

void configre_save_session_logging_parames(void)
{
  session_parames_config_data_t *parames_config_data = (session_parames_config_data_t *)session_parames_buf;
  uint8_t *features_id_buf = &session_parames_buf[sizeof(session_parames_config_data_t)];

  session_id = parames_config_data->session_id[0] << 24 | parames_config_data->session_id[1] << 16 | parames_config_data->session_id[2] << 8 | parames_config_data->session_id[3];
  m_record_cycle_time = parames_config_data->log_interval[0] << 8 | parames_config_data->log_interval[1];
  m_session_run_time = parames_config_data->running_duration[0] << 8 | parames_config_data->running_duration[1];
  preparation_run_time = parames_config_data->preparation_duration[0] << 8 | parames_config_data->preparation_duration[1];
  m_logging_need_to_record_feature_num = parames_config_data->log_features_length;
  memcpy(m_logging_feature_id_list, features_id_buf, m_logging_need_to_record_feature_num);
}

/*
msig_type : 1: 主动上传，0，应答时的MSsig
*/


static void send_session_status(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t cmd, uint32_t session_id, uint8_t session_status, uint16_t session_time, uint16_t last_log_index,uint8_t msig_type)
{
  uint8_t len = 0;
	if(msig_type==1)
  set_response_packet_head(bluetooth_connection_status_task->send_buf, cmd, sizeof(session_status_packet_t),1);
	else 
	set_response_packet_head(bluetooth_connection_status_task->send_buf, cmd, sizeof(session_status_packet_t),bluetooth_connection_status_task->rv_MsgID);
	
  len = sizeof(response_header_packet_t);
  session_status_packet_t *data_packet = (session_status_packet_t *)&bluetooth_connection_status_task->send_buf[len];
  data_packet->session_id[0] = session_id >> 24;
  data_packet->session_id[1] = session_id >> 16;
  data_packet->session_id[2] = session_id >> 8;
  data_packet->session_id[3] = session_id;

  data_packet->session_status = session_status;

  data_packet->session_time[0] = session_time >> 8;
  data_packet->session_time[1] = session_time;

  data_packet->latest_log_index[0] = last_log_index >> 8;
  data_packet->latest_log_index[1] = last_log_index;

  len += sizeof(session_status_packet_t);

  uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
  bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
  bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
                                                  //   NRF_LOG_HEXDUMP_INFO(m_send_buf, len);
  bluetooth_connection_status_task->send_buf_len =len ;
  ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void control_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
  uint32_t ctl_session_id = 0;
  uint8_t head_length = sizeof(header_packet_t);
  int16_t payload_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
  if (payload_len == -1 || payload_len != 5)
    response_ok_or_failed(bluetooth_connection_status_task, CONTROL_SESSION_RESPONSE, false);
  else
  {
    ctl_session_id = p_data[head_length] << 24 | p_data[head_length + 1] << 16 | p_data[head_length + 2] << 8 | p_data[head_length + 3];
    if (session_id != ctl_session_id)
    {
      response_ok_or_failed(bluetooth_connection_status_task, CONTROL_SESSION_RESPONSE, false);
      NRF_LOG_INFO("ctrl session id no match session_id=%d clr_session_id=%d", session_id, ctl_session_id);
      return;
    }
    session_action = (session_action_code)p_data[head_length + 4];

    NRF_LOG_INFO("session_action =%d", session_action);
    // response_ok_or_failed(CONTROL_SESSION_RESPONSE,true);
  }
}
void session_status_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
  int16_t payload_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
  if (payload_len == -1)
    response_ok_or_failed(bluetooth_connection_status_task, SESSION_STATUS_RESPONSE, false);
  else
  {
    send_session_status(bluetooth_connection_status_task, SESSION_STATUS_RESPONSE, session_id, session_status, session_run_time_count, m_logging_save_ring_buf.count,0);
  }
}

static void clear_session(void)
{
  session_status = SESSION_READY;
  session_action = SESSION_NONE;
  session_id = 0;
  session_run_time_count = 0; // preparation time (in preparation mode), running time (in running mode)
  clear_logging();
	clear_session_all_sharing_user();
}
void clear_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len)
{
//  uint32_t clr_session_id = 0;
//  uint8_t head_length = sizeof(header_packet_t);
  int16_t payload_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
  if (payload_len == -1 || payload_len != 4)
    response_ok_or_failed(bluetooth_connection_status_task, CLEAR_SESSION_RESPONSE, false);
  else
  {
   // clr_session_id = p_data[head_length] << 24 | p_data[head_length + 1] << 16 | p_data[head_length + 2] << 8 | p_data[head_length + 3];
    //    if (session_id != clr_session_id && session_id !=0)
    //    {
    //      response_ok_or_failed(conn_handle,CLEAR_SESSION_RESPONSE, false);
    //      NRF_LOG_INFO("clear session id no match session_id=%d clr_session_id=%d", session_id, clr_session_id);
    //      return;
    //    }
    NRF_LOG_INFO("clear session ok");
    clear_session();
    response_ok_or_failed(bluetooth_connection_status_task, CLEAR_SESSION_RESPONSE, true);
  }
}

/*SESSION_START=0x00,  // start(from preparation)
SESSION_SKIP =0x01,  // skip(skip preparation, start the session timer and logging)
SESSION_STOP =0x02,  // stop(either in preparation or running mode)
SESSION_NONE =0xff,*/

void session_task_exec(void)
{
  static uint32_t send_temp_second = 0;
//  static uint32_t stopped_staus_time_count = 0;
	bluetooth_connection_status_task_t *bluetooth_connection_ownership_task;
  switch (session_status)
  {
  case SESSION_READY: // ready(not configured)
    switch (session_action)
    {
    case SESSION_START:
    case SESSION_SKIP:
    case SESSION_STOP:
      session_action = SESSION_NONE;
	  	bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, false);
      break;
    case SESSION_NONE:
      break;

    default:
      break;
    }

    break;
  case SESSION_CONFIGURED: // configured,have a session
  {
    switch (session_action)
    {
    case SESSION_START:

      session_status = SESSION_PREPARATION;
      //	  configre_save_session_logging_parames();
      clear_logging();
      set_preparation_stage_status(PREPARATION_STAGE_START);
      break;

    case SESSION_SKIP:
      clear_logging();
      session_status = SESSION_PREPARATION;
      //	configre_save_session_logging_parames();
      set_preparation_stage_status(PREPARATION_STAGE_STOP); // in the preparation_stage_task() function,executing stop will execute session run stage
      break;

    case SESSION_STOP:
      session_status = SESSION_STOPPED;
      break;

    case SESSION_NONE:
      break;

    default:
      break;
    }
    if (session_action != SESSION_NONE)
    {
      session_action = SESSION_NONE;
	  	bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, true);
      //    NRF_LOG_INFO("session_action != SESSION_NONE response_ok");
    }
  }

  break;
  case SESSION_PREPARATION: // preparation
    preparation_stage_task();
    switch (session_action)
    {
    case SESSION_STOP:

      set_preparation_stage_status(PREPARATION_STAGE_STOP); // enter sessisin run stage continue execute stopped

      break;

    case SESSION_START:
      session_action = SESSION_NONE;
		 bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, true);
      break;

    case SESSION_SKIP: // skip preparation stage will execut session run stage
      set_preparation_stage_status(PREPARATION_STAGE_STOP);
      session_action = SESSION_NONE;
		  bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, true);
      break;

    case SESSION_NONE:
      break;

    default:
      break;
    }

    break;
  case SESSION_RUNNING: // running
    session_run_stage_task();
    switch (session_action)
    {
    case SESSION_STOP:
      set_session_run_stage_status(SESSION_RUN_FORCE_STOP);
      session_action = SESSION_NONE;
		  bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, true);
      break;
    case SESSION_START:
    case SESSION_SKIP:
      session_action = SESSION_NONE;
		  bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, false);
      break;
    case SESSION_NONE:
      break;

    default:
      break;
    }

    break;
  case SESSION_STOPPED: // stopped
    switch (session_action)
    {
    case SESSION_START:

      // session_status=  SESSION_PREPARATION ;
      // set_preparation_stage_status(PREPARATION_STAGE_START);
		  bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, false);
      break;

    case SESSION_SKIP:
      //  session_status=  SESSION_PREPARATION ;
      //  set_preparation_stage_status(PREPARATION_STAGE_STOP); //in the preparation_stage_task() function,executing stop will execute session run stage
      	bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
		  response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, false);
      break;

    case SESSION_STOP:
			bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
			if (bluetooth_connection_ownership_task != NULL)
      response_ok_or_failed(bluetooth_connection_ownership_task, CONTROL_SESSION_RESPONSE, false);
      break;
    }
    if (session_action != SESSION_NONE)
    {
      session_action = SESSION_NONE;
    }
    break;
  default:
    break;
  }

  save_logging_feature_data();
  //send_logging_recrod_data_task();

  if (send_temp_second == get_system_time_second())
    return;
  send_temp_second = get_system_time_second();

//  switch (session_status)
//  {
//  case SESSION_PREPARATION:
//  case SESSION_RUNNING:
//    if (cts_send_state == TX_COMPLETE)
//      send_session_status(m_current_conn_handle, SESSION_BROADCAST, session_id, session_status, session_run_time_count, m_logging_save_ring_buf.count);

//    stopped_staus_time_count = 0;
//    break;

//  case SESSION_STOPPED:
//    stopped_staus_time_count++;
//    if (stopped_staus_time_count < 2)
//    {
//      if (cts_send_state == TX_COMPLETE)
//        send_session_status(m_current_conn_handle, SESSION_BROADCAST, session_id, session_status, session_run_time_count, m_logging_save_ring_buf.count);
//    }

//    break;

//  default:
//    break;
//  }

  update_adv_data_device_state_of_session(session_status);

//  if ((get_system_time_second() % m_record_cycle_time) == 0)
//  {
//    if (session_status == SESSION_RUNNING || session_status == SESSION_STOPPED)
//    {
//      startup_proactively_send_log();
//    }
//  }
}

void session_status_update(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	if(!is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
	 return ;

	if (bluetooth_connection_status_task->status_send_temp_second == bluetooth_connection_status_task->secondCount )
    return;
  bluetooth_connection_status_task->status_send_temp_second = bluetooth_connection_status_task->secondCount;
	
	//NRF_LOG_INFO("session_status_update");
	
	switch (session_status)
  {
  case SESSION_PREPARATION:
  case SESSION_RUNNING:
    if (bluetooth_connection_status_task->cts_send_state == TX_COMPLETE)
      send_session_status(bluetooth_connection_status_task, SESSION_BROADCAST, session_id, session_status, session_run_time_count, m_logging_save_ring_buf.count,1);

    bluetooth_connection_status_task->stopped_staus_time_count = 0;
    break;

  case SESSION_STOPPED:
	  if(bluetooth_connection_status_task->stopped_staus_time_count>2)
		return ;
	
    bluetooth_connection_status_task->stopped_staus_time_count++;
    if (bluetooth_connection_status_task->stopped_staus_time_count < 2)
    {
      if (bluetooth_connection_status_task->cts_send_state == TX_COMPLETE)
       send_session_status(bluetooth_connection_status_task, SESSION_BROADCAST, session_id, session_status, session_run_time_count, m_logging_save_ring_buf.count,1);
    }

    break;

  default:
    break;
  }
	
}

void proactively_send_log_update(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	if(!is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
	 return ;
	
	
	if ((bluetooth_connection_status_task->secondCount % m_record_cycle_time) == 0)
	{
			if (session_status == SESSION_RUNNING || session_status == SESSION_STOPPED)
	    startup_proactively_send_log(bluetooth_connection_status_task);
			
	
	}
	
	
}

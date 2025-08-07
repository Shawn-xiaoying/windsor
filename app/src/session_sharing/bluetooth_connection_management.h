#ifndef BLUETOOTH_CONNECTION_MANAGEMENT_H
#define BLUETOOTH_CONNECTION_MANAGEMENT_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "efsm.h"
#include "session_log_def.h"
#include "ble_ctcs.h"
typedef struct
{
  efsm_t super;
  uint32_t timeCount;
	uint32_t secondCount;
  uint32_t globalTick;
	// wait_apply_for_permission command_time_count  When receiving the take ownership command successfully or the join session command response successfully, 
	//it will be reset to zero. If these two commands are not received for more than 30 seconds, the connection will be closed
	uint32_t wait_apply_for_permission_command_time_count; 
  uint8_t ShortUserID[5];
	uint8_t rv_MsgID ;
	uint8_t rv_lock ;
  uint16_t conn_handle;
	CTS_SEND_STATE cts_send_state;
	session_log_send_ctrl_parames_t session_log_send_ctrl_parames ;
	
	
	uint8_t stopped_staus_time_count; //Delay count variable when stopping session
	uint32_t status_send_temp_second ;     //Time controlled temporary variable when sending status data
	uint32_t upload_update_feature_num_temp_second ;
	
	uint8_t update_feature_num ; //When there is a change in feature ID data, record the number of changes in the data
	
  bool isOnline;
  uint8_t rev_buf[NRF_SDH_BLE_GAP_DATA_LENGTH];
	uint8_t send_buf[NRF_SDH_BLE_GAP_DATA_LENGTH];
  uint8_t rev_buf_len;
	uint16_t send_buf_len;
} bluetooth_connection_status_task_t;
#define BLUETOOTH_CONNECTION_STATUS_OBJ (efsm_t *)bluetooth_connection_status_task

bluetooth_connection_status_task_t* find_ownership_bluetooth_connection_task(uint8_t* ownership_user);
uint16_t get_ownership_conn_handle(void) ;


void bluetooth_all_connection_status_task_ctor(void);
void bluetooth_all_connection_status_task_efsm_hand(void);
void bluetooth_all_connection_status_task_efsm_evt_trig(void) ;
void bluetooth_all_init_log_send_ctrl_parames(void);
void bluetooth_all_connection_set_update_feature_num(uint8_t update_feature_num) ;

void user_bluetooth_connection(uint16_t conn_handle);
void user_bluetooth_disconnection(uint16_t conn_handle);
void user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
void user_bluetooth_trans_status_to_quit_session(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
void user_bluetooth_trans_status_wait_relinquish_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
void user_bluetooth_trans_status_wait_join_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task);

void user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task_t *bluetooth_connection_status_task);

void user_bluetooth_recv_cmd(uint16_t conn_handle, uint8_t *rev_buf, uint16_t len);
void user_bluetooth_set_cts_send_state(uint16_t conn_handle,CTS_SEND_STATE state);

void user_bluetooth_force_all_disconnction(void);


void reset_ShortUserID(void);

#endif

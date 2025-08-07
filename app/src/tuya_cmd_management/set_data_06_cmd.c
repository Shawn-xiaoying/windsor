#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "set_data_06_cmd.h"
#include "app_set_cmd_management.h"
#include "app_session_set_cmd_management.h"

#define NRF_LOG_MODULE_NAME set_data_06cmd
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

SET_CMD_SEND_TYPE set_cmd_send_type = APP_SET_NO ;

static void cmd_start_send(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	send_dpID_cmd(&tuya_cmd_send_task->tyua_cmd_fun.current_cmd_data);
}
static void cmd_start_sending(void *data_ptr)
{
}

static void cmd_ack_timeout(void *data_ptr)
{
	// tuya_cmd_send_task_t* tuya_cmd_send_task =(tuya_cmd_send_task_t*) data_ptr ;
	current_set_cmd_data.cmd = 0;
	
	if(set_cmd_send_type == APP_SESSION_SET_CMD )
	{
		EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_EXTERN_OBJ, app_session_set_cmd_send_task_ack_timeout);
		set_cmd_send_type= APP_SET_NO;
	}
	else 
	EFSM_TRANS(APP_SET_DATA_CMD_TASK_EXTERN_OBJ, app_set_cmd_send_task_ack_timeout);
}

static void cmd_ack(void *data_ptr)
{
	// tuya_cmd_send_task_t* tuya_cmd_send_task =(tuya_cmd_send_task_t*) data_ptr ;
	current_set_cmd_data.cmd = 0;
	if(set_cmd_send_type == APP_SESSION_SET_CMD )
	{
		EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_EXTERN_OBJ, app_session_set_cmd_send_task_start);
		set_cmd_send_type= APP_SET_NO;
	}
	else 
	EFSM_TRANS(APP_SET_DATA_CMD_TASK_EXTERN_OBJ, app_set_cmd_send_task_start);
}

static void cmd_ack_error(void *data_ptr)
{
	// tuya_cmd_send_task_t* tuya_cmd_send_task =(tuya_cmd_send_task_t*) data_ptr ;
	current_set_cmd_data.cmd = 0;
	if(set_cmd_send_type == APP_SESSION_SET_CMD )
	{
		EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_EXTERN_OBJ, app_session_set_cmd_send_task_ack_error);
		set_cmd_send_type= APP_SET_NO;
	}
	else 
	EFSM_TRANS(APP_SET_DATA_CMD_TASK_EXTERN_OBJ, app_set_cmd_send_task_ack_error);
}

// static void rev_cb_prt(void* data_ptr,unsigned char rev_cmd_type,unsigned short offset,unsigned char* data_proces_buf)
//{
//
// }

static void recv_cb_error_ptr(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack_error);
}

static void set_data_upload_callback(void *data_ptr, unsigned short dpid, const unsigned char value[], unsigned short length)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	// NRF_LOG_INFO("set_data_upload_callback dpid=%d",dpid);
	if (tuya_cmd_send_task->tyua_cmd_fun.current_cmd_data.dpID == dpid)
	{
		NRF_LOG_INFO("set_data_upload_callback current_cmd_data.dpID == dpid=%d", dpid);
		EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack);
	}
}

void startup_tuya_set_06cmd(tuya_cmd_send_task_t *tuya_cmd_send_task)
{
	// tuya_cmd_send_task->tyua_cmd_fun.current_cmd_data.cmd  =DATA_QUERT_CMD;
	memcpy(&tuya_cmd_send_task->tyua_cmd_fun.current_cmd_data, &current_set_cmd_data, sizeof(cmd_data_description));
	tuya_cmd_send_task->tyua_cmd_fun.start_send = cmd_start_send;
	tuya_cmd_send_task->tyua_cmd_fun.sending = cmd_start_sending;
	tuya_cmd_send_task->tyua_cmd_fun.ack_timeout = cmd_ack_timeout;
	tuya_cmd_send_task->tyua_cmd_fun.ack = cmd_ack;
	tuya_cmd_send_task->tyua_cmd_fun.ack_error = cmd_ack_error;

	// set_recv_callback(rev_cb_prt);
	set_dpid_upload_callback(set_data_upload_callback);
	set_recv_error_callback(recv_cb_error_ptr);

	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_start_send);
}

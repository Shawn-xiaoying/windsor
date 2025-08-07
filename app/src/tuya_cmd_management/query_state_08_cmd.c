#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "query_state_08_cmd.h"
#include "app_set_cmd_management.h"

#define NRF_LOG_MODULE_NAME query_state_08_cmd
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static bool have_ack = false;
static void cmd_start_send(void *data_ptr)
{
	//	  tuya_cmd_send_task_t* tuya_cmd_send_task =(tuya_cmd_send_task_t*) data_ptr ;
	have_ack = false;
	send_status_query();
}
static void cmd_start_sending(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	//  NRF_LOG_INFO("query_state sending");
	if (have_ack) // //处理查询数据时，控制器会连续发送应答包的情况，如果超过TIME_OUT_NUM 没有应答数据包，则认为查询结束
	{
		if (tuya_cmd_send_task->timeCount >= 2)
		{
			tuya_cmd_send_task->timeCount = 0;
			EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack);
		}
	}
}

static void cmd_ack_timeout(void *data_ptr)
{
	// tuya_cmd_send_task_t* tuya_cmd_send_task =(tuya_cmd_send_task_t*) data_ptr ;
	// current_set_cmd_data.cmd=0;
	NRF_LOG_INFO("query_state timeout");
}

static void cmd_ack(void *data_ptr)
{
	// tuya_cmd_send_task_t* tuya_cmd_send_task =(tuya_cmd_send_task_t*) data_ptr ;
	//  NRF_LOG_INFO("query_state ack ok");
	//	current_set_cmd_data.cmd=0;
}

static void cmd_ack_error(void *data_ptr)
{
	// tuya_cmd_send_task_t* tuya_cmd_send_task =(tuya_cmd_send_task_t*) data_ptr ;
	// current_set_cmd_data.cmd=0;
	NRF_LOG_INFO("query_state ack error");
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

static void query_state_upload_callback(void *data_ptr, unsigned short dpid, const unsigned char value[], unsigned short length)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	tuya_cmd_send_task->timeCount = 0;
	have_ack = true;
	//  NRF_LOG_INFO("query_state_upload_callback dpid=%d",dpid);
}

void startup_tuya_query_08_cmd(tuya_cmd_send_task_t *tuya_cmd_send_task)
{
	tuya_cmd_send_task->tyua_cmd_fun.current_cmd_data.cmd = STATE_QUERY_CMD;
	tuya_cmd_send_task->tyua_cmd_fun.start_send = cmd_start_send;
	tuya_cmd_send_task->tyua_cmd_fun.sending = cmd_start_sending;
	tuya_cmd_send_task->tyua_cmd_fun.ack_timeout = cmd_ack_timeout;
	tuya_cmd_send_task->tyua_cmd_fun.ack = cmd_ack;
	tuya_cmd_send_task->tyua_cmd_fun.ack_error = cmd_ack_error;

	// set_recv_callback(rev_cb_prt);
	set_dpid_upload_callback(query_state_upload_callback);
	set_recv_error_callback(recv_cb_error_ptr);

	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_start_send);
}

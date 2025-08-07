#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heart_cmd.h"
#include "app_cfg_cmd.h"

#define NRF_LOG_MODULE_NAME heart_cmd
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static void cmd_start_send(void *data_ptr)
{
	send_heart_to_mcu();
}
static void cmd_start_sending(void *data_ptr)
{
}

static void cmd_ack_timeout(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	tuya_cmd_send_task->heart_send_status = SEND_AFTER_POWER_ON;
}

static void cmd_ack(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	tuya_cmd_send_task->heart_send_status = SEND_AFTER_MCU_RESPONDS;
}

static void cmd_ack_error(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	tuya_cmd_send_task->heart_send_status = SEND_AFTER_POWER_ON;
}

static void rev_cb_prt(void *data_ptr, unsigned char rev_cmd_type, unsigned short offset, unsigned char *data_proces_buf)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	mcu_heart_reset_state = data_proces_buf[offset + DATA_START];
	// NRF_LOG_INFO("heart recv callback");

	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack);
}

static void recv_cb_error_ptr(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack_error);
}

void startup_tuya_heart_cmd(tuya_cmd_send_task_t *tuya_cmd_send_task)
{
	tuya_cmd_send_task->tyua_cmd_fun.current_cmd_data.cmd = HEAT_BEAT_CMD;

	tuya_cmd_send_task->tyua_cmd_fun.start_send = cmd_start_send;
	tuya_cmd_send_task->tyua_cmd_fun.sending = cmd_start_sending;
	tuya_cmd_send_task->tyua_cmd_fun.ack_timeout = cmd_ack_timeout;
	tuya_cmd_send_task->tyua_cmd_fun.ack = cmd_ack;
	tuya_cmd_send_task->tyua_cmd_fun.ack_error = cmd_ack_error;

	set_recv_callback(rev_cb_prt);
	set_recv_error_callback(recv_cb_error_ptr);

	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_start_send);
}

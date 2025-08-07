#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "device_info_cmd.h"
#include "app_set_cmd_management.h"

#define NRF_LOG_MODULE_NAME device_info_cmd
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

extern void device_info_proc(uint8_t *data, uint16_t length);

static void cmd_start_send(void *data_ptr)
{
	get_product_info_cmd();
}
static void cmd_start_sending(void *data_ptr)
{
}

static void cmd_ack_timeout(void *data_ptr)
{
//	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	current_set_cmd_data.cmd = 0;
	// tuya_cmd_send_task->heart_send_status = SEND_AFTER_POWER_ON ;
}

static void cmd_ack(void *data_ptr)
{
	//tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	// tuya_cmd_send_task->heart_send_status = SEND_AFTER_MCU_RESPONDS ;
	current_set_cmd_data.cmd = 0;
}

static void cmd_ack_error(void *data_ptr)
{
	//tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	// tuya_cmd_send_task->heart_send_status = SEND_AFTER_POWER_ON ;
	current_set_cmd_data.cmd = 0;
}

static void rev_cb_prt(void *data_ptr, unsigned char rev_cmd_type, unsigned short offset, unsigned char *data_proces_buf)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	uint8_t *data_buf = &data_proces_buf[offset + DATA_START];
	uint16_t data_length = data_proces_buf[offset + LENGTH_HIGH] << 8 | data_proces_buf[offset + LENGTH_LOW];
	NRF_LOG_INFO("device info recv callback");
	device_info_proc(data_buf, data_length);
	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack);
}

static void recv_cb_error_ptr(void *data_ptr)
{
	tuya_cmd_send_task_t *tuya_cmd_send_task = (tuya_cmd_send_task_t *)data_ptr;
	NRF_LOG_INFO("device info recv error");
	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack_error);
}

void startup_tuya_device_info_cmd(tuya_cmd_send_task_t *tuya_cmd_send_task)
{
	tuya_cmd_send_task->tyua_cmd_fun.current_cmd_data.cmd = PRODUCT_INFO_CMD;

	tuya_cmd_send_task->tyua_cmd_fun.start_send = cmd_start_send;
	tuya_cmd_send_task->tyua_cmd_fun.sending = cmd_start_sending;
	tuya_cmd_send_task->tyua_cmd_fun.ack_timeout = cmd_ack_timeout;
	tuya_cmd_send_task->tyua_cmd_fun.ack = cmd_ack;
	tuya_cmd_send_task->tyua_cmd_fun.ack_error = cmd_ack_error;

	set_recv_callback(rev_cb_prt);
	set_recv_error_callback(recv_cb_error_ptr);

	EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_start_send);
}

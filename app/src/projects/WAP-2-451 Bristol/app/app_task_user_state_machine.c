#include "mcu.h"
#include "app_cfg_cmd.h"
#include "app_task.h"
#include "dpid_map_c.h"
#include "mt_gpio.h"
#include "mcu_api.h"
#include "logging_rw_implent.h"
#include "fstorage.h"
#include "mt_ble_adv.h"
#include "session.h"
#include "tuya_cmd_send_task.h"
#include "tuya_recv_data_task.h"
#include "controller_connection_status_task.h"
#include "app_set_cmd_management.h"
#include "query_state_08_cmd.h"
#include "device_info_cmd.h"
#include "comm_lib.h"
#include "mt_cst_protocol.h"

#define NRF_LOG_MODULE_NAME app_task_user_state_machine
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#ifdef USE_STATE_MACHINE

static bool user_connect_state = false;
bool bluetooth_connect_status = false;
#define QUERY_STATE_CYCLE 20

//uint16_t m_current_conn_handle = BLE_CONN_HANDLE_INVALID;

bool get_user_connect_state(void)
{
	return user_connect_state;
}
void clear_usr_connect_state(void)
{
	user_connect_state = false;
}

void set_user_connect_state(bool value)
{
	user_connect_state = value;
}

void start_tuya_query_device_info_cmd(void)
{
	current_set_cmd_data.cmd = PRODUCT_INFO_CMD;
}

void start_tuya_set_data_06cmd(uint8_t feature_id)
{

	unsigned char dpid = get_dpid(feature_id);
	unsigned char value_len = 0;
	unsigned char *value_buf = (unsigned char *)get_set_value_adr(feature_id, &value_len);
	memset((void *)&current_set_cmd_data, 0, sizeof(cmd_data_description));
	current_set_cmd_data.cmd = DATA_QUERT_CMD;
	current_set_cmd_data.dpID = dpid;
	memcpy(current_set_cmd_data.data.buffer, value_buf, value_len);
}

void start_tuya_session_set_data_06cmd(uint8_t feature_id)
{
	unsigned char dpid = get_dpid(feature_id);
	unsigned char value_len = 0;
	unsigned char *value_buf = (unsigned char *)get_session_set_value_adr(feature_id, &value_len);
	memset((void *)&current_set_cmd_data, 0, sizeof(cmd_data_description));
	current_set_cmd_data.cmd = DATA_QUERT_CMD;
	current_set_cmd_data.dpID = dpid;
	memcpy(current_set_cmd_data.data.buffer, value_buf, value_len);
	
}


void device_connection_callback(void)
{

	startup_tuya_device_info_cmd(&ao_tuya_cmd_send_task);
}

void cmd_idle_tick_callback(tuya_cmd_send_task_t *tuya_cmd_send_task)
{

	if (tuya_cmd_send_task->globalTick % QUERY_STATE_CYCLE == 0)
	{
		//NRF_LOG_INFO("08 cmd query globalTick=%d", tuya_cmd_send_task->globalTick);
		startup_tuya_query_08_cmd(tuya_cmd_send_task);
	}
}

void device_info_proc(uint8_t *data, uint16_t length)
{
	char buffer[64]={0};
  uint8_t device_name_length = sizeof(Get_Response_Pack.red_light_bed_device_name.attribute);
	NRF_LOG_INFO("device_info=%s", data);
	json_error_t result = json_extract_value((const char*)data, (const char*)"name", buffer, sizeof(buffer));
	if (result == JSON_PARSE_OK)
	{
		NRF_LOG_INFO("Extracted value: %s ", buffer);
		memset(Get_Response_Pack.red_light_bed_device_name.attribute, 0, device_name_length);
		if (strlen(buffer) <= device_name_length)
		memcpy(Get_Response_Pack.red_light_bed_device_name.attribute, buffer, strlen(buffer));
		else
		memcpy(Get_Response_Pack.red_light_bed_device_name.attribute, buffer, device_name_length);
#ifdef PROTOCOL_DEBUG_OUT
		NRF_LOG_INFO("red_light_bed_device_name=%s", Get_Response_Pack.red_light_bed_device_name.attribute);
#endif
	}
	else
	{
		NRF_LOG_INFO("Error code: %d ", result);
	}
}

void device_code_write_finished_proc(bool result)
{
	uint16_t device_code;
	if (result)
	{
		device_code = fstorge_read_device_code();
		if (SettingData.device_code == device_code)
		{
			update_adv_data_device_code(device_code);
			EFSM_TRANS(APP_SET_DATA_CMD_TASK_EXTERN_OBJ, app_set_cmd_send_task_start);
		}
		else

			EFSM_TRANS(APP_SET_DATA_CMD_TASK_EXTERN_OBJ, app_set_cmd_send_task_ack_error);
	}
	else
		EFSM_TRANS(APP_SET_DATA_CMD_TASK_EXTERN_OBJ, app_set_cmd_send_task_ack_error);
}

void startup_features_set_cmd(void)
{
	start_dpid_set();
}

static uint32_t efsm_tick_time = 0;
void app_task(void)
{

	task_check_input_io();

	tuya_recv_data_task();

	Efsm_Hand((efsm_t *)&ao_tuya_cmd_send_task);
	Efsm_Hand((efsm_t *)&ao_app_set_cmd_send_task);
	Efsm_Hand((efsm_t *)&ao_controller_connection_status_task);
	bluetooth_all_connection_status_task_efsm_hand();

	session_task_exec();

	if (get_system_time_tick() == efsm_tick_time)
		return;
	efsm_tick_time = get_system_time_tick();

	Efsm_EvtTrig((efsm_t *)&ao_tuya_cmd_send_task, EFSM_EVT_TICK);
	Efsm_EvtTrig((efsm_t *)&ao_controller_connection_status_task, EFSM_EVT_TICK);
	Efsm_EvtTrig((efsm_t *)&ao_app_set_cmd_send_task, EFSM_EVT_TICK);
	bluetooth_all_connection_status_task_efsm_evt_trig();
}

void custom_protocol_init(void)
{
	NRF_LOG_INFO("custom_protocol_init");
	memset(&Last_Get_Response_Pack, 0, sizeof(Last_Get_Response_Pack));
	mcu_protocol_init();
	clear_recv_callbak();
	tuya_cmd_send_task_ctor(&ao_tuya_cmd_send_task);
	controller_connection_status_task_ctor(&ao_controller_connection_status_task);
	app_set_cmd_send_task_ctor(&ao_app_set_cmd_send_task);
	bluetooth_all_connection_status_task_ctor();
}
#endif

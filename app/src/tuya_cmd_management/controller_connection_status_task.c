#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "app_cfg_cmd.h"
// #include "mt_gpio.h"
#include "controller_connection_status_task.h"
#include "tuya_cmd_send_task.h"
#include "proactively_upload_data_management.h"

#define NRF_LOG_MODULE_NAME controller_connection_status_task
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();
extern void device_connection_callback(void);

controller_connection_status_task_t ao_controller_connection_status_task;
void start_tuya_query_state_08cmd(void);

void controller_connection_status_online(controller_connection_status_task_t *controller_connection_status_task, uint16_t evt)
{
	switch (evt)
	{
	case EFSM_EVT_EXIT:
	//	NRF_LOG_INFO("online exit =%d", controller_connection_status_task->globalTick);
		break;
	case EFSM_EVT_ENTRY:
		RUN_LED_OFF();
		controller_connection_status_task->timeCount = 0;
	//	NRF_LOG_INFO("online entry =%d", controller_connection_status_task->globalTick);
		device_connection_callback();

		Get_Response_Pack.connction_status.attribute = 1;
		break;
	case EFSM_EVT_TICK:
		++controller_connection_status_task->globalTick;
		++controller_connection_status_task->timeCount;

		if (controller_connection_status_task->globalTick % 20 == 0)
			RUN_LED_ON();
		else if ((controller_connection_status_task->globalTick) % 20 == 10)
			RUN_LED_OFF();

//		if (mt_is_ble_connected() == true)
		{
			if (controller_connection_status_task->globalTick % 10 == 0)
//				proactively_upload_change_data_to_the_app();
			   refresh_change_data_feature_id_list();
		}

		if (ao_tuya_cmd_send_task.heart_send_status == SEND_AFTER_POWER_ON)
			EFSM_TRANS(CONTROLLER_STATUS_OBJ, controller_connection_status_offline);

		break;
	}
}

void controller_connection_status_offline(controller_connection_status_task_t *controller_connection_status_task, uint16_t evt)
{
	switch (evt)
	{
	case EFSM_EVT_EXIT:
	//	NRF_LOG_INFO("offline exit =%d", controller_connection_status_task->globalTick);
		break;
	case EFSM_EVT_ENTRY:
		RUN_LED_OFF();
		controller_connection_status_task->timeCount = 0;
		Get_Response_Pack.connction_status.attribute = 0;
	//	NRF_LOG_INFO("offline entry =%d", controller_connection_status_task->globalTick);

		break;
	case EFSM_EVT_TICK:
		++controller_connection_status_task->globalTick;
		++controller_connection_status_task->timeCount;

		if (controller_connection_status_task->globalTick % 150 == 0)
			RUN_LED_ON();
		else if ((controller_connection_status_task->globalTick - 20) % 150 == 0)
			RUN_LED_OFF();

		if (ao_tuya_cmd_send_task.heart_send_status == SEND_AFTER_MCU_RESPONDS)
			EFSM_TRANS(CONTROLLER_STATUS_OBJ, controller_connection_status_online);

		break;
	}
}
void controller_connection_status_task_ctor(controller_connection_status_task_t *controller_connection_status_task)
{
	Efsm_Ctor((efsm_t *)controller_connection_status_task, (efsmState_t)controller_connection_status_offline);
	controller_connection_status_task->globalTick = 0;
	EFSM_REG_STATE(CONTROLLER_STATUS_OBJ, controller_connection_status_online);
	EFSM_REG_STATE(CONTROLLER_STATUS_OBJ, controller_connection_status_offline);
}

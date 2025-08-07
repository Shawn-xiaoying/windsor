#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heart_cmd.h"
#include "set_data_06_cmd.h"
#include "query_state_08_cmd.h"
#include "upload_up_cmd.h"
#include "tuya_cmd_send_task.h"
#include "app_set_cmd_management.h"
#include "device_info_cmd.h"

#define NRF_LOG_MODULE_NAME tuya_cmd_send_task
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

tuya_cmd_send_task_t ao_tuya_cmd_send_task =
	{
		.number_of_send_cmd = 0,
		.timeCount = 0,
		.globalTick = 0,
		.heart_send_status = SEND_AFTER_POWER_ON,

};

#define POWER_ON_AFTER_SENDING_CYCLE 10 // 10*100ms
#define MCU_RESPONDS_SEND_CYCLE 150		// 150*100 ms

TUYA_SEND_TASK_STATUS tuya_send_task_state = TUYA_SEND_IDLE ;

extern void cmd_idle_tick_callback(tuya_cmd_send_task_t *tuya_cmd_send_task);

static void clear_current_cmd(void)
{
	cmd_data_description *current_cmd_data = &ao_tuya_cmd_send_task.tyua_cmd_fun.current_cmd_data;
	current_cmd_data->cmd = HEAT_BEAT_CMD;
}
static void tuya_cmd_send_manage(tuya_cmd_send_task_t *tuya_cmd_send_task)
{
	switch (current_set_cmd_data.cmd)
	{
	case DATA_QUERT_CMD:
		startup_tuya_set_06cmd(tuya_cmd_send_task);
		break;

	case STATE_QUERY_CMD:
		startup_tuya_query_08_cmd(tuya_cmd_send_task);
		break;

	case PRODUCT_INFO_CMD:
		startup_tuya_device_info_cmd(tuya_cmd_send_task);
		break;

	default:

		switch (tuya_cmd_send_task->heart_send_status)
		{
		case SEND_AFTER_POWER_ON:
			if (tuya_cmd_send_task->globalTick % POWER_ON_AFTER_SENDING_CYCLE == 0)
				startup_tuya_heart_cmd(tuya_cmd_send_task);
			break;
		case SEND_AFTER_MCU_RESPONDS:
			if (tuya_cmd_send_task->globalTick % MCU_RESPONDS_SEND_CYCLE == 0)
				startup_tuya_heart_cmd(tuya_cmd_send_task);

			else
			{
				tuya_send_task_state = TUYA_SEND_IDLE;
				cmd_idle_tick_callback(tuya_cmd_send_task);
			}
			break;
		default:
			break;
		}

		break;
	}
}

/**
 * @brief Handle the idle state of the Tuya command send task.
 *
 * This function is responsible for handling events when the Tuya command send task is in the idle state.
 *
 * @param tuya_cmd_send_task A pointer to the Tuya command send task structure.
 * @param evt The event that triggered the state handling.
 */
void tuya_cmd_send_task_idle(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt)
{
	// The function currently does nothing and can be expanded to handle specific events.
	switch (evt)
	{
	case EFSM_EVT_EXIT:
		//	NRF_LOG_INFO("tuya_cmd_send_task_idle exit =%d",tuya_cmd_send_task->globalTick);
		break;
	case EFSM_EVT_ENTRY:
		//		NRF_LOG_INFO("tuya_cmd_send_task_idle entry =%d",tuya_cmd_send_task->globalTick);
		tuya_cmd_send_task->timeCount = 0;

		break;
	case EFSM_EVT_TICK:
		++tuya_cmd_send_task->globalTick;
		++tuya_cmd_send_task->timeCount;
		tuya_cmd_send_manage(tuya_cmd_send_task);

		//		if(tuya_cmd_send_task->timeCount%100 == 0)
		//		NRF_LOG_INFO("tuya_cmd_send_task_idle evt tick=%d",tuya_cmd_send_task->timeCount);

		break;
	}
}

/**
 * @brief Start the sending process of the Tuya command send task.
 *
 * This function is responsible for initiating the sending process of the Tuya command send task.
 * It can be expanded to handle specific events related to starting the send operation.
 *
 * @param tuya_cmd_send_task A pointer to the Tuya command send task structure.
 * @param evt The event that triggered the start of the send process.
 */
void tuya_cmd_send_task_start_send(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt)
{
	switch (evt)
	{
	case EFSM_EVT_EXIT:

		break;
	case EFSM_EVT_ENTRY:
		tuya_cmd_send_task->timeCount = 0;
	  tuya_send_task_state = TUYA_SEND_BUSY;
		tuya_cmd_send_task->tyua_cmd_fun.start_send(tuya_cmd_send_task);
		EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_sending);
		break;
	case EFSM_EVT_TICK:
		++tuya_cmd_send_task->globalTick;
		break;
	}
}

void tuya_cmd_send_task_sending(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt)
{
	switch (evt)
	{
	case EFSM_EVT_EXIT:
		break;
	case EFSM_EVT_ENTRY:
		tuya_cmd_send_task->timeCount = 0;
		break;
	case EFSM_EVT_TICK:
		++tuya_cmd_send_task->globalTick;
		tuya_cmd_send_task->tyua_cmd_fun.sending(tuya_cmd_send_task);
		if (++tuya_cmd_send_task->timeCount >= TIME_OUT_NUM)
		{

			// 连续三次发送应答超时后会设置超时状态
			if (++tuya_cmd_send_task->number_of_send_cmd >= CMD_SEND_NUN)
			{
				EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack_timeout);
			}
			else
			{
				// 不足三次时，继续启动发送
				EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_start_send);
			}
		}
		break;
	}
}

void tuya_cmd_send_task_ack_timeout(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt)
{
	switch (evt)
	{
	case EFSM_EVT_EXIT:
		break;
	case EFSM_EVT_ENTRY:
		tuya_cmd_send_task->timeCount = 0;
		tuya_cmd_send_task->number_of_send_cmd = 0;
		tuya_cmd_send_task->tyua_cmd_fun.ack_timeout(tuya_cmd_send_task);
		clear_recv_callbak();
		clear_current_cmd();
		EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_idle);
		break;
	case EFSM_EVT_TICK:
		++tuya_cmd_send_task->globalTick;
		break;
	}
}

void tuya_cmd_send_task_ack(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt)
{
	switch (evt)
	{
	case EFSM_EVT_EXIT:
		break;
	case EFSM_EVT_ENTRY:
		tuya_cmd_send_task->timeCount = 0;
		tuya_cmd_send_task->number_of_send_cmd = 0;
		tuya_cmd_send_task->tyua_cmd_fun.ack(tuya_cmd_send_task);
		clear_recv_callbak();
		clear_current_cmd();
		EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_idle);
		break;
	case EFSM_EVT_TICK:
		++tuya_cmd_send_task->globalTick;
		break;
	}
}

void tuya_cmd_send_task_ack_error(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt)
{
	switch (evt)
	{
	case EFSM_EVT_EXIT:

		break;
	case EFSM_EVT_ENTRY:
		tuya_cmd_send_task->timeCount = 0;
		tuya_cmd_send_task->number_of_send_cmd = 0;
		tuya_cmd_send_task->tyua_cmd_fun.ack_error(tuya_cmd_send_task);
		clear_current_cmd();
		clear_recv_callbak();
		EFSM_TRANS(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_idle);
		break;
	case EFSM_EVT_TICK:
		++tuya_cmd_send_task->globalTick;

		break;
	}
}

void tuya_cmd_send_task_ctor(tuya_cmd_send_task_t *tuya_cmd_send_task)
{
	Efsm_Ctor((efsm_t *)tuya_cmd_send_task, (efsmState_t)tuya_cmd_send_task_idle);
	tuya_cmd_send_task->globalTick = 0;
	EFSM_REG_STATE(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_idle); // 提前注册state，用于Efsm_Trans()的参数检测
	EFSM_REG_STATE(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_start_send);
	EFSM_REG_STATE(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_sending);
	EFSM_REG_STATE(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack_timeout);
	EFSM_REG_STATE(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack);
	EFSM_REG_STATE(CMD_SEND_TASK_OBJ, tuya_cmd_send_task_ack_error);
}

void init_tuya_cmd_send_task(void)
{
	clear_recv_callbak();
}

void efsm_tuya_cmd_send_task_hand(void)
{
	Efsm_Hand((efsm_t *)&ao_tuya_cmd_send_task);
}

void efsm_tuya_cmd_send_task_evt_trig_tick(void)
{
	Efsm_EvtTrig((efsm_t *)&ao_tuya_cmd_send_task, EFSM_EVT_TICK);
}

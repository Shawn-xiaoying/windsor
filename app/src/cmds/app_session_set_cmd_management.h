#ifndef APP_SESSION_SET_CMD_MANAGEMENT_H
#define APP_SESSION_SET_CMD_MANAGEMENT_H

#include "efsm.h"

typedef struct
{
    efsm_t super;
    uint16_t timeCount;
    uint32_t globalTick;
	  uint16_t set_feature_id_index;
} app_session_set_cmd_send_task_t;

void app_session_set_cmd_send_task_idle(app_session_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
void app_session_set_cmd_send_task_start(app_session_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
void app_session_set_cmd_send_task_waiting(app_session_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
void app_session_set_cmd_send_task_ack_timeout(app_session_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
void app_session_set_cmd_send_task_ack(app_session_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
void app_session_set_cmd_send_task_ack_error(app_session_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
extern app_session_set_cmd_send_task_t ao_app_session_set_cmd_send_task ;

#define APP_SESSION_SET_DATA_CMD_TASK_OBJ (efsm_t *)app_session_set_cmd_send_task
#define APP_SESSION_SET_DATA_CMD_TASK_EXTERN_OBJ (efsm_t *)&ao_app_session_set_cmd_send_task

void app_session_set_cmd_send_task_ctor(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task);
void start_session_dpid_set(void);
extern cmd_data_description	current_set_cmd_data ; 

#endif

#ifndef APP_SET_CMD_MANAGEMENT_H
#define APP_SET_CMD_MANAGEMENT_H

#include "efsm.h"

typedef struct
{
    efsm_t super;
    uint16_t timeCount;
    uint32_t globalTick;
	  uint16_t set_feature_id_index;
} app_set_cmd_send_task_t;



void 		app_set_cmd_send_task_idle(app_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
void 		app_set_cmd_send_task_start(app_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
void 		app_set_cmd_send_task_waiting(app_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt);
extern 	app_set_cmd_send_task_t ao_app_set_cmd_send_task ;

#define APP_SET_DATA_CMD_TASK_OBJ (efsm_t *)app_set_cmd_send_task
#define APP_SET_DATA_CMD_TASK_EXTERN_OBJ (efsm_t *)&ao_app_set_cmd_send_task

void app_set_cmd_send_task_ctor(app_set_cmd_send_task_t *app_set_cmd_send_task);
void start_dpid_set(void);


#endif

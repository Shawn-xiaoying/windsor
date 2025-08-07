#ifndef CONTROLLER_CONNECTION_STATUS_TASK_H
#define CONTROLLER_CONNECTION_STATUS_TASK_H

#include "efsm.h"

typedef struct
{
  efsm_t super;
  uint32_t timeCount;
  uint32_t globalTick;
} controller_connection_status_task_t;

void controller_connection_status_online(controller_connection_status_task_t *controller_connection_status_task, uint16_t evt);
void controller_connection_status_offline(controller_connection_status_task_t *controller_connection_status_task, uint16_t evt);

#define CONTROLLER_STATUS_OBJ (efsm_t *)controller_connection_status_task
extern controller_connection_status_task_t ao_controller_connection_status_task;
void controller_connection_status_task_ctor(controller_connection_status_task_t *controller_connection_status_task);

#endif

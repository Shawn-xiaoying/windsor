/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#ifndef APP_MULTI_USER_SERVICE_CFG_H__
#define APP_MULTI_USER_SERVICE_CFG_H__
#include <stdint.h>


void multi_user_service_cmd(bluetooth_connection_status_task_t* bluetooth_connection_status_task,uint16_t conn_handle, uint8_t *p_data, uint8_t rev_len);
void supplier_set_request_cmd_process(bluetooth_connection_status_task_t* bluetooth_connection_status_task, uint8_t *p_data, uint8_t len);
void supplier_get_request_cmd_process(bluetooth_connection_status_task_t* bluetooth_connection_status_task, int16_t temp_i16);


#endif

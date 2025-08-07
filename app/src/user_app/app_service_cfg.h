/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#ifndef APP_SERVICE_CFG_H__
#define APP_SERVICE_CFG_H__
#include <stdint.h>
#include "wap-1-100-windsor-master-protocol.h"

void user_service_cmd(uint16_t conn_handle, uint8_t *p_data, uint8_t rev_len);
void reset_ShortUserID(void);

void supplier_set_request_cmd_process(uint16_t conn_handle, uint8_t *p_data, uint8_t len);
void supplier_get_request_cmd_process(uint16_t conn_handle, int16_t temp_i16);

void check_ShortUserID_bond_state(void);
void check_ShortUserID_bond_timeout(void);
#endif

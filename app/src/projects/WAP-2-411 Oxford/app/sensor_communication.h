/* Copyright (C)
   All rights reserved. */

#ifndef SENSOER_COMMUNICATION_H__
#define SENSOER_COMMUNICATION_H__

#include <stdint.h>
#include "mt_cst_protocol.h"

void uart_rev_sensor_data_handler(uint8_t *pdata, uint8_t len);
void get_all_sensor_value(uint8_t *pdata, uint8_t len);
bool uart_send_read_sensor_type(uint8_t read_mode);

#endif

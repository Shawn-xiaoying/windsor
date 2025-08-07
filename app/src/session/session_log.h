#ifndef _SESSION_LOG_H_
#define _SESSION_LOG_H_
#include "stdint.h"

#include "bluetooth_connection_management.h"

void get_session_log_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len);
void save_logging_feature_data(void);
void send_logging_recrod_data_task(void);
void startup_proactively_send_log(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
void init_save_logging(void);
void clear_logging(void);
void start_record(void);
void stop_record(void);

#endif

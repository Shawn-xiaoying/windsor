#ifndef _SESSION_H_
#define _SESSION_H_

#include "stdint.h"
#define PARAMES_BUF_SIZE 254
#include "bluetooth_connection_management.h"
typedef enum
{
    SESSION_READY = 0x00,       // ready(not configured)
    SESSION_CONFIGURED = 0x01,  // configured,have a session
    SESSION_PREPARATION = 0x02, // preparation
    SESSION_RUNNING = 0x03,     // running
    SESSION_STOPPED = 0x04,     // stopped
	  FREE = 0x05,                // free (ownership has not been taken,ownerID ==0)

} session_state_code;

typedef enum
{
    SESSION_START = 0x00, // start(from preparation)
    SESSION_SKIP = 0x01,  // skip(skip preparation, start the session timer and logging)
    SESSION_STOP = 0x02,  // stop(either in preparation or running mode)
    SESSION_NONE = 0xff,

} session_action_code;

typedef struct
{
    uint8_t session_id[4];
    uint8_t session_status;
    uint8_t session_time[2];
    uint8_t latest_log_index[2];
} __attribute__((packed, aligned(1))) session_status_packet_t;

typedef struct
{
    uint8_t session_id[4];
    uint8_t preparation_duration[2];
    uint8_t running_duration[2];
    uint8_t log_interval[2];
    uint8_t log_features_length;
} __attribute__((packed, aligned(1))) session_parames_config_data_t;


void session_status_update(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
void proactively_send_log_update(bluetooth_connection_status_task_t *bluetooth_connection_status_task);

void control_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len);
void session_status_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len);
void clear_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len);
void configre_save_session_logging_parames(void);
void session_task_exec(void);

extern uint32_t session_id;
extern uint8_t session_parames_buf[PARAMES_BUF_SIZE];
extern uint8_t session_config_parames_total_length;
extern session_state_code session_status;
extern uint16_t session_run_time_count;
extern uint16_t preparation_run_time;

extern uint8_t m_logging_need_to_record_feature_num;
extern unsigned char m_logging_feature_id_list[MAX_FEATURE_NUM];
extern uint8_t m_record_cycle_time; // 记录间隔,以秒为单位,默认1s
extern uint16_t m_session_run_time; // 以秒为单位,最大3600s
extern uint8_t m_command_type;      // start or stop logging record

//extern void startup_features_set_cmd(void);
extern void start_session_dpid_set(void);

#endif

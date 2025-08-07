#ifndef _SESSION_RUN_STAGE_H_
#define _SESSION_RUN_STAGE_H_

typedef enum
{
    SESSION_RUN_STAGE_STOP = 0x00,
    SESSION_RUN_STAGE_START = 0x01,
    SESSION_RUN_STAGE_RUNNING = 0x02,
    SESSION_RUN_STAGE_EXEC_POINT = 0x03,
    SESSION_RUN_FORCE_STOP = 0x04,
    SESSION_RUN_STATE_DELAY_CLEAR = 0x05,
    SESSION_RUN_STAGE_NONE = 0xFF,
} session_run_stage_status;

typedef struct
{
    uint16_t time;
    uint8_t length;
} __attribute__((packed, aligned(1))) session_status_n_parames_t;

typedef struct
{
    session_status_n_parames_t parames;
    uint8_t features_list_pos;

} __attribute__((packed, aligned(1))) sesssion_status_n_run_parames_t;

typedef struct
{

    uint8_t start_session_list_pos;
    uint8_t end_session_list_pos;
    uint8_t total_length;
    uint8_t cur_session_list_totle_count;

} __attribute__((packed, aligned(1))) session_status_parmaes_t;

void set_session_run_stage_status(session_run_stage_status status);
void session_run_stage_task(void);

#endif

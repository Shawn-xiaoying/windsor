#ifndef SESSION_LOG_DEF_H
#define SESSION_LOG_DEF_H

#define MARGIN_LENGTH 2					 // ???????,????????????,??????,?????? MARGIN_LENGTH???????,??????MARGIN_LENGTH???
#define LOGGING_MAX_RING_BUF_LENGTH 3600 // ????????1s ,???????,???3600
#define SAVE_BUF_MAX_LENGTH (LOGGING_MAX_RING_BUF_LENGTH + MARGIN_LENGTH)
typedef enum
{
	LOGGING_RECORD_DATA_SEND_NONE,
	LOGGING_RECORD_DATA_SEND_START,
	LOGGING_RECORD_DATA_SEND_WAIT,

	LOGGING_RECORD_DATA_PROACTIVELY_SEND_START,
	LOGGING_RECORD_DATA_PROACTIVELY_SEND_WAIT,

	LOGGING_RECORD_DATA_SEND_END,

} SESSION_LOGGING_RECORD_DATA_SEND_STATE;

typedef struct
{
	uint8_t session_id[4];
	uint8_t start_index[2];
	uint8_t length;

} __attribute__((packed, aligned(1))) session_log_requeset_payload_t;

typedef struct{
uint16_t upload_start_index;
uint8_t  upload_length ;
bool     start_upload_flag ;
uint16_t index_pos	;
uint16_t actual_reading_index ;

bool startup_proactively_send_log_flag;
uint16_t proactively_send_index_pos	;
SESSION_LOGGING_RECORD_DATA_SEND_STATE logging_send_state;	
	
} __attribute__((packed, aligned(1))) session_log_send_ctrl_parames_t;

#endif

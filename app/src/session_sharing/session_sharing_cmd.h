#ifndef _SESSION_SHARING_CMD_H_
#define _SESSION_SHARING_CMD_H_

typedef enum
{
    RELINQUISH_OWNERSHIP = 0,                 // no session configured or started,leave session
    RELINQUISH_OWNERSHIP_STAY_IN_SESSION = 1, // session configured and started,stay in session
    RELINQUISH_OWNERSHIP_LEAVE_SESSION = 2,   // session configured and started,leave session
    NO_RELINQUISHING_OWNERSHIP = 3,						// refuse relinquishing ownership ,stay in session
    RELINQUISH_OWNERSHIP_RESPONSE_RESULT_UNKNOWN = 0xFF,

} RELINQUISH_OWNERSHIP_RESPONSE_RESULT;


bool is_sharing_uesr(uint8_t* userid);
bool is_ownership_user(uint8_t* userid);
void clear_take_ownership_count(void) ;
void clear_join_session_count(void) ;

uint8_t recv_app_take_ownership_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len);
void send_app_take_ownership_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result);

void recv_app_release_ownership_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len);
void send_app_release_ownership_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result);

void send_relinquish_ownership_request_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *userid,uint8_t userid_len);
void recv_app_relinquish_ownership_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len);

void recv_app_join_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len);
void recv_app_join_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len);
void send_app_join_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result);

void recv_app_quit_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len);
void send_app_quit_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result);

#endif

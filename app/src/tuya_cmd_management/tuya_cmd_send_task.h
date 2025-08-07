#ifndef TUYA_CMD_SEND_TASK_H
#define TUYA_CMD_SEND_TASK_H

#include "efsm.h"
#include "mcu.h"

#define TIME_OUT_NUM 30
#define CMD_SEND_NUN 3


typedef enum
{
   TUYA_SEND_IDLE,
	 TUYA_SEND_BUSY

} TUYA_SEND_TASK_STATUS;




typedef void (*func_ptr)(void *tuya_cmd_send_task_);
typedef void (*func_rev_cb_prt)(void *tuya_cmd_send_task, unsigned char rev_cmd_type, unsigned short offset, unsigned char *data_proces_buf);
typedef void (*func_recv_cb_error_ptr)(void *tuya_cmd_send_task);
typedef void (*func_dpid_upload_cb_ptr)(void *tuya_cmd_send_task, unsigned short dpid, const unsigned char value[], unsigned short length);

typedef struct
{
  cmd_data_description current_cmd_data;
  func_ptr start_send;
  func_ptr sending;
  func_ptr ack_timeout;
  func_ptr ack;
  func_ptr ack_error;
  // func_rev_cb_prt rev_cb ;

} tyua_cmd_fun_t;

typedef struct
{
  efsm_t super;
  uint16_t timeCount;
  uint32_t globalTick;
  HEART_SEND_STATUS heart_send_status;
  uint8_t number_of_send_cmd;
  tyua_cmd_fun_t tyua_cmd_fun;
} tuya_cmd_send_task_t;

#define CMD_SEND_TASK_OBJ (efsm_t *)tuya_cmd_send_task
void tuya_cmd_send_task_idle(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt);
void tuya_cmd_send_task_start_send(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt);
void tuya_cmd_send_task_sending(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt);
void tuya_cmd_send_task_ack_timeout(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt);
void tuya_cmd_send_task_ack(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt);
void tuya_cmd_send_task_ack_error(tuya_cmd_send_task_t *tuya_cmd_send_task, uint16_t evt);
extern tuya_cmd_send_task_t ao_tuya_cmd_send_task;

void  tuya_cmd_send_task_ctor(tuya_cmd_send_task_t *tuya_cmd_send_task);

extern TUYA_SEND_TASK_STATUS tuya_send_task_state ;

#endif

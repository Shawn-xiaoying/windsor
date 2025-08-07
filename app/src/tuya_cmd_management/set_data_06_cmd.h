#ifndef SET_DATA_06_CMD_H
#define SET_DATA_06_CMD_H

#include "tuya_cmd_send_task.h"
#include "tuya_recv_data_task.h"

typedef enum
{
	 APP_SET_NO,
   APP_SET_CMD,  //通过app set 命令
	 APP_SESSION_SET_CMD  //通过app session 进行 set 

} SET_CMD_SEND_TYPE;

extern SET_CMD_SEND_TYPE set_cmd_send_type ;
void startup_tuya_set_06cmd(tuya_cmd_send_task_t *tuya_cmd_send_task);

#endif

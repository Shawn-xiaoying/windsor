#ifndef UPLOAD_UP_CMD_H
#define UPLOAD_UP_CMD_H

#include "tuya_cmd_send_task.h"

void proactively_upload_callback(void *tuya_cmd_send_task, unsigned short dpid, const unsigned char value[], unsigned short length);

#endif

#ifndef TUYA_RECV_DATA_TASK_H
#define TUYA_RECV_DATA_TASK_H

void tuya_recv_data_task(void);
void clear_recv_callbak(void);
void set_recv_callback(func_rev_cb_prt fun);
void set_recv_error_callback(func_recv_cb_error_ptr fun);
void set_dpid_upload_callback(func_dpid_upload_cb_ptr fun);

#endif

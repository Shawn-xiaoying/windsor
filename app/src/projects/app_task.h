#ifndef _APP_TASK_H
#define _APP_TASK_H

#define EQUIPMENT_OFF 0
#define EQUIPMENT_ON 1

typedef enum
{
    Equipment_Status_None,
    Equipment_Stop,
    Equipment_Stop_Timing, // ͣ����ʱ�������м�ʱʱ�䵽���Ժ󣬿�ʼ��ʱ���ڼ�ʱʱ�������û���յ�ͣ��ָ���ִ�йػ�
    Equipment_Started
} Equipment_Startup_Status_e;
//extern unsigned short m_current_conn_handle;
void app_task(void);
void custom_protocol_init(void);
void uart_receive_handler(unsigned char value[], unsigned short data_len);
#define USE_STATE_MACHINE

#endif

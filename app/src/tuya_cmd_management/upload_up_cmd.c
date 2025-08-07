#define NRF_LOG_MODULE_NAME upload_up_log
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

void proactively_upload_callback(void* tuya_cmd_send_task,unsigned short dpid,const unsigned char value[], unsigned short length)
{
	
	//	NRF_LOG_INFO("proactively_upload_callback_callback  dpid=%d",dpid);
}


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "app_cfg_cmd.h"
#include "app_task.h"
#include "mt_cst_protocol.h"
#include "session_sharing_user_list.h"
#include "session_sharing_cmd.h"
#define NRF_LOG_MODULE_NAME proactively_upload_data_management
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

void proactively_upload_change_data_to_the_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	
	//refresh_change_data_feature_id_list();
	if(!is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
	 return ;
	
	if (bluetooth_connection_status_task->upload_update_feature_num_temp_second == bluetooth_connection_status_task->secondCount )
    return;
  bluetooth_connection_status_task->upload_update_feature_num_temp_second = bluetooth_connection_status_task->secondCount;
	
	
	
//	NRF_LOG_INFO("is sharing user ver second");
	 if (bluetooth_connection_status_task->cts_send_state == TX_COMPLETE)
	 {
		 
	//	 NRF_LOG_INFO("is sharing user ");
	//   NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->ShortUserID, sizeof(bluetooth_connection_status_task->ShortUserID));
		 
		// NRF_LOG_INFO("is sharing user TX COMPLETE");
			if(bluetooth_connection_status_task->update_feature_num!=0)
			{
			//	NRF_LOG_INFO("is sharing user update_feature_num");
				send_feature_id_data_to_app(bluetooth_connection_status_task , \
															    Update_Feature_Id_list.feature_id, \
																	bluetooth_connection_status_task->update_feature_num, \
																	ACTIVE_UPLOAD_CMD);
				
				bluetooth_connection_status_task->update_feature_num =0 ;
			}
	 }
	
}

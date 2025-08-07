#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "app_task.h"
#include "app_cfg_cmd.h"
#include "set_data_06_cmd.h"
#include "tuya_cmd_send_task.h"
#include "fstorage.h"
#include "app_session_set_cmd_management.h"
#include "mt_cst_protocol.h"

#define NRF_LOG_MODULE_NAME app_session_set_cmd_management
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

app_session_set_cmd_send_task_t ao_app_session_set_cmd_send_task;

//cmd_data_description	current_set_cmd_data ;

void start_tuya_session_set_data_06cmd(uint8_t feature_id) ;

void app_session_set_cmd_send_task_idle(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task, uint16_t evt)
{
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_session_set_cmd_send_task_idle exit =%d",app_session_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_session_set_cmd_send_task_idle entry =%d",app_session_set_cmd_send_task->globalTick);
            app_session_set_cmd_send_task->timeCount = 0;
				    app_session_set_cmd_send_task->set_feature_id_index = 0;
        
            break;
        case EFSM_EVT_TICK:
            ++app_session_set_cmd_send_task->globalTick;
						++app_session_set_cmd_send_task->timeCount ;
						
						
				
            break;
    }
}
void app_session_set_cmd_send_task_start(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task, uint16_t evt)
{
	uint8_t feature_id ;
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_session_set_cmd_send_task_start exit =%d",app_session_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_session_set_cmd_send_task_start entry =%d",app_session_set_cmd_send_task->globalTick);
            app_session_set_cmd_send_task->timeCount = 0;

   				  if( tuya_send_task_state == TUYA_SEND_BUSY )
						  break ;
				
				    if(app_session_set_cmd_send_task->set_feature_id_index <Session_Set_Feature_Id_list.update_feature_num)
						{
								feature_id = Session_Set_Feature_Id_list.feature_id[app_session_set_cmd_send_task->set_feature_id_index++] ;
								{
								#ifdef USE_STATE_MACHINE
								set_cmd_send_type = APP_SESSION_SET_CMD ;
								start_tuya_session_set_data_06cmd(feature_id);
								EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_waiting);
								#endif 
								
								}
							
							
					  }
					  else {
						NRF_LOG_INFO("start_session_dpid_set DPID_SET_ACK_OK end ");
						EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_ack);
						
					
		
			
					 }
		    
            break;
        case EFSM_EVT_TICK:
            ++app_session_set_cmd_send_task->globalTick;
						++app_session_set_cmd_send_task->timeCount ;
						NRF_LOG_INFO("app_session_set_cmd_send_task_start evt tick =%d",app_session_set_cmd_send_task->timeCount);
					   if( tuya_send_task_state == TUYA_SEND_IDLE )
						EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_EXTERN_OBJ, app_session_set_cmd_send_task_start);
				
	          break;
				
				default:
					break ;
    }
	
}

// Set command timeout of 10 seconds
#define SET_CMD_MANAGE_TIMEOUT 100  
void app_session_set_cmd_send_task_waiting(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task, uint16_t evt)
{
	switch (evt)
    {
        case EFSM_EVT_EXIT:
				//		NRF_LOG_INFO("app_set_cmd_send_task_waiting exit =%d",app_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
				//		NRF_LOG_INFO("app_set_cmd_send_task_waiting entry =%d",app_set_cmd_send_task->globalTick);
            app_session_set_cmd_send_task->timeCount = 0;
        
            break;
        case EFSM_EVT_TICK:
            ++app_session_set_cmd_send_task->globalTick;
						++app_session_set_cmd_send_task->timeCount ;
						if(app_session_set_cmd_send_task->timeCount >= SET_CMD_MANAGE_TIMEOUT)
						EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_ack_timeout);
				
            break;
    }
}
void app_session_set_cmd_send_task_ack_timeout(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task, uint16_t evt)
{
//	bluetooth_connection_status_task_t *bluetooth_connection_ownership_task;
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_session_set_cmd_send_task_timeout exit =%d",app_session_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_session_set_cmd_send_task_timeout entry =%d",app_session_set_cmd_send_task->globalTick);
            app_session_set_cmd_send_task->timeCount = 0;
							
				
				   	Session_Set_Feature_Id_list.update_feature_num = 0;
//				    bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
//						if (bluetooth_connection_ownership_task != NULL)
//				   	device_Set_Response(bluetooth_connection_ownership_task,false)	;
//						
//						 bluetooth_connection_ownership_task->rv_lock = 0 ;
				   	EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_idle);
        
            break;
        case EFSM_EVT_TICK:
            ++app_session_set_cmd_send_task->globalTick;
						++app_session_set_cmd_send_task->timeCount ;
						
						
				
            break;
    }
}
void app_session_set_cmd_send_task_ack(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task, uint16_t evt)
{
//	 bluetooth_connection_status_task_t *bluetooth_connection_ownership_task;
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_session_set_cmd_send_task_ack exit =%d",app_session_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_session_set_cmd_send_task_ack entry =%d",app_session_set_cmd_send_task->globalTick);
            app_session_set_cmd_send_task->timeCount = 0;
           	Session_Set_Feature_Id_list.update_feature_num = 0;
//				    bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
//						if (bluetooth_connection_ownership_task != NULL)
//				    device_Set_Response(bluetooth_connection_ownership_task,true);
//						
//						 bluetooth_connection_ownership_task->rv_lock = 0 ;
				   	EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_idle);
            break;
        case EFSM_EVT_TICK:
            ++app_session_set_cmd_send_task->globalTick;
						++app_session_set_cmd_send_task->timeCount ;
            break;
				default:
					  break ;
    }
	
}
void app_session_set_cmd_send_task_ack_error(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task, uint16_t evt)
{
//	bluetooth_connection_status_task_t *bluetooth_connection_ownership_task;
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_session_set_cmd_send_task_ack_error exit =%d",app_session_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_session_set_cmd_send_task_ack_error entry =%d",app_session_set_cmd_send_task->globalTick);
            app_session_set_cmd_send_task->timeCount = 0;
				    Session_Set_Feature_Id_list.update_feature_num = 0;
//						bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
//						if (bluetooth_connection_ownership_task != NULL)
//				   	device_Set_Response(bluetooth_connection_ownership_task,false)	;
//						
//						 bluetooth_connection_ownership_task->rv_lock = 0 ;
				   	EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_idle);
        
            break;
        case EFSM_EVT_TICK:
            ++app_session_set_cmd_send_task->globalTick;
						++app_session_set_cmd_send_task->timeCount ;
						
						
				
            break;
    }
	
}

#ifdef USE_STATE_MACHINE
void  start_session_dpid_set(void)
{
	EFSM_TRANS(APP_SESSION_SET_DATA_CMD_TASK_EXTERN_OBJ,app_session_set_cmd_send_task_start);
	
}
#endif 


void app_session_set_cmd_send_task_ctor(app_session_set_cmd_send_task_t *app_session_set_cmd_send_task)
{
	  Efsm_Ctor((efsm_t *)app_session_set_cmd_send_task, (efsmState_t)app_session_set_cmd_send_task_idle);
    app_session_set_cmd_send_task->globalTick = 0;
	  app_session_set_cmd_send_task->set_feature_id_index = 0;
    EFSM_REG_STATE(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_idle); 
    EFSM_REG_STATE(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_start);
	  EFSM_REG_STATE(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_waiting);
	  EFSM_REG_STATE(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_ack_timeout);
	  EFSM_REG_STATE(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_ack);
	  EFSM_REG_STATE(APP_SESSION_SET_DATA_CMD_TASK_OBJ,app_session_set_cmd_send_task_ack_error);
	
	
}

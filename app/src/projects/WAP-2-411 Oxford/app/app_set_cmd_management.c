#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "app_task.h"
#include "app_cfg_cmd.h"
#include "app_set_cmd_management.h"
#include "HBOT_uart_cmd.h"

#define NRF_LOG_MODULE_NAME app_set_cmd_management
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

app_set_cmd_send_task_t ao_app_set_cmd_send_task;


void app_set_cmd_send_task_idle(app_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt)
{
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_set_cmd_send_task_idle exit =%d",app_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_set_cmd_send_task_idle entry =%d",app_set_cmd_send_task->globalTick);
            app_set_cmd_send_task->timeCount = 0;
				    app_set_cmd_send_task->set_feature_id_index = 0;
        
            break;
        case EFSM_EVT_TICK:
            ++app_set_cmd_send_task->globalTick;
						++app_set_cmd_send_task->timeCount ;
						
						
				
            break;
    }
}
void app_set_cmd_send_task_start(app_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt)
{
	uint8_t feature_id ;
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_set_cmd_send_task_start exit =%d",app_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_set_cmd_send_task_start entry =%d",app_set_cmd_send_task->globalTick);
            app_set_cmd_send_task->timeCount = 0;

   				  if( get_uart_write_state() < READ_AIR_CONDITI_STATE  )
						EFSM_TRANS(APP_SET_DATA_CMD_TASK_OBJ,app_set_cmd_send_task_waiting);
						else 
						{
						 pSet_Feature_Id_list = &Set_Feature_Id_list ;
             pSettingData = &SettingData ;	
						 uart_switch_cmd_state(SET_AIR_CONDITI_STATE); // 
						 EFSM_TRANS(APP_SET_DATA_CMD_TASK_OBJ,app_set_cmd_send_task_idle);	
						}
						
					  break ;
				

		    
          
        case EFSM_EVT_TICK:
            ++app_set_cmd_send_task->globalTick;
						++app_set_cmd_send_task->timeCount ;
						NRF_LOG_INFO("app_set_cmd_send_task_start evt tick =%d",app_set_cmd_send_task->timeCount);
					   
					
				
	          break;
				
				default:
					break ;
    }
	
}

// Set command timeout of 10 seconds
#define SET_CMD_MANAGE_TIMEOUT 100  
void app_set_cmd_send_task_waiting(app_set_cmd_send_task_t *app_set_cmd_send_task, uint16_t evt)
{
	 bluetooth_connection_status_task_t *bluetooth_connection_ownership_task; 
	 bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
	
	switch (evt)
    {
        case EFSM_EVT_EXIT:
						NRF_LOG_INFO("app_set_cmd_send_task_waiting exit =%d",app_set_cmd_send_task->globalTick);
            break;
        case EFSM_EVT_ENTRY:
						NRF_LOG_INFO("app_set_cmd_send_task_waiting entry =%d",app_set_cmd_send_task->globalTick);
            app_set_cmd_send_task->timeCount = 0;
        
            break;
        case EFSM_EVT_TICK:
            ++app_set_cmd_send_task->globalTick;
						++app_set_cmd_send_task->timeCount ;
						if(app_set_cmd_send_task->timeCount >= SET_CMD_MANAGE_TIMEOUT){
							EFSM_TRANS(APP_SET_DATA_CMD_TASK_OBJ,app_set_cmd_send_task_idle);
							device_Set_Response(bluetooth_connection_ownership_task, false);
							bluetooth_connection_ownership_task->rv_lock = 0 ;	
						}
						else{
						 if( get_uart_write_state() >= READ_AIR_CONDITI_STATE  )
						 EFSM_TRANS(APP_SET_DATA_CMD_TASK_OBJ,app_set_cmd_send_task_start);

						}
							
				
            break;
    }
}



void  start_dpid_set(void)
{
	EFSM_TRANS(APP_SET_DATA_CMD_TASK_EXTERN_OBJ,app_set_cmd_send_task_start);
	
}



void app_set_cmd_send_task_ctor(app_set_cmd_send_task_t *app_set_cmd_send_task)
{
	  Efsm_Ctor((efsm_t *)app_set_cmd_send_task, (efsmState_t)app_set_cmd_send_task_idle);
    app_set_cmd_send_task->globalTick = 0;
	  app_set_cmd_send_task->set_feature_id_index = 0;
    EFSM_REG_STATE(APP_SET_DATA_CMD_TASK_OBJ,app_set_cmd_send_task_idle); 
    EFSM_REG_STATE(APP_SET_DATA_CMD_TASK_OBJ,app_set_cmd_send_task_start);
	  EFSM_REG_STATE(APP_SET_DATA_CMD_TASK_OBJ,app_set_cmd_send_task_waiting);
}

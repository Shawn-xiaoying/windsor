#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ble.h"
#include "nrf_sdh.h"
#include "nrf_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_qwr.h"
#include "nrf_ble_gatt.h"
#include "mt_ble_adv.h"
#include "mt_ble_cfg.h"
#include "ble_gap.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "mt_gpio.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "bluetooth_connection_management.h"
#include "proactively_upload_data_management.h"
#include "app_multi_user_service_cfg.h"
#include "mt_cst_protocol.h"
#include "session_log.h"
#include "session.h"
#include "session_sharing_cmd.h"
#include "session_sharing_user_list.h"
#define NRF_LOG_MODULE_NAME bluetooth_connection_management
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define PERMISSION_REQUEST_RESPONSE_TIMEOUT 50
#define TIME_OUT_CLOSE_CONNECT_TIME    300    //30S

void startup_session_log_upload(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
bluetooth_connection_status_task_t ao_bluetooth_connection_status_task[NRF_SDH_BLE_PERIPHERAL_LINK_COUNT]={
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
#if (NRF_SDH_BLE_PERIPHERAL_LINK_COUNT==8)
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
{	.conn_handle = BLE_CONN_HANDLE_INVALID,.rv_lock=0},
#endif 	
	
};
void bluetooth_connection_status_offline(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt);


static void second_count(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	  if(bluetooth_connection_status_task->globalTick%10 ==0)
		{
			 bluetooth_connection_status_task->secondCount++;
		}
	
}


static void bluetooth_connection_status_connect(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
        NRF_LOG_INFO("bluetooth_connection_status_connect exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
        CONNCET_BOND_LED_ON();
        NRF_LOG_INFO("bluetooth_connection_status_connecte entry =%d", bluetooth_connection_status_task->globalTick);
				update_adv_data_device_state_of_connect(CONNECTED);
        bluetooth_connection_status_task->timeCount = 0;
        bluetooth_connection_status_task->isOnline = true;
        bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
		    ++bluetooth_connection_status_task->wait_apply_for_permission_command_time_count ;
		    second_count(bluetooth_connection_status_task);
        if (bluetooth_connection_status_task->timeCount > TIME_OUT_CLOSE_CONNECT_TIME)  //If no command is received within 30 seconds, the connection will be closed
        {
            NRF_LOG_INFO("bluetooth online timetick =%d connect timeout no cmd", bluetooth_connection_status_task->globalTick);
            mt_ble_force_disconnect(bluetooth_connection_status_task->conn_handle);
        }
        break;
    }
}
static void bluetooth_connection_status_disconnect(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
    uint32_t periph_link_cnt;
    switch (evt)
    {
    case EFSM_EVT_EXIT:
        NRF_LOG_INFO("bluetooth_connection_status_disconnect exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
        NRF_LOG_INFO("bluetooth_connection_status_disconnect entry =%d", bluetooth_connection_status_task->globalTick);
        periph_link_cnt = ble_conn_state_peripheral_conn_count(); // Number of peripheral links.
        if (periph_link_cnt == 0)
				{   CONNCET_BOND_LED_OFF();
					  update_adv_data_device_state_of_connect(NOT_CONNECTED);
				}

        bluetooth_connection_status_task->timeCount = 0;
        bluetooth_connection_status_task->isOnline = false;
        bluetooth_connection_status_task->conn_handle = BLE_CONN_HANDLE_INVALID;
        memset(bluetooth_connection_status_task->ShortUserID, 0, sizeof(bluetooth_connection_status_task->ShortUserID));
     //   NRF_LOG_INFO("bluetooth offline entry =%d", bluetooth_connection_status_task->globalTick);
        EFSM_TRANS(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_offline);
        break;
		
		 case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
				second_count(bluetooth_connection_status_task);
		    break ;
				
    default:
        break;
        //    case EFSM_EVT_TICK:
        //        ++bluetooth_connection_status_task->globalTick;
        //        ++bluetooth_connection_status_task->timeCount;

        //        break;
    }
}

static void bluetooth_connection_status_online_idle(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
      //  NRF_LOG_INFO("bluetooth_connection_status_online_idle exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
     //   NRF_LOG_INFO("bluetooth_connection_status_online_idle entry =%d", bluetooth_connection_status_task->globalTick);
        bluetooth_connection_status_task->timeCount = 0;
				
		    
        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
	//		  ++bluetooth_connection_status_task->wait_apply_for_permission_command_time_count;
		   	second_count(bluetooth_connection_status_task);
		    
		
				startup_session_log_upload(bluetooth_connection_status_task);
		    session_status_update(bluetooth_connection_status_task);
        proactively_send_log_update(bluetooth_connection_status_task);
		    proactively_upload_change_data_to_the_app(bluetooth_connection_status_task);
		
		  
		

        break;
    }
}
static void bluetooth_connection_status_offline(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
      //  NRF_LOG_INFO("bluetooth offline exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
     
        bluetooth_connection_status_task->timeCount = 0;
        bluetooth_connection_status_task->isOnline = false;
     //   NRF_LOG_INFO("bluetooth offline entry =%d", bluetooth_connection_status_task->globalTick);

        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
		   	second_count(bluetooth_connection_status_task);

        break;
    }
}

static void wait_app_response_join_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
        NRF_LOG_INFO("wait_app_response_relinquish_ownership_request exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
        NRF_LOG_INFO("wait_app_response_relinquish_ownership_request entry =%d", bluetooth_connection_status_task->globalTick);
        bluetooth_connection_status_task->timeCount = 0;

        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
		   	second_count(bluetooth_connection_status_task);
        if (bluetooth_connection_status_task->timeCount >= PERMISSION_REQUEST_RESPONSE_TIMEOUT)
        {
						header_packet_t *head_pack = (header_packet_t *)bluetooth_connection_status_task->rev_buf;
            memcpy(bluetooth_connection_status_task->ShortUserID, head_pack->ShortUserID,sizeof(bluetooth_connection_status_task->ShortUserID));
  				  NRF_LOG_INFO("join session userid");
					 	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->ShortUserID,sizeof(bluetooth_connection_status_task->ShortUserID));
					  bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
					  
					    /*此处应该加上获取设备分享权限的应答  */
					 // send_app_join_session_response(bluetooth_connection_status_task, 0x01);; //需测试后再加上
					
					  bluetooth_connection_status_task->rv_lock =0 ;
					  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
					  clear_join_session_count() ;
        }

        break;
    }
}

static void wait_app_response_relinquish_ownership_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
        NRF_LOG_INFO("wait_app_response_relinquish_ownership_request exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
        NRF_LOG_INFO("wait_app_response_relinquish_ownership_request entry =%d", bluetooth_connection_status_task->globalTick);
        bluetooth_connection_status_task->timeCount = 0;

        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
		   	second_count(bluetooth_connection_status_task);
        if (bluetooth_connection_status_task->timeCount >= PERMISSION_REQUEST_RESPONSE_TIMEOUT)
        {
					
					  header_packet_t *head_pack = (header_packet_t *)bluetooth_connection_status_task->rev_buf;
            memcpy(bluetooth_connection_status_task->ShortUserID, head_pack->ShortUserID,sizeof(head_pack->ShortUserID));
	          memcpy(device_infor.ShortUserID, bluetooth_connection_status_task->ShortUserID, sizeof(device_infor.ShortUserID));
  				  NRF_LOG_INFO("recv_app_relinquish_ownership_response---- send app take ownership response 0x01 device userid: ");
					  /*此处应该加上获取设备主权限的应答  */
					 // send_app_take_ownership_response(bluetooth_connection_status_task, 0x01); //需测试后再加上
					 	NRF_LOG_HEXDUMP_INFO(device_infor.ShortUserID,sizeof(device_infor.ShortUserID));
					  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
					  bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
					  bluetooth_connection_status_task->rv_lock =0 ;
					  clear_take_ownership_count() ;

        }

        break;
    }
}



static void bluetooth_connection_status_recv_cmd(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
     //   NRF_LOG_INFO("bluetooth_connection_status_recv_cmd exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
     //   NRF_LOG_INFO("bluetooth_connection_status_recv_cmd entry =%d", bluetooth_connection_status_task->globalTick);
        bluetooth_connection_status_task->timeCount = 0;
        multi_user_service_cmd(bluetooth_connection_status_task, bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->rev_buf, bluetooth_connection_status_task->rev_buf_len);

        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
		    ++bluetooth_connection_status_task->wait_apply_for_permission_command_time_count ;
			 	second_count(bluetooth_connection_status_task);
		
		   
       		
		
        break;
    }
}

static void bluetooth_connection_status_wait_apply_for_permission_command(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
		switch (evt)
    {
    case EFSM_EVT_EXIT:
    //    NRF_LOG_INFO("bluetooth_connection_status_wait_apply_for_permission_command exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
   //     NRF_LOG_INFO("bluetooth_connection_status_wait_apply_for_permission_command entry =%d", bluetooth_connection_status_task->globalTick);
        bluetooth_connection_status_task->timeCount = 0;
    
        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
		    ++bluetooth_connection_status_task->wait_apply_for_permission_command_time_count ;
			 	second_count(bluetooth_connection_status_task);
		    
			  if (bluetooth_connection_status_task->wait_apply_for_permission_command_time_count > TIME_OUT_CLOSE_CONNECT_TIME)  //If no take ownership or jion session command is received within 30 seconds, the connection will be closed
        {
            NRF_LOG_INFO("close connect  timetick =%d  no take_ownership or join session cmd", bluetooth_connection_status_task->globalTick);
					  bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
            mt_ble_force_disconnect(bluetooth_connection_status_task->conn_handle);
					
        }
			
		
        break;
    }
	
}


static void bluetooth_connection_status_quit_session(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t evt)
{
	switch (evt)
    {
    case EFSM_EVT_EXIT:
        NRF_LOG_INFO("bluetooth_connection_status_quit_session exit =%d", bluetooth_connection_status_task->globalTick);
        break;
    case EFSM_EVT_ENTRY:
        NRF_LOG_INFO("bluetooth_connection_status_quit_session entry =%d", bluetooth_connection_status_task->globalTick);
        bluetooth_connection_status_task->timeCount = 0;
    
        break;
    case EFSM_EVT_TICK:
        ++bluetooth_connection_status_task->globalTick;
        ++bluetooth_connection_status_task->timeCount;
			 	second_count(bluetooth_connection_status_task);
		    
			  if(bluetooth_connection_status_task->timeCount>10)   //close the connection ater 2 second
				{
					mt_ble_force_disconnect(bluetooth_connection_status_task->conn_handle);
				}
			
		
        break;
    }
	
}

static void set_session_log_send_ctrl_parames_init_value(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	  bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_start_index = 0;
	  bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length = 0;
    bluetooth_connection_status_task->session_log_send_ctrl_parames.start_upload_flag  = false ;
	  bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos = 0 ;
	  bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index = 0 ;
	  bluetooth_connection_status_task->session_log_send_ctrl_parames.startup_proactively_send_log_flag = false ;
    bluetooth_connection_status_task->session_log_send_ctrl_parames.proactively_send_index_pos = 1 ;
	  bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_SEND_NONE ;
}

static void bluetooth_connection_status_task_ctor(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
    Efsm_Ctor((efsm_t *)bluetooth_connection_status_task, (efsmState_t)bluetooth_connection_status_offline);
    bluetooth_connection_status_task->globalTick = 0;
	  bluetooth_connection_status_task->secondCount =0;
	  bluetooth_connection_status_task->status_send_temp_second = 0;
	  bluetooth_connection_status_task->upload_update_feature_num_temp_second=0 ;
	  bluetooth_connection_status_task->update_feature_num =0 ;
	  bluetooth_connection_status_task->cts_send_state = TX_NONE ;
	
    set_session_log_send_ctrl_parames_init_value(bluetooth_connection_status_task);
	
    memset(bluetooth_connection_status_task->ShortUserID, 0, sizeof(bluetooth_connection_status_task->ShortUserID));
    EFSM_REG_STATE(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_connect);
    EFSM_REG_STATE(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_disconnect);
    EFSM_REG_STATE(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_online_idle);
    EFSM_REG_STATE(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_offline);
    EFSM_REG_STATE(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_recv_cmd);
}

void bluetooth_all_connection_status_task_ctor(void)
{
    for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
    {
        bluetooth_connection_status_task_ctor(&ao_bluetooth_connection_status_task[i]);
    }
}

void bluetooth_all_init_log_send_ctrl_parames()
{
	for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
    {
        set_session_log_send_ctrl_parames_init_value(&ao_bluetooth_connection_status_task[i]);
    }
}

void bluetooth_all_connection_set_update_feature_num(uint8_t update_feature_num)
{
	 for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
		 if(update_feature_num ==0 &&  ao_bluetooth_connection_status_task[i].update_feature_num !=0)
		 break ;
		 
 	   ao_bluetooth_connection_status_task[i].update_feature_num = update_feature_num ;
		 
	 }
 
}


void bluetooth_all_connection_status_task_efsm_hand(void)
{
    for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
    {
        Efsm_Hand((efsm_t *)&ao_bluetooth_connection_status_task[i]);
    }
}

void bluetooth_all_connection_status_task_efsm_evt_trig(void)
{
    for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
    {
        Efsm_EvtTrig((efsm_t *)&ao_bluetooth_connection_status_task[i], EFSM_EVT_TICK);
    }
}

void user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
    EFSM_TRANS(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_online_idle);
}

void user_bluetooth_trans_status_wait_relinquish_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
    EFSM_TRANS(BLUETOOTH_CONNECTION_STATUS_OBJ, wait_app_response_relinquish_ownership_request);
}

void user_bluetooth_trans_status_wait_join_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
    EFSM_TRANS(BLUETOOTH_CONNECTION_STATUS_OBJ, wait_app_response_join_session_request);
}
void user_bluetooth_trans_status_to_quit_session(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	  EFSM_TRANS(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_quit_session);
}

void user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	  EFSM_TRANS(BLUETOOTH_CONNECTION_STATUS_OBJ, bluetooth_connection_status_wait_apply_for_permission_command);
}

bluetooth_connection_status_task_t *find_ownership_bluetooth_connection_task(uint8_t *ownership_user)
{
    for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
    {
        if (memcmp(ao_bluetooth_connection_status_task[i].ShortUserID, ownership_user,sizeof(ao_bluetooth_connection_status_task[i].ShortUserID)) == 0)
        {
            return &ao_bluetooth_connection_status_task[i];
        }
    }

    return NULL;
}

uint16_t get_ownership_conn_handle()
{
	bluetooth_connection_status_task_t * owner_ship_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID) ;
	if(owner_ship_task!=NULL)
	return owner_ship_task->conn_handle ;
	else 
	return BLE_CONN_HANDLE_INVALID ;
	 
}

void user_bluetooth_connection(uint16_t conn_handle)
{
    ao_bluetooth_connection_status_task[conn_handle].conn_handle = conn_handle;
    EFSM_TRANS((efsm_t *)&ao_bluetooth_connection_status_task[conn_handle], bluetooth_connection_status_connect);
}

void user_bluetooth_disconnection(uint16_t conn_handle)
{
    EFSM_TRANS((efsm_t *)&ao_bluetooth_connection_status_task[conn_handle], bluetooth_connection_status_disconnect);
}
void user_bluetooth_recv_cmd(uint16_t conn_handle, uint8_t *rev_buf, uint16_t len)
{
    memcpy(ao_bluetooth_connection_status_task[conn_handle].rev_buf, rev_buf, len);
    ao_bluetooth_connection_status_task[conn_handle].rev_buf_len = len;
    EFSM_TRANS((efsm_t *)&ao_bluetooth_connection_status_task[conn_handle], bluetooth_connection_status_recv_cmd);
}

void user_bluetooth_set_cts_send_state(uint16_t conn_handle,CTS_SEND_STATE state)
{
	  ao_bluetooth_connection_status_task[conn_handle].cts_send_state = state;
	
}

void user_bluetooth_force_all_disconnction(void)
{
	for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
		 if( ao_bluetooth_connection_status_task[i].conn_handle !=BLE_CONN_HANDLE_INVALID)
 	     mt_ble_force_disconnect(ao_bluetooth_connection_status_task[i].conn_handle);
		 
	 }
	
}

void reset_ShortUserID(void)
{
    memset(device_infor.ShortUserID, 0, sizeof(device_infor.ShortUserID));
#ifdef APP_SERVICE_CFG_DEBUG_OUT
    NRF_LOG_INFO("[ShortUserID] clean_ShortUserID =0x%02x %02x %02x %02x %02x",
                 device_infor.ShortUserID[0], device_infor.ShortUserID[1],
                 device_infor.ShortUserID[2], device_infor.ShortUserID[3],
                 device_infor.ShortUserID[4]);
#endif
		user_bluetooth_force_all_disconnction();
	  clear_all_sharing_user();
  //  check_ShortUserID_bond_flg = false;
  //  set_user_connect_state(false);
  //  mt_ble_force_disconnect(m_current_conn_handle);
}

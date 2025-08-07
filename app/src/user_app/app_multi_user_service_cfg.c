/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "mt_ble_adv.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "mt_param_def.h"
#include "bluetooth_connection_management.h"
#include "app_multi_user_service_cfg.h"

#include "comm_lib.h"
#include "mt_cst_protocol.h"
#include "nrf52840.h"
#include "mt_ble_service.h"
#include "app_cfg_cmd.h"
#include "mt_ble_cfg.h"
#include "mt_gpio.h"

#include "session.h"
#include "set_get_session.h"
#include "session_log.h"
#include "session_sharing_cmd.h"
#include "session_sharing_user_list.h"

#define NRF_LOG_MODULE_NAME app_multi_user_server_cfg
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define APP_MULTI_USER_SERVICE_CFG_DEBUG_OUT

extern uint32_t m_device_code ;

static void  set_single_user_userid(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len)
{
		uint16_t temp_crc = 0, rev_data_crc = 0;
    header_packet_t *head_pack = (header_packet_t *)p_data;

    temp_crc = CalcCrc16(p_data, len - 2);

    /*revice data crc 大端表示法 */
    rev_data_crc = p_data[len - 2];                       // 高位在前，也就是在内存的低地址处
    rev_data_crc = (rev_data_crc << 8) | p_data[len - 1]; // 低位在后，也就是在内存的高地址处

    /*CRC 检验*/
    if (rev_data_crc != temp_crc)
    {
        return;
    }
	
		if(!is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
		{
			clear_all_sharing_user();
			insert_session_sharing_user(head_pack->ShortUserID);
			memcpy(bluetooth_connection_status_task->ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID));
			memcpy(device_infor.ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID));
	//		session_status = SESSION_READY ;
			bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
			NRF_LOG_INFO("take_ownership_request userid info:");
			NRF_LOG_HEXDUMP_INFO(device_infor.ShortUserID, sizeof(device_infor.ShortUserID));
			
			
			
		}else {
			
			if(memcmp(bluetooth_connection_status_task->ShortUserID, head_pack->ShortUserID,sizeof(device_infor.ShortUserID)) !=0 )
			{
					clear_all_sharing_user();
				  insert_session_sharing_user(head_pack->ShortUserID);
					memcpy(bluetooth_connection_status_task->ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID));
					memcpy(device_infor.ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID));
			//		session_status = SESSION_READY ;
					bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
					NRF_LOG_INFO("take_ownership_request userid info:");
					NRF_LOG_HEXDUMP_INFO(device_infor.ShortUserID, sizeof(device_infor.ShortUserID));
				
				
			}
			
			
		}
	
	
	
}


static int16_t set_existing_user_to_bluetooth_connect_status_task(bluetooth_connection_status_task_t *task,uint8_t *p_data, uint8_t len)
{
 	 uint16_t temp_crc = 0, rev_data_crc = 0;
   header_packet_t *head_pack = (header_packet_t *)p_data;

    temp_crc = CalcCrc16(p_data, len - 2);

    /*revice data crc 大端表示法 */
    rev_data_crc = p_data[len - 2];                       // 高位在前，也就是在内存的低地址处
    rev_data_crc = (rev_data_crc << 8) | p_data[len - 1]; // 低位在后，也就是在内存的高地址处

    /*CRC 检验*/
    if (rev_data_crc != temp_crc)
    {
        return -1;
    }
	  if(!is_sharing_uesr(task->ShortUserID))
		{
			if( find_session_sharing_user(head_pack->ShortUserID) )
			{
				memcpy(task->ShortUserID,head_pack->ShortUserID,sizeof(device_infor.ShortUserID));
			}
		}else {
			
		//	if(memcmp(task->ShortUserID, head_pack->ShortUserID,sizeof(device_infor.ShortUserID)) !=0 )
			{
				 if( find_session_sharing_user(head_pack->ShortUserID) )
				{
				memcpy(task->ShortUserID,head_pack->ShortUserID,sizeof(device_infor.ShortUserID));
				}else 
				{
					
				 memset(task->ShortUserID,0x00,sizeof(device_infor.ShortUserID));	
				}
				
				
			}
			
			
		}
		return 0 ;
}


void multi_user_service_cmd(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint16_t conn_handle, uint8_t *p_data, uint8_t rev_len)
{

    uint8_t cmd_head = 0, cmd = 0;
    int16_t temp_i16 = 0xff;
	  
	  if(bluetooth_connection_status_task->rv_lock==1)
			{
				NRF_LOG_INFO("rv_lock ==1 return");
				return ; 
				
			}

	  bluetooth_connection_status_task->rv_lock = 1 ;
	
    cmd_head = p_data[0];
    cmd = p_data[1];
	  bluetooth_connection_status_task->rv_MsgID = p_data[2];

    if (cmd_head != WINDSOR_PROTOCOL_APP_TO_DEVICE_HEAD  )
    {
			bluetooth_connection_status_task->rv_lock = 0 ;
        return;
    }

    NRF_LOG_HEXDUMP_INFO(p_data, rev_len);
#if (NRF_SDH_BLE_PERIPHERAL_LINK_COUNT==8)
		set_existing_user_to_bluetooth_connect_status_task(bluetooth_connection_status_task, p_data, rev_len);
#else 
	   set_single_user_userid(bluetooth_connection_status_task, p_data, rev_len);
#endif 		
		
		print_all_sharing_user();
	  
		
		
		
    switch (cmd)
    {
    case INFOR_REQUESET:
        /*Infor 功能请求*/
        NRF_LOG_INFO("INFOR_REQUSET :");
        if (app_Info_Request(bluetooth_connection_status_task,p_data, rev_len) == true) /*后面2 个字节是crc检验*/
        {

            device_Info_Response(bluetooth_connection_status_task);
        }
				if(is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
				user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				else 
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
				bluetooth_connection_status_task->rv_lock = 0 ;
        break;
    case TAKE_OWNERSHIP_REQUEST:
        recv_app_take_ownership_request(bluetooth_connection_status_task, p_data, rev_len);
        break;
    case RELEASE_OWNERSHIP_REQUEST:
        recv_app_release_ownership_request(bluetooth_connection_status_task, p_data, rev_len);
				bluetooth_connection_status_task->rv_lock = 0 ;
        break;
  
    case JOIN_SESSION_REQUEST:
        recv_app_join_session_request(bluetooth_connection_status_task, p_data, rev_len);
        break;
    case QUIT_SESSION_REQUEST:
        recv_app_quit_session_request(bluetooth_connection_status_task, p_data, rev_len);
		    bluetooth_connection_status_task->rv_lock = 0 ;
        break;
    case RELINQUISH_OWNERSHIP_RESPONSE:
			  
        recv_app_relinquish_ownership_response(bluetooth_connection_status_task, p_data, rev_len);
		    bluetooth_connection_status_task->rv_lock = 0 ;
        break;
    case JOIN_SESSION_RESPONSE:
        recv_app_join_session_response(bluetooth_connection_status_task, p_data, rev_len);
		    bluetooth_connection_status_task->rv_lock = 0 ;
        break;
    case GET_REQUESET:                                           /*GET 功能请求*/
        if(is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
				{
					temp_i16 = app_Get_Request_cmd(bluetooth_connection_status_task,p_data, rev_len);         /*后面2 个字节是crc检验*/
					supplier_get_request_cmd_process(bluetooth_connection_status_task, temp_i16); // 不同的设备连接实现该函数，根据该函数实现不同的功能
					user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				}else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,GET_RESPONSE);
				bluetooth_connection_status_task->rv_lock = 0 ;
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
					
				}
				
        break;

    case SET_REQUESET: /*app Set设置功能请求*/
			  if(is_ownership_user(bluetooth_connection_status_task->ShortUserID) || m_device_code ==0 || m_device_code==0xffffffff )
				{
          supplier_set_request_cmd_process(bluetooth_connection_status_task, p_data, rev_len);
			  	user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);	
				}else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,SET_RESPONSE);
				bluetooth_connection_status_task->rv_lock = 0 ;
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
					
				}
				
        break;

    case GET_FEATURES:
			  if(is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
        {
					app_get_features_supported_by_the_device_request(bluetooth_connection_status_task, p_data, rev_len);
				
				 user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
					
				}else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,GET_FEATURES_RESPONSE);
			
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
					
				}
					bluetooth_connection_status_task->rv_lock = 0 ; 
        break;

    case GET_LOG_FEATURES:
			  if(is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
				{
					app_get_log_features_supported_by_the_device_request(bluetooth_connection_status_task, p_data, rev_len);
					
					user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				}else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,GET_LOG_FEATURES_RESPONSE);
				
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
					
				}
				bluetooth_connection_status_task->rv_lock = 0 ;
        break;

    case SET_SESSION:
			  if(is_ownership_user(bluetooth_connection_status_task->ShortUserID))
				{
					set_session_requeset(bluetooth_connection_status_task, p_data, rev_len);
					user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				}else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,SET_SESSION_RESPONSE);	
				
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
					
				}
				bluetooth_connection_status_task->rv_lock = 0 ;
        break;

    case GET_SESSION:
			  if(is_sharing_uesr(bluetooth_connection_status_task->ShortUserID)){
         get_session_request(bluetooth_connection_status_task, p_data, rev_len);
				
				 user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				}else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,GET_SESSION_RESPONSE);
				
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
					
				}
				bluetooth_connection_status_task->rv_lock = 0 ;
        break;

    case CONTROL_SESSION:
			   if(is_ownership_user(bluetooth_connection_status_task->ShortUserID))
				 {
					 control_session_request(bluetooth_connection_status_task, p_data, rev_len);
					 
				   user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				 }else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,CONTROL_SESSION_RESPONSE);	 
				
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
					 
				 }
				 bluetooth_connection_status_task->rv_lock = 0 ;
        break;

    case SESSION_STATUS:
			 if(is_sharing_uesr(bluetooth_connection_status_task->ShortUserID)){
        session_status_request(bluetooth_connection_status_task, p_data, rev_len);
				
			  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
			 }else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,SESSION_STATUS_RESPONSE);
			
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
				 
			 }
			 	bluetooth_connection_status_task->rv_lock = 0 ;
        break;

    case GET_LOGS:
			 if(is_sharing_uesr(bluetooth_connection_status_task->ShortUserID)){
         get_session_log_request(bluetooth_connection_status_task, p_data, rev_len);
				
			   user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
			 }else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,GET_LOGS_RESPONSE); 
				
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
				 
			 }
			   bluetooth_connection_status_task->rv_lock = 0 ;
        break;

    case CLEAR_SESSION:
			   if(is_ownership_user(bluetooth_connection_status_task->ShortUserID)){
          clear_session_request(bluetooth_connection_status_task, p_data, rev_len);
				
				  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				 }else {
				send_unauthorized_response_packet(bluetooth_connection_status_task,CLEAR_SESSION_RESPONSE);
				
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
				 
				 }
				 bluetooth_connection_status_task->rv_lock = 0 ;
        break;

    default:
			 //user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
		   bluetooth_connection_status_task->rv_lock = 0 ;
		   user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
        break;
    }

		
}

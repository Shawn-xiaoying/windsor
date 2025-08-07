#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "mt_ble_adv.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "mt_param_def.h"
#include "bluetooth_connection_management.h"
#include "session_sharing_cmd.h"

#include "comm_lib.h"
#include "mt_param_def.h"
#include "mt_cst_protocol.h"

#include "nrf52840.h"
#include "mt_ble_service.h"
#include "app_cfg_cmd.h"

#include "mt_ble_cfg.h"
#include "mt_gpio.h"

#include "session.h"
#include "set_get_session.h"
#include "session_log.h"
#include "session_sharing_user_list.h"

#define NRF_LOG_MODULE_NAME session_sharing_cmd_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static bluetooth_connection_status_task_t *current_take_ownership_status_task = NULL;
static bluetooth_connection_status_task_t *current_join_session_status_task = NULL;
static RELINQUISH_OWNERSHIP_RESPONSE_RESULT m_relinquish_ownership_response_result = RELINQUISH_OWNERSHIP_RESPONSE_RESULT_UNKNOWN;
static uint8_t take_ownership_count = 0;
static uint8_t join_session_count = 0;

bool is_ownership_user(uint8_t* userid)
{
	uint8_t zero_value[sizeof(device_infor.ShortUserID)] ={0x00};
	if(memcmp(device_infor.ShortUserID,userid,sizeof(device_infor.ShortUserID))==0 && \
		 memcmp(device_infor.ShortUserID,zero_value,sizeof(device_infor.ShortUserID))!=0 )
		return true ;
	else 
		return false ;
	
	
	
}
void clear_take_ownership_count()
{
	take_ownership_count = 0;
}
void clear_join_session_count()
{
	join_session_count = 0;
}
	
bool is_sharing_uesr(uint8_t* userid)
{
	uint8_t zero_value[sizeof(device_infor.ShortUserID)] ={0x00};
	if(memcmp(userid,zero_value,sizeof(device_infor.ShortUserID))!=0 )
	return true ;
	else 
	return false ;
	
}


static uint8_t set_take_ownership_user(bluetooth_connection_status_task_t *bluetooth_connection_status_task,header_packet_t *head_pack)
{
	  uint8_t ret = 0;
	 				   		if(insert_session_sharing_user(head_pack->ShortUserID))
									{
										memcpy(device_infor.ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID));
										memcpy(bluetooth_connection_status_task->ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID));
										send_app_take_ownership_response(bluetooth_connection_status_task, 0x01);
										session_status = SESSION_READY ;
										bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
										ret = 1;
										NRF_LOG_INFO("take_ownership_request ack 0x01 device_info:");
										NRF_LOG_HEXDUMP_INFO(device_infor.ShortUserID, sizeof(device_infor.ShortUserID));
									}else 
									{
											send_app_take_ownership_response(bluetooth_connection_status_task, 0x00);
										  ret=0 ;
										  NRF_LOG_INFO("insert session_sharing_user_list failed");	
									}
									bluetooth_connection_status_task->rv_lock = 0 ;
	   return ret ;
}


uint8_t recv_app_take_ownership_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len)
{
    uint16_t data_packent_length;
    uint8_t ret = 0;
	  uint8_t zero_value[5]={0x00};
    bluetooth_connection_status_task_t *bluetooth_connection_ownership_task;

    if (take_ownership_count != 0)
    {
        send_app_take_ownership_response(bluetooth_connection_status_task, 0x0);
			  bluetooth_connection_status_task->rv_lock = 0 ;
			  user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
        return 0;
    }
    else
    {
        take_ownership_count++;
        current_take_ownership_status_task = bluetooth_connection_status_task;
        data_packent_length = check_the_data_packet(bluetooth_connection_status_task,p_data, rev_len);

        if (data_packent_length == 0)
        {
            header_packet_t *head_pack = (header_packet_t *)p_data;
					  if( memcmp (head_pack->ShortUserID,zero_value,sizeof(head_pack->ShortUserID)) != 0) 
					  {
								if (memcmp(device_infor.ShortUserID, zero_value, sizeof(device_infor.ShortUserID)) == 0) // If the user is not a user with control rights.
								{
                   // Set the user as a user with control rights.
									  ret = set_take_ownership_user(bluetooth_connection_status_task,head_pack);
								
								
							  }
								else if (memcmp(device_infor.ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID)) != 0) // If the user is not a user with control rights.
								{
										// Set the user as a user with control rights.
										// Send a control request command to the user with control rights to request control.
										bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
									  if (bluetooth_connection_ownership_task != NULL)
										{
                    send_relinquish_ownership_request_to_app(bluetooth_connection_ownership_task, head_pack->ShortUserID,sizeof(head_pack->ShortUserID));
									  NRF_LOG_INFO("take_ownership_request send_relinquish_ownership_request_to_app usid ");
										NRF_LOG_HEXDUMP_INFO(head_pack->ShortUserID, sizeof(head_pack->ShortUserID));
                    ret = 2;
										}
										else if (bluetooth_connection_ownership_task == NULL)
										{
//											
												ret = set_take_ownership_user(bluetooth_connection_status_task,head_pack);
										}
							 }
							 else
							 {
										// .This user is already a user with control rights.
										NRF_LOG_INFO("take_ownership_request This user is already a user with control rights ");
										send_app_take_ownership_response(bluetooth_connection_status_task, 0x01);
										bluetooth_connection_status_task->wait_apply_for_permission_command_time_count =0 ;
								    bluetooth_connection_status_task->rv_lock = 0 ;
										ret = 1;
							 }
							 
					 }
					else
					{
									
									send_app_take_ownership_response(bluetooth_connection_status_task, 0x00);
						      bluetooth_connection_status_task->rv_lock = 0 ;
									ret = 0;
									NRF_LOG_INFO("userid is zero");

					}
						
						
        }
        else
        {
					  NRF_LOG_INFO("take_ownership_request data have error ");
            send_app_take_ownership_response(bluetooth_connection_status_task, 0x0);
					  bluetooth_connection_status_task->rv_lock = 0 ;

            ret = 0;
        }
    }
    if (ret == 2)
    {
        // Switch the current connection status to pending control request acknowledgment state.
			  NRF_LOG_INFO("user_bluetooth_trans_status_wait_relinquish_response ");
        user_bluetooth_trans_status_wait_relinquish_response(bluetooth_connection_status_task);
    }
    else
    {
        // Switch the current connection status to idle state.
			  
        take_ownership_count = 0;
        current_take_ownership_status_task = NULL;
			  if(ret==1){
			//  NRF_LOG_INFO("user_bluetooth_trans_status_to_online_idle ");
        user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
				}else 
				user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
    }
		
		return ret ;
}

static void send_response_data_packet(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t cmd, uint8_t result)
{
    uint8_t len = 0;
    set_response_packet_head(bluetooth_connection_status_task->send_buf, cmd, 1,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    bluetooth_connection_status_task->send_buf[len++] = result; // status  success

    uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
    bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
    bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
    NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	  bluetooth_connection_status_task->send_buf_len =len ;
    ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}

void send_app_take_ownership_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result)
{
    send_response_data_packet(bluetooth_connection_status_task, TAKE_OWNERSHIP_RESPONSE, result);
}

void recv_app_release_ownership_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len)
{
    uint16_t data_packent_length;
 	  uint8_t  zero_value[5]={0x00};
    data_packent_length = check_the_data_packet(bluetooth_connection_status_task,p_data, rev_len);
    if (data_packent_length == 0)
    {
        header_packet_t *head_pack = (header_packet_t *)p_data;
        if (memcmp(device_infor.ShortUserID, head_pack->ShortUserID, sizeof(device_infor.ShortUserID)) != 0) // If the user is not a user with control rights.
        {
            send_app_release_ownership_response(bluetooth_connection_status_task, 0x0); //. This user is not a user with control rights.
					  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
					  NRF_LOG_INFO("release_ownership failed user id is not a user with control rights ");
        }
        else
        {
					
					   
			
					if(memcmp(head_pack->ShortUserID,bluetooth_connection_status_task->ShortUserID,sizeof(bluetooth_connection_status_task->ShortUserID)) == 0 )
					 {
	          // Release the user's control rights.
					  clear_session_sharing_user(bluetooth_connection_status_task->ShortUserID);
            memcpy(device_infor.ShortUserID, zero_value, sizeof(device_infor.ShortUserID));
            memcpy(bluetooth_connection_status_task->ShortUserID, zero_value, sizeof(device_infor.ShortUserID));
					 
					  session_status = FREE ;
            send_app_release_ownership_response(bluetooth_connection_status_task, 0x01);
					  user_bluetooth_trans_status_to_quit_session(bluetooth_connection_status_task);
					  NRF_LOG_INFO("release_ownership  ok ");
					 }
					 else 
					 {
						 send_app_release_ownership_response(bluetooth_connection_status_task, 0x0); //. This user is not a user with control rights.
						 user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
						 NRF_LOG_INFO("the user id is different from the connected user id ");
        
						 
					 }
						 
        }
    }
    else
    {
        send_app_release_ownership_response(bluetooth_connection_status_task, 0x0);
			  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
			  NRF_LOG_INFO("recv_app_release_ownership_request response 0 data have error ");
    }

    
}
void send_app_release_ownership_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result)
{

    send_response_data_packet(bluetooth_connection_status_task, RELEASE_OWNERSHIP_RESPONSE, result);
}

void send_relinquish_ownership_request_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *userid,uint8_t userid_len)
{
   
	  uint8_t len = 0;
    set_response_packet_head(bluetooth_connection_status_task->send_buf, RELINQUISH_OWNERSHIP_REQUEST, userid_len,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    memcpy(&bluetooth_connection_status_task->send_buf[len],userid,userid_len);
	  len+=userid_len ;

    uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
    bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
    bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
    NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	  bluetooth_connection_status_task->send_buf_len =len ;
    ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
	
}
/*
After the current user gives up control, it is decided whether 
to stay in the shared user list and continue sharing the session or
exit the sharing based on the results sent by the app control user

*/

static void current_ownership_user_state_processing(bluetooth_connection_status_task_t *bluetooth_connection_status_task,RELINQUISH_OWNERSHIP_RESPONSE_RESULT  result)
{
	uint8_t zero_value[5]={0x00};
	switch(result)
	{
		case 	RELINQUISH_OWNERSHIP :                 // no session configured or started
		case  RELINQUISH_OWNERSHIP_LEAVE_SESSION :   // session configured and started
		clear_session_sharing_user(bluetooth_connection_status_task->ShortUserID);
		memcpy(bluetooth_connection_status_task->ShortUserID, zero_value, sizeof(device_infor.ShortUserID));	
		user_bluetooth_trans_status_to_quit_session(bluetooth_connection_status_task);
		break ;
    case  RELINQUISH_OWNERSHIP_STAY_IN_SESSION :  // session configured and started
    break ;
		default:
    break ;
		
	}
	
}

void recv_app_relinquish_ownership_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len)
{
    uint16_t data_packent_length;
    data_packent_length = check_the_data_packet(bluetooth_connection_status_task,p_data, rev_len);
    if (data_packent_length == 1)
    {
			  NRF_LOG_INFO("recv_app_relinquish_ownership_response data packet good");
        take_ownership_count = 0;
        m_relinquish_ownership_response_result =(RELINQUISH_OWNERSHIP_RESPONSE_RESULT) p_data[sizeof(header_packet_t)];
        if (m_relinquish_ownership_response_result < NO_RELINQUISHING_OWNERSHIP) // elinquishment of control application acknowledged with success
        {
					  header_packet_t *head_pack = (header_packet_t *)current_take_ownership_status_task->rev_buf;
            
					  if(insert_session_sharing_user(head_pack->ShortUserID)) {
					
            memcpy(current_take_ownership_status_task->ShortUserID, head_pack->ShortUserID,sizeof(head_pack->ShortUserID));
					
            memcpy(device_infor.ShortUserID, current_take_ownership_status_task->ShortUserID, sizeof(device_infor.ShortUserID));
						current_take_ownership_status_task->wait_apply_for_permission_command_time_count =0 ;
							
            send_app_take_ownership_response(current_take_ownership_status_task, 0x01);
						current_take_ownership_status_task->rv_lock =0 ;
					  NRF_LOG_INFO("recv_app_relinquish_ownership_response---- send app take ownership response 0x01 device userid: ");
					 	NRF_LOG_HEXDUMP_INFO(device_infor.ShortUserID,sizeof(device_infor.ShortUserID));
					  current_ownership_user_state_processing(bluetooth_connection_status_task,m_relinquish_ownership_response_result);
						user_bluetooth_trans_status_to_online_idle(current_take_ownership_status_task);
						} else {
						
						send_app_take_ownership_response(current_take_ownership_status_task, 0x00);
						 user_bluetooth_trans_status_wait_apply_for_permission_command(current_take_ownership_status_task);
							current_take_ownership_status_task->rv_lock =0 ;
						 NRF_LOG_INFO("insert session_sharing_user_list failed");	
						}
							
            
        }
        else if (m_relinquish_ownership_response_result == NO_RELINQUISHING_OWNERSHIP)
        {
            send_app_take_ownership_response(current_take_ownership_status_task, 0x00);
					  NRF_LOG_INFO("recv_app_relinquish_ownership_response---- send app take ownership response 0x00 ");
            //user_bluetooth_trans_status_to_online_idle(current_take_ownership_status_task);
					  user_bluetooth_trans_status_wait_apply_for_permission_command(current_take_ownership_status_task);
					 current_take_ownership_status_task->rv_lock =0 ;
        }
    }
    else
    {
        //  If there is an error in the data, do nothing and let the timeout mechanism handle it
			 user_bluetooth_trans_status_wait_apply_for_permission_command(current_take_ownership_status_task);
			 	current_take_ownership_status_task->rv_lock =0 ;
    }

   
}

/*

*/
static void send_join_session_request_to_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *userid,uint8_t userid_len)
{
   uint8_t len = 0;
    set_response_packet_head(bluetooth_connection_status_task->send_buf, JOIN_SESSION_REQUEST, userid_len,bluetooth_connection_status_task->rv_MsgID);
    len = sizeof(response_header_packet_t);
    memcpy(&bluetooth_connection_status_task->send_buf[len],userid,userid_len);
	  len+=userid_len ;

    uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
    bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;              //
    bluetooth_connection_status_task->send_buf[len++] = temp_crc;                   //
    NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
    ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}
/*
The device receives a request from a user to join a session
*/

void recv_app_join_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len)
{
    uint16_t data_packent_length;
    bluetooth_connection_status_task_t *bluetooth_connection_ownership_task;

    if (join_session_count != 0)
    {
        send_app_join_session_response(bluetooth_connection_status_task, 0x0);
      //  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
			 user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
    }
    else
    {
        join_session_count++;
        current_join_session_status_task = bluetooth_connection_status_task;
        data_packent_length = check_the_data_packet(bluetooth_connection_status_task,p_data, rev_len);
			  
        if (data_packent_length == 0)
        {
            // Forward the sharing permission request to the ownership
            bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
            if (bluetooth_connection_ownership_task != NULL)
            {
               // memcpy(bluetooth_connection_ownership_task->send_buf, bluetooth_connection_status_task->rev_buf, bluetooth_connection_status_task->rev_buf_len);
							  header_packet_t *head_pack = (header_packet_t *)p_data;
                send_join_session_request_to_app(bluetooth_connection_ownership_task,head_pack->ShortUserID,sizeof(head_pack->ShortUserID));
                user_bluetooth_trans_status_wait_join_session_response(bluetooth_connection_status_task);
            }
            else if (bluetooth_connection_ownership_task == NULL)
            {
                send_app_join_session_response(bluetooth_connection_status_task, 0x0);
             //   user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
						    user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
            }
        }
        else
        {
            send_app_join_session_response(bluetooth_connection_status_task, 0x0);
         //   user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
					  user_bluetooth_trans_status_wait_apply_for_permission_command(bluetooth_connection_status_task);
        }
    }
}
/*
The device receives the session join response sent by the user with control authority
*/

void recv_app_join_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len)
{
    uint16_t data_packent_length;
    uint8_t result;
    data_packent_length = check_the_data_packet(bluetooth_connection_status_task,p_data, rev_len);
    if (data_packent_length == 1)
    {
        join_session_count = 0;
        
        result = p_data[sizeof(header_packet_t)];
        if (result == 0x01) // elinquishment of control application acknowledged with success
        {
            // memcpy(device_infor.ShortUserID, current_take_ownership_status_task->ShortUserID, sizeof(device_infor.ShortUserID));
					  header_packet_t *head_pack = (header_packet_t *)current_join_session_status_task->rev_buf;
					
					  if(insert_session_sharing_user(head_pack->ShortUserID)) {
							
            send_app_join_session_response(current_join_session_status_task, 0x01);
            memcpy(current_join_session_status_task->ShortUserID, head_pack->ShortUserID,sizeof(current_join_session_status_task->ShortUserID));
						current_join_session_status_task->wait_apply_for_permission_command_time_count =0 ;
            // Add the sharing user to the sharing list
					   NRF_LOG_INFO("join session userid");
						user_bluetooth_trans_status_to_online_idle(current_join_session_status_task);
						current_join_session_status_task->rv_lock =0;
					 	NRF_LOG_HEXDUMP_INFO(current_join_session_status_task->ShortUserID,sizeof(device_infor.ShortUserID));
						}else {
							
						 send_app_join_session_response(current_join_session_status_task, 0x00);	
						  NRF_LOG_INFO("insert session_sharing_user_list failed");	
							current_join_session_status_task->rv_lock =0;
						user_bluetooth_trans_status_wait_apply_for_permission_command(current_join_session_status_task);
						}
					
        }
        else
        {
            send_app_join_session_response(current_join_session_status_task, 0x0);
					  current_join_session_status_task->rv_lock =0;
					  user_bluetooth_trans_status_wait_apply_for_permission_command(current_join_session_status_task);
        }
        
    }
    else
    {
        //  If there is an error in the data, do nothing and let the timeout mechanism handle it
			   current_join_session_status_task->rv_lock =0;
			   user_bluetooth_trans_status_wait_apply_for_permission_command(current_join_session_status_task);
    }
  
}

/*
The device sends a response to the user applying to join the session

*/
void send_app_join_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result)
{

    send_response_data_packet(bluetooth_connection_status_task, JOIN_SESSION_RESPONSE, result);
}

void recv_app_quit_session_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t rev_len)
{
    uint16_t data_packent_length;
	//	uint8_t  zero_value[5]={0x00};
    data_packent_length = check_the_data_packet(bluetooth_connection_status_task,p_data, rev_len);
    if (data_packent_length == 0)
    {
			  header_packet_t *head_pack = (header_packet_t *)p_data;
			
			  if(memcmp(head_pack->ShortUserID,bluetooth_connection_status_task->ShortUserID,sizeof(bluetooth_connection_status_task->ShortUserID)) == 0 )
			  {
					  if(memcmp(device_infor.ShortUserID,head_pack->ShortUserID,sizeof(device_infor.ShortUserID)) !=0 )  
					  {
								// Remove the user from the sharing list
								clear_session_sharing_user(bluetooth_connection_status_task->ShortUserID);
								memset(bluetooth_connection_status_task->ShortUserID, 0x00, sizeof(bluetooth_connection_status_task->ShortUserID));
								send_app_quit_session_response(bluetooth_connection_status_task, 0x01);
							  NRF_LOG_INFO("user quit ok");
  							user_bluetooth_trans_status_to_quit_session(bluetooth_connection_status_task);
							
						}else {
							 NRF_LOG_INFO("user is ownership user, not quit");
							 send_app_quit_session_response(bluetooth_connection_status_task, 0x00);
							 user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
							
							
						}
						
					
				}else 
				{
					 NRF_LOG_INFO("uiserid error not quit");
					 send_app_quit_session_response(bluetooth_connection_status_task, 0x00);
					 user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
					
				}
			 
			  //print_all_sharing_user();
    }
    else
    {
			  NRF_LOG_INFO("check data packet error not quit");
        send_app_quit_session_response(bluetooth_connection_status_task, 0x00);
			  user_bluetooth_trans_status_to_online_idle(bluetooth_connection_status_task);
    }
    
}
void send_app_quit_session_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t result)
{
    send_response_data_packet(bluetooth_connection_status_task, QUIT_SESSION_RESPONSE, result);
}

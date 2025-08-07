

#include "mcu.h"
#include "app_cfg_cmd.h"
#include "app_task.h"
#include "dpid_map_c.h"
#include "mt_gpio.h"
#include "mcu_api.h"
#include "session.h"

#define NRF_LOG_MODULE_NAME app_task_log
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#include "fstorage.h"
#include "mt_ble_adv.h"

//#define APP_TASK_DEBUG_OUT 

extern uint32_t ble_ctcs_s_send_handler(uint8_t *p_data, uint16_t len, uint16_t uuid) ;

#ifndef USE_STATE_MACHINE

#define POWER_ON_AFTER_SENDING_CYCLE 10 //10*100 1秒中
#define MCU_RESPONDS_SEND_CYCLE 150     //15 秒


#define TIME_OUT_NUM 10 
#define CMD_SEND_NUN 3
extern uint32_t get_system_time_tick(void) ;

static uint32_t temp_time_tick = 0;
static uint16_t time_cnt = 0 ;
static uint16_t send_count =0 ;
static uint32_t cmd_status_none_time_count=0;

//上传数据给app 后的延时
static uint32_t send_data_app_delay_time_tick =0 ;
static uint32_t send_data_app_delay_time=0 ;


static bool         user_connect_state =false ;       //用户发来第一条指令时表示进入用户连接状态

uint16_t m_current_conn_handle ;
/*
启动状态查询命令
*/
void start_status_query(void)
{
	  send_cmd_state = CMD_STATUS_START ;
		current_cmd_data.cmd  = 	STATE_QUERY_CMD;  
	  Response_Feature_Id_list.update_feature_num = 0;
	  
}
/*
启动设置
*/
static void set_cmd_state_value(unsigned char feature_id)
{
	unsigned char dpid = get_dpid(feature_id) ;
	memset((void*)&current_cmd_data,0,sizeof(cmd_data_description));
	send_cmd_state = CMD_STATUS_START ;
	current_cmd_data.cmd  = 	DATA_QUERT_CMD;  //涂鸦06命令，进行数据设置
	current_cmd_data.dpID =	  dpid ;
	current_cmd_data.data.int_data=	*((unsigned char*)get_set_value_adr(feature_id) );
	
	
	
}

/*
<1> 接收到手机app的set 命令后，完成数据解析后会调用start_dpid_set函数，
启动涂鸦协议DATA_QUERY_CMD(06)set 命令（调用 set_cmd_state_value函数),进行控制操作。

<2> 在涂鸦协议命令发送状态机mcu_cmd_send_task()中,
   涂鸦06命令正确应答后也会调用该函数，判断 Set_Feature_Id_list  中是否还有set 命令，
   如果没有了，则设置   dpid_setting_status 为  DPID_SET_ACK_OK,
   如果还有set  命令 则继续调用 start_dpid_set()函数，进行设置命令

<3> 在涂鸦协议命令发送状态机mcu_cmd_send_task()中，根据send_cmd_state状态进行涂鸦协议状态机管理， 
根据通讯状态结果 设置 dpid_setting_status 的状态 

<4> 在 app_task 中，再根据  dpid_setting_status 的状态回复应答給手机app 


<5> 在配置一个session后，启动session 后则会进行 session set命令分析，填充Set_Feature_Id_list 后，启动

*/



void start_dpid_set(void)
{
	unsigned char feature_id;
	static uint16_t set_feature_id_index =0;
	
	
	switch(dpid_setting_status)
	{
		case DPID_SET_STATE_NONE:
		Response_Feature_Id_list.update_feature_num=0;
		set_feature_id_index =0;
		feature_id = Set_Feature_Id_list.feature_id[set_feature_id_index++] ;
		dpid_setting_status	= DPID_SET_PROCESSING;
		
		if(feature_id ==DEVICE_CODE_FEATURE_ID ) //设置设备代码,
		{
				if(fstorge_write_device_code(SettingData.device_code)==false ) //往flash 写入 device_code ,在falsh写入成功事件中会device_code_write_finished_proc(void)
				{ 
				   dpid_setting_status = DPID_SET_ACK_DATA_ERROR ;
				}
					
		}
		else 
		set_cmd_state_value(feature_id);
	
		
			
	
		
		break ;
		case DPID_SET_PROCESSING:
		if(set_feature_id_index <Set_Feature_Id_list.update_feature_num)
		{
			feature_id = Set_Feature_Id_list.feature_id[set_feature_id_index++] ;
			set_cmd_state_value(feature_id);
		}
		else {
			dpid_setting_status = DPID_SET_ACK_OK ;
			Set_Feature_Id_list.update_feature_num = 0;
			set_feature_id_index =0;
	//	dpid_setting_status	= DPID_SET_STATE_NONE;
	//	device_Set_Response(true);	
			//在这边再上一个状态主动上传	
			
		}
		
	
		break ;
		default:
			
		break ;
	}
	
}

void startup_features_set_cmd(void)
{
	 start_dpid_set();
}


//在falsh写入成功事件中会device_code_write_finished_proc(void)
void device_code_write_finished_proc(bool result)
{
	  uint16_t device_code ;
	  if(result){
				device_code = fstorge_read_device_code(); 
				if(SettingData.device_code == device_code ){
				update_adv_data_device_code(device_code);
				start_dpid_set();
				}else //写入后读到的值与设置值不相符
				dpid_setting_status = DPID_SET_ACK_DATA_ERROR ;
	}else 
	dpid_setting_status = DPID_SET_ACK_DATA_ERROR ;
	
}



bool  get_user_connect_state(void)
{
  	return user_connect_state ;
}
void  clear_usr_connect_state(void)
{
	    user_connect_state = false ;
}

void set_user_connect_state(bool value )
{
	    user_connect_state = value ;
}

/* 功能点设置  06命令*/

void  data_query_cmd_ack_timeout_cb(void)
{
		
	  dpid_setting_status = DPID_SET_ACK_TIMEOUT ;
	  
}

void  data_query_cmd_ack_ok_cb(void) 
{
	
	   start_dpid_set();
}

/* 功能点状态查询 08命令 */

void state_query_cmd_ack_timeout_cb(void)
{
	
	  send_failed_to_app(m_current_conn_handle);
	
}
void state_query_cmd_ack_ok_cb(void)
{
		  uint8_t send_feature_id_num=0; 
	    switch(App_Request_Data_Type)
					{
			    	case BLUETOOTH_CON_AFTER_SEND_ALL_ID:  //蓝牙连接后，蓝牙板主动发送一次08命令查询所有状态，返回后，将所有feature_id 状态上传
						
						send_feature_id_num = set_all_upload_feature_id();
						send_feature_id_data_to_app(m_current_conn_handle,Get_Request_id,send_feature_id_num,ACTIVE_UPLOAD_CMD);
						break ;
						
						case REQUEST_ALL_FEATURE_ID:        // Get Request 命令，发送08命令，将08命令请求的状态上传
						//再手动增加一条featureid 0x00 connect_state 
						Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] = Connect_Status_ID ;
						send_feature_id_data_to_app(m_current_conn_handle,Response_Feature_Id_list.feature_id,Response_Feature_Id_list.update_feature_num,GET_REQUEST_ACK_CMD);
						
						
						break ;
						
						case REQUEST_APPOINT_FEATURE_ID:         // Get Request 命令，发送08命令，将Get Request命令中的feature_id 数据上传，
																									   //命令中的ID状态除了08命令能返回的状态，其他状态是在其他时候被更新
						send_feature_id_data_to_app(m_current_conn_handle,Get_Request_id,allRequestID_Num,GET_REQUEST_ACK_CMD)	;	
						
						break ;
						
						
					}
	
}
	

void static none_status_process(void)
{
	      if( temp_time_tick == get_system_time_tick() ) {
					return ;
					
				}
         temp_time_tick = get_system_time_tick(); 
		     cmd_status_none_time_count ++ ;
		     
		     switch(heart_send_status)
				 {
					 case SEND_AFTER_POWER_ON :
					 if(	cmd_status_none_time_count%POWER_ON_AFTER_SENDING_CYCLE	== 0 )
					 {
						 send_cmd_state = CMD_STATUS_START ;
				     current_cmd_data.cmd  = 	HEAT_BEAT_CMD;
					 }
					 
					 break ;
					 
					 case SEND_AFTER_MCU_RESPONDS:
						if(	cmd_status_none_time_count%MCU_RESPONDS_SEND_CYCLE	== 0 )
					 {
						 send_cmd_state = CMD_STATUS_START ;
				     current_cmd_data.cmd  = 	HEAT_BEAT_CMD;
						 cmd_status_none_time_count = 0;
						
					 }else if( cmd_status_none_time_count%(MCU_RESPONDS_SEND_CYCLE-50)	== 0 ) 
					 {
						// send_cmd_state = CMD_STATUS_START ;
				    // current_cmd_data.cmd  = 	PRODUCT_INFO_CMD;
						 
					 }
					 
					 break ;
					 default:
					 break ;
					 
				 }
	
}

void static start_status_process(void)
{
	       switch(current_cmd_data.cmd)
		     {
					 case STATE_QUERY_CMD:
						  Response_Feature_Id_list.update_feature_num=0;
						  send_status_query();
					 
					 #ifdef APP_TASK_DEBUG_OUT
					    NRF_LOG_INFO("send_status_query");
					 #endif 
					 
					    send_cmd_state = CMD_STATUS_SEND_IN ;
					 break ;
					 
					 case HEAT_BEAT_CMD : // send heartbeat command
						 #ifdef APP_TASK_DEBUG_OUT
						 NRF_LOG_INFO("send heard cmd");
						 #endif 
					 
						 send_heart_to_mcu();
					   send_cmd_state = CMD_STATUS_SEND_IN ;
					   break ;
					 
					 case PRODUCT_INFO_CMD :  //red product information 
					    get_product_info_cmd();
						  send_cmd_state = CMD_STATUS_SEND_IN ;
					 
					    break ;
			     case DATA_QUERT_CMD:
						 cmd_status_none_time_count = 0;
						 
					   if(current_cmd_data.dpID !=  DPID_CONNECT_STATE){ 
				     send_dpID_cmd(&current_cmd_data);
			       send_cmd_state = CMD_STATUS_SEND_IN ;
						 }
						 else {
							 //是App 连接状态命令
							 user_connect_state  = (current_cmd_data.data.byte_data==1 ) ?  true: false ;
							 
							 if(!user_connect_state)
							 CONNCET_BOND_LED_OFF();
							 
							 send_cmd_state = CMD_STATUS_OK ;
							 
						 }
						 
			      
					 
			       break ;
			
			    default:
			       break ;
			
	    	 }
	   temp_time_tick = get_system_time_tick();
		 time_cnt = 0;
	
}

void static send_in_status_process(void)
{
			if(get_system_time_tick() == temp_time_tick)   return; //100ms 检查一次
		
		  temp_time_tick = get_system_time_tick();
		  
		  //NRF_LOG_INFO("time_cnt==%d",time_cnt);
		  if(++time_cnt>=TIME_OUT_NUM){
			
						 if(  ++send_count >=	CMD_SEND_NUN ) {
						  send_cmd_state = CMD_STATUS_TIMEOUT ;
				
						  }else {
										
							        send_cmd_state = CMD_STATUS_START ;
						          time_cnt =0 ;	
											
											
						}
		   }
	
}

void static timeout_status_process(void)
{
		  time_cnt =0;
		  send_count=0;
		  send_cmd_state = CMD_STATUS_NONE;
		   
		  switch(current_cmd_data.cmd)
			{
				case HEAT_BEAT_CMD:
				heart_send_status = SEND_AFTER_POWER_ON ;
				break ;
				
				case STATE_QUERY_CMD:  //08状态查询命令
				state_query_cmd_ack_timeout_cb();	
				break ;
				
				case DATA_QUERT_CMD:
				data_query_cmd_ack_timeout_cb();	
				break ;
				
				
				default:
				break ;
			}
	
	
}
void static ok_status_process(void)
{
			time_cnt =0;
		  send_count=0;
		  send_cmd_state = CMD_STATUS_NONE;
	    
		 
		  switch(current_cmd_data.cmd)
			{
				case HEAT_BEAT_CMD:
				 heart_send_status = SEND_AFTER_MCU_RESPONDS ;
				 #ifdef APP_TASK_DEBUG_OUT
				 NRF_LOG_INFO("hear_beat ok");
				 #endif 
				
				break ;
				
				case PRODUCT_INFO_CMD:
				#ifdef APP_TASK_DEBUG_OUT
				NRF_LOG_INFO("product_info: %s",product_info);
				#endif 
				
				
				break ;
				
				case STATE_QUERY_CMD:  //08状态查询命令
					
					#ifdef APP_TASK_DEBUG_OUT
				  NRF_LOG_INFO("state_query ok");	
					#endif 
				
				  state_query_cmd_ack_ok_cb();
				
				break ;
				
				case DATA_QUERT_CMD:
					current_cmd_data.dpID =	  0 ;
				  data_query_cmd_ack_ok_cb();
						break ;
				
				
			} //switch(current_cmd_data.cmd)
		
	
	
}
void static ack_check_error_status_process(void)
{
		time_cnt =0;
		send_count=0;
		send_cmd_state = CMD_STATUS_NONE;
	
		#ifdef APP_TASK_DEBUG_OUT
	  NRF_LOG_INFO("ack_check_error");	
		#endif 
	
	    switch(current_cmd_data.cmd)
			{
				case HEAT_BEAT_CMD:
				heart_send_status = SEND_AFTER_POWER_ON ;
				break ;
				
				case STATE_QUERY_CMD:  //08状态查询命令
				 send_failed_to_app(m_current_conn_handle);
				break ;
				
				case DATA_QUERT_CMD:
				 dpid_setting_status = DPID_SET_ACK_DATA_ERROR ;
				break ;
				
				
				default:
				break ;
			}
	
	 
	
}

void  mcu_cmd_send_task(void)
{
	
	switch(send_cmd_state)
	{
		case CMD_STATUS_NONE :
		    none_status_process();
		  	break ;
		
		case CMD_STATUS_START:
		    start_status_process();
			 break ;
		
		case CMD_STATUS_SEND_IN:
		  	send_in_status_process();
		
		   break ;	
	  case CMD_STATUS_TIMEOUT:
		   timeout_status_process();
		
			 break ;
		
		case CMD_STATUS_OK: //有接收到正确应答
	    ok_status_process();
	   	break ;
		
		case CMD_STATUS_ACK_CHECK_ERROR:
			ack_check_error_status_process();
			break ;
		
	}
	
	
}

static uint32_t run_led_time_tick =0 ;
static uint32_t run_led_time_count=0;
static HEART_SEND_STATUS  prev_heart_send_status =  HEART_SEND_NONE ;
/*
根据心跳状态，控制run led 灯的状态，心跳正常，led灯每秒闪烁一次，或者15s 闪烁一次
注意，根据正常状态下每隔15S发送一次心跳，因为如果UART断先，将要15S状态才会改变
同时，记录下与uart 的连接状态保存到 Get_Response_Pack.connction_status.attribute

*/
extern bool mt_is_ble_connected(void);

static void run_state_task(void)
{
	if(prev_heart_send_status != heart_send_status) //状态变化时
	{
		prev_heart_send_status = heart_send_status;
		run_led_time_tick = get_system_time_tick() ;
		run_led_time_count = 0;
		RUN_LED_OFF() ;
		//CONNCET_BOND_LED_OFF() ;
		switch(heart_send_status)
		{
			case SEND_AFTER_POWER_ON :
	//		Get_Response_Pack.connction_status.attribute = 0 ; 
			break ;
			
			case SEND_AFTER_MCU_RESPONDS:
	//		Get_Response_Pack.connction_status.attribute = 1 ; 	
			break ;
			
			
		}
		if(get_user_connect_state()) {  //将运行状态主动上传
		  Get_Request_id[0]= Connect_Status_ID;
			send_feature_id_data_to_app(Get_Request_id, 1,ACTIVE_UPLOAD_CMD) ;
				
			}
		
	}
	
	switch(heart_send_status)
	{
		case HEART_SEND_NONE:
		run_led_time_tick = get_system_time_tick() ;
		run_led_time_count = 0;
		break ;
		
		case SEND_AFTER_POWER_ON :   //没有心率应答，也就是控制器无回复,15s闪烁一次
    if( get_system_time_tick() == run_led_time_tick ) return ;
		run_led_time_tick = get_system_time_tick() ;
		run_led_time_count++ ;
		
		if(run_led_time_count>=10) //该状态正常保持1秒针记录下状态
		{
			if(Get_Response_Pack.connction_status.attribute!=0) //记录下连接断开的状态，并主动上传
			{
			    Get_Response_Pack.connction_status.attribute =0 ;
					#ifdef APP_TASK_DEBUG_OUT
				  NRF_LOG_INFO("Controller disconnected");
					#endif 
			}
		}
		
		if(run_led_time_count>=150 && run_led_time_count<=160)
		{
			RUN_LED_ON() ;
			
		}else if(run_led_time_count>160 )
		{
			RUN_LED_OFF() ;
			run_led_time_count =0 ;
		}
		
		break ;
		
		case  SEND_AFTER_MCU_RESPONDS  :     //有心率数据应答，1s 闪烁一次
     if( get_system_time_tick() == run_led_time_tick ) return ;
		run_led_time_tick = get_system_time_tick() ;		
		run_led_time_count++ ;
		
		if(run_led_time_count>=10)  //该状态正常保持1秒针记录下状态
		{
			if(Get_Response_Pack.connction_status.attribute!=1) //记录下连接状态，并主动上传
			{
			    Get_Response_Pack.connction_status.attribute =1 ;
				  NRF_LOG_INFO("Controller connected");
				  start_status_query();
				
			}
		}
		
		
		if(run_led_time_count>=10 && run_led_time_count<=20)
		{
			RUN_LED_ON() ;
		}else if(run_led_time_count>20 )
		{
			RUN_LED_OFF() ;
			run_led_time_count =0 ;
		}
		
		
		break ;
		
		
	}
	  
	
}


/*
 由于蓝牙与手机连接时，蓝牙会将连接状态发送给桑拿控制，桑拿控制器得到连接状态后会主动将数据状态传上来
						      因此上面并不需要发送状态查询命令
		 
   				 
蓝牙刚刚连接时，检测UART连接状态，如果uart 没有连接，则延时200ms 后将连接状态发送给手机

*/

bool bluetooth_connect_status = false ;
static void  user_connect_after_processing(void)
{
	 static  uint32_t bluetooth_connect_timetick =0; 
	 static  uint32_t bluetooth_connect_time_count =0;
	 		    
			   	if(get_user_connect_state()){
							 /*   由于蓝牙与手机连接时，蓝牙会将连接状态发送给桑拿控制，桑拿控制器得到连接状态后会主动将数据状态传上来
						      因此上面并不需要发送状态查询命令
						 */		 
   				 
						    //蓝牙刚刚连接时，检测UART连接状态，如果uart 没有连接，则延时200ms 后将连接状态发送给手机
					      switch(bluetooth_connect_timetick)
									 {
										 case 0:
											 bluetooth_connect_timetick = get_system_time_tick() ;
										   break ;
										 
										 default:
											
											if(bluetooth_connect_status)return ;
										  
											if( bluetooth_connect_timetick ==  get_system_time_tick() ) return ;
										 
										  bluetooth_connect_timetick =  get_system_time_tick();
										  
										  bluetooth_connect_time_count ++ ;
										  
											if(bluetooth_connect_time_count > 15 )
											{
												 
												  bluetooth_connect_status = true ;
													if(Get_Response_Pack.connction_status.attribute==0)
													{
														#ifdef APP_TASK_DEBUG_OUT
														 NRF_LOG_INFO("send uart no connect state to app");
														#endif 
														
														Get_Request_id[0]= Connect_Status_ID;
											    	send_feature_id_data_to_app(Get_Request_id, 1,ACTIVE_UPLOAD_CMD) ;
														
													}
												
											}
										 
										 break ;
										 
										 
									 }
					
			  	}else {
							 
						   //NRF_LOG_INFO("bluetooth_connect_status = false");
							 bluetooth_connect_timetick = 0 ;
							 bluetooth_connect_status = false;
						   bluetooth_connect_time_count = 0;
						// CONNCET_BOND_LED_OFF();
							 
				  }
					
				

	
	
}

/*
桑拿控制器状态主动上传


*/

void proactively_upload_processing_procedure(void)
{
	 	 
							if (mt_is_ble_connected() == true) 
							{ 
							   if(upload_update_status == START_UPDATE) //主动上传的feature id 
							   {
//									  if(cts_send_state == TX_COMPLETE) {
//							     send_feature_id_data_to_app(Update_Feature_Id_list.feature_id,Update_Feature_Id_list.update_feature_num,ACTIVE_UPLOAD_CMD);
//									 
//									 #ifdef APP_TASK_DEBUG_OUT
//							     NRF_LOG_INFO("start update feature num =%d",Update_Feature_Id_list.update_feature_num);
//									 #endif 
//									 
//										}
							     Update_Feature_Id_list.update_feature_num =0 ;
							     upload_update_status = UPDATE_NONE ;
							   } 
					    }else {
								 
								  if(upload_update_status == START_UPDATE) //主动上传的feature id 
							   {
							   //  send_feature_id_data_to_app(Update_Feature_Id_list.feature_id,Update_Feature_Id_list.update_feature_num,ACTIVE_UPLOAD_CMD);
									 #ifdef APP_TASK_DEBUG_OUT
							     NRF_LOG_INFO("no connect app start update feature num =%d",Update_Feature_Id_list.update_feature_num);
									 #endif 
									 
							     Update_Feature_Id_list.update_feature_num =0 ;
							     upload_update_status = UPDATE_NONE ;
							   } 
								
								
							}
							
		
}





void app_task(void)
{
	
	
		
	   task_check_input_io();
	   run_state_task();
		 bluetooth_uart_service();
		 mcu_cmd_send_task();
     
	   switch( dpid_setting_status )
		 {
			 case DPID_SET_STATE_NONE:
				 proactively_upload_processing_procedure();	 //状态主动上传处理 (session 版本中已经屏蔽，没有使用)
			   user_connect_after_processing();   //根据uart 与桑拿控制器的连接状态，将连接状态上传給手机( session 版本中已经屏蔽，没有使用) 
			 
			 break ;
			 case DPID_SET_PROCESSING:
			 break ;
			 
			 case DPID_SET_ACK_TIMEOUT:
			 case DPID_SET_ACK_DATA_ERROR:
				device_Set_Response(false)	;
			  dpid_setting_status = DPID_SET_STATE_NONE ;
			  break ;
			 
			 
			 case DPID_SET_ACK_OK:
				 device_Set_Response(true);
			   dpid_setting_status =DPID_SET_ACK_SEND_WAIT ;
			   send_data_app_delay_time_tick =get_system_time_tick() ;
			   send_data_app_delay_time=0 ;
			 
			  break ;
			 case DPID_SET_ACK_SEND_WAIT:
			   if( get_system_time_tick() ==  send_data_app_delay_time_tick ) break ;
 				 send_data_app_delay_time ++ ;
			   
			   send_data_app_delay_time_tick =get_system_time_tick() ; 
			 
			   if(send_data_app_delay_time == 1) //100ms  过后，主动发送 控制器应答给蓝牙的dpid 数据
				 {
					  if(Response_Feature_Id_list.update_feature_num !=0){
					  send_feature_id_data_to_app(m_current_conn_handle,Response_Feature_Id_list.feature_id,Response_Feature_Id_list.update_feature_num,ACTIVE_UPLOAD_CMD);
							
						#ifdef APP_TASK_DEBUG_OUT
	  		    NRF_LOG_INFO("Response_Feature_Id_list num =%d",Response_Feature_Id_list.update_feature_num);
						#endif 
							
			      Response_Feature_Id_list.update_feature_num =0 ;
							
						}else {
						 dpid_setting_status =DPID_SET_STATE_NONE ;
							send_data_app_delay_time =0 ;
						}
					 
				 }else if( send_data_app_delay_time >= 2) //200ms  后清除状态
				 {
							#ifdef APP_TASK_DEBUG_OUT
					    NRF_LOG_INFO("dpid set clearn state to none");
							#endif 
					 
					    dpid_setting_status =DPID_SET_STATE_NONE ;
							send_data_app_delay_time =0 ;
					 
				 }
				 break ;
				 
				 
			 default :
				 break ;
				 
				 
				
		 }
	 
		 session_task_exec();  //session 任务管理
	
}

void custom_protocol_init(void)
{
	#ifdef APP_TASK_DEBUG_OUT
	NRF_LOG_INFO("custom_protocol_init");
	#endif 
	
	mcu_protocol_init();
}


#endif 




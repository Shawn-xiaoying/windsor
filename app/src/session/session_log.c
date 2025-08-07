#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "app_cfg_cmd.h"
#include "mt_ble_service.h"
#include "mt_cst_protocol.h"
#include "ble_ctcs.h"
#include "app_task.h"
#include "session_log_def.h"
#include "session_log.h"
#include "session.h"
#include "bluetooth_connection_management.h"
#include "logging_rw_implent.h"

#define NRF_LOG_MODULE_NAME session_log_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

//uint16_t upload_start_index = 0;
//uint8_t upload_length = 0;
//bool start_upload_flag = false;
//static uint16_t index_pos = 0;
//static uint16_t actual_reading_index = 0;
//static SESSION_LOGGING_RECORD_DATA_SEND_STATE logging_send_state = LOGGING_RECORD_DATA_SEND_NONE;
//extern uint8_t m_send_buf[300];

//static uint16_t proactively_send_index_pos = 1; // Record every save cycle and actively upload to the app ,Variable record sending location

uint8_t m_logging_need_to_record_feature_num = 0;
uint32_t m_logging_record_time_count = 0;
unsigned char m_logging_feature_id_list[MAX_FEATURE_NUM];

// uint32_t									m_session_id=0xffffffff ;       // 一个会话ID
uint8_t m_record_cycle_time = 1;	// 记录间隔,以秒为单位,默认1s
uint16_t m_session_run_time = 3600; // 以秒为单位,最大3600s
uint8_t m_command_type;				// start or stop logging record

static bool m_start_record_flag = false;
//static bool startup_proactively_send_log_flag = false;

extern uint8_t get_feature_log_data(uint16_t read_idnex, uint8_t *out_data_buf);

bool logging_save_ring_buf_is_empty(void)
{
	return (m_logging_save_ring_buf.write_index == m_logging_save_ring_buf.read_index);
}

//void startup_proactively_send_log(void)
//{
//	if (proactively_send_index_pos <= m_logging_save_ring_buf.count)
//	{

//		startup_proactively_send_log_flag = true;
//	}
//}

void startup_proactively_send_log(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
		if (bluetooth_connection_status_task->session_log_send_ctrl_parames.proactively_send_index_pos < m_logging_save_ring_buf.count)
	{

					bluetooth_connection_status_task->session_log_send_ctrl_parames.startup_proactively_send_log_flag = true;
		      NRF_LOG_INFO("startup_proactively_send_log flag");
 	}
	
}

void logging_save_ring_buf_clear_empty(void)
{
	m_logging_save_ring_buf.write_index = 0;
	m_logging_save_ring_buf.read_index = 0;
	m_logging_save_ring_buf.count = 0;
}

bool logging_save_ring_buf_is_full(uint16_t read_index)
{
	return (m_logging_save_ring_buf.write_index + 1) % SAVE_BUF_MAX_LENGTH == read_index;
}

//static void send_session_log(uint16_t conn_handle, uint8_t cmd, uint32_t session_id, uint16_t read_index, uint16_t index_pos)
//{
//	uint8_t len = sizeof(response_header_packet_t);
//	uint8_t payload_len = 0;
//	m_send_buf[len++] = session_id >> 24;
//	m_send_buf[len++] = session_id >> 16;
//	m_send_buf[len++] = session_id >> 8;
//	m_send_buf[len++] = session_id;

//	m_send_buf[len++] = index_pos >> 8;
//	m_send_buf[len++] = index_pos;
//	//   NRF_LOG_INFO("send_session_log read_index=%d,index_pos=%d",read_index,index_pos);
//	payload_len = get_feature_log_data(read_index, &m_send_buf[len]);
//	len += payload_len;
//	payload_len += 6;
//	set_response_packet_head(m_send_buf, cmd, payload_len);
//	uint16_t temp_crc = CalcCrc16(m_send_buf, len); // crc
//	m_send_buf[len++] = temp_crc >> 8;				//
//	m_send_buf[len++] = temp_crc;					//
//	NRF_LOG_HEXDUMP_INFO(m_send_buf, len);
//	ble_ctcs_s_send_handler(conn_handle, m_send_buf, len, CTCS_TRANSPORT_UUID);
//}

/*
msig_type : 1: 代表主动上传log ,应答的消息ID直接为1， 0： 代表应答app的log请求，消息ID 为从app接收到的Msg id
*/
static void send_session_log(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t cmd, uint32_t session_id, uint16_t read_index, uint16_t index_pos,uint8_t msig_type)
{
	uint8_t len = sizeof(response_header_packet_t);
	uint8_t payload_len = 0;
	
	bluetooth_connection_status_task->send_buf[len++] = session_id >> 24;
	bluetooth_connection_status_task->send_buf[len++] = session_id >> 16;
	bluetooth_connection_status_task->send_buf[len++] = session_id >> 8;
	bluetooth_connection_status_task->send_buf[len++] = session_id;

	bluetooth_connection_status_task->send_buf[len++] = index_pos >> 8;
	bluetooth_connection_status_task->send_buf[len++] = index_pos;
	//   NRF_LOG_INFO("send_session_log read_index=%d,index_pos=%d",read_index,index_pos);
	payload_len = get_feature_log_data(read_index, &bluetooth_connection_status_task->send_buf[len]);
	len += payload_len;
	payload_len += 6;
	if(msig_type ==1)
	set_response_packet_head(bluetooth_connection_status_task->send_buf, cmd, payload_len,1);
	else 
	set_response_packet_head(bluetooth_connection_status_task->send_buf, cmd, payload_len,bluetooth_connection_status_task->rv_MsgID);
	
	uint16_t temp_crc = CalcCrc16(bluetooth_connection_status_task->send_buf, len); // crc
	bluetooth_connection_status_task->send_buf[len++] = temp_crc >> 8;				//
	bluetooth_connection_status_task->send_buf[len++] = temp_crc;					//
	NRF_LOG_HEXDUMP_INFO(bluetooth_connection_status_task->send_buf, len);
	bluetooth_connection_status_task->send_buf_len =len ;
	ble_ctcs_s_send_handler(bluetooth_connection_status_task->conn_handle, bluetooth_connection_status_task->send_buf, &bluetooth_connection_status_task->send_buf_len, CTCS_TRANSPORT_UUID);
}


//void get_session_log_request(uint16_t conn_handle, uint8_t *p_data, uint8_t len)
//{
//	uint32_t get_session_log_session_id = 0;
//	int16_t payload_len = check_the_data_packet(p_data, len);
//	if (payload_len == -1)
//		response_ok_or_failed(conn_handle, GET_LOGS_RESPONSE, false);
//	else
//	{
//		session_log_requeset_payload_t *paylaod = (session_log_requeset_payload_t *)&m_rev_buf[sizeof(header_packet_t)];
//		upload_start_index = paylaod->start_index[0] << 8 | paylaod->start_index[1];

//		upload_length = paylaod->length;
//		NRF_LOG_INFO("start_index=%d,length=%d", upload_start_index, upload_length);
//		get_session_log_session_id = paylaod->session_id[0] << 24 | paylaod->session_id[1] << 16 | paylaod->session_id[2] << 8 | paylaod->session_id[3];
//		if (session_id != get_session_log_session_id)
//		{
//			response_ok_or_failed(conn_handle, GET_LOGS_RESPONSE, false);
//			NRF_LOG_INFO("session_id not match");
//			return;
//		}
//		if (session_status != SESSION_RUNNING && session_status != SESSION_STOPPED)
//		{
//			response_ok_or_failed(conn_handle, GET_LOGS_RESPONSE, false);
//			NRF_LOG_INFO("session state is not in a stopped or running state");
//			return;
//		}

//		start_upload_flag = true;
//	}
//}

void  get_session_log_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len)
{
	uint32_t get_session_log_session_id = 0;
	int16_t payload_len = check_the_data_packet(bluetooth_connection_status_task,p_data, len);
	if (payload_len == -1)
		response_ok_or_failed(bluetooth_connection_status_task, GET_LOGS_RESPONSE, false);
	else
	{
		session_log_requeset_payload_t *paylaod = (session_log_requeset_payload_t *)&p_data[sizeof(header_packet_t)];
		bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_start_index = paylaod->start_index[0] << 8 | paylaod->start_index[1];

		bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length = paylaod->length;
		NRF_LOG_INFO("start_index=%d,length=%d", bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_start_index, bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length);
		get_session_log_session_id = paylaod->session_id[0] << 24 | paylaod->session_id[1] << 16 | paylaod->session_id[2] << 8 | paylaod->session_id[3];
		if (session_id != get_session_log_session_id)
		{
			response_ok_or_failed(bluetooth_connection_status_task, GET_LOGS_RESPONSE, false);
			NRF_LOG_INFO("session_id not match");
			return;
		}
		if (session_status != SESSION_RUNNING && session_status != SESSION_STOPPED)
		{
			response_ok_or_failed(bluetooth_connection_status_task, GET_LOGS_RESPONSE, false);
			NRF_LOG_INFO("session state is not in a stopped or running state");
			return;
		}

		bluetooth_connection_status_task->session_log_send_ctrl_parames.start_upload_flag = true;
	}
	
}
/*
启动logging数据上传
*/
void startup_session_log_upload(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	 switch(bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state)
	 {
		 case  LOGGING_RECORD_DATA_SEND_NONE :
	
			if (bluetooth_connection_status_task->session_log_send_ctrl_parames.start_upload_flag) // response app get log cmd
	  	{
				if (  bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_start_index > m_logging_save_ring_buf.count \
				 || bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length == 0  \
			   || bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length > m_logging_save_ring_buf.count)
				{
				response_ok_or_failed(bluetooth_connection_status_task, GET_LOGS_RESPONSE, false);
				bluetooth_connection_status_task->session_log_send_ctrl_parames.start_upload_flag = false;
				return;
				}

				if ((bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_start_index + bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length) > m_logging_save_ring_buf.count)
				{
				bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length = m_logging_save_ring_buf.count - bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_start_index + 1;
				}

				bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos = bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_start_index;
				if (bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos == 0)
				bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos = 1;

				bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index = \
				(m_logging_save_ring_buf.read_index + bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos - 1) % SAVE_BUF_MAX_LENGTH;
				
				NRF_LOG_INFO("actual_reading_index =%d start_index=%d,read_inde=%d", \
				bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index, \
				bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos, \
				m_logging_save_ring_buf.read_index);
			
				send_session_log(bluetooth_connection_status_task, \
												 GET_LOGS_RESPONSE, \
												 session_id, \
												 bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index, \
												 bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos,0);
				bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_SEND_WAIT;
			
		}else {
			
			if (bluetooth_connection_status_task->session_log_send_ctrl_parames.startup_proactively_send_log_flag)
			{
				 NRF_LOG_INFO("startup_session_log_upload");
				
				bluetooth_connection_status_task->session_log_send_ctrl_parames.startup_proactively_send_log_flag = false;
				bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index = \
				(m_logging_save_ring_buf.read_index + bluetooth_connection_status_task->session_log_send_ctrl_parames.proactively_send_index_pos - 1) % SAVE_BUF_MAX_LENGTH;
				
				send_session_log(bluetooth_connection_status_task, \
												 SESSION_LOG, \
												 session_id, \
												 bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index, \
												 bluetooth_connection_status_task->session_log_send_ctrl_parames.proactively_send_index_pos,1);
	     	bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_PROACTIVELY_SEND_WAIT;
				
				
			}
			
			
		}
	  break ;
		
		 case LOGGING_RECORD_DATA_SEND_START:
			  NRF_LOG_INFO("startup_session_log_upload  LOGGING_RECORD_DATA_SEND_START");
		 	send_session_log(bluetooth_connection_status_task, \
												 GET_LOGS_RESPONSE, \
												 session_id, \
												 bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index, \
												 bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos,0);
				bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_SEND_WAIT;
		 break ;
    
   	case LOGGING_RECORD_DATA_SEND_END:
		bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_SEND_NONE;
		bluetooth_connection_status_task->session_log_send_ctrl_parames.start_upload_flag = false;
		break;		 
		 
		case LOGGING_RECORD_DATA_SEND_WAIT:
			if (bluetooth_connection_status_task->cts_send_state == TX_COMPLETE)
			{
      NRF_LOG_INFO("wait_session_log_sending_completed  LOGGING_RECORD_DATA_SEND_WAIT tx_complete");
			bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos++;
			bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index =  \
			(m_logging_save_ring_buf.read_index + bluetooth_connection_status_task->session_log_send_ctrl_parames.index_pos - 1) % SAVE_BUF_MAX_LENGTH;
			bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length--;

			// m_logging_save_ring_buf.write_index ==  actual_reading_index  queue is empty
					if ((m_logging_save_ring_buf.write_index == bluetooth_connection_status_task->session_log_send_ctrl_parames.actual_reading_index) \
						|| bluetooth_connection_status_task->session_log_send_ctrl_parames.upload_length == 0)
						bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_SEND_END;
					else
					bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_SEND_START;
			
			}
	 	break;	
			
			
			case LOGGING_RECORD_DATA_PROACTIVELY_SEND_WAIT:
			if (bluetooth_connection_status_task->cts_send_state == TX_COMPLETE)
			{
				 NRF_LOG_INFO("wait_session_log_sending_completed  LOGGING_RECORD_DATA_PROACTIVELY_SEND_WAIT tx_complete");
				bluetooth_connection_status_task->session_log_send_ctrl_parames.logging_send_state = LOGGING_RECORD_DATA_SEND_NONE;
				bluetooth_connection_status_task->session_log_send_ctrl_parames.proactively_send_index_pos++;
				
			}
		  break ;
		
		
	 default:
			
		 break ;
		 
	 }
}






/*
index_pos 从1 开始
*/
void send_logging_recrod_data_task(void)
{

//	switch (logging_send_state)
//	{
//	case LOGGING_RECORD_DATA_SEND_NONE:
//		if (start_upload_flag) // response app get log cmd
//		{
//			if (upload_start_index > m_logging_save_ring_buf.count || upload_length == 0 || upload_length > m_logging_save_ring_buf.count)
//			{
//				response_ok_or_failed(m_current_conn_handle, GET_LOGS_RESPONSE, false);
//				start_upload_flag = false;
//				return;
//			}

//			if ((upload_start_index + upload_length) > m_logging_save_ring_buf.count)
//			{
//				upload_length = m_logging_save_ring_buf.count - upload_start_index + 1;
//			}

//			index_pos = upload_start_index;
//			if (index_pos == 0)
//				index_pos = 1;

//			actual_reading_index = (m_logging_save_ring_buf.read_index + index_pos - 1) % SAVE_BUF_MAX_LENGTH;
//			NRF_LOG_INFO("actual_reading_index =%d start_index=%d,read_inde=%d", actual_reading_index, index_pos, m_logging_save_ring_buf.read_index);
//			logging_send_state = LOGGING_RECORD_DATA_SEND_START;
//		}
//		else // proactively sending
//		{

//			if (startup_proactively_send_log_flag)
//			{
//				startup_proactively_send_log_flag = false;
//				actual_reading_index = (m_logging_save_ring_buf.read_index + proactively_send_index_pos - 1) % SAVE_BUF_MAX_LENGTH;
//				logging_send_state = LOGGING_RECORD_DATA_PROACTIVELY_SEND_START;
//			}
//		}

//		break;

//	case LOGGING_RECORD_DATA_SEND_START:

//		send_session_log(m_current_conn_handle, GET_LOGS_RESPONSE, session_id, actual_reading_index, index_pos);
//		logging_send_state = LOGGING_RECORD_DATA_SEND_WAIT;
//		break;

//	case LOGGING_RECORD_DATA_SEND_WAIT:
//		if (cts_send_state == TX_COMPLETE)
//		{

//			index_pos++;
//			actual_reading_index = (m_logging_save_ring_buf.read_index + index_pos - 1) % SAVE_BUF_MAX_LENGTH;
//			upload_length--;

//			// m_logging_save_ring_buf.write_index ==  actual_reading_index  queue is empty
//			if ((m_logging_save_ring_buf.write_index == actual_reading_index) || upload_length == 0)
//				logging_send_state = LOGGING_RECORD_DATA_SEND_END;
//			else
//				logging_send_state = LOGGING_RECORD_DATA_SEND_START;
//		}
//		break;

//	case LOGGING_RECORD_DATA_SEND_END:
//		logging_send_state = LOGGING_RECORD_DATA_SEND_NONE;
//		start_upload_flag = false;
//		break;

//	case LOGGING_RECORD_DATA_PROACTIVELY_SEND_START:
//		send_session_log(m_current_conn_handle, SESSION_LOG, session_id, actual_reading_index, proactively_send_index_pos);
//		logging_send_state = LOGGING_RECORD_DATA_PROACTIVELY_SEND_WAIT;
//		break;
//	case LOGGING_RECORD_DATA_PROACTIVELY_SEND_WAIT:
//		if (cts_send_state == TX_COMPLETE)
//		{
//			logging_send_state = LOGGING_RECORD_DATA_SEND_NONE;
//			proactively_send_index_pos++;
//		}
//		break;

//	default:
//		break;
//	}

}

void start_record(void)
{
	//  if(get_user_connect_state())
	{
		//			  m_first_logging_time = device_infor.m_system_clock ;
		m_start_record_flag = true;
		m_logging_record_time_count = 0;
	//	proactively_send_index_pos = 1;
		logging_save_ring_buf_clear_empty();
		bluetooth_all_init_log_send_ctrl_parames();
	}
}
void stop_record(void)
{
	m_start_record_flag = false;
}

void save_logging_feature_data(void)
{
	static uint32_t temp_second;
	if (m_start_record_flag)
	{
		if (temp_second == get_system_time_second())
			return;
		temp_second = get_system_time_second();

		if ((get_system_time_second() % m_record_cycle_time) == 0)
		{
			//	if(m_logging_record_time_count< m_session_run_time )
			{
				//		NRF_LOG_INFO("m_logging_record_time_count=%d",m_logging_record_time_count);
				m_logging_record_time_count += m_record_cycle_time;
				logging_save_ring_buf_in_queue(&Get_Response_Pack);
			}

			//					else
			//					{
			//					  m_start_record_flag = false ;
			//					  m_command_type =0 ;
			//
			//           NRF_LOG_INFO("stop logging record,record total num=%d",m_logging_save_ring_buf.count);
			//					}
		}
	}
}

void clear_logging(void)
{
	m_logging_save_ring_buf.write_index = 0;
	m_logging_save_ring_buf.read_index = 0;
	m_logging_save_ring_buf.count = 0;
	//proactively_send_index_pos = 1;
	bluetooth_all_init_log_send_ctrl_parames();
}

void init_save_logging(void)
{
	m_logging_need_to_record_feature_num = 0;
	m_start_record_flag = false;
	clear_logging();
}

#include "app_task.h"
#include "mt_gpio.h"
#include "HBOT_uart_cmd.h"
#include "app_cfg_cmd.h"
#include "session.h"
#include "HBOT_uart_cmd.h"
#include "session_sharing_cmd.h"
#include "app_set_cmd_management.h"
#include "app_session_set_cmd_management.h"

#define NRF_LOG_MODULE_NAME app_task
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

typedef enum
{
	AirComp_START_NONE,
	AirComp_START_ON,
	AirComp_START_ON_DELAY,
	AirComp_START_ON_CLEAR

} AirComp_START_STATE_E; // ��ѹ��״̬����

typedef enum
{
	Oxygen_START_NONE,
	Oxygen_START_ON,
	Oxygen_START_ON_DELAY,
	Oxygen_START_ON_CLEAR

} Oxygen_START_STATE_E; // ������״̬����

static AirComp_START_STATE_E air_comp_start_state = AirComp_START_NONE;
static Oxygen_START_STATE_E Oxygen_start_state = Oxygen_START_NONE;

static bool user_connect_state = false;
//uint16_t m_current_conn_handle;
bool get_user_connect_state(void)
{
	return user_connect_state;
}
void clear_usr_connect_state(void)
{
	user_connect_state = false;
}

void set_user_connect_state(bool value)
{
	user_connect_state = value;
}

void start_oxygen_ctrl_task(void)
{
	Oxygen_start_state = Oxygen_START_ON;
}
void start_air_comp_ctrl_task(void)
{
	air_comp_start_state = AirComp_START_ON;
}

void startup_features_set_cmd(void)
{
	uart_switch_cmd_state(SET_AIR_CONDITI_STATE);
}

void proactively_upload_change_data_to_the_app(bluetooth_connection_status_task_t *bluetooth_connection_status_task)
{
	//refresh_change_data_feature_id_list();
	if(!is_sharing_uesr(bluetooth_connection_status_task->ShortUserID))
	 return ;
	//NRF_LOG_INFO("is sharing user");
	if (bluetooth_connection_status_task->upload_update_feature_num_temp_second == bluetooth_connection_status_task->secondCount )
    return;
  bluetooth_connection_status_task->upload_update_feature_num_temp_second = bluetooth_connection_status_task->secondCount;
	
//	NRF_LOG_INFO("is sharing user ver second");
	 if (bluetooth_connection_status_task->cts_send_state == TX_COMPLETE)
	 {
		// NRF_LOG_INFO("is sharing user TX COMPLETE");
			if(bluetooth_connection_status_task->update_feature_num!=0)
			{
				NRF_LOG_INFO("is sharing user update_feature_num");
				send_feature_id_data_to_app(bluetooth_connection_status_task , \
															    Update_Feature_Id_list.feature_id, \
																	bluetooth_connection_status_task->update_feature_num, \
																	ACTIVE_UPLOAD_CMD);
				
				bluetooth_connection_status_task->update_feature_num =0 ;
			}
	 }
	
}

//extern Set_Request_Data_Pack_t SettingData;

extern Feature_Id_List* pSet_Feature_Id_list ;
extern Set_Request_Data_Pack_t* pSettingData ;


static void oxygen_ctrl_task(void)
{
	static uint16_t oxygen_time_cnt = 0;
	static uint32_t oxygen_temp_time_tick = 0;

	switch (Oxygen_start_state)
	{
	case Oxygen_START_ON:
		NRF_LOG_INFO("Oxygen_Concent_Pwr_on=%d",pSettingData->Oxygen_Concent_Pwr_on);
		bsp_IO_Oxygen_ctrl(pSettingData->Oxygen_Concent_Pwr_on);
		Oxygen_start_state = Oxygen_START_ON_DELAY;
		break;
	case Oxygen_START_ON_DELAY:
		if (get_system_time_tick() == oxygen_temp_time_tick)
			return;
		oxygen_temp_time_tick = get_system_time_tick();
		oxygen_time_cnt++;
		if (oxygen_time_cnt >= 5)
		{
			oxygen_time_cnt = 0;
			Oxygen_start_state = Oxygen_START_ON_CLEAR;
		}
		break;
	case Oxygen_START_ON_CLEAR:
		bsp_IO_Oxygen_clear();
		Get_Response_Pack.Oxygen_Concent.attribute = pSettingData->Oxygen_Concent_Pwr_on;
		Oxygen_start_state = Oxygen_START_NONE;
	 // uart_write_state = SET_AIR_CONDITI_STATE;
	  set_uart_wirte_sate(SET_AIR_CONDITI_STATE);
		break;
	default:
		break;
	}
}
static void air_comp_ctrl_task(void)
{
	static uint16_t air_comp_time_cnt = 0;
	static uint32_t air_comp_temp_time_tick = 0;

	switch (air_comp_start_state)
	{
	case AirComp_START_ON:
		NRF_LOG_INFO("Air_Compressor_Pwr_on=%d",pSettingData->Air_Compressor_Pwr_on);
		bsp_IO_AirComp_ctrl(pSettingData->Air_Compressor_Pwr_on);
		air_comp_start_state = AirComp_START_ON_DELAY;
		break;
	case AirComp_START_ON_DELAY:
		if (get_system_time_tick() == air_comp_temp_time_tick)
			return;
		air_comp_temp_time_tick = get_system_time_tick();
		air_comp_time_cnt++;
		if (air_comp_time_cnt >= 5)
		{
			air_comp_time_cnt = 0;
			air_comp_start_state = AirComp_START_ON_CLEAR;
		}
		break;
	case AirComp_START_ON_CLEAR:
		bsp_IO_AirComp_Clear();
		air_comp_start_state = AirComp_START_NONE;
		Get_Response_Pack.Air_Compressor.attribute = pSettingData->Air_Compressor_Pwr_on;
	  //uart_write_state = SET_AIR_CONDITI_STATE;
	  set_uart_wirte_sate(SET_AIR_CONDITI_STATE);
		break;
	default:
		break;
	}
}



void task_set_request(void)
{

	oxygen_ctrl_task();
	air_comp_ctrl_task();
	task_uart_send_cmd_handle();
}
static uint32_t efsm_tick_time = 0;

void app_task(void)
{
	static uint32_t send_temp_second  ;
	task_check_input_io();
	task_read_Oxygen_AirComp_state();
	task_set_request();
	
	bluetooth_all_connection_status_task_efsm_hand();
	Efsm_Hand((efsm_t *)&ao_app_set_cmd_send_task);
	Efsm_Hand((efsm_t *)&ao_app_session_set_cmd_send_task);
	
	session_task_exec();
	
	if (get_system_time_tick() == efsm_tick_time)
		return;
	efsm_tick_time = get_system_time_tick();


	bluetooth_all_connection_status_task_efsm_evt_trig();
	
	if (send_temp_second == get_system_time_second())
    return;
  send_temp_second = get_system_time_second();
	
  refresh_change_data_feature_id_list();
	
}

void custom_protocol_init(void)
{
	bluetooth_all_connection_status_task_ctor();
	app_set_cmd_send_task_ctor(&ao_app_set_cmd_send_task);
	app_session_set_cmd_send_task_ctor(&ao_app_session_set_cmd_send_task);
}

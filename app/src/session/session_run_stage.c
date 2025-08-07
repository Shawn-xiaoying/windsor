#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "app_cfg_cmd.h"
#include "mt_cst_protocol.h"
#include "logging_rw_implent.h"

#include "session.h"
#include "session_log.h"
#include "session_run_stage.h"
#include "preparation_stage.h"

#define NRF_LOG_MODULE_NAME session_run_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static session_run_stage_status session_run_status = SESSION_RUN_STAGE_NONE;
static session_status_parmaes_t session_status_parames = {0, 0, 0, 0};
static sesssion_status_n_run_parames_t cur_sesssion_status_n_run_parames = {{0, 0}, 0};

static uint32_t delay_clear_statue_time_count = 0;

void set_session_run_stage_status(session_run_stage_status status)
{
	session_run_status = status;
}

/*
reture value:
0x00 : when session start,
0xffff : when session end ;


*/

static uint16_t get_current_session_status_list_parames(uint8_t cur_session_status_pos, sesssion_status_n_run_parames_t *cur)
{

	cur->parames.time = session_parames_buf[cur_session_status_pos] << 8 | session_parames_buf[cur_session_status_pos + 1];
	cur->parames.length = session_parames_buf[cur_session_status_pos + 2];
	cur->features_list_pos = cur_session_status_pos + sizeof(session_status_n_parames_t);
	NRF_LOG_INFO("time=%d,length=%d,pos=%d", cur->parames.time, cur->parames.length, cur->features_list_pos);
	return cur->parames.time;
}

/*
retur value
false : no have next

*/

static uint16_t get_next_session_status_list_parames(uint8_t cur_session_features_pos, sesssion_status_n_run_parames_t *next)
{

	sesssion_status_n_run_parames_t cur;
	uint8_t pos;
	get_current_session_status_list_parames((cur_session_features_pos - sizeof(session_status_n_parames_t)), &cur);
	pos = (cur_session_features_pos + cur.parames.length);
	return get_current_session_status_list_parames(pos, next);
}

uint8_t get_session_status_parames(void)
{
	session_parames_config_data_t *session_parames_config_data = (session_parames_config_data_t *)session_parames_buf;
	sesssion_status_n_run_parames_t cur;
	uint8_t preparation_featreus_pos = 0, preparation_features_data_length = 0, pos = 0;
	preparation_features_data_length = get_preparation_features_pos(&preparation_featreus_pos);
	pos = preparation_featreus_pos + preparation_features_data_length;
	if ((session_parames_config_data->running_duration[0] << 8 | session_parames_config_data->running_duration[1]) == 0)
	{
		memset((void *)&session_status_parames, 0, sizeof(session_status_parmaes_t));
		return 0;
	}

	session_status_parames.start_session_list_pos = (pos + 1);
	session_status_parames.total_length = session_parames_buf[pos];

	NRF_LOG_INFO("preparation_features_pos=%d,length=%d", preparation_featreus_pos, preparation_features_data_length);

	get_current_session_status_list_parames(session_status_parames.start_session_list_pos, &cur_sesssion_status_n_run_parames);
	// look up end session status list pos
	session_status_parames.cur_session_list_totle_count = cur_sesssion_status_n_run_parames.parames.length + sizeof(session_status_n_parames_t);
	cur.features_list_pos = cur_sesssion_status_n_run_parames.features_list_pos;

	while (session_status_parames.cur_session_list_totle_count < session_status_parames.total_length)
	{

		get_next_session_status_list_parames(cur.features_list_pos, &cur);
		session_status_parames.cur_session_list_totle_count += (cur.parames.length + sizeof(session_status_n_parames_t));
	}

	session_status_parames.end_session_list_pos = cur.features_list_pos;
	session_status_parames.cur_session_list_totle_count = 0;

	NRF_LOG_INFO("total_lenght=%d,start_pos =%d,end_pos=%d", session_status_parames.total_length, session_status_parames.start_session_list_pos, session_status_parames.end_session_list_pos);

	return 1;
}

void session_run_stage_task(void)
{
	static uint32_t temp_second_time = 0;
	switch (session_run_status)
	{

	case SESSION_RUN_STAGE_START:
		get_session_status_parames();
		start_record();

		if (cur_sesssion_status_n_run_parames.parames.time == 0)
		{ // execute immediately
			session_run_status = SESSION_RUN_STAGE_EXEC_POINT;
		}
		else
		{
			session_run_time_count = 0;
			temp_second_time = get_system_time_second();
			session_run_status = SESSION_RUN_STAGE_RUNNING;
		}
		break;
	case SESSION_RUN_STAGE_RUNNING:
		if (get_system_time_second() == temp_second_time)
			return;
		temp_second_time = get_system_time_second();
		session_run_time_count++;
		NRF_LOG_INFO("session_run_time_count == %d", session_run_time_count);
		if (session_run_time_count >= cur_sesssion_status_n_run_parames.parames.time)
		{

			session_run_status = SESSION_RUN_STAGE_EXEC_POINT;
		}

		break;
	case SESSION_RUN_STAGE_EXEC_POINT:

		if (analysis_set_request_features_data_packet(&session_parames_buf[cur_sesssion_status_n_run_parames.features_list_pos],
													  cur_sesssion_status_n_run_parames.parames.length,
														&Session_Set_Feature_Id_list,&Session_SettingData) != 0)
		{

//			startup_features_set_cmd();
			start_session_dpid_set();
			session_status_parames.cur_session_list_totle_count += (cur_sesssion_status_n_run_parames.parames.length + sizeof(session_status_n_parames_t));
			NRF_LOG_INFO("session_list_total_count=%d total=%d ", session_status_parames.cur_session_list_totle_count, session_status_parames.total_length);
			if (session_status_parames.cur_session_list_totle_count < session_status_parames.total_length)
			{
				get_next_session_status_list_parames(cur_sesssion_status_n_run_parames.features_list_pos, &cur_sesssion_status_n_run_parames);

				if (cur_sesssion_status_n_run_parames.parames.time == 0xffff)
					cur_sesssion_status_n_run_parames.parames.time = m_session_run_time;

				session_run_status = SESSION_RUN_STAGE_RUNNING;
				temp_second_time = get_system_time_second();
			}
			else
			{

				session_run_status = SESSION_RUN_STAGE_STOP;
			}
		}
		else
		{
			// send control session response failed  to app
		}

		break;
	case SESSION_RUN_STAGE_STOP:
 	 // session_run_time_count = 0;  //2025/6/11 修改，即在停止后不会将运行时间清零，APP端出现运行时间为0
		session_run_status = SESSION_RUN_STATE_DELAY_CLEAR;
		session_status_parames.cur_session_list_totle_count = 0;
		// startup session run startusing
		temp_second_time = get_system_time_second();
		delay_clear_statue_time_count = 0;
		break;

	case SESSION_RUN_STATE_DELAY_CLEAR:
		// Stop recording after a delay of some time
		if (get_system_time_second() == temp_second_time)
			return;
		temp_second_time = get_system_time_second();
		delay_clear_statue_time_count++;

		if (delay_clear_statue_time_count >= 2 * m_record_cycle_time)
		{
			stop_record();
			session_run_status = SESSION_RUN_STAGE_NONE;
			delay_clear_statue_time_count = 0;
			session_status = SESSION_STOPPED;
		}

		break;

	case SESSION_RUN_FORCE_STOP:
		// Execute the last session-stats_list
		if (analysis_set_request_features_data_packet(&session_parames_buf[session_status_parames.end_session_list_pos],
													  session_parames_buf[session_status_parames.end_session_list_pos - 1],
														&Session_Set_Feature_Id_list,&Session_SettingData) != 0)
		{

			//startup_features_set_cmd();
			start_session_dpid_set();
			session_run_status = SESSION_RUN_STAGE_STOP;
		}
		break;

	case SESSION_RUN_STAGE_NONE:
		break;
	}
}

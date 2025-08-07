#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include "app_cfg_cmd.h"
#include "mt_cst_protocol.h"
#include "session.h"
#include "session_log_def.h"
#include "session_parameters_check.h"
#include "session_parameters_check_implent.h"

#define NRF_LOG_MODULE_NAME session_parameter_check
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static uint8_t check_log_features_id_parames(uint8_t log_features_length)
{
	uint8_t *log_features_list = &session_parames_buf[sizeof(session_parames_config_data_t)];
	for (int i = 0; i < log_features_length; i++)
	{
		if (check_the_log_feature_id_is_valid(log_features_list[i]) == 1)
			return 1;
	}
	return 0;
}

static uint8_t check_preparation_status(uint8_t log_features_length, uint8_t *error_code, uint8_t *preparation_status_length)
{
	uint8_t *preparation_status = &session_parames_buf[sizeof(session_parames_config_data_t) + log_features_length];
	uint8_t index = 0;
	uint8_t *preparation_features = &preparation_status[1];
	*preparation_status_length = preparation_status[0];
	if (preparation_status[0] == 0)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_PREPARATION_STATUS_LENGTH;
		return 1;
	}
	do
	{

		if (check_the_featrue_id_is_valid(preparation_features[index]) == 1)
		{
			*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_PREPARATION_STATUS_FEATRUES_ID;
			return 1;
		}

		if (check_the_feature_id_length_is_valid(preparation_features[index], preparation_features[index + 1]) == 1)

		{
			*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_PREPARATION_STATUS_FEATRUSE_ID_LENGTH;
			return 1;
		}

		if (check_the_feature_id_value_is_valid(preparation_features[index], preparation_features[index + 1], &preparation_features[index + 2]) == 1)
		{
			*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_PREPARATION_STATUS_FEATRUSE_ID_VALUE;
			return 1;
		}
		index += (preparation_features[index + 1] + 2);

	} while (index < preparation_status[0]);

	return 0;
}

static uint8_t check_running_status(uint8_t log_features_length, uint8_t preparation_length, uint8_t *error_code)
{
	uint8_t index = 0;
	uint8_t status_list_count = 0;
	uint16_t time = 0;
	uint16_t last_time = 0;
	uint8_t length = 0;
	uint8_t status_n_count = 0;
	NRF_LOG_INFO("log_featreus_length=%d,preparation_lenght=%d", log_features_length, preparation_length);
	uint8_t *running_status = &session_parames_buf[sizeof(session_parames_config_data_t) + log_features_length + preparation_length];

	if (running_status[0] == 0)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_LENGTH;
		return 1;
	}
	uint8_t *status_list = &running_status[1];
	index = 0;
	do
	{
		time = status_list[index] << 8 | status_list[index + 1];
		length = status_list[index + 2];
		NRF_LOG_INFO("index=%d ,time=%0x,length=%d", index, time, length);
		if (length == 0)
		{
			*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_N_LENGTH;
			return 1;
		}

		switch (status_list_count)
		{
		case 0:
			if (time != 0)
			{
				*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_N_SESSION_START_TIME;
				return 1;
			}
			break;

		default:
			if (time == 0)
			{
				*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_N_TIME;
				return 1;
			}
			break;
		}
		if (status_list_count != 0)
		{
			if (time <= last_time)
			{
				*error_code = SESSION_PARAMES_ER_CODE_RUNNING_STATUS_SESSION_TIME_NOT_SORTING;
				return 1;
			}
		}
		last_time = time;
		index += 3;
		status_n_count = 0;
		do
		{
			if (check_the_featrue_id_is_valid(status_list[index]) == 1)
			{
				*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_N_FEATUREIS_ID;
				return 1;
			}

			if (check_the_feature_id_length_is_valid(status_list[index], status_list[index + 1]) == 1)
			{
				*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_N_FEATRUSE_ID_LENGTH;
				return 1;
			}

			if (check_the_feature_id_value_is_valid(status_list[index], status_list[index + 1], &status_list[index + 2]) == 1)
			{
				*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_N_FEATRUSE_ID_VALUE;
				return 1;
			}
			status_n_count += (status_list[index + 1] + 2);
			NRF_LOG_INFO("feature_id=%d ,length=%d,value=%d status_n_count=%d length=%d ", status_list[index], status_list[index + 1], status_list[index + 2], status_n_count, length);
			index += (status_list[index + 1] + 2);

		} while (status_n_count < length);

		status_list_count++;

	} while ((index) < running_status[0]);

	// when session stop ,time is not 0xffff
	if (time != 0xffff)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_STATUS_N_SESSION_STOP_TIME;
		return 1;
	}
	return 0;
}

/*
error_code : retur session parames error code
return value : 0  session parames check success,1 session parames check error
*/
uint8_t session_parames_check(uint8_t *error_code)
{
	uint8_t preparation_status_length;
	uint8_t preparation_length;
	uint16_t run_duration_time = 0;
	uint16_t log_interval = 0;
	session_parames_config_data_t *session_parames_config_data = (session_parames_config_data_t *)session_parames_buf;
	if ((session_parames_config_data->session_id[0] << 24 | session_parames_config_data->session_id[1] << 16 |
		 session_parames_config_data->session_id[2] << 8 | session_parames_config_data->session_id[3]) == 0)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_SESSION_ID;
		NRF_LOG_INFO("SESSION_PARAMES_ER_CODE_ILLEGAL_SESSION_ID");
		return 1;
	}
	run_duration_time = (session_parames_config_data->running_duration[0] << 8 | session_parames_config_data->running_duration[1]);
	log_interval = (session_parames_config_data->log_interval[0] << 8 | session_parames_config_data->log_interval[1]);
	if (run_duration_time == 0)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_DURATION;
		NRF_LOG_INFO("SESSION_PARAMES_ER_CODE_ILLEGAL_RUNNING_DURATION");
		return 1;
	}
	if ((run_duration_time / log_interval) > LOGGING_MAX_RING_BUF_LENGTH)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_LOG_INTERVAL;
		NRF_LOG_INFO("SESSION_PARAMES_ER_CODE_ILLEGAL_LOG_INTERVAL");
		return 1;
	}
	if (session_parames_config_data->log_features_length == 0)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_LOG_FEATURES_LENGTH;
		NRF_LOG_INFO("SESSION_PARAMES_ER_CODE_ILLEGAL_LOG_FEATURES_LENGTH");
		return 1;
	}

	if (check_log_features_id_parames(session_parames_config_data->log_features_length) == 1)
	{
		*error_code = SESSION_PARAMES_ER_CODE_ILLEGAL_LOG_FEATREUS_ID;
		NRF_LOG_INFO("SESSION_PARAMES_ER_CODE_ILLEGAL_LOG_FEATREUS_ID");
		return 1;
	}

	if ((session_parames_config_data->preparation_duration[0] << 8 | session_parames_config_data->preparation_duration[1]) != 0)
	{
		if (check_preparation_status(session_parames_config_data->log_features_length, error_code, &preparation_status_length) == 1)
		{
			NRF_LOG_INFO("check_preparation_status erroe")
			return 1;
		}
		preparation_length = preparation_status_length + 1; // preparation data length 1 byte + preparation_status_length bytes;
	}
	else
	{
		preparation_length = 1; // preparation data length 1 byte
	}

	if (check_running_status(session_parames_config_data->log_features_length, preparation_length, error_code) == 1)
	{
		NRF_LOG_INFO("check_running_status")
		return 1;
	}

	return 0;
	
}

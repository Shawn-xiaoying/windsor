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
#include "preparation_stage.h"
#include "session_run_stage.h"

#define NRF_LOG_MODULE_NAME preparation_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static preparation_stage_status preparation_status = PREPARATION_STAGE_NONE;
uint16_t preparation_run_time = 0;

void set_preparation_stage_status(preparation_stage_status status)
{
    preparation_status = status;
}
uint8_t get_preparation_features_pos(uint8_t *pos)
{
    session_parames_config_data_t *session_parames_config_data = (session_parames_config_data_t *)session_parames_buf;
    uint8_t preparation_status_pos = sizeof(session_parames_config_data_t) + session_parames_config_data->log_features_length;
    if (session_parames_config_data->preparation_duration == 0)
    {
        *pos = preparation_status_pos;
        return 0;
    }

    *pos = (preparation_status_pos + 1);                // features data pos ;
    return session_parames_buf[preparation_status_pos]; // features data length
}



void preparation_stage_task(void)
{
    static uint32_t temp_second_time = 0;
    uint8_t preparation_features_data_length = 0, preparation_features_data_pos = 0;

    switch (preparation_status)
    {

    case PREPARATION_STAGE_START:
        preparation_features_data_length = get_preparation_features_pos(&preparation_features_data_pos);
        NRF_LOG_INFO("preparation_features_data_length = %d", preparation_features_data_length);
        if (preparation_features_data_length == 0)
        {
            preparation_status = PREPARATION_STAGE_STOP;
            break;
        }
        if (analysis_set_request_features_data_packet(&session_parames_buf[preparation_features_data_pos], preparation_features_data_length,&Session_Set_Feature_Id_list,&Session_SettingData) != 0)
        {
            NRF_LOG_INFO("analysis_set_request_features_data_packet != 0");
         //   startup_features_set_cmd();
					  start_session_dpid_set();
            preparation_status = PREPARATION_STAGE_RUNNING;

            session_run_time_count = 0;
            temp_second_time = get_system_time_second();
        }
        else
        {
            NRF_LOG_INFO("analysis_set_request_features_data_packet == 0");
            // send control session response failed  to app
        }
        break;
    case PREPARATION_STAGE_RUNNING:
        if (get_system_time_second() == temp_second_time)
            return;
        temp_second_time = get_system_time_second();
        session_run_time_count++;
        NRF_LOG_INFO("session_run_time_count == %d", session_run_time_count);
        if (session_run_time_count >= preparation_run_time)
        {
            NRF_LOG_INFO("session_run_time_count >= preparation_run_time");
            preparation_status = PREPARATION_STAGE_STOP;
        }

        break;
    case PREPARATION_STAGE_STOP:
        session_run_time_count = 0;
        session_status = SESSION_RUNNING;
        preparation_status = PREPARATION_STAGE_NONE;
        NRF_LOG_INFO("preparation_status =PREPARATION_STAGE_STOP ");
        set_session_run_stage_status(SESSION_RUN_STAGE_START);
        // startup session run startusing
        break;

    case PREPARATION_STAGE_NONE:
        break;
    }
}

/* Copyright (C)
   All rights reserved. */

#ifndef APP_CFG_CMD_H__
#define APP_CFG_CMD_H__

#include <stdint.h>
#include "controller_config.h"
//#include "app_service_cfg.h"
#include "implemention_cfg_cmd.h"
#include "mt_gpio.h"
#include "bluetooth_connection_management.h"
#define REFRES_UPDATE_FEATURE_ID_LIST(OBJECT)                                                                                 \
	if (memcmp(&Last_Get_Response_Pack.OBJECT, &Get_Response_Pack.OBJECT, sizeof(Get_Response_Pack.OBJECT)) != 0)             \
	{                                                                                                                         \
		Update_Feature_Id_list.feature_id[Update_Feature_Id_list.update_feature_num++] = Get_Response_Pack.OBJECT.Feature_ID; \
		memcpy(&Last_Get_Response_Pack.OBJECT, &Get_Response_Pack.OBJECT, sizeof(Get_Response_Pack.OBJECT));                  \
	}

extern const feature_value_length_map_t features_supported_by_the_device[];
extern const uint8_t features_length_supported_by_the_device;

extern const uint8_t log_features_length_supported_by_the_device;
extern const uint8_t log_features_supported_by_the_device[];

int16_t app_Get_Request_cmd(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len);
int16_t check_the_data_packet(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len);
uint8_t check_the_featrue_id_is_valid(uint8_t featrue_id);
uint8_t check_the_log_feature_id_is_valid(uint8_t featrue_id);
uint8_t check_the_feature_id_length_is_valid(uint8_t feature_id, uint8_t length);
void app_get_features_supported_by_the_device_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len);
void app_get_log_features_supported_by_the_device_request(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t *p_data, uint8_t len);
void response_ok_or_failed(bluetooth_connection_status_task_t *bluetooth_connection_status_task, unsigned char ack_cmd, bool set_value);
void set_response_packet_head(uint8_t *sned_buf, uint8_t cmd, uint16_t payload_size,uint8_t msg_id);
void device_Get_Response_ids(bluetooth_connection_status_task_t *bluetooth_connection_status_task, uint8_t id_len);
uint8_t analysis_set_request_features_data_packet(uint8_t *id_data, int16_t temp_Payload_Size,Feature_Id_List* Set_Feature_Id_list,Set_Request_Data_Pack_t* set_pack_data);

bool app_Info_Request(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len);
uint8_t app_Set_Request(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t *p_data, uint8_t len);
void device_Set_Response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool set_valu);
void device_Info_Response(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
void set_log_parames_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool set_valu);
void start_stop_logging_record_response(bluetooth_connection_status_task_t *bluetooth_connection_status_task, bool set_value);
void get_logging_record_response_failed(bluetooth_connection_status_task_t *bluetooth_connection_status_task);
void send_unauthorized_response_packet(bluetooth_connection_status_task_t *bluetooth_connection_status_task,uint8_t cmd);
uint8_t set_Response_all_id(void);

#endif

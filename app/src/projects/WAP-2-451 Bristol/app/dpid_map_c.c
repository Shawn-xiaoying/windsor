#include "dpid_map_c.h"
#include "mcu.h"
#include "app_cfg_cmd.h"

static const FEATRUE_DPID_MAP_ST DpidMap[] = {
	{connect_status_ID, DPID_CONNECT_STATE},
	{pulse_output_ID, DPID_PULSE_OUTPUT},
	{pulse_selection_ID, DPID_PULSE_SELECTION},
	{pulse_frequency_setting_ID, DPID_PULSE_FREQUENCY_SETTING},
	{pulse_duty_cycle_setting_ID, DPID_PULSE_DUTY_CYCLE_SETTING},
	{working_time_setting_ID, DPID_WORKING_TIME_SETTING},
	{music_bluetooth_connection_status_ID, DPID_MUSIC_BLUETOOTH_CONNECT_STATUS},
	{music_control_ID, DPID_MUSIC_CONTROL},
	{music_volume_ID, DPID_MUSIC_VOLUME},
	{fan_gear_ID, DPID_FAN_GEAR},
	{equipment_time_setting_ID, DPID_EQUIMENT_DATE_TIME_SETTING},
	{fan_temperature_setting_ID, DPID_TEMPERATURE_SET},
	{fan_control_ID, DPID_FAN_CONTROL},
	{red_light_bed_dev_name_ID, DPID_DEV_NAME},
	{red_light_bed_device_current_temperature_ID, DPID_DEV_CURRENT_TEMPERATURE},
	{red_light_bed_device_warn_ID, DPID_DEV_WARN},

};
static const FEATRUE_SETVALUE_MAP_ST SetValueAdrMap[] = {

	{connect_status_ID, &SettingData.app_connect_state, sizeof(SettingData.app_connect_state)},
	{pulse_output_ID, &SettingData.pulse_output, sizeof(SettingData.pulse_output)},
	{pulse_selection_ID, SettingData.pulse_selection, sizeof(SettingData.pulse_selection)},
	{pulse_frequency_setting_ID, SettingData.pulse_frequency_setting, sizeof(SettingData.pulse_frequency_setting)},
	{pulse_duty_cycle_setting_ID, SettingData.pulse_duty_cycle_setting, sizeof(SettingData.pulse_duty_cycle_setting)},
	{working_time_setting_ID, &SettingData.working_time_setting, sizeof(SettingData.working_time_setting)},
	{music_bluetooth_connection_status_ID, &SettingData.music_bluetooth_connection_status, sizeof(SettingData.music_bluetooth_connection_status)},
	{music_control_ID, &SettingData.music_control, sizeof(SettingData.music_control)},
	{music_volume_ID, &SettingData.music_volume, sizeof(SettingData.music_volume)},
	{fan_gear_ID, &SettingData.fan_gear, sizeof(SettingData.fan_gear)},
	{equipment_time_setting_ID, SettingData.equipment_time_setting, sizeof(SettingData.equipment_time_setting)},
	{fan_temperature_setting_ID, &SettingData.fan_ctrl_temperature_setting, sizeof(SettingData.fan_ctrl_temperature_setting)},
	{fan_control_ID, &SettingData.fan_ctrl, sizeof(SettingData.fan_ctrl)},
	{red_light_bed_dev_name_ID, SettingData.red_light_bed_dev_name, sizeof(SettingData.red_light_bed_dev_name)}

};

static const FEATRUE_SETVALUE_MAP_ST SessionSetValueAdrMap[] = {

	{connect_status_ID, &Session_SettingData.app_connect_state, sizeof(Session_SettingData.app_connect_state)},
	{pulse_output_ID, &Session_SettingData.pulse_output, sizeof(Session_SettingData.pulse_output)},
	{pulse_selection_ID, Session_SettingData.pulse_selection, sizeof(Session_SettingData.pulse_selection)},
	{pulse_frequency_setting_ID, Session_SettingData.pulse_frequency_setting, sizeof(Session_SettingData.pulse_frequency_setting)},
	{pulse_duty_cycle_setting_ID, Session_SettingData.pulse_duty_cycle_setting, sizeof(Session_SettingData.pulse_duty_cycle_setting)},
	{working_time_setting_ID, &Session_SettingData.working_time_setting, sizeof(Session_SettingData.working_time_setting)},
	{music_bluetooth_connection_status_ID, &Session_SettingData.music_bluetooth_connection_status, sizeof(Session_SettingData.music_bluetooth_connection_status)},
	{music_control_ID, &Session_SettingData.music_control, sizeof(Session_SettingData.music_control)},
	{music_volume_ID, &Session_SettingData.music_volume, sizeof(Session_SettingData.music_volume)},
	{fan_gear_ID, &Session_SettingData.fan_gear, sizeof(Session_SettingData.fan_gear)},
	{equipment_time_setting_ID, Session_SettingData.equipment_time_setting, sizeof(Session_SettingData.equipment_time_setting)},
	{fan_temperature_setting_ID, &Session_SettingData.fan_ctrl_temperature_setting, sizeof(Session_SettingData.fan_ctrl_temperature_setting)},
	{fan_control_ID, &Session_SettingData.fan_ctrl, sizeof(Session_SettingData.fan_ctrl)},
	{red_light_bed_dev_name_ID, Session_SettingData.red_light_bed_dev_name, sizeof(Session_SettingData.red_light_bed_dev_name)}

};


void feature_dpid_setValueAdr_map_init(void)
{
}

unsigned char get_dpid(unsigned char feature_id)
{
	for (unsigned char i = 0; i < sizeof(DpidMap) / sizeof(FEATRUE_DPID_MAP_ST); i++)
	{
		if (DpidMap[i].feature_id == feature_id)
			return DpidMap[i].dpid;
	}
	return 0;
}

void *get_set_value_adr(unsigned char feature_id, unsigned char *value_len)
{
	for (unsigned char i = 0; i < sizeof(SetValueAdrMap) / sizeof(FEATRUE_SETVALUE_MAP_ST); i++)
	{
		if (SetValueAdrMap[i].feature_id == feature_id)
		{

			*value_len = SetValueAdrMap[i].value_len;
			return SetValueAdrMap[i].value_adr;
		}
	}
	return 0;
}

void *get_session_set_value_adr(unsigned char feature_id,unsigned char *value_len)
{
	for (unsigned char i = 0; i < sizeof(SessionSetValueAdrMap) / sizeof(FEATRUE_SETVALUE_MAP_ST); i++)
	{
		if (SessionSetValueAdrMap[i].feature_id == feature_id)
		{

			*value_len = SessionSetValueAdrMap[i].value_len;
			return SessionSetValueAdrMap[i].value_adr;
		}
	}
	return 0;
	
}


#include "dpid_map_c.h"
#include "mcu.h"
#include "app_cfg_cmd.h"

static const FEATRUE_DPID_MAP_ST DpidMap[] = {
	{Connect_Status_ID, DPID_CONNECT_STATE},				// 0x00, 100
	{Power_Switch_ID, DPID_POWER_SWITCH},					// 0x01, 101
	{Heat_Switch_ID, DPID_HEAT_SWITCH},						// 0x02, 102
	{Heat_Time_ID, DPID_TIME_SET},							// 0x03, 103
	{Celsius_Temperature_Setting_ID, DPID_TEMP_C_SET},		// 0x04, 104
	{Temperature_Scale_Switching_ID, DPID_TEM_CHANGE},		// 0x05, 105
	{Timer_ON_OFF_ID, DPID_TIMING_SWITCH},					// 0x06, 106
	{Internal_Reading_Light_Switch_ID, DPID_LIGHT_SWITCH1}, // 0x07, 107
	{External_Reading_Light_Switch_ID, DPID_LIGHT_SWITCH2}, // 0x08, 108
	{Heating_Display_ID, DPID_HEATING_DISPLAY},				// 0x09,	109

	{Light_Color_ID, DPID_LIGHT_COLOR},					// 0x0a, 110
	{Temp_Set_ID, DPID_TEMP_SET},						// 0x0b, 111
	{Media_Switch_ID, DPID_MEDIA_SWITCH},				// 0x0c, 112
	{Fan_Switch_ID, DPID_FAN_SWITCH},					// 0x0d, 113
	{Color_Light_Switch_ID, DPID_COLOR_SWITCH},			// 0x0e, 115
	{Color_Change_Switch_ID, DPID_COLOR_CHANGE_SWITCH}, // 0x0f, 116
	{0x10, 0},											// 0x10, 0
	{0x11, 0},											// 0x11, 0
	{0x12, 0},											// 0x12, 0
	{0x13, 0},											// 0x13, 0
	{0x14, 0},											// 0x14, 0
	{0x15, 0},											// 0x15, 0

	{heat1_power_ID, DPID_HEAT1_POWER},						// 0x16, 116
	{heat2_power_ID, DPID_HEAT2_POWER},						// 0x17, 116
	{heat3_power_ID, DPID_HEAT3_POWER},						// 0x18, 116
	{0x19, 0},												// 0x19, 0
	{0x1A, 0},												// 0x1A, 0
	{0x1B, 0},												// 0x1B, 0
	{0x1C, 0},												// 0x1C, 0
	{0x1D, 0},												// 0x1D, 0
	{0x1E, 0},												// 0x1E, 0
	{0x1F, 0},												// 0x1F, 0
	{Heat_Reservation_Switch_ID, DPID_HEATING_RESERVATION}, // 0x20, 116

};

static const FEATRUE_SETVALUE_MAP_ST SetValueAdrMap[] = {
	{Connect_Status_ID, &SettingData.app_connect_state},
	{Power_Switch_ID, &SettingData.power_switch},									// 101
	{Heat_Switch_ID, &SettingData.heat_switch},										// 102
	{Heat_Time_ID, &SettingData.heatint_time},										// 103
	{Celsius_Temperature_Setting_ID, &SettingData.celsius_temp_setting},			// 104
	{Temperature_Scale_Switching_ID, &SettingData.temp_scale_switching},			// 105
	{Timer_ON_OFF_ID, (void *)&SettingData.timer_on_off},							// 106
	{Internal_Reading_Light_Switch_ID, &SettingData.internal_reading_light_switch}, // 107
	{External_Reading_Light_Switch_ID, &SettingData.external_reading_light_switch}, // 108
	{Heating_Display_ID, &SettingData.heating_display},								//	109

	{Light_Color_ID, SettingData.led_ambient_light_adjustment},					  // 110
	{Temp_Set_ID, &SettingData.fahrenheit_temperature_setting},					  // 111
	{Media_Switch_ID, &SettingData.multimedia_switch},							  // 112
	{Fan_Switch_ID, &SettingData.fan_switch},									  // 113
	{Color_Light_Switch_ID, &SettingData.color_light_switch},					  // 115
	{Color_Change_Switch_ID, &SettingData.led_ambient_light_color_change_switch}, // 116
	{0x10, 0},																	  // 0x10, 0
	{0x11, 0},																	  // 0x11, 0
	{0x12, 0},																	  // 0x12, 0
	{0x13, 0},																	  // 0x13, 0
	{0x14, 0},																	  // 0x14, 0
	{0x15, 0},																	  // 0x15, 0
	{heat1_power_ID, &SettingData.heat1_power},
	{heat2_power_ID, &SettingData.heat2_power},
	{heat3_power_ID, &SettingData.heat3_power},
	{0x19, 0},															   // 0x19, 0
	{0x1A, 0},															   // 0x1A, 0
	{0x1B, 0},															   // 0x1B, 0
	{0x1C, 0},															   // 0x1C, 0
	{0x1D, 0},															   // 0x1D, 0
	{0x1E, 0},															   // 0x1E, 0
	{0x1F, 0},															   // 0x1F, 0
	{Heat_Reservation_Switch_ID, &SettingData.heating_reservation_switch}, // 0x20, 117

};

static const FEATRUE_SETVALUE_MAP_ST SessionSetValueAdrMap[] = {
	{Connect_Status_ID, &Session_SettingData.app_connect_state},
	{Power_Switch_ID, &Session_SettingData.power_switch},									// 101
	{Heat_Switch_ID, &Session_SettingData.heat_switch},										// 102
	{Heat_Time_ID, &Session_SettingData.heatint_time},										// 103
	{Celsius_Temperature_Setting_ID, &Session_SettingData.celsius_temp_setting},			// 104
	{Temperature_Scale_Switching_ID, &Session_SettingData.temp_scale_switching},			// 105
	{Timer_ON_OFF_ID, (void *)&Session_SettingData.timer_on_off},							// 106
	{Internal_Reading_Light_Switch_ID, &Session_SettingData.internal_reading_light_switch}, // 107
	{External_Reading_Light_Switch_ID, &Session_SettingData.external_reading_light_switch}, // 108
	{Heating_Display_ID, &Session_SettingData.heating_display},								//	109

	{Light_Color_ID, Session_SettingData.led_ambient_light_adjustment},					  // 110
	{Temp_Set_ID, &Session_SettingData.fahrenheit_temperature_setting},					  // 111
	{Media_Switch_ID, &Session_SettingData.multimedia_switch},							  // 112
	{Fan_Switch_ID, &Session_SettingData.fan_switch},									  // 113
	{Color_Light_Switch_ID, &Session_SettingData.color_light_switch},					  // 115
	{Color_Change_Switch_ID, &Session_SettingData.led_ambient_light_color_change_switch}, // 116
	{0x10, 0},																	  // 0x10, 0
	{0x11, 0},																	  // 0x11, 0
	{0x12, 0},																	  // 0x12, 0
	{0x13, 0},																	  // 0x13, 0
	{0x14, 0},																	  // 0x14, 0
	{0x15, 0},																	  // 0x15, 0
	{heat1_power_ID, &Session_SettingData.heat1_power},
	{heat2_power_ID, &Session_SettingData.heat2_power},
	{heat3_power_ID, &Session_SettingData.heat3_power},
	{0x19, 0},															   // 0x19, 0
	{0x1A, 0},															   // 0x1A, 0
	{0x1B, 0},															   // 0x1B, 0
	{0x1C, 0},															   // 0x1C, 0
	{0x1D, 0},															   // 0x1D, 0
	{0x1E, 0},															   // 0x1E, 0
	{0x1F, 0},															   // 0x1F, 0
	{Heat_Reservation_Switch_ID, &Session_SettingData.heating_reservation_switch}, // 0x20, 117

};


void feature_dpid_setValueAdr_map_init(void)
{
}

unsigned char get_dpid(unsigned char feature_id)
{
	
	return DpidMap[feature_id].dpid;
}

void *get_set_value_adr(unsigned char feature_id)
{

	return SetValueAdrMap[feature_id].value_adr;
}

void *get_session_set_value_adr(unsigned char feature_id)
{
	return SessionSetValueAdrMap[feature_id].value_adr;
}


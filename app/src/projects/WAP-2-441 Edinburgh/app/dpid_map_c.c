#include "dpid_map_c.h"
#include "mcu.h"
#include "app_cfg_cmd.h"

/*
  Connect_Status_ID  =0x00,
	Real_Display_Of_Fahrenheit_Temp_ID=0x21,
  Indoor_Temp_ID=0x31,
  Power_Switch_ID =0x01,
	Heat_Switch_ID , //0x02
	Heat_Time_ID,   //0x03
	Celsius_Temperature_Setting_ID, //0x04
	Temperature_Scale_Switching_ID, //0x05
	Timer_ON_OFF_ID,  //0x06
	Internal_Reading_Light_Switch_ID, //0x07
	External_Reading_Light_Switch_ID, //0x08
	Heating_Display_ID,  //0x09
	Light_Color_ID,  //led_ambient_light_adjustment 0x0A
	Temp_Set_ID,  //0X0B
	Media_Switch_ID, //0X0C
	Fan_Switch_ID, //0X0D
	Color_Light_Switch_ID, //0X0E
	Color_Change_Switch_ID, //0X0F
  Heat_Reservation_Switch_ID=0x20
*/

static const FEATRUE_DPID_MAP_ST DpidMap[] = {
	{Connect_Status_ID, DPID_CONNECT_STATE},
	{Power_Switch_ID, DPID_POWER_SWITCH},
	{Celsius_Temperature_Setting_ID, DPID_TEMP_C_SET},
	{Temperature_Scale_Switching_ID, DPID_TEMP_SCALE_SWITCH},
	{Fahrenheit_Temperature_Setting_ID, DPID_TEMP_F_SET},
	{Real_Display_Of_Fahrenheit_Temp_ID, DPID_TEMP_F_DISP},
	{Lock_Screen_ID, DPID_LOCK_SCREEN_SWITCH},
	{Water_Pump_Flow_Switch_ID, DPID_WATER_PUMP_FLOW_SWITCH},
	{Water_Flow_Rate_ID, DPID_WATER_FLOW_RATE},
	{Touch_Screen_Button_Sound_ID, DPID_SCREEN_BUTTON_SOUND_SWITCH},

	{Ozone_valve_opening_time_ID, DPID_OZONE_VALUE_OPEN_TIME},
	{Auto_Turn_on_After_PowerOn_ID, DPID_AUTO_TURN_ON_AFTER_POWER_ON},
	{Vacuum_Pump_Water_Flow_Switch_ID, DPID_VACUUM_PUMP_WATER_FLOW_SWITCH},
	{Real_Display_Of_Celsius_Temp_ID, DPID_TEMP_C_DISP}

};
static const FEATRUE_SETVALUE_MAP_ST SetValueAdrMap[] = {

	{Connect_Status_ID, &SettingData.app_connect_state},
	{Power_Switch_ID, &SettingData.power_switch},
	{Celsius_Temperature_Setting_ID, &SettingData.celsius_temp_setting},
	{Temperature_Scale_Switching_ID, &SettingData.temp_scale_switching},
	{Fahrenheit_Temperature_Setting_ID, &SettingData.fahrenheit_temperature_setting},

	{Lock_Screen_ID, &SettingData.lock_screen},
	{Touch_Screen_Button_Sound_ID, &SettingData.touch_screen_button_sound},

	{Ozone_valve_opening_time_ID, &SettingData.qzone_valve_opening_time},
	{Auto_Turn_on_After_PowerOn_ID, &SettingData.auto_turn_on_after_powerOn}

};

static const FEATRUE_SETVALUE_MAP_ST SessionSetValueAdrMap[] = {

	{Connect_Status_ID, &Session_SettingData.app_connect_state},
	{Power_Switch_ID, &Session_SettingData.power_switch},
	{Celsius_Temperature_Setting_ID, &Session_SettingData.celsius_temp_setting},
	{Temperature_Scale_Switching_ID, &Session_SettingData.temp_scale_switching},
	{Fahrenheit_Temperature_Setting_ID, &Session_SettingData.fahrenheit_temperature_setting},

	{Lock_Screen_ID, &Session_SettingData.lock_screen},
	{Touch_Screen_Button_Sound_ID, &Session_SettingData.touch_screen_button_sound},

	{Ozone_valve_opening_time_ID, &Session_SettingData.qzone_valve_opening_time},
	{Auto_Turn_on_After_PowerOn_ID, &Session_SettingData.auto_turn_on_after_powerOn}

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

void *get_set_value_adr(unsigned char feature_id)
{
	for (unsigned char i = 0; i < sizeof(SetValueAdrMap) / sizeof(FEATRUE_SETVALUE_MAP_ST); i++)
	{
		if (SetValueAdrMap[i].feature_id == feature_id)
			return SetValueAdrMap[i].value_adr;
	}
	return 0;

	
}

void *get_session_set_value_adr(unsigned char feature_id)
{
	for (unsigned char i = 0; i < sizeof(SessionSetValueAdrMap) / sizeof(FEATRUE_SETVALUE_MAP_ST); i++)
	{
		if (SessionSetValueAdrMap[i].feature_id == feature_id)
			return SessionSetValueAdrMap[i].value_adr;
	}
	return 0;
	
}

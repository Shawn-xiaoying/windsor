#include "dpid_map.h"
#include "app_cfg_cmd.h"
#include "protocol.h"
#include <map>
using namespace std ;
#ifdef __cplusplus
extern "C" {
#endif


static  std::map<unsigned char , unsigned char > DpidMap ;
static  std::map<unsigned char , void* > SetValueAdrMap ;

void  feature_dpid_setValueAdr_map_init(void)
{
	DpidMap[1]= DPID_POWER_SWITCH  ; //101
	DpidMap[2]=DPID_HEAT_SWITCH  ;  //102
	DpidMap[3]=DPID_TIME_SET  ;  //103
	DpidMap[4]=DPID_TEM_SET;                // 104
	DpidMap[5]=DPID_TEM_CHANGE;            // 105
	DpidMap[6]=DPID_TIMING_SWITCH;    				//106
	DpidMap[7]=DPID_LIGHT_SWITCH1;    				//107
	DpidMap[8]=DPID_LIGHT_SWITCH2;    				//108
	DpidMap[9]=DPID_HEATING_DISPLAY;    				//	109
	
	
	DpidMap[0x0a]=DPID_LIGHT_COLOR ;        // 110
	DpidMap[0x0b]=DPID_TEMP_SET	;			//111
	DpidMap[0x0c]=DPID_MEDIA_SWITCH;			//112
  DpidMap[0x0d]=DPID_FAN_SWITCH;			//113
	DpidMap[0x31]=DPID_INDOOR_TEM;			//114
	DpidMap[0x0e]=DPID_COLOR_SWITCH;			//115
  DpidMap[0x0f]=DPID_COLOR_CHANGE_SWITCH;				//116
	DpidMap[0x20]=DPID_HEATING_RESERVATION;				//117
	DpidMap[0x21]=DPID_TEMP_DISPLAY;				//119
	
		
	SetValueAdrMap[1]= &SettingData.power_switch ;
	SetValueAdrMap[2]=&SettingData.heat_switch;
	SetValueAdrMap[3]=&SettingData.heatint_time;
	SetValueAdrMap[4]=&SettingData.celsius_temp_setting;
	SetValueAdrMap[5]=&SettingData.temp_scale_switching;
	SetValueAdrMap[6]=(void*)&SettingData.timer_on_off;
	SetValueAdrMap[7]=&SettingData.internal_reading_light_switch;
	SetValueAdrMap[8]=&SettingData.external_reading_light_switch;
	SetValueAdrMap[9]=&SettingData.heating_display;
	
	
	SetValueAdrMap[0x0a]=SettingData.led_ambient_light_adjustment;
	SetValueAdrMap[0x0b]=&SettingData.fahrenheit_temperature_setting;
	SetValueAdrMap[0x0c]=&SettingData.multimedia_switch;
  SetValueAdrMap[0x0d]=&SettingData.fan_switch;
	SetValueAdrMap[0x0e]=&SettingData.color_light_switch;
  SetValueAdrMap[0x0f]=&SettingData.led_ambient_light_color_change_switch;
	SetValueAdrMap[0x20]=&SettingData.heating_reservation_switch;
	

}	

unsigned char get_dpid(unsigned char feature_id)
{
	
	return DpidMap[feature_id] ;
}

void* get_set_value_adr(unsigned char feature_id)
{
	return SetValueAdrMap[feature_id];
	
}


#ifdef __cplusplus
}
#endif
#include "dpid_map_c.h"
#include "mcu.h"
#include "app_cfg_cmd.h"

static const FEATRUE_DPID_MAP_ST DpidMap[] = {
	{connect_status_ID, DPID_CONNECT_STATE},				
	{air_conditioning_temp_set_ID, DPID_AIR_CONDITION_TEMP_SET},				
	{air_conditioning_remote_switch_ID, DPID_AIR_CONDITION_REMOET_SWITCH},						
	{ac_power_switch_ID, DPID_INT_AC_POWER_SWITCH},							
	{ac_onoff_switch_ID, DPID_INT_AC_ONOFF_SWITCH},		
	{select_ata_ID, DPID_SELECT_ATA},		
	{bibs1_air_switch_ID, DPID_BIBS1_AIR_SWITCH},				
	{bibs1_o2_switch_ID, DPID_BIBS1_O2_SWITCH}, 
	{bibs2_air_switch_ID, DPID_BIBS2_AIR_SWITCH}, 
	{bibs2_o2_switch_ID, DPID_BIBS2_O2_SWITCH},				

	{door_power_switch_ID, DPID_DOOR_POWER_SWITCH},					
	{value_chamber_switch_ID, DPID_VALUE_CHAMBER_SWITCH},				
	{top_light_switch_ID, DPID_TOP_LIGHT_SWITCH},				
	{bottom_light_switch_ID, DPID_BOTTOM_LIGHT_SWITCH},			
	{top_light_color_ID, DPID_TOP_LIGHT_ADJUST},			
	{bottom_light_color_ID, DPID_BOTTOM_LIGHT_ADJUST},
	{fcv_exit_set_value_ID, DPID_FCV_EXIT_SET_VALUE},
  {fcv_entry_set_value_ID,DPID_FCV_ENTRY_SET_VALUE},
	{acp1_remote_cmd_ID, DPID_ACP1_REMOTE_CMD},					
	{acp2_remote_cmd_ID, DPID_ACP2_REMOTE_CMD},						
	{o2_generator_1_run_state_ID, DPID_O2_GENERATOR_1_RUN_STATE},					
	{o2_generator_2_run_state_ID, DPID_O2_GENERATOR_2_RUN_STATE}, 

};

static const FEATRUE_SETVALUE_MAP_ST SetValueAdrMap[] = {
	{connect_status_ID, &SettingData.app_connect_state,sizeof(SettingData.app_connect_state)},
	{air_conditioning_temp_set_ID, (void*)&SettingData.air_conditioning_temp_set,sizeof(SettingData.air_conditioning_temp_set)},				
	{air_conditioning_remote_switch_ID, &SettingData.air_conditioning_remote_switch,sizeof(SettingData.air_conditioning_remote_switch)},						
	{ac_power_switch_ID, &SettingData.ac_power_switch,sizeof(SettingData.ac_power_switch)},							
	{ac_onoff_switch_ID, &SettingData.ac_onoff_switch,sizeof(SettingData.ac_onoff_switch)},		
	{select_ata_ID, &SettingData.select_ata,sizeof(SettingData.select_ata)},		
	{bibs1_air_switch_ID, &SettingData.bibs1_air_switch,sizeof(SettingData.bibs1_air_switch)},				
	{bibs1_o2_switch_ID, &SettingData.bibs1_o2_switch,sizeof(SettingData.bibs1_o2_switch)}, 
	{bibs2_air_switch_ID, &SettingData.bibs2_air_switch,sizeof(SettingData.bibs2_air_switch)}, 
	{bibs2_o2_switch_ID, &SettingData.bibs2_o2_switch,sizeof(SettingData.bibs2_o2_switch)},				

	{door_power_switch_ID, &SettingData.door_power_switch,sizeof(SettingData.door_power_switch)},					
	{value_chamber_switch_ID, &SettingData.value_chamber_switch,sizeof(SettingData.value_chamber_switch)},				
	{top_light_switch_ID, &SettingData.top_light_switch,sizeof(SettingData.top_light_switch)},				
	{bottom_light_switch_ID, &SettingData.bottom_light_switch,sizeof(SettingData.bottom_light_switch)},			
	{top_light_color_ID, (void*)&SettingData.top_light_color,sizeof(SettingData.top_light_color)},			
	{bottom_light_color_ID, (void*)&SettingData.bottom_light_color,sizeof(SettingData.bottom_light_color)},
	{fcv_exit_set_value_ID, (void*)&SettingData.fcv_exit_set_value,sizeof(SettingData.fcv_exit_set_value)},
	{fcv_entry_set_value_ID,(void*)&SettingData.fcv_entry_set_value,sizeof(SettingData.fcv_entry_set_value)},
	{acp1_remote_cmd_ID, &SettingData.acp1_remote_cmd,sizeof(SettingData.acp1_remote_cmd)},					
	{acp2_remote_cmd_ID, &SettingData.acp2_remote_cmd,sizeof(SettingData.acp2_remote_cmd)},						
	{o2_generator_1_run_state_ID, &SettingData.o2_generator_1_run_state,sizeof(SettingData.o2_generator_1_run_state)},					
	{o2_generator_2_run_state_ID, &SettingData.o2_generator_2_run_state,sizeof(SettingData.o2_generator_2_run_state)} 
};

static const FEATRUE_SETVALUE_MAP_ST SessionSetValueAdrMap[] = {
	{connect_status_ID, &Session_SettingData.app_connect_state,sizeof(Session_SettingData.app_connect_state)},
	{air_conditioning_temp_set_ID, (void*)&Session_SettingData.air_conditioning_temp_set,sizeof(Session_SettingData.air_conditioning_temp_set)},				
	{air_conditioning_remote_switch_ID, &Session_SettingData.air_conditioning_remote_switch,sizeof(Session_SettingData.air_conditioning_remote_switch)},						
	{ac_power_switch_ID, &Session_SettingData.ac_power_switch,sizeof(Session_SettingData.ac_power_switch)},							
	{ac_onoff_switch_ID, &Session_SettingData.ac_onoff_switch,sizeof(Session_SettingData.ac_onoff_switch)},		
	{select_ata_ID, &Session_SettingData.select_ata,sizeof(Session_SettingData.select_ata)},		
	{bibs1_air_switch_ID, &Session_SettingData.bibs1_air_switch,sizeof(Session_SettingData.bibs1_air_switch)},				
	{bibs1_o2_switch_ID, &Session_SettingData.bibs1_o2_switch,sizeof(Session_SettingData.bibs1_o2_switch)}, 
	{bibs2_air_switch_ID, &Session_SettingData.bibs2_air_switch,sizeof(Session_SettingData.bibs2_air_switch)}, 
	{bibs2_o2_switch_ID, &Session_SettingData.bibs2_o2_switch,sizeof(Session_SettingData.bibs2_o2_switch)},				

	{door_power_switch_ID, &Session_SettingData.door_power_switch,sizeof(Session_SettingData.door_power_switch)},					
	{value_chamber_switch_ID, &Session_SettingData.value_chamber_switch,sizeof(Session_SettingData.value_chamber_switch)},				
	{top_light_switch_ID, &Session_SettingData.top_light_switch,sizeof(Session_SettingData.top_light_switch)},				
	{bottom_light_switch_ID, &Session_SettingData.bottom_light_switch,sizeof(Session_SettingData.bottom_light_switch)},			
	{top_light_color_ID, (void*)&Session_SettingData.top_light_color,sizeof(Session_SettingData.top_light_color)},			
	{bottom_light_color_ID, (void*)&Session_SettingData.bottom_light_color,sizeof(Session_SettingData.bottom_light_color)},
	{fcv_exit_set_value_ID, (void*)&Session_SettingData.fcv_exit_set_value,sizeof(Session_SettingData.fcv_exit_set_value)},
	{fcv_entry_set_value_ID,(void*)&Session_SettingData.fcv_entry_set_value,sizeof(Session_SettingData.fcv_entry_set_value)},
	{acp1_remote_cmd_ID, &Session_SettingData.acp1_remote_cmd,sizeof(Session_SettingData.acp1_remote_cmd)},					
	{acp2_remote_cmd_ID, &Session_SettingData.acp2_remote_cmd,sizeof(Session_SettingData.acp2_remote_cmd)},						
	{o2_generator_1_run_state_ID, &Session_SettingData.o2_generator_1_run_state,sizeof(Session_SettingData.o2_generator_1_run_state)},					
	{o2_generator_2_run_state_ID, &Session_SettingData.o2_generator_2_run_state,sizeof(Session_SettingData.o2_generator_2_run_state)} 
};

void feature_dpid_setValueAdr_map_init(void)
{
	
}

unsigned short get_dpid(unsigned char feature_id)
{
	for (unsigned char i = 0; i < sizeof(DpidMap) / sizeof(FEATRUE_DPID_MAP_ST); i++)
	{
		if (DpidMap[i].feature_id == feature_id)
			return DpidMap[i].dpid;
	}
	return 0;
}


void *get_set_value_adr(unsigned char feature_id,unsigned char *value_len)
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

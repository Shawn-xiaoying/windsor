#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "implemention_cfg_cmd.h"
#include "session_parameters_check_implent.h"

uint8_t check_the_feature_id_value_is_valid(uint8_t feature_id, uint8_t length, uint8_t *value)
{
	int16_t word_value = 0;
	switch (feature_id)
	{
	case air_conditioning_remote_switch_ID:
	case ac_power_switch_ID:
	case ac_onoff_switch_ID:
	case bibs1_air_switch_ID:
	case bibs1_o2_switch_ID:
	case bibs2_air_switch_ID:
	case bibs2_o2_switch_ID:
	case door_power_switch_ID:
	case value_chamber_switch_ID:
	case top_light_switch_ID:
	case bottom_light_switch_ID:
	case acp1_remote_cmd_ID:
	case acp2_remote_cmd_ID:
	case o2_generator_1_run_state_ID:
	case o2_generator_2_run_state_ID:
		if (*value == 0 || *value == 1)
			return 0;
		else
			return 1;

	case air_conditioning_temp_set_ID:

	 	word_value = value[0] << 8 | value[1];
		if ( word_value  <= 400 && word_value  >= -200)
		return 0;
		else
		return 1;
	
	case fcv_exit_set_value_ID:
	case fcv_entry_set_value_ID:
		
	  	word_value = value[0] << 8 | value[1];
			if (word_value > 1000)
				return 1;
	
	    return 0;
	
	default:
		break;
	}

	return 0;
}

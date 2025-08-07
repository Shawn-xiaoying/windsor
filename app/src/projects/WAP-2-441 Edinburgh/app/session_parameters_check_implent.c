#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "implemention_cfg_cmd.h"
#include "session_parameters_check_implent.h"

uint8_t check_the_feature_id_value_is_valid(uint8_t feature_id, uint8_t length, uint8_t *value)
{
	switch (feature_id)
	{
	case Power_Switch_ID:
	case Lock_Screen_ID:
	case Touch_Screen_Button_Sound_ID:
	case Auto_Turn_on_After_PowerOn_ID:
	case Temperature_Scale_Switching_ID:

		if (*value == 0 || *value == 1)
			return 0;
		else
			return 1;
	case Ozone_valve_opening_time_ID:
		if (*value >= 0 && *value <= 5)
			return 0;
		else
			return 1;
		
	case Fahrenheit_Temperature_Setting_ID:
		if (*value >= 37 && *value <= 108)
			return 0;
		else
			return 1;

	
	case Celsius_Temperature_Setting_ID:
		if (*value >= 3 && *value <= 42)
			return 0;
		else
			return 1;

		
	default:
		break;
	}

	return 0;
}

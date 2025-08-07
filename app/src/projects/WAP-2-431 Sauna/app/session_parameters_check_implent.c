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
	case Heat_Switch_ID:
	case Internal_Reading_Light_Switch_ID:
	case External_Reading_Light_Switch_ID:
	case Media_Switch_ID:
	case Fan_Switch_ID:
	case Color_Light_Switch_ID:
	case Color_Change_Switch_ID:
	case Heat_Reservation_Switch_ID:
	case Temperature_Scale_Switching_ID:
		if (*value == 0 || *value == 1)
			return 0;
		else
			return 1;

	case Heat_Time_ID:
		if (*value <= 90)
			return 0;
		else
			return 1;
	case heat1_power_ID:
	case heat2_power_ID:
	case heat3_power_ID:
		if (*value <= 100)
			return 0;
		else
			return 1;
		break;
	case Celsius_Temperature_Setting_ID:
		if (*value >= 20 && *value <= 65)
			return 0;
		else
			return 1;

	case Timer_ON_OFF_ID:
		if (*value <= 1440)
			return 0;
		else
			return 1;

	case Heating_Display_ID:
		if (*value == 0 || *value == 1 || *value == 2)
			return 0;
		else
			return 1;

	case Light_Color_ID:

		break;
	case Temp_Set_ID:

		break;

		//  case  Real_Display_Of_Fahrenheit_Temp_ID:
		//  case  Indoor_Temp_ID

	default:
		break;
	}

	return 0;
}

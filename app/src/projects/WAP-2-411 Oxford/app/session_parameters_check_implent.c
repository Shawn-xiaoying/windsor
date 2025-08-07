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
	case Air_Compressor_ID:
	case Oxygen_Concentrator_ID:
	case Air_Conditioner_ID:
		if (*value == 0 || *value == 1)
			return 0;
		else
			return 1;

	case Remaining_Minutes_Timer_ID:

		return 0;

	case Target_Temperature_ID:
		//		 if(*value < 20) *value =20 ;
		//
		//	   if(*value>250)  *value =250 ;
		return 0;

	default:
		break;
	}

	return 0;
}

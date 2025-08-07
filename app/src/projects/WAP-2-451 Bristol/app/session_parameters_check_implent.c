#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "app_util.h"
#include "app_error.h"
#include "implemention_cfg_cmd.h"
#include "session_parameters_check_implent.h"

uint8_t check_the_feature_id_value_is_valid(uint8_t feature_id, uint8_t length, uint8_t *value)
{
	uint8_t array_index = 0;
	uint16_t word_value = 0;
	switch (feature_id)
	{
	case working_time_setting_ID:
		if (*value <= 30)
			return 0;
		else
			return 1;

	case pulse_output_ID:
		if (*value <= 2)
			return 0;
		else
			return 1;

	case pulse_selection_ID:
		if (*value <= 0x3f)
			return 0;
		else
			return 1;

	case pulse_frequency_setting_ID:
		if (length != 12)
			return 1;
		for (array_index = 0; array_index < length / 2; array_index++)
		{
			word_value = value[array_index * 2] << 8 | value[array_index * 2 + 1];
			if (word_value > 10000)
				return 1;
		}
		return 0;

	case pulse_duty_cycle_setting_ID:

		if (length != 6)
			return 1;
		for (array_index = 0; array_index < length; array_index++)
		{

			if (value[array_index] > 100)
				return 1;
		}
		return 0;

	case music_control_ID:
		if (*value <= 3)
			return 0;
		else
			return 1;

	case music_volume_ID:
		if (*value <= 10)
			return 0;
		else
			return 1;

	case fan_gear_ID:
		if (*value <= 3)
			return 0;
		else
			return 1;
	case equipment_time_setting_ID:
		if (length != 7)
			return 1;
		if (value[0] << 8 | value[1] < 2025)
			return 1; // year
		if (value[2] > 12)
			return 1; // month
		if (value[3] > 31)
			return 1; // day
		if (value[4] > 23)
			return 1; // hour
		if (value[5] > 59)
			return 1; // minute
		if (value[6] > 59)
			return 1; // second
		return 0;

	case fan_temperature_setting_ID:
		if (length != 2)
			return 1;
		if (value[0] > 99 || value[1] > 99)
			return 1;

		return 0;

	case fan_control_ID:
		if (length != 2)
			return 1;
		if (value[0] > 3 || value[1] > 60)
			return 1;

		return 0;

	case red_light_bed_dev_name_ID:
		if (length != 12)
			return 1;
		return 0;

	default:
		break;
	}

	return 0;
}

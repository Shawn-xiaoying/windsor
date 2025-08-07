
#ifndef MT_GPIO_H__
#define MT_GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf_gpio.h"
#include "mt_param_def.h"
/*
Hardware version R2 is a Bluetooth board developed by the company itself.
Hardware version R3 is an externally purchased Bluetooth board
*/
#define QINGFENG_DEV_BOARD 0
#define WAP_1_100_R2 1
#define WAP_1_100_R3 2

#ifndef DEVE_BOARD
#define HARD_VERSION WAP_1_100_R3
#else
#define HARD_VERSION QINGFENG_DEV_BOARD
#endif

#define TIME_45MS 45UL

#if (HARD_VERSION == WAP_1_100_R2)

#define IO_RESET_IN NRF_GPIO_PIN_MAP(1, 6) // 清除ShortUserID并进入配对模式

#define CONNCET_BOND_LED_PIN NRF_GPIO_PIN_MAP(0, 13) // P0.13//蓝牙绑定led指示，蓝牙配对成功输出1，未配对输出0
#define CONNCET_BOND_LED_ON() nrf_gpio_pin_set(CONNCET_BOND_LED_PIN)
#define CONNCET_BOND_LED_OFF() nrf_gpio_pin_clear(CONNCET_BOND_LED_PIN)

#define RUN_LED_PIN NRF_GPIO_PIN_MAP(1, 4)
#define RUN_LED_ON() nrf_gpio_pin_clear(RUN_LED_PIN)
#define RUN_LED_OFF() nrf_gpio_pin_set(RUN_LED_PIN)

#elif (HARD_VERSION == WAP_1_100_R3)
#define IO_RESET_IN NRF_GPIO_PIN_MAP(0, 6) // 清除ShortUserID并进入配对模式

#define CONNCET_BOND_LED_PIN NRF_GPIO_PIN_MAP(0, 13) // P0.13//蓝牙绑定led指示，蓝牙配对成功输出1，未配对输出0
#define CONNCET_BOND_LED_ON() nrf_gpio_pin_set(CONNCET_BOND_LED_PIN)
#define CONNCET_BOND_LED_OFF() nrf_gpio_pin_clear(CONNCET_BOND_LED_PIN)

#define RUN_LED_PIN NRF_GPIO_PIN_MAP(0, 4)
#define RUN_LED_ON() nrf_gpio_pin_clear(RUN_LED_PIN)
#define RUN_LED_OFF() nrf_gpio_pin_set(RUN_LED_PIN)

#define STM32_BOOT0_PIN NRF_GPIO_PIN_MAP(0, 12)
#define STM32_BOOT0_LOW() nrf_gpio_pin_clear(STM32_BOOT0_PIN)
#define STM32_BOOT0_HIGHT() nrf_gpio_pin_set(STM32_BOOT0_PIN)

#define STM32_RST_PIN NRF_GPIO_PIN_MAP(0, 10)
#define STM32_RST_LOW() nrf_gpio_pin_clear(STM32_RST_PIN)
#define STM32_RST_HIGHT() nrf_gpio_pin_set(STM32_RST_PIN)


#elif (HARD_VERSION == QINGFENG_DEV_BOARD)
#define IO_RESET_IN NRF_GPIO_PIN_MAP(0, 18) // 清除ShortUserID并进入配对模式

#define CONNCET_BOND_LED_PIN NRF_GPIO_PIN_MAP(0, 13) // P0.13//蓝牙绑定led指示，蓝牙配对成功输出1，未配对输出0
#define CONNCET_BOND_LED_ON() nrf_gpio_pin_set(CONNCET_BOND_LED_PIN)
#define CONNCET_BOND_LED_OFF() nrf_gpio_pin_clear(CONNCET_BOND_LED_PIN)

#define RUN_LED_PIN NRF_GPIO_PIN_MAP(0, 14)
#define RUN_LED_ON() nrf_gpio_pin_clear(RUN_LED_PIN)
#define RUN_LED_OFF() nrf_gpio_pin_set(RUN_LED_PIN)

#endif

enum
{
    SET_OFF = 0,
    SET_ON,
};

void key_timer_start(bool set_valu, uint16_t time);

void user_gpio_init(void);
void task_check_input_io(void);

#endif

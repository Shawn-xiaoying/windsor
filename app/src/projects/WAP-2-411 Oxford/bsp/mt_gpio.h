
#ifndef MT_GPIO_H__
#define MT_GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf_gpio.h"

#define TIME_45MS 45UL

#define IO_Oxygen_IN 15  // P0.15  制氧机的工作状态：0关机1开机
#define IO_AirComp_IN 22 // P0.22  空压机的工作状态：0关机1开机

#define IO_Oxygen_OUT 13  // P0.13  制氧机控制管脚：0关机1开机
#define IO_AirComp_OUT 20 // P0.20   空压机的控制管脚：0关机1开机

#define IO_RESET_IN NRF_GPIO_PIN_MAP(1, 6) // 清除ShortUserID并进入配对模式

#define CONNCET_BOND_LED_PIN 24 // 蓝牙绑定led指示，蓝牙配对成功输出1，未配对输出0
#define CONNCET_BOND_LED_ON() nrf_gpio_pin_set(CONNCET_BOND_LED_PIN)
#define CONNCET_BOND_LED_OFF() nrf_gpio_pin_clear(CONNCET_BOND_LED_PIN)

enum
{
    SET_OFF = 0,
    SET_ON,
};

void bsp_IO_Oxygen_ctrl(uint8_t set_valu);
void bsp_IO_AirComp_ctrl(uint8_t set_valu);
void bsp_IO_Oxygen_clear(void);
void bsp_IO_AirComp_Clear(void);

void key_timer_start(bool set_valu, uint16_t time);

void user_gpio_init(void);
void task_check_input_io(void);

void task_read_Oxygen_AirComp_state(void);

#endif

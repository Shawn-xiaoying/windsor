
#ifndef MT_PARAM_DEF_H__
#define MT_PARAM_DEF_H__

#include "controller_config.h"
// #include "nrf_gpio.h"

// #define DEVE_BOARD

/* 软件配置 */

  #define DEF_ADV_NAME     "OneBase" /**< 默认设备名称 */
// #define DEF_ADV_NAME "OneBbse" /**< 默认设备名称 */

#define UART1_BAUD UARTE_BAUDRATE_BAUDRATE_Baud9600

#define PRODUCT_TYPE 0x04
#define PRODUCT_PARAMETERS 0x12345678

#define DEF_CONN_PSWD "12345678" /**< 默认连接密码 */

#if 0
#define DEF_CONN_INT_MIN 20 /**< 默认最小连接间隔单位 ms */
#define DEF_CONN_INT_MAN 40 /**< 默认最大连接间隔单位 ms */

#define SLAVE_LATENCY 0                                  /**< 从机延迟 */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS) /**< 连接超时时间 */

#else
#define DEF_CONN_INT_MIN 20 /**< 默认最小连接间隔单位 ms */
#define DEF_CONN_INT_MAN 40 /**< 默认最大连接间隔单位 ms */

#define SLAVE_LATENCY 30                                 /**< 从机延迟 */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS) /**< 连接超时时间 */

#endif

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(100) /**< 第一次连接参数更新时间 */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000) /**< 第二次连接参数更新时间 */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                      /**< 连接参数更新最大次数 */

#define DEF_ADV_INTERVAL 1 /**< 默认广播间隔单位 100ms*/
#define DEF_TX_POWER 8     /**< 默认发射功率 4dbm*/

/* 硬件配置 */
#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

// #define DEVE_BOARD

#ifdef DEVE_BOARD

#define UART_RX_PIN NRF_GPIO_PIN_MAP(0, 8) // 8 /**< 串口接收引脚. */
#define UART_TX_PIN NRF_GPIO_PIN_MAP(0, 6) // 6 /**< 串口发送引脚. */

#else
// #define UART_RX_PIN     NRF_GPIO_PIN_MAP(1,15)   //8 /**< 串口接收引脚. */
// #define UART_TX_PIN     26// 6 /**< 串口发送引脚. */

#define UART_RX_PIN NRF_GPIO_PIN_MAP(1, 0) // 8 /**< 串口接收引脚. */
#define UART_TX_PIN 24                     // 6 /**< 串口发送引脚. */

#endif

#define UART_RTS_PIN 0xff /**< 流控引脚. */
#define UART_CTS_PIN 0xff /**< 流控引脚. */

#endif

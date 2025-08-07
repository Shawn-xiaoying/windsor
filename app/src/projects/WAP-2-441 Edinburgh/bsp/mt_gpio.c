
#include "nrf_soc.h"

#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_timer.h"
#include "app_cfg_cmd.h"
#include "mt_cst_protocol.h"
#include "nrf_delay.h"
#include "app_service_cfg.h"

#include "mt_gpio.h"
#define NRF_LOG_MODULE_NAME GPIO_INIT
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define MT_GPIO_DEBUG_OUT

static bool start_key_timer_flg = false;

/********************************
 * io 初始化
 * ******************/
static void mt_gpio_init(void)
{

   nrf_gpio_cfg_default(IO_RESET_IN);
   nrf_gpio_cfg_input(IO_RESET_IN, NRF_GPIO_PIN_PULLUP);

   nrf_gpio_cfg_default(CONNCET_BOND_LED_PIN);
   nrf_gpio_cfg_output(CONNCET_BOND_LED_PIN);
   nrf_gpio_pin_clear(CONNCET_BOND_LED_PIN);

   nrf_gpio_cfg_default(RUN_LED_PIN);
   nrf_gpio_cfg_output(RUN_LED_PIN);
   nrf_gpio_pin_set(RUN_LED_PIN);

   // nrf_gpio_cfg_input(18, NRF_GPIO_PIN_PULLUP);

   nrf_gpio_cfg_default(NRF_GPIO_PIN_MAP(0, 18));
   nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0, 18));
   nrf_gpio_pin_set(NRF_GPIO_PIN_MAP(0, 18));
}

// 以下代码为设置 io输入引脚功能检测
/****************************************************************************************
 *function:key_irq_hand
 *description: hanlde pin irq function
 **********************/
void pin_irq_handle(nrf_drv_gpiote_pin_t pin_no, nrf_gpiote_polarity_t action)
{
   if (nrf_gpio_pin_read(IO_RESET_IN) == 0)
   {
      key_timer_start(true, 50);
   }
}

/**********************************************************************
 *function:enable_pin_irq
 *description:  enable pin irq
 **************************************************************/
static void enable_pin_irq(uint32_t pin, bool enable_flg)
{
   uint32_t err_code;

   nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
   config.pull = NRF_GPIO_PIN_PULLUP;

   config.sense = NRF_GPIOTE_POLARITY_HITOLO;
   config.hi_accuracy = false;

   if (!nrf_drv_gpiote_is_init())
   {
      err_code = nrf_drv_gpiote_init();
      APP_ERROR_CHECK(err_code);
   }

   if (true == enable_flg)
   {
      err_code = nrf_drv_gpiote_in_init(pin, &config, pin_irq_handle);
      APP_ERROR_CHECK(err_code);
      nrf_drv_gpiote_in_event_enable(pin, true);
   }
   else
   {
      nrf_drv_gpiote_in_event_disable(pin);
      nrf_drv_gpiote_in_uninit(pin);
   }
}

/********************************************************************/
// timer
APP_TIMER_DEF(key_timer_id);
/*******************************************************/
static void key_timer_Hand(void *p_context)
{
   UNUSED_PARAMETER(p_context);
   start_key_timer_flg = true;
}

void key_timer_start(bool set_valu, uint16_t time)
{
   ret_code_t err_code;
   static uint8_t temp_value = 0xff;

   if (temp_value == set_valu)
      return;
   temp_value = set_valu;

   if (set_valu == true)
   {
      err_code = app_timer_start(key_timer_id, APP_TIMER_TICKS(time), NULL);
   }
   else
   {
      err_code = app_timer_stop(key_timer_id);
   }
//   BLE_RTT("[key timer]  timer_stop err_code=%x...temp_value=%d\r\n",err_code,temp_value);
#ifdef MT_GPIO_DEBUG_OUT
   NRF_LOG_INFO("[KEY] key_timer_start =%d err_code=0x%x", temp_value, err_code);
#endif
}

static void key_timer_init(void)
{
   uint32_t err_code;
   err_code = app_timer_create(&key_timer_id,
                               APP_TIMER_MODE_REPEATED,
                               key_timer_Hand);
   APP_ERROR_CHECK(err_code);
}

extern void clear_mutex(void);
extern void mt_serial_flush(void);

#ifdef USER_PEER_MANGAGE
extern void delete_bonds(void);
#endif

/****************************************
 * 检查输入 io
 **************************/
void task_check_input_io(void)
{
   static uint16_t time_cnt = 0;

   if (start_key_timer_flg == false)
      return;
   start_key_timer_flg = false;

   /****************************************/
   if (1 == nrf_gpio_pin_read(IO_RESET_IN))
   {
      key_timer_start(false, 50);
#ifdef MT_GPIO_DEBUG_OUT
      NRF_LOG_INFO("[KEY] key timer stop %%%%%%%% ");
#endif
      time_cnt = 0;
      //  mt_serial_flush();
   }

   /* 检测 IO_RESET_IN */
   if (0 == nrf_gpio_pin_read(IO_RESET_IN))
   {
      time_cnt++;
   }

   /**************检查io 状态已完成**************************/
   if (time_cnt >= (TIME_45MS * 2)) // 100ms
   //		if(time_cnt >= (10*2))//100ms
   {
      time_cnt = 0;

#ifdef USER_PEER_MANGAGE
      delete_bonds();
#endif

      reset_ShortUserID();
      //	   clear_mutex();
#ifdef MT_GPIO_DEBUG_OUT
      NRF_LOG_INFO("[KEY] time_cnt =%d", time_cnt);
#endif
   }
}

/**********************************************************
 *用户io  口初始化
 * *************************/
void user_gpio_init(void)
{
   key_timer_init();
   mt_gpio_init();

   enable_pin_irq(IO_RESET_IN, true);
}

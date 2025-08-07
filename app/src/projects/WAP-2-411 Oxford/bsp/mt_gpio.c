
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

static bool start_key_timer_flg = false;

/*********************************************
 * 制氧机控制 0关机 --1开机
 ***********************************/
void bsp_IO_Oxygen_ctrl(uint8_t set_valu)
{
   if (set_valu == SET_OFF) // off制氧机关机控制
   {
      if (Get_Response_Pack.Oxygen_Concent.attribute == 1) // 制氧机已经处于开机的状态下才允许关机
      {
         nrf_gpio_pin_set(IO_Oxygen_OUT);
         //  nrf_delay_ms(500);
         //  nrf_gpio_pin_clear(IO_Oxygen_OUT);
      }
   }
   else // on制氧机开机控制
   {
      if (Get_Response_Pack.Oxygen_Concent.attribute == 0) // 制氧机已经处于关机的状态下才允许开机
      {
         nrf_gpio_pin_set(IO_Oxygen_OUT);
         //  nrf_delay_ms(500);
         //  nrf_gpio_pin_clear(IO_Oxygen_OUT);
      }
   }
   NRF_LOG_INFO("[IO] bsp_IO_Oxygen_ctrl =%d", set_valu);
}

void bsp_IO_Oxygen_clear(void)
{
   nrf_gpio_pin_clear(IO_Oxygen_OUT);
}

/*********************************************
 * 0关机 --1开机---空压机
 ***********************************/
void bsp_IO_AirComp_ctrl(uint8_t set_valu)
{
   if (set_valu == SET_OFF) // off空压机关机控制
   {
      if (Get_Response_Pack.Air_Compressor.attribute == 1) // 空压机已经处于开机的状态下才允许关机
      {
         nrf_gpio_pin_set(IO_AirComp_OUT);
         // nrf_delay_ms(500);
         //  nrf_gpio_pin_clear(IO_AirComp_OUT);
      }
   }
   else // on空压机开机控制
   {
      if (Get_Response_Pack.Air_Compressor.attribute == 0) // 空压机已经处于开机的状态下才允许关机
      {
         nrf_gpio_pin_set(IO_AirComp_OUT);
         // nrf_delay_ms(500);
         //  nrf_gpio_pin_clear(IO_AirComp_OUT);
      }
   }
   NRF_LOG_INFO("[IO] bsp_IO_AirComp_ctrl =%d", set_valu);
}

void bsp_IO_AirComp_Clear(void)
{

   nrf_gpio_pin_clear(IO_AirComp_OUT);
}

/********************************
 * io 初始化
 * ******************/
static void mt_gpio_init(void)
{
   // input io
   nrf_gpio_cfg_input(IO_Oxygen_IN, NRF_GPIO_PIN_PULLUP);
   nrf_gpio_cfg_input(IO_AirComp_IN, NRF_GPIO_PIN_PULLUP);

   nrf_gpio_cfg_default(IO_RESET_IN);
   nrf_gpio_cfg_input(IO_RESET_IN, NRF_GPIO_PIN_PULLUP);

   // output io
   nrf_gpio_cfg_default(IO_Oxygen_OUT);
   nrf_gpio_cfg_output(IO_Oxygen_OUT);
   nrf_gpio_pin_clear(IO_Oxygen_OUT);

   nrf_gpio_cfg_default(IO_AirComp_OUT);
   nrf_gpio_cfg_output(IO_AirComp_OUT);
   nrf_gpio_pin_clear(IO_AirComp_OUT);

   nrf_gpio_cfg_default(CONNCET_BOND_LED_PIN);
   nrf_gpio_cfg_output(CONNCET_BOND_LED_PIN);
   nrf_gpio_pin_clear(CONNCET_BOND_LED_PIN);

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

   NRF_LOG_INFO("[KEY] key_timer_start =%d err_code=0x%x", temp_value, err_code);
}

static void key_timer_init(void)
{
   uint32_t err_code;
   // Create timers.
   err_code = app_timer_create(&key_timer_id,
                               APP_TIMER_MODE_REPEATED,
                               key_timer_Hand);
   APP_ERROR_CHECK(err_code);
}

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

      NRF_LOG_INFO("[KEY] key timer stop %%%%%%%% ");
      time_cnt = 0;
   }

   /* 检测 IO_RESET_IN */
   if (0 == nrf_gpio_pin_read(IO_RESET_IN))
   {
      time_cnt++;
   }

   /**************检查io 状态已完成**************************/
   if (time_cnt >= (TIME_45MS * 2)) // 100ms
   {
      time_cnt = 0;
#ifdef USER_PEER_MANGAGE
      delete_bonds();
#endif
      reset_ShortUserID();
      NRF_LOG_INFO("[KEY] time_cnt =%d", time_cnt);
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

/************************************************************************
 * IO_Oxygen_IN---读取制氧器开关状态
 *******************************************/
static void read_Oxygen_OnOff_state(void)
{
   static uint8_t sta = 0, cur_io_sta = 0xff, next_io_sta = 0xff;

   switch (sta)
   {
   case 0: /*上电初始化状态*/
      /*读取当前 io 状态*/
      cur_io_sta = nrf_gpio_pin_read(IO_Oxygen_IN);
      sta = 1;
      break;
   case 1: /*100ms 检测一次*/
      next_io_sta = nrf_gpio_pin_read(IO_Oxygen_IN);
      sta = 1;
      if (cur_io_sta != next_io_sta)
         sta = 2;
      break;

   case 2: /*100ms 检测一次*/
      cur_io_sta = nrf_gpio_pin_read(IO_Oxygen_IN);
      if (cur_io_sta == next_io_sta) /*本次检测有效，赋值给设备信息状态，方便app读取*/
      {
         // Get_Response_Pack.Oxygen_Concent.Pwr_on  = cur_io_sta;
         Get_Response_Pack.Oxygen_Concent.attribute = (cur_io_sta == 0) ? 1 : 0;
         NRF_LOG_INFO("[goip] read_Oxygen_OnOff_state cur_io_sta=%d ", cur_io_sta);
      }
      sta = 1;
      break;
   default:
      break;
   }
}

/************************************************************************
 * IO_AirComp_IN---读取空压机开关状态
 *******************************************/
static void read_AirComp_OnOff_state(void)
{
   static uint8_t sta = 0, cur_io_sta = 0xff, next_io_sta = 0xff;

   switch (sta)
   {
   case 0: /*上电初始化状态*/
      /*读取当前 io 状态*/
      cur_io_sta = nrf_gpio_pin_read(IO_AirComp_IN);
      sta = 1;
      break;
   case 1: /*100ms 检测一次*/
      next_io_sta = nrf_gpio_pin_read(IO_AirComp_IN);
      sta = 1;
      if (cur_io_sta != next_io_sta)
         sta = 2;
      break;

   case 2: /*100ms 检测一次*/
      cur_io_sta = nrf_gpio_pin_read(IO_AirComp_IN);
      if (cur_io_sta == next_io_sta) /*本次检测有效，赋值给设备信息状态，方便app读取*/
      {
         NRF_LOG_INFO("[goip] read_AirComp_OnOff_state cur_io_sta=%d ", cur_io_sta);
         // Get_Response_Pack.Air_Compressor.Pwr_on  = cur_io_sta;
         Get_Response_Pack.Air_Compressor.attribute = (cur_io_sta == 0) ? 1 : 0;
      }
      sta = 1;
      break;
   default:
      break;
   }
}

/****************************************************************
 * 读取制氧器和空压机状态
 * **********************/
void task_read_Oxygen_AirComp_state(void)
{
   static uint32_t temp_time_tick = 0;

   /*100ms 进来一次*/
   if (get_system_time_tick() == temp_time_tick)
      return;
   temp_time_tick = get_system_time_tick();

   read_Oxygen_OnOff_state();
   read_AirComp_OnOff_state();
}


#include "mt_cst_protocol.h"
#include "mt_ble_service.h"
#include "mt_serial.h"
#include "app_timer.h"
#include "mt_ble_service.h"
#include "mt_ble_cfg.h"
#include "ble_gap.h"
#include "aes.h"

//#include "app_service_cfg.h"
#include "bluetooth_connection_management.h"
#include "comm_lib.h"
// #include "HBOT_uart_cmd.h"

#define NRF_LOG_MODULE_NAME cst_data
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define MT_CST_PROTOCOL_DEBUG_OUT

/***********************************************/
DEVICE_INFOR_Pack device_infor = {
    .ShortUserID = {0x00, 0x00, 0x00, 0x00, 0x00}

}; /*设备信息结构体*/

#define M_TEMP_BUF_L 128

static uint8_t m_disconnect_flag = 1;         /* 指示是否要断开连接 0 断开连接 */
static uint8_t m_tmp_buf[M_TEMP_BUF_L] = {0}; /**< 存放解密后的 app 数据 . */
static const uint8_t AESKey[] = DEF_PASSCODE; /**< aes key . */

static uint32_t system_time_tick = 0;
static uint32_t system_time_second = 0;

APP_TIMER_DEF(m_timeout_timer); /**< 命令超时定时器. */
APP_TIMER_DEF(m_sys_timer);     /**< 系统时间定时器. */

#define CMD_TIMEOUT_TIME APP_TIMER_TICKS(SYSTEM_TIME_UINT) /**< 命令超时定时器超时时间 */

/************************************************************************************
 * @brief 命令超时定时器超时处理函数.
 */
static void tm_timeout_handler(void *p_context)
{
    //    m_cmd_statu = CMD_IDLE;
}

/************************************************************************************
 * @brief 系统定时器超时处理函数.
 */
static void sys_timeout_handler(void *p_context)
{
    // device_infor.m_system_clock += SYSTEM_TIME_UINT;//
    system_time_tick++;

    if (system_time_tick % 10 == 0) // 每隔1s
    {
        system_time_second++;
        device_infor.m_system_clock += 1;
    }

   // check_ShortUserID_bond_timeout();
}

uint32_t get_system_time_tick(void)
{
    return system_time_tick;
}
uint32_t get_system_time_second(void)
{
    return system_time_second;
}

/************************************************************
 * check conncet passdword
 ******************************************/
static void check_passcode(uint16_t conn_handle, uint8_t *p_data, uint16_t len)
{
    uint8_t passcode[] = DEF_PASSCODE;
    uint16_t tmp = 0;
	  uint16_t send_len =1;
    tmp = sizeof(passcode) - 1;
	  

    if ((tmp != len) || memcmp(passcode, p_data, tmp))
    {
        m_disconnect_flag = 0;
    }
    else
    {
        m_disconnect_flag = 1;
    }
    ble_ctcs_s_send_handler(conn_handle, &m_disconnect_flag, &send_len, CTCS_PASSCODE_UUID);
}

/************************************************************************************
 * @brief 解密 app 发送数据
 *
 * @param[in] p_data  指向 app 发送数据
 * @param[in] len     app 发送数据长度
 */

void decrypt_app_data(uint8_t *p_data, uint16_t len) __attribute__((unused));
static void decrypt_app_data(uint8_t *p_data, uint16_t len)
{
    uint8_t tmp_buf[128] = {0};

    memcpy(tmp_buf, p_data, len);
    memset(m_tmp_buf, 0, 128);

    for (uint8_t i = 0; i < len; i += 16)
    {
        AES128_ECB_decrypt(&tmp_buf[i], AESKey, &m_tmp_buf[i]);
    }
}

/*********************************************************************
 *  app 服务读取和设置数据 处理函数
 * *******************************/
void ctcs_data_handler(ble_ctcs_evt_t *p_evt)
{

    switch (p_evt->type)
    {
    case BLE_CTCS_EVT_PASSCODE_RX_DATA:
        check_passcode(p_evt->p_ctcs->conn_handle, (uint8_t *)p_evt->params.attachment_data.p_data,
                       p_evt->params.attachment_data.length);
        break;

    case BLE_CTCS_EVT_TRANSPORT_RX_DATA:

//#if OPEN_ASE
//        decrypt_app_data((uint8_t *)p_evt->params.attachment_data.p_data,
//                         p_evt->params.attachment_data.length);
//#else
//        memset(m_tmp_buf, 0, M_TEMP_BUF_L);
//        memcpy(m_tmp_buf,
//               (uint8_t *)p_evt->params.attachment_data.p_data,
//               p_evt->params.attachment_data.length);
//#endif

//#ifdef MT_CST_PROTOCOL_DEBUG_OUT
//        NRF_LOG_INFO("received app data:");

//        NRF_LOG_HEXDUMP_INFO(m_tmp_buf, p_evt->params.attachment_data.length);
//#endif

//        /*app 协议*/

//        user_service_cmd(p_evt->p_ctcs->conn_handle, m_tmp_buf, p_evt->params.attachment_data.length);
			 user_bluetooth_recv_cmd(p_evt->p_ctcs->conn_handle, (uint8_t *)p_evt->params.attachment_data.p_data, p_evt->params.attachment_data.length);
			
        break;
    case BLE_CTCS_EVT_TX_COMPLETE:
#ifdef MT_CST_PROTOCOL_DEBUG_OUT
        NRF_LOG_INFO("cts tx complete:");
#endif

        break;
    default:
        break;
    }
}

/************************************************
 * device_infor_init
 ****************************/
static void device_infor_init(void)
{
    ble_gap_addr_t ble_gap_mac;

    device_infor.ChipID[0] = NRF_FICR->DEVICEID[0] >> 24;
    device_infor.ChipID[1] = NRF_FICR->DEVICEID[0] >> 16;
    device_infor.ChipID[2] = NRF_FICR->DEVICEID[0] >> 8;
    device_infor.ChipID[3] = NRF_FICR->DEVICEID[0];

    device_infor.ChipID[4] = NRF_FICR->DEVICEID[1] >> 24;
    device_infor.ChipID[5] = NRF_FICR->DEVICEID[1] >> 16;
    device_infor.ChipID[6] = NRF_FICR->DEVICEID[1] >> 8;
    device_infor.ChipID[7] = NRF_FICR->DEVICEID[1];

    APP_ERROR_CHECK(sd_ble_gap_addr_get(&ble_gap_mac));
    BIG_LITTLE_ENDIAN_CONVERTER(ble_gap_mac.addr, 6);
    memcpy(device_infor.mac_addr, ble_gap_mac.addr, 6);
}

/************************************************
 * mt_user_protocol_init
 ****************************/
void mt_user_protocol_init(void)
{
    device_infor_init();

    APP_ERROR_CHECK(app_timer_create(&m_timeout_timer, APP_TIMER_MODE_SINGLE_SHOT, tm_timeout_handler));
    APP_ERROR_CHECK(app_timer_create(&m_sys_timer, APP_TIMER_MODE_REPEATED, sys_timeout_handler));
    APP_ERROR_CHECK(app_timer_start(m_sys_timer, CMD_TIMEOUT_TIME, NULL)); // 100ms
}

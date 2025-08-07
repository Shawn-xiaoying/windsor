
#include <string.h>
#include "ble.h"
#include "nrf_sdh.h"
#include "nrf_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_qwr.h"
#include "nrf_ble_gatt.h"
#include "ble_gap.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "mt_ble_adv.h"
#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "mt_param_def.h"
#include "mt_ble_cfg.h"
#include "mt_ble_service.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "controller_config.h"

#define NRF_LOG_MODULE_NAME ble_cfg
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define MT_BLE_CFG_DEBUG_OUT

#define SEC_PARAM_BOND 1
#define SEC_PARAM_MITM 0
#define SEC_PARAM_LESC 0
#define SEC_PARAM_KEYPRESS 0
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_NONE
#define SEC_PARAM_OOB 0
#define SEC_PARAM_MIN_KEY_SIZE 7
#define SEC_PARAM_MAX_KEY_SIZE 16

// static uint16_t                   m_conn_handle  = BLE_CONN_HANDLE_INVALID; /**< 连接句柄. */
extern uint16_t m_current_conn_handle;
static nrf_ble_gatt_evt_handler_t m_gatt_evt_hdl = NULL; /**< gatt 事件处理回调 */

NRF_BLE_GATT_DEF(m_gatt);

#ifdef MULTIPLE_DEVICES
NRF_BLE_QWRS_DEF(m_qwr, NRF_SDH_BLE_TOTAL_LINK_COUNT); /**< Context for the Queued Write module.*/

/**
 * @brief 处理 Queue_Write 模块的错误
 *
 * @param[in] nrf_error 模块出现的具体的错误码
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

#define LINK_TOTAL NRF_SDH_BLE_PERIPHERAL_LINK_COUNT + \
                       NRF_SDH_BLE_CENTRAL_LINK_COUNT

void qwr_int(void)
{
    ret_code_t err_code;

    nrf_ble_qwr_init_t qwr_init = {0};
    // Initialize Queued Write Module instances.
    qwr_init.error_handler = nrf_qwr_error_handler;

    for (uint32_t i = 0; i < LINK_TOTAL; i++)
    {
        err_code = nrf_ble_qwr_init(&m_qwr[i], &qwr_init);
        APP_ERROR_CHECK(err_code);
    }
}

#endif

APP_TIMER_DEF(m_delay_start_broadcast_timer); /**< 延时启动广播定时器. */

/************************************************************************************
 * @brief 命令超时定时器超时处理函数.
 */
static void tm_delay_start_broadcast_timer_handler(void *p_context)
{
    ble_adv_start();
}

static void on_connected(const ble_gap_evt_t *const p_gap_evt)
{

#ifdef MULTIPLE_DEVICES
    ret_code_t err_code;

    uint32_t periph_link_cnt = ble_conn_state_peripheral_conn_count(); // Number of peripheral links.

    NRF_LOG_INFO("Connection with link 0x%x established. link_cnt=%d", p_gap_evt->conn_handle, periph_link_cnt);

    // Assign connection handle to available instance of QWR module.
    for (uint32_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
    {
        //			   if(m_conn_handle != BLE_CONN_HANDLE_INVALID )
        //				 break ;

        if (m_qwr[i].conn_handle == BLE_CONN_HANDLE_INVALID)
        {
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr[i], p_gap_evt->conn_handle);
            APP_ERROR_CHECK(err_code);
            //					  if(i==0)
            //						m_conn_handle = p_gap_evt->conn_handle;
            break;
        }
    }

    if (periph_link_cnt == NRF_SDH_BLE_PERIPHERAL_LINK_COUNT)
    {
        //   bsp_board_led_off(ADVERTISING_LED);
    }
    else
    {

        ble_adv_start();
    }
#else
    m_conn_handle = p_gap_evt->conn_handle;

#endif
}

static void on_disconnected(ble_gap_evt_t const *const p_gap_evt)
{
#ifdef MULTIPLE_DEVICES
    ret_code_t __attribute__((unused)) err_code;
    uint32_t periph_link_cnt = ble_conn_state_peripheral_conn_count(); // Number of peripheral links.

    NRF_LOG_INFO("Connection 0x%x has been disconnected. Reason: 0x%X",
                 p_gap_evt->conn_handle,
                 p_gap_evt->params.disconnected.reason);

    if (periph_link_cnt == (NRF_SDH_BLE_PERIPHERAL_LINK_COUNT - 1))
    {
        // Advertising is not running when all connections are taken, and must therefore be started.
        // advertising_start();
        ble_adv_start();
    }
#else

    m_conn_handle = BLE_CONN_HANDLE_INVALID;

#endif
}

/**
 * @brief 处理来自协议栈的BLE事件回调函数.
 *
 * @param[in] p_ble_evt BLE 协议栈事件
 * @param[in] p_context 未使用
 */

extern void clear_mutex(void);
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;
    ret_code_t err_code;
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_DISCONNECTED:
        //            m_conn_handle = BLE_CONN_HANDLE_INVALID;
        on_disconnected(&p_ble_evt->evt.gap_evt);
//        if (p_ble_evt->evt.gap_evt.conn_handle == m_current_conn_handle)
//            update_adv_data_device_state_of_connect(NOT_CONNECTED);
        NRF_LOG_INFO("Disconnected.");

        break;

    case BLE_GAP_EVT_CONNECTED:
        // m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        NRF_LOG_INFO("Connected.");
//        update_adv_data_device_state_of_connect(CONNECTED);
        on_connected(&p_ble_evt->evt.gap_evt);
        break;

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle,
                                               BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                               NULL,
                                               NULL);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
        break;

    case BLE_GAP_EVT_AUTH_KEY_REQUEST:
        NRF_LOG_INFO("BLE_GAP_EVT_AUTH_KEY_REQUEST");
        break;

    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
        NRF_LOG_INFO("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
        break;

    case BLE_GAP_EVT_AUTH_STATUS:
        NRF_LOG_INFO("BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
                     p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                     p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                     p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                     *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                     *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
        break;

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        //  APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0));
        APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gap_evt.conn_handle, NULL, 0, 0));
        break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_DEBUG("PHY update request.");
        ble_gap_phys_t const phys = {
            .rx_phys = BLE_GAP_PHY_AUTO,
            .tx_phys = BLE_GAP_PHY_AUTO,
        };
        APP_ERROR_CHECK(sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys));
    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE:
    {
        ble_gap_evt_phy_update_t const *p_phy_evt = &p_ble_evt->evt.gap_evt.params.phy_update;
        NRF_LOG_INFO("PHY update tx %d rx %d", p_phy_evt->tx_phy, p_phy_evt->rx_phy);
    }
    break;

    case BLE_GAP_EVT_TIMEOUT:
        if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
        {
            NRF_LOG_INFO("Connection timed out.");
        }
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        APP_ERROR_CHECK(sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        APP_ERROR_CHECK(sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
        break;

    default:
        break;
    }
}

/**
 * @brief 初始化协议栈.
 */
static void ble_stack_init(void)
{
    uint32_t ram_start = 0;

    APP_ERROR_CHECK(nrf_sdh_enable_request());
    APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(1, &ram_start));
    APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
    NRF_SDH_BLE_OBSERVER(m_ble_observer, 2, ble_evt_handler, NULL);
}

/**
 * @brief 用于配置连接参数更新的具体参数, 设备名称
 */
static void gap_params_init(void)
{
    ble_gap_conn_params_t conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    APP_ERROR_CHECK(
        sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEF_ADV_NAME, strlen(DEF_ADV_NAME)));

    APP_ERROR_CHECK(sd_ble_gap_appearance_set(BLE_APPEARANCE_HID_MOUSE));
    memset(&conn_params, 0, sizeof(conn_params));

    conn_params.min_conn_interval = MSEC_TO_UNITS(DEF_CONN_INT_MIN, UNIT_1_25_MS);
    conn_params.max_conn_interval = MSEC_TO_UNITS(DEF_CONN_INT_MAN, UNIT_1_25_MS);
    conn_params.slave_latency = SLAVE_LATENCY;
    conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    APP_ERROR_CHECK(sd_ble_gap_ppcp_set(&conn_params));
}

/**
 * @brief 连接参数模块事件处理.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {

        //			 if(m_conn_handle == p_evt->conn_handle){
        //        NRF_LOG_WARNING("Conn params evt failed dinconnect!!!");
        //
        //				#ifndef MULTIPLE_DEVICES
        //	      APP_ERROR_CHECK(sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE));
        //				#endif
        //			 }
        //
    }
}

/**@brief 连接参数错误处理函数
 *
 * @param[in] nrf_error 连接参数错误值.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    if (nrf_error != NRF_ERROR_INVALID_STATE) /* ignore */
        APP_ERROR_HANDLER(nrf_error);
    else
    {
        NRF_LOG_WARNING("Conn param invaild state disconnect!!!");

#ifndef MULTIPLE_DEVICES
        APP_ERROR_CHECK(sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE));
#endif
    }
}

/**
 * @brief 连接参数模块初始化
 */
static void conn_params_init(void)
{
    ble_conn_params_init_t cp_init;
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    APP_ERROR_CHECK(ble_conn_params_init(&cp_init));
}

/**
 * @brief gatt 事件处理.
 */
static void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt)
{
    if (m_gatt_evt_hdl)
    {
        m_gatt_evt_hdl(p_gatt, p_evt);
    }
}

/**
 * @brief gatt 初始化, 设置 MTU 长度.
 */
static void gatt_init(void)
{
    APP_ERROR_CHECK(nrf_ble_gatt_init(&m_gatt, gatt_evt_handler));
    APP_ERROR_CHECK(nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE));
}

void mt_ble_conn_param_update(void)
{
    ble_gap_conn_params_t conn_params;

    memset(&conn_params, 0, sizeof(conn_params));
    conn_params.min_conn_interval = MSEC_TO_UNITS(DEF_CONN_INT_MIN, UNIT_1_25_MS);
    conn_params.max_conn_interval = MSEC_TO_UNITS(DEF_CONN_INT_MAN, UNIT_1_25_MS);
    conn_params.slave_latency = SLAVE_LATENCY;
    conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    APP_ERROR_CHECK(sd_ble_gap_ppcp_set(&conn_params));
}

void mt_ble_gatt_register(nrf_ble_gatt_evt_handler_t cb)
{
    m_gatt_evt_hdl = cb;
}

bool mt_is_ble_connected(uint16_t conn_handle)
{
    //  return (m_conn_handle == BLE_CONN_HANDLE_INVALID) ? false : true;
    return (conn_handle == BLE_CONN_HANDLE_INVALID) ? false : true;
}

void mt_ble_force_disconnect(uint16_t conn_handle)
{
    ret_code_t err_code;
    if (mt_is_ble_connected(conn_handle) == true)
    {

        //			  err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        //        if (err_code != NRF_SUCCESS)
        //        {
        //            NRF_LOG_WARNING("sd_ble_gap_disconnect() returned %s on conn_handle %d.",
        //                            nrf_strerror_get(err_code),
        //                            m_conn_handle);
        //        }

        err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        if (err_code != NRF_SUCCESS)
        {
            NRF_LOG_WARNING("sd_ble_gap_disconnect() returned %s on conn_handle %d.",
                            nrf_strerror_get(err_code),
                            conn_handle);
        }
    }
}
#ifdef USER_PEER_MANGAGE

/**
 * @brief peer manager 事件处理函数.
 *
 * @param[in] p_evt 事件
 */
static void pm_evt_handler(pm_evt_t const *p_evt)
{

    if (p_evt->evt_id == PM_EVT_CONN_SEC_CONFIG_REQ)
    {
        pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
        pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
    }
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);
}



/**
 * @brief peer_manager 初始化
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t __attribute__((unused)) err_code;

    APP_ERROR_CHECK(pm_init());

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    sec_param.bond = SEC_PARAM_BOND;
    sec_param.mitm = SEC_PARAM_MITM;
    sec_param.lesc = SEC_PARAM_LESC;
    sec_param.keypress = SEC_PARAM_KEYPRESS;
    sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob = SEC_PARAM_OOB;
    sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;

    APP_ERROR_CHECK(pm_sec_params_set(&sec_param));
    APP_ERROR_CHECK(pm_register(pm_evt_handler));
}

#endif

void mt_ble_config_init(void)
{
    ble_stack_init();
#ifdef USER_PEER_MANGAGE
    peer_manager_init();
#endif
    gap_params_init();
    conn_params_init();
    gatt_init();
    ble_service_init();

    APP_ERROR_CHECK(app_timer_create(&m_delay_start_broadcast_timer, APP_TIMER_MODE_SINGLE_SHOT, tm_delay_start_broadcast_timer_handler));
}

#ifdef USER_PEER_MANGAGE
void delete_bonds(void)
{
    ret_code_t err_code;
    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}
#endif

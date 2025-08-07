/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "mt_ble_adv.h"
#include "wap-1-100-windsor-master-protocol.h"
#include "mt_param_def.h"
#include "app_service_cfg.h"
#include "comm_lib.h"
#include "mt_param_def.h"
#include "mt_cst_protocol.h"

#include "nrf52840.h"
#include "mt_ble_service.h"
#include "app_cfg_cmd.h"

#include "mt_ble_cfg.h"
#include "mt_gpio.h"

#include "session.h"
#include "set_get_session.h"
#include "session_log.h"

#define NRF_LOG_MODULE_NAME app_server_cfg
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define APP_SERVICE_CFG_DEBUG_OUT

/*********************************************************/
uint8_t m_send_buf[WINDSOR_MAX_BUF_LENGTH] = {0};
uint8_t m_rev_buf[WINDSOR_MAX_BUF_LENGTH] = {0};

static bool check_ShortUserID_bond_flg = false; // 绑定状态检查标志

extern void set_user_connect_state(bool value);
extern uint16_t m_current_conn_handle;

/*********************************************************/

/******************************************************
 * 对ShortUserID 清零
 * */

void reset_ShortUserID(void)
{
    memset(device_infor.ShortUserID, 0, sizeof(device_infor.ShortUserID));
#ifdef APP_SERVICE_CFG_DEBUG_OUT
    NRF_LOG_INFO("[ShortUserID] clean_ShortUserID =0x%02x %02x %02x %02x %02x",
                 device_infor.ShortUserID[0], device_infor.ShortUserID[1],
                 device_infor.ShortUserID[2], device_infor.ShortUserID[3],
                 device_infor.ShortUserID[4]);
#endif

    check_ShortUserID_bond_flg = false;
    set_user_connect_state(false);
    mt_ble_force_disconnect(m_current_conn_handle);
}

/******************************************************
 * （1）设备初始化时，允许任意用户连接，等待app和设备配对完成后，之后的蓝牙设备只允许配过特定一个用户用户连接，
 *  （2）配对过设备，用户连接后需要在30秒内发送配对信息，超时则认为连接失败，蓝牙设备端会断开app连接。
 * */
static void init_ShortUserID_bond_valu(uint8_t *p_data, uint8_t len)
{
    uint8_t data_00[5];

    memset(data_00, 0x00, 5);

    // 检测是否配对过，是否赋值过
    if (memcmp(device_infor.ShortUserID, data_00, 5) == 0)
    {
        memcpy(device_infor.ShortUserID, p_data + 3, 5);

        check_ShortUserID_bond_flg = false;
        set_user_connect_state(true);

        CONNCET_BOND_LED_ON(); //
#ifdef APP_SERVICE_CFG_DEBUG_OUT
        NRF_LOG_INFO("[ShortUserID]  device first input bond  success !!!!!! \r\n");
#endif
    }
    else /*配对过的*/
    {
        if (memcmp(device_infor.ShortUserID, p_data + 3, 5) == 0) /*配对成功检测*/
        {
            check_ShortUserID_bond_flg = false;
            set_user_connect_state(true);

            CONNCET_BOND_LED_ON(); //
#ifdef APP_SERVICE_CFG_DEBUG_OUT
            NRF_LOG_INFO("[ShortUserID]  check input bond infor success !!!!!! \r\n");
#endif
        }
    }
}

/****************************************************
 * 连接成功后，检查ShortUserID是否被绑定过，
 * 绑定过，等待30s，验证不通过即将断开设备
 * ************************************/
void check_ShortUserID_bond_state(void)
{
    uint8_t data_00[5];
    memset(data_00, 0x00, 5);

    // 绑定过
    if (memcmp(device_infor.ShortUserID, data_00, 5) != 0)
    {
        check_ShortUserID_bond_flg = true;
    }
#ifdef APP_SERVICE_CFG_DEBUG_OUT
    NRF_LOG_INFO("[ShortUserID]  check_ShortUserID_bond_state =%d\r\n", check_ShortUserID_bond_flg);
    NRF_LOG_INFO("[ShortUserID]   already present_ShortUserID =0x%02x %02x %02x %02x %02x",
                 device_infor.ShortUserID[0], device_infor.ShortUserID[1],
                 device_infor.ShortUserID[2], device_infor.ShortUserID[3],
                 device_infor.ShortUserID[4]);
#endif
}

/******************************************************
 * 处于检查配对状态时，需要检查等待超时动作
 *
 ********************************************/
#define BOND_TIME_OUT (30000 / 100) // D
void check_ShortUserID_bond_timeout(void)
{
    static uint16_t time_cnt = 0;
    if ((check_ShortUserID_bond_flg == false) ||
        (mt_is_ble_connected(m_current_conn_handle) == false))
    {
        time_cnt = 0;
        return;
    }

    if (time_cnt++ >= BOND_TIME_OUT)
    {
        // mt_ble_force_disconnect();
        reset_ShortUserID();
#ifdef APP_SERVICE_CFG_DEBUG_OUT
        NRF_LOG_INFO("check_ShortUserID_bond_timeout....time_cnt=%d\r\n", time_cnt);
#endif
    }
}

// #define  TEST_SAUNA

/***********************************************************
 *  app 协议数据解析函数
 * */
void user_service_cmd(uint16_t conn_handle, uint8_t *p_data, uint8_t rev_len)
{

    uint8_t cmd_head = 0, cmd = 0;
    int16_t temp_i16 = 0xff;

    cmd_head = p_data[0];
    cmd = p_data[1];

    if (m_current_conn_handle != conn_handle && m_current_conn_handle != BLE_CONN_HANDLE_INVALID)
        return;

    if (m_current_conn_handle == BLE_CONN_HANDLE_INVALID)
        m_current_conn_handle = conn_handle;

    if (cmd_head != WINDSOR_PROTOCOL_APP_TO_DEVICE_HEAD)
    {
        return;
    }

    //  NRF_LOG_INFO("[APP DATA] user_service_cmd :");
    NRF_LOG_HEXDUMP_INFO(p_data, rev_len);

    /*ShortUserID 处理*/
    init_ShortUserID_bond_valu(p_data, rev_len);

    switch (cmd)
    {
    case INFOR_REQUESET:                               /*Infor 功能请求*/
        if (app_Info_Request(p_data, rev_len) == true) /*后面2 个字节是crc检验*/
        {
            device_Info_Response(conn_handle);
        }
        break;

    case GET_REQUESET:                                           /*GET 功能请求*/
        temp_i16 = app_Get_Request_cmd(p_data, rev_len);         /*后面2 个字节是crc检验*/
        supplier_get_request_cmd_process(conn_handle, temp_i16); // 不同的设备连接实现该函数，根据该函数实现不同的功能
        break;

    case SET_REQUESET: /*app Set设置功能请求*/

        supplier_set_request_cmd_process(conn_handle, p_data, rev_len);
        break;

    case GET_FEATURES:
        app_get_features_supported_by_the_device_request(conn_handle, p_data, rev_len);
        break;

    case GET_LOG_FEATURES:
        app_get_log_features_supported_by_the_device_request(conn_handle, p_data, rev_len);

        break;

    case SET_SESSION:
        set_session_requeset(conn_handle, p_data, rev_len);
        break;

    case GET_SESSION:
        get_session_request(conn_handle, p_data, rev_len);
        break;

    case CONTROL_SESSION:
        control_session_request(conn_handle, p_data, rev_len);
        break;

    case SESSION_STATUS:
        session_status_request(conn_handle, p_data, rev_len);
        break;

    case GET_LOGS:
        get_session_log_request(conn_handle, p_data, rev_len);
        break;

    case CLEAR_SESSION:
        clear_session_request(conn_handle, p_data, rev_len);
        break;

    default:
        break;
    }
}

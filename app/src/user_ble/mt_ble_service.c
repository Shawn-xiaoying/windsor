
#include "mt_ble_service.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_error.h"
#include "ble_ctcs.h"
#include "mt_cst_protocol.h"
#include "ble_dfu.h"
#include "ble_hids.h"
#include "controller_config.h"

#define NRF_LOG_MODULE_NAME ble_srv
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define MT_BLE_SERVICE_DEBUG_OUT

#define INPUT_REPORT_COUNT 3
#define INPUT_REP_BUTTONS_LEN 3
#define INPUT_REP_MOVEMENT_LEN 3
#define INPUT_REP_MEDIA_PLAYER_LEN 1
#define INPUT_REP_BUTTONS_INDEX 0
#define INPUT_REP_MOVEMENT_INDEX 1
#define INPUT_REP_MPLAYER_INDEX 2
#define INPUT_REP_REF_BUTTONS_ID 1
#define INPUT_REP_REF_MOVEMENT_ID 2
#define INPUT_REP_REF_MPLAYER_ID 3
#define BASE_USB_HID_SPEC_VERSION 0x0101

#ifndef MULTIPLE_DEVICES
NRF_BLE_QWR_DEF(m_qwr);
#else
extern void qwr_int(void);
#endif

BLE_CTCS_DEF(m_ctcs);

BLE_HIDS_DEF(m_hids, NRF_SDH_BLE_TOTAL_LINK_COUNT, 3);

static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
    case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
    {
        NRF_LOG_INFO("Device is preparing to enter bootloader mode.");
        break;
    }

    case BLE_DFU_EVT_BOOTLOADER_ENTER:
        NRF_LOG_INFO("Device will enter bootloader mode.");
        break;

    case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
        NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
        break;

    case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
        NRF_LOG_ERROR("Request to send a response to client failed.");
        APP_ERROR_CHECK(false);
        break;

    default:
        NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
        break;
    }
}

uint32_t ble_ctcs_s_send_handler(uint16_t conn_handle, uint8_t *p_data, uint16_t* len, uint16_t uuid)
{
    return ble_ctcs_notify_send(&m_ctcs, conn_handle, p_data, len, uuid);
}

/**
 * @brief ctcs 服務初始化.
 */

#ifndef MULTIPLE_DEVICES
/**
 * @brief 处理 Queue_Write 模块的错误
 *
 * @param[in] nrf_error 模块出现的具体的错误码
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}
#endif

static void user_ctcs_init(void)
{

    ble_ctcs_init_t ctcs_init;
#ifndef MULTIPLE_DEVICES
    nrf_ble_qwr_init_t qwr_init = {0};
#endif
    ble_dfu_buttonless_init_t dfus_init = {0};

#ifdef MULTIPLE_DEVICES
    qwr_int();
#else
    qwr_init.error_handler = nrf_qwr_error_handler;
    APP_ERROR_CHECK(nrf_ble_qwr_init(&m_qwr, &qwr_init));
#endif

    memset(&ctcs_init, 0, sizeof(ctcs_init));
    ctcs_init.data_handler = ctcs_data_handler;
    APP_ERROR_CHECK(ble_ctcs_init(&m_ctcs, &ctcs_init));

    dfus_init.evt_handler = ble_dfu_evt_handler;
    APP_ERROR_CHECK(ble_dfu_buttonless_init(&dfus_init));
}

static void on_hids_evt(ble_hids_t *p_hids, ble_hids_evt_t *p_evt)
{
    /* todo */
}

static void hid_service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void hids_init(void) __attribute__((unused));
static void hids_init(void)
{
    ret_code_t err_code;
    ble_hids_init_t hids_init_obj;
    ble_hids_inp_rep_init_t *p_input_report;
    uint8_t hid_info_flags;

    static ble_hids_inp_rep_init_t inp_rep_array[INPUT_REPORT_COUNT];
    /* clang-format off */    
    static uint8_t rep_map_data[] =
    {
        0x05, 0x01, // Usage Page (Generic Desktop)
        0x09, 0x02, // Usage (Mouse)

        0xA1, 0x01, // Collection (Application)

        // Report ID 1: Mouse buttons + scroll/pan
        0x85, 0x01,       // Report Id 1
        0x09, 0x01,       // Usage (Pointer)
        0xA1, 0x00,       // Collection (Physical)
        0x95, 0x05,       // Report Count (3)
        0x75, 0x01,       // Report Size (1)
        0x05, 0x09,       // Usage Page (Buttons)
        0x19, 0x01,       // Usage Minimum (01)
        0x29, 0x05,       // Usage Maximum (05)
        0x15, 0x00,       // Logical Minimum (0)
        0x25, 0x01,       // Logical Maximum (1)
        0x81, 0x02,       // Input (Data, Variable, Absolute)
        0x95, 0x01,       // Report Count (1)
        0x75, 0x03,       // Report Size (3)
        0x81, 0x01,       // Input (Constant) for padding
        0x75, 0x08,       // Report Size (8)
        0x95, 0x01,       // Report Count (1)
        0x05, 0x01,       // Usage Page (Generic Desktop)
        0x09, 0x38,       // Usage (Wheel)
        0x15, 0x81,       // Logical Minimum (-127)
        0x25, 0x7F,       // Logical Maximum (127)
        0x81, 0x06,       // Input (Data, Variable, Relative)
        0x05, 0x0C,       // Usage Page (Consumer)
        0x0A, 0x38, 0x02, // Usage (AC Pan)
        0x95, 0x01,       // Report Count (1)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0xC0,             // End Collection (Physical)

        // Report ID 2: Mouse motion
        0x85, 0x02,       // Report Id 2
        0x09, 0x01,       // Usage (Pointer)
        0xA1, 0x00,       // Collection (Physical)
        0x75, 0x0C,       // Report Size (12)
        0x95, 0x02,       // Report Count (2)
        0x05, 0x01,       // Usage Page (Generic Desktop)
        0x09, 0x30,       // Usage (X)
        0x09, 0x31,       // Usage (Y)
        0x16, 0x01, 0xF8, // Logical maximum (2047)
        0x26, 0xFF, 0x07, // Logical minimum (-2047)
        0x81, 0x06,       // Input (Data, Variable, Relative)
        0xC0,             // End Collection (Physical)
        0xC0,             // End Collection (Application)

        // Report ID 3: Advanced buttons
        0x05, 0x0C,       // Usage Page (Consumer)
        0x09, 0x01,       // Usage (Consumer Control)
        0xA1, 0x01,       // Collection (Application)
        0x85, 0x03,       // Report Id (3)
        0x15, 0x00,       // Logical minimum (0)
        0x25, 0x01,       // Logical maximum (1)
        0x75, 0x01,       // Report Size (1)
        0x95, 0x01,       // Report Count (1)

        0x09, 0xCD,       // Usage (Play/Pause)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x0A, 0x83, 0x01, // Usage (AL Consumer Control Configuration)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x09, 0xB5,       // Usage (Scan Next Track)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x09, 0xB6,       // Usage (Scan Previous Track)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)

        0x09, 0xEA,       // Usage (Volume Down)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x09, 0xE9,       // Usage (Volume Up)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x0A, 0x25, 0x02, // Usage (AC Forward)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0x0A, 0x24, 0x02, // Usage (AC Back)
        0x81, 0x06,       // Input (Data,Value,Relative,Bit Field)
        0xC0              // End Collection
    };
    /* clang-format on */

    memset(inp_rep_array, 0, sizeof(inp_rep_array));
    // Initialize HID Service.
    p_input_report = &inp_rep_array[INPUT_REP_BUTTONS_INDEX];
    p_input_report->max_len = INPUT_REP_BUTTONS_LEN;
    p_input_report->rep_ref.report_id = INPUT_REP_REF_BUTTONS_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr = SEC_JUST_WORKS;
    p_input_report->sec.rd = SEC_JUST_WORKS;

    p_input_report = &inp_rep_array[INPUT_REP_MOVEMENT_INDEX];
    p_input_report->max_len = INPUT_REP_MOVEMENT_LEN;
    p_input_report->rep_ref.report_id = INPUT_REP_REF_MOVEMENT_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr = SEC_JUST_WORKS;
    p_input_report->sec.rd = SEC_JUST_WORKS;

    p_input_report = &inp_rep_array[INPUT_REP_MPLAYER_INDEX];
    p_input_report->max_len = INPUT_REP_MEDIA_PLAYER_LEN;
    p_input_report->rep_ref.report_id = INPUT_REP_REF_MPLAYER_ID;
    p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

    p_input_report->sec.cccd_wr = SEC_JUST_WORKS;
    p_input_report->sec.wr = SEC_JUST_WORKS;
    p_input_report->sec.rd = SEC_JUST_WORKS;

    hid_info_flags = HID_INFO_FLAG_REMOTE_WAKE_MSK | HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK;

    memset(&hids_init_obj, 0, sizeof(hids_init_obj));

    hids_init_obj.evt_handler = on_hids_evt;
    hids_init_obj.error_handler = hid_service_error_handler;

    hids_init_obj.is_kb = false;
    hids_init_obj.is_mouse = true;

    hids_init_obj.inp_rep_count = INPUT_REPORT_COUNT;
    hids_init_obj.p_inp_rep_array = inp_rep_array;
    hids_init_obj.outp_rep_count = 0;
    hids_init_obj.p_outp_rep_array = NULL;

    hids_init_obj.feature_rep_count = 0;
    hids_init_obj.p_feature_rep_array = NULL;

    hids_init_obj.rep_map.data_len = sizeof(rep_map_data);
    hids_init_obj.rep_map.p_data = rep_map_data;

    hids_init_obj.hid_information.bcd_hid = BASE_USB_HID_SPEC_VERSION;
    hids_init_obj.hid_information.b_country_code = 0;
    hids_init_obj.hid_information.flags = hid_info_flags;
    hids_init_obj.included_services_count = 0;
    hids_init_obj.p_included_services_array = NULL;

    hids_init_obj.rep_map.rd_sec = SEC_JUST_WORKS;
    hids_init_obj.hid_information.rd_sec = SEC_JUST_WORKS;

    hids_init_obj.boot_mouse_inp_rep_sec.cccd_wr = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.wr = SEC_JUST_WORKS;
    hids_init_obj.boot_mouse_inp_rep_sec.rd = SEC_JUST_WORKS;

    hids_init_obj.protocol_mode_rd_sec = SEC_JUST_WORKS;
    hids_init_obj.protocol_mode_wr_sec = SEC_JUST_WORKS;
    hids_init_obj.ctrl_point_wr_sec = SEC_JUST_WORKS;

    err_code = ble_hids_init(&m_hids, &hids_init_obj);
    APP_ERROR_CHECK(err_code);
}

void ble_service_init(void)
{
    user_ctcs_init();
    //  hids_init();
}


#include "sdk_common.h"
#include "ble.h"
#include "ble_ctcs.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "mt_s_public.h"
#include "bluetooth_connection_management.h"

#define NRF_LOG_MODULE_NAME ble_ctcs
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

CTS_SEND_STATE cts_send_state = TX_NONE;

/* clang-format off */
#define CTCS_BASE_UUID {{0xfb, 0x34, 0x9b, 0x5f, \
                        0x80, 0x00, 0x00, 0x80, \
                        0x00, 0x10, 0x00, 0x00, \
                        0x00, 0x00, 0x00, 0x00}}
/* clang-format on */

/**
 * @brief ctcs 连接事件处理.
 *
 * @param[in] p_ctcs
 * @param[in] p_ble_evt 事件
 */
static void on_connect(ble_ctcs_t *p_ctcs, ble_evt_t const *p_ble_evt)
{
   // p_ctcs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**
 * @brief ctcs 断开连接事件处理.
 *
 * @param[in] p_ctcs
 * @param[in] p_ble_evt 事件
 */
static void on_disconnect(ble_ctcs_t *p_ctcs, ble_evt_t const *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
   // p_ctcs->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**
 * @brief ctcs 写入事件处理.
 *
 * @param[in] p_ctcs
 * @param[in] p_ble_evt 事件
 */
static void on_write(ble_ctcs_t *p_ctcs, ble_evt_t const *p_ble_evt)
{
    ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    ble_ctcs_evt_t evt;

    memset(&evt, 0, sizeof(ble_ctcs_evt_t));
    p_ctcs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    evt.p_ctcs = p_ctcs;

    if (p_ctcs->data_handler != NULL)
    {
        evt.params.attachment_data.p_data = p_evt_write->data;
        evt.params.attachment_data.length = p_evt_write->len;
        evt.uuid = p_evt_write->uuid.uuid;
        if (p_evt_write->handle == p_ctcs->passcode_handles.value_handle)
        {
            evt.type = BLE_CTCS_EVT_PASSCODE_RX_DATA;
            p_ctcs->data_handler(&evt);
        }
        else if (p_evt_write->handle == p_ctcs->transport_handles.value_handle)
        {
            evt.type = BLE_CTCS_EVT_TRANSPORT_RX_DATA;
            p_ctcs->data_handler(&evt);
        }
    }
}

void ble_ctcs_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    ble_ctcs_t *p_ctcs = (ble_ctcs_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        NRF_LOG_INFO("cts connect");
        on_connect(p_ctcs, p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("cts dis_connect");
        on_disconnect(p_ctcs, p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_ctcs, p_ble_evt);
        break;

    case BLE_GATTS_EVT_HVN_TX_COMPLETE:
    {
        // notify with empty data that some tx was completed.
        ble_ctcs_evt_t evt = {.type = BLE_CTCS_EVT_TX_COMPLETE, .p_ctcs = p_ctcs};
        cts_send_state = TX_COMPLETE;
				user_bluetooth_set_cts_send_state(p_ble_evt->evt.gap_evt.conn_handle,cts_send_state);
  //      NRF_LOG_INFO("TX_COMPLETE evt.gap_evt.conn_handle =%d",p_ble_evt->evt.gap_evt.conn_handle);
        p_ctcs->data_handler(&evt);
        break;
    }
    default:
        // No implementation needed.
        break;
    }
}

uint32_t ble_ctcs_init(ble_ctcs_t *p_ctcs, ble_ctcs_init_t const *p_ctcs_init)
{
    ble_uuid_t ble_uuid;
    ble_gatt_char_props_t char_props;
    ble_gatts_attr_md_t m_attr_md;
    ble_uuid128_t ctcs_base_uuid = CTCS_BASE_UUID;

    VERIFY_PARAM_NOT_NULL(p_ctcs);
    VERIFY_PARAM_NOT_NULL(p_ctcs_init);
    p_ctcs->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_ctcs->data_handler = p_ctcs_init->data_handler;
    p_ctcs->is_notification_enabled = false;

    VERIFY_SUCCESS(sd_ble_uuid_vs_add(&ctcs_base_uuid, &p_ctcs->uuid_type));
    ble_uuid.type = p_ctcs->uuid_type;
    ble_uuid.uuid = CTCS_SERVER_UUID;
    VERIFY_SUCCESS(
        sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_ctcs->service_handle));

    /* 添加 Attachment 特征值 */
    fill_in_character_paraments(&char_props, &m_attr_md);
    char_props.read = 0;
    VERIFY_SUCCESS(common_char_add(p_ctcs->uuid_type,
                                   p_ctcs->service_handle,
                                   char_props,
                                   &p_ctcs->passcode_handles,
                                   m_attr_md,
                                   CTCS_PASSCODE_UUID));
    /* 添加 Zone Boost 特征值 */
    fill_in_character_paraments(&char_props, &m_attr_md);
    char_props.read = 0;
    VERIFY_SUCCESS(common_char_add(p_ctcs->uuid_type,
                                   p_ctcs->service_handle,
                                   char_props,
                                   &p_ctcs->transport_handles,
                                   m_attr_md,
                                   CTCS_TRANSPORT_UUID));

    return NRF_SUCCESS;
}

uint32_t
ble_ctcs_notify_send(ble_ctcs_t *p_ctcs, uint16_t conn_handle, uint8_t *p_string, uint16_t *p_length, uint16_t uuid)
{
    ble_gatts_hvx_params_t hvx_params;
    ret_code_t err_code;
    VERIFY_PARAM_NOT_NULL(p_ctcs);

    if (conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        cts_send_state = TX_ERR;
		//	  NRF_LOG_INFO("ble_ctcs_notify_send  conn_handle = BLE_CONN_HANDLE_INVALID pcts conn handle=%d",conn_handle);
			  user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        return NRF_ERROR_INVALID_STATE;
    }

     

    if (*p_length > BLE_CTCS_MAX_DATA_LEN)
    {
        cts_send_state = TX_ERR;
		//	  NRF_LOG_INFO("ble_ctcs_notify_send  p_length > BLE_CTCS_MAX_DATA_LEN pcts conn handle=%d",conn_handle);
			  user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        return NRF_ERROR_INVALID_PARAM;
    }

    memset(&hvx_params, 0, sizeof(hvx_params));
    switch (uuid)
    {
    case CTCS_PASSCODE_UUID:
        hvx_params.handle = p_ctcs->passcode_handles.value_handle;
        break;

    case CTCS_TRANSPORT_UUID:
        hvx_params.handle = p_ctcs->transport_handles.value_handle;
        break;

    default:
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        return NRF_ERROR_INVALID_PARAM;
    }

    hvx_params.p_data = p_string;
    hvx_params.p_len = p_length;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    cts_send_state = TX_ING;
//		NRF_LOG_INFO("sd ble gatts conn_handle=%0x",conn_handle);
    err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    
    switch (err_code)
    {
    case NRF_SUCCESS:
        // Successfully queued a notification or indication for transmission, and optionally updated the attribute value.
		  //  user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
//        NRF_LOG_INFO("p_ctcs->conn_handle =%d cts successfully",conn_handle);
        break;

    case BLE_ERROR_INVALID_CONN_HANDLE:
        //			NRF_LOG_INFO("Invalid Connection Handle");
        // Invalid Connection Handle.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;

    case NRF_ERROR_INVALID_STATE:
        //			NRF_LOG_INFO("Notifications and/or indications not enabled in the CCCD");
        // One or more of the following is true:
        //- Invalid Connection State
        //- Notifications and/or indications not enabled in the CCCD
        //- An ATT_MTU exchange is ongoing
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_INVALID_ADDR:
        //			NRF_LOG_INFO("Invalid pointer supplied");
        // Invalid pointer supplied.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_INVALID_PARAM:
        // Invalid parameter(s) supplied.
        //		NRF_LOG_INFO("Invalid parameter(s) supplied");
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case BLE_ERROR_INVALID_ATTR_HANDLE:

        //		 NRF_LOG_INFO("BLE_ERROR_INVALID_ATTR_HANDLE");
        // Invalid attribute handle(s) supplied. Only attributes added directly by the application are available to notify and indicate.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case BLE_ERROR_GATTS_INVALID_ATTR_TYPE:

        //			NRF_LOG_INFO("Invalid attribute type(s) supplied");
        //  Invalid attribute type(s) supplied, only characteristic values may be notified and indicated.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_NOT_FOUND:
        //			NRF_LOG_INFO("Attribute not found");
        //				Attribute not found.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_FORBIDDEN:

        //			NRF_LOG_INFO("The connection's current security level is lower");
        // The connection's current security level is lower than the one required by the write permissions of the CCCD associated with this characteristic.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_DATA_SIZE:
        //			NRF_LOG_INFO("Invalid data size(s) supplied");
        // Invalid data size(s) supplied.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_BUSY:
        //			NRF_LOG_INFO("busy, Procedure already in progress. Wait for a ");
        // For @ref BLE_GATT_HVX_INDICATION Procedure already in progress. Wait for a @ref BLE_GATTS_EVT_HVC event and retry.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case BLE_ERROR_GATTS_SYS_ATTR_MISSING:
        //			NRF_LOG_INFO("System attributes missing ");
        // System attributes missing, use @ref sd_ble_gatts_sys_attr_set to set them to a known value.
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_RESOURCES:
        // Too many notifications queued.Wait for a @ref BLE_GATTS_EVT_HVN_TX_COMPLETE event and retry.
        //			NRF_LOG_INFO("NRF_ERROR_RESOURCES ,Too many notifications queued ");
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;
    case NRF_ERROR_TIMEOUT:
        // There has been a GATT procedure timeout. No new GATT procedure can be performed without reestablishing the connection.
        //			NRF_LOG_INFO("NRF_ERROR_TIMEOUT ,There has been a GATT procedure timeout ");
        cts_send_state = TX_ERR;
		    user_bluetooth_set_cts_send_state(conn_handle,cts_send_state);
        break;

    default:
        break;
    }

    return err_code;
}

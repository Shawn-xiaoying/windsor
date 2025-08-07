/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "mt_ble_adv.h"
#include "mt_param_def.h"
#include "mt_cst_protocol.h"
#include "fstorage.h"

#define NRF_LOG_MODULE_NAME ble_adv
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define ADV_INTERVAL_STEP 100 /**< 广播间隔等级步长, 单位 100ms. */

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET; /**< 广播句柄 */
static uint8_t m_adv_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];	  /**< 存放广播数据数组 */
static uint8_t m_rsp_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];	  /**< 存放扫描回复数据数组 */
static ble_gap_adv_params_t m_adv_params;					  /**< 广播参数结构体. */
extern DEVICE_INFOR_Pack device_infor;
uint32_t m_device_code =0 ;

/**< 广播数据信息结构体 */
static ble_gap_adv_data_t m_ble_adv_data = {.adv_data = {.p_data = m_adv_data},
											.scan_rsp_data = {.p_data = m_rsp_data}};

void update_adv_data_device_state_of_connect(unsigned char state)
{
#ifdef ADV_USER_DEVICE_STATE
	adv_data_t *p_adv_data = (adv_data_t *)m_adv_data;
	p_adv_data->service_data.device_state.on_off_state.on_off_st_value.connected_status = state;
#endif
}

void update_adv_data_device_state_of_power(unsigned char state)
{
#ifdef ADV_USER_DEVICE_STATE
	adv_data_t *p_adv_data = (adv_data_t *)m_adv_data;
	p_adv_data->service_data.device_state.on_off_state.on_off_st_value.power_status = state;
#endif
}

void update_adv_data_device_state_of_session(unsigned char state)
{
#ifdef ADV_USER_DEVICE_STATE
	adv_data_t *p_adv_data = (adv_data_t *)m_adv_data;
	p_adv_data->service_data.device_state.on_off_state.on_off_st_value.session_status = state;
#endif
}

void update_adv_data_device_state_of_temperature(unsigned char value)
{
#ifdef ADV_USER_DEVICE_STATE
	adv_data_t *p_adv_data = (adv_data_t *)m_adv_data;
	p_adv_data->service_data.device_state.temperature = value;
#endif
}

void update_adv_data_device_state_of_pressure(unsigned char value)
{
#ifdef ADV_USER_DEVICE_STATE
	adv_data_t *p_adv_data = (adv_data_t *)m_adv_data;
	p_adv_data->service_data.device_state.pressure = value;
#endif
}

void update_adv_data_device_code(uint16_t device_code)
{

	adv_data_t *p_adv_data = (adv_data_t *)m_adv_data;
	p_adv_data->service_data.device_code[0] = device_code >> 8;
	p_adv_data->service_data.device_code[1] = device_code;
}

void update_adv_date(unsigned char data)
{

	m_adv_data[30] = data;
}

/**
 * @brief 透传广播模式广播数据组包
 */
static void adv_data_set()
{

#ifdef ADV_USER_DEVICE_CODE

	m_device_code = fstorge_read_device_code();
	uint16_t device_code = m_device_code ;
	
	
	#ifdef HBOT_MOD_1_3_ATA
//只针对氧舱设备Hyperbaric chamber - Mod 1.3ATA 写死设备码，以解决该设备以前已经出厂固件无设备码升级要写设备码的问题
//这样，以后该固件也就只针对这款设备使用	
	
  m_device_code = 10005 ;	
	device_code = m_device_code ;
	#endif 


	adv_data_t *p_adv_data = (adv_data_t *)m_adv_data;

	// service uiid
	p_adv_data->service_uiid.length = 0x03;
	p_adv_data->service_uiid.type = 0x03;
	p_adv_data->service_uiid.value[0] = 0x42;
	p_adv_data->service_uiid.value[1] = 0x4f;

	// short name
	p_adv_data->short_name.length = 0x08;
	p_adv_data->short_name.type = 0x08;
	strncpy((char *)p_adv_data->short_name.value, DEF_ADV_NAME, strlen(DEF_ADV_NAME));

	// service data
	p_adv_data->service_data.length = 0x11;
	p_adv_data->service_data.type = 0x16;
	memcpy(p_adv_data->service_data.service_uiid, p_adv_data->service_uiid.value, 2);
	p_adv_data->service_data.device_code[0] = device_code >> 8;
	p_adv_data->service_data.device_code[1] = device_code;
	memset((char *)&p_adv_data->service_data.device_state, 0x00, 4);
	memcpy(p_adv_data->service_data.chip_id, device_infor.ChipID, 8);

	m_ble_adv_data.adv_data.len = sizeof(adv_data_t);
	m_ble_adv_data.scan_rsp_data.len = 0;

#else
	uint8_t idx = 0;
	unsigned int prodcut_parameters_c = PRODUCT_PARAMETERS;
	// SERVICE UIID
	m_adv_data[idx++] = 0x03; // length
	m_adv_data[idx++] = 0x03; // type

	m_adv_data[idx++] = 0x42; // two byte content
	m_adv_data[idx++] = 0x4f;

	// short local name
	m_adv_data[idx++] = 0x08; // length
	m_adv_data[idx++] = 0x08; // type
	m_adv_data[idx++] = 0x4F; // 7 byte content
	m_adv_data[idx++] = 0x6E;
	m_adv_data[idx++] = 0x65;
	m_adv_data[idx++] = 0x42;
	m_adv_data[idx++] = 0x61;
	m_adv_data[idx++] = 0x73;
	m_adv_data[idx++] = 0x65;

	// service data
	m_adv_data[idx++] = 0x11; // length
	m_adv_data[idx++] = 0x16; // type

	// content
	// UUID
	m_adv_data[idx++] = 0x42;
	m_adv_data[idx++] = 0x4F;

	// Product Type
	m_adv_data[idx++] = PRODUCT_TYPE;

	// product parameter
	memcpy(&m_adv_data[idx], &prodcut_parameters_c, 4);
	idx += 4;

	// chipID
	memcpy(&m_adv_data[idx], device_infor.ChipID, 8);
	idx += 8;

	// Session Stat
	m_adv_data[idx++] = 0;

	m_ble_adv_data.adv_data.len = idx;

	idx = 0;

	m_ble_adv_data.scan_rsp_data.len = idx;

#endif
}

void ble_adv_start()
{
	APP_ERROR_CHECK(sd_ble_gap_adv_start(m_adv_handle, 1));
}

void ble_adv_stop(void)
{
	if (m_adv_handle != BLE_GAP_ADV_SET_HANDLE_NOT_SET)
		APP_ERROR_CHECK(sd_ble_gap_adv_stop(m_adv_handle));
}

void advertisting_init(void)
{
	m_adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
	m_adv_params.interval = MSEC_TO_UNITS(DEF_ADV_INTERVAL * ADV_INTERVAL_STEP, UNIT_0_625_MS);
	//  m_adv_params.interval      = MSEC_TO_UNITS(DEF_ADV_INTERVAL * ADV_INTERVAL_STEP, UNIT_1_25_MS);
	m_adv_params.p_peer_addr = NULL;
	m_adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;
	m_adv_params.duration = 0;
	m_adv_params.primary_phy = BLE_GAP_PHY_1MBPS;

	adv_data_set();
	APP_ERROR_CHECK(sd_ble_gap_adv_set_configure(&m_adv_handle, &m_ble_adv_data, &m_adv_params));
	APP_ERROR_CHECK(sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, DEF_TX_POWER));
}

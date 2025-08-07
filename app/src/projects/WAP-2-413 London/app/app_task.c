#include "mcu.h"
#include "app_cfg_cmd.h"
#include "app_task.h"
#include "dpid_map_c.h"
#include "mt_gpio.h"
#include "mcu_api.h"
#include "logging_rw_implent.h"
#include "fstorage.h"
#include "mt_ble_adv.h"
#include "session.h"

#define NRF_LOG_MODULE_NAME app_task_log
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

// #define APP_TASK_DEBUG_OUT

#ifndef USE_STATE_MACHINE

extern uint32_t ble_ctcs_s_send_handler(uint8_t *p_data, uint16_t len, uint16_t uuid);

#define POWER_ON_AFTER_SENDING_CYCLE 10 // 10*100 1秒中
#define MCU_RESPONDS_SEND_CYCLE 150		// 15 秒

#define TIME_OUT_NUM 10
#define CMD_SEND_NUN 3
extern uint32_t get_system_time_tick(void);

static uint32_t temp_time_tick = 0;
static uint16_t time_cnt = 0;
static uint16_t send_count = 0;
static uint32_t cmd_status_none_time_count = 0;

// 上传数据给app 后的延时
static uint32_t send_data_app_delay_time_tick = 0;
static uint32_t send_data_app_delay_time = 0;

static bool user_connect_state = false; // 用户发来第一条指令时表示进入用户连接状态

bool bluetooth_connect_status = false;
uint16_t m_current_conn_handle;
/*
启动状态查询命令08 命令
*/
void start_status_query(void)
{
	send_cmd_state = CMD_STATUS_START;
	current_cmd_data.cmd = STATE_QUERY_CMD;
	Response_Feature_Id_list.update_feature_num = 0;
}

/*
启动设置 06命令
*/
static void set_cmd_state_value(unsigned char feature_id)
{
	unsigned char dpid = get_dpid(feature_id);
	memset((void *)&current_cmd_data, 0, sizeof(cmd_data_description));
	send_cmd_state = CMD_STATUS_START;
	current_cmd_data.cmd = DATA_QUERT_CMD;
	current_cmd_data.dpID = dpid;

	if (dpid == DPID_TIMING_SWITCH) // 106 两个字节
	{
		current_cmd_data.data.int_data = *((unsigned short *)get_set_value_adr(feature_id));
	}
	else if (dpid == DPID_LIGHT_COLOR) //	110 12 byte
	{
		memcpy((void *)current_cmd_data.data.buffer, (void *)get_set_value_adr(feature_id), 12);
	}
	else
	{

		current_cmd_data.data.int_data = *((unsigned char *)get_set_value_adr(feature_id));
	}
}

/*
<1> 接收到手机app的set 命令后，完成数据解析后会调用start_dpid_set函数，
启动涂鸦协议DATA_QUERY_CMD(06)set 命令（调用 set_cmd_state_value函数),进行控制操作。

<2> 在涂鸦协议命令发送状态机mcu_cmd_send_task()中,
   涂鸦06命令正确应答后也会调用该函数，判断 Set_Feature_Id_list  中是否还有set 命令，
   如果没有了，则设置   dpid_setting_status 为  DPID_SET_ACK_OK,
   如果还有set  命令 则继续调用 start_dpid_set()函数，进行设置命令

<3> 在涂鸦协议命令发送状态机mcu_cmd_send_task()中，根据send_cmd_state状态进行涂鸦协议状态机管理，
根据通讯状态结果 设置 dpid_setting_status 的状态

<4> 在 app_task 中，再根据  dpid_setting_status 的状态回复应答給手机app


<5> 在配置一个session后，启动session 后则会进行 session set命令分析，填充Set_Feature_Id_list 后，启动
start_dpid_set 进行 set  操作
*/

void start_dpid_set(void)
{
	unsigned char feature_id;
	static uint16_t set_feature_id_index = 0;
	//	NRF_LOG_INFO("dpid_setting_status =%d",dpid_setting_status);
	switch (dpid_setting_status)
	{
	case DPID_SET_STATE_NONE:
		Response_Feature_Id_list.update_feature_num = 0;
		set_feature_id_index = 0;
		feature_id = Set_Feature_Id_list.feature_id[set_feature_id_index++];
		dpid_setting_status = DPID_SET_PROCESSING;
		//  NRF_LOG_INFO("start_dpid_set DPID_SET_STATE_NONE");
		if (feature_id == DEVICE_CODE_FEATURE_ID) // 设置设备代码,
		{
			if (fstorge_write_device_code(SettingData.device_code) == false) // 往flash 写入 device_code ,在falsh写入成功事件中会device_code_write_finished_proc(void)
			{
				dpid_setting_status = DPID_SET_ACK_DATA_ERROR;
			}
		}
		else
			set_cmd_state_value(feature_id);

		break;
	case DPID_SET_PROCESSING:
		if (set_feature_id_index < Set_Feature_Id_list.update_feature_num)
		{
			//	 NRF_LOG_INFO("set_feature_id_index <Set_Feature_Id_list.update_feature_num");
			feature_id = Set_Feature_Id_list.feature_id[set_feature_id_index++];
			set_cmd_state_value(feature_id);
		}
		else
		{
			NRF_LOG_INFO("start_dpid_set DPID_SET_ACK_OK");
			dpid_setting_status = DPID_SET_ACK_OK;
			Set_Feature_Id_list.update_feature_num = 0;
			set_feature_id_index = 0;
			//	dpid_setting_status	= DPID_SET_STATE_NONE;
			//	device_Set_Response(true);
			// 在这边再上一个状态主动上传
		}

		break;
	default:

		break;
	}
}
void startup_features_set_cmd(void)
{
	start_dpid_set();
}

// 在falsh写入成功事件中会device_code_write_finished_proc(void)

void device_code_write_finished_proc(bool result)
{
	uint16_t device_code;
	if (result)
	{
		device_code = fstorge_read_device_code();
		if (SettingData.device_code == device_code)
		{
			update_adv_data_device_code(device_code);
			start_dpid_set();
		}
		else // 写入后读到的值与设置值不相符
			dpid_setting_status = DPID_SET_ACK_DATA_ERROR;
	}
	else
		dpid_setting_status = DPID_SET_ACK_DATA_ERROR;
}

bool get_user_connect_state(void)
{
	return user_connect_state;
}
void clear_usr_connect_state(void)
{
	user_connect_state = false;
}

void set_user_connect_state(bool value)
{
	user_connect_state = value;
}

/* 功能点设置  06命令 应答超时后调用的功能函数*/

static void data_query_cmd_ack_timeout_cb(void)
{

	dpid_setting_status = DPID_SET_ACK_TIMEOUT;
}

/* 功能点设置  06命令 应答成功后调用的功能函数*/

static void data_query_cmd_ack_ok_cb(void)
{
	NRF_LOG_INFO("data_query_cmd_ack_ok_cb call start_dpid_set");
	start_dpid_set();
}

/* 功能点状态查询 08命令 应答超时后调用的功能函数 */

static void state_query_cmd_ack_timeout_cb(void)
{

	send_failed_to_app(m_current_conn_handle);
}

/* 功能点状态查询 08命令 应答成功后调用的功能函数 */

static void state_query_cmd_ack_ok_cb(void)
{
	uint8_t send_feature_id_num = 0;
	switch (App_Request_Data_Type)
	{
	case BLUETOOTH_CON_AFTER_SEND_ALL_ID: // 蓝牙连接后，蓝牙板主动发送一次08命令查询所有状态，返回后，将所有feature_id 状态上传

		send_feature_id_num = set_all_upload_feature_id();
		send_feature_id_data_to_app(m_current_conn_handle, Get_Request_id, send_feature_id_num, ACTIVE_UPLOAD_CMD);
		break;

	case REQUEST_ALL_FEATURE_ID: // Get Request 命令，发送08命令，将08命令请求的状态上传
		Response_Feature_Id_list.feature_id[Response_Feature_Id_list.update_feature_num++] = Connect_Status_ID;
		send_feature_id_data_to_app(m_current_conn_handle, Response_Feature_Id_list.feature_id, Response_Feature_Id_list.update_feature_num, GET_RESPONSE);

		break;

	case REQUEST_APPOINT_FEATURE_ID: // Get Request 命令，发送08命令，将Get Request命令中的feature_id 数据上传，
									 // 命令中的ID状态除了08命令能返回的状态，其他状态是在其他时候被更新
		send_feature_id_data_to_app(m_current_conn_handle, Get_Request_id, allRequestID_Num, GET_RESPONSE);

		break;
	}
}

/*
mcu_cmd_send_task() 涂鸦协议命令发送状态机没有命令发送时调用的功能函数
*/

void static none_status_process(void)
{
	if (temp_time_tick == get_system_time_tick())
	{
		return;
	}
	temp_time_tick = get_system_time_tick();
	cmd_status_none_time_count++;

	switch (heart_send_status)
	{
	case SEND_AFTER_POWER_ON:
		if (cmd_status_none_time_count % POWER_ON_AFTER_SENDING_CYCLE == 0)
		{
			send_cmd_state = CMD_STATUS_START;
			current_cmd_data.cmd = HEAT_BEAT_CMD;
		}

		break;

	case SEND_AFTER_MCU_RESPONDS:
		if (cmd_status_none_time_count % MCU_RESPONDS_SEND_CYCLE == 0)
		{
			send_cmd_state = CMD_STATUS_START;
			current_cmd_data.cmd = HEAT_BEAT_CMD;
			cmd_status_none_time_count = 0;
		}
		else if (cmd_status_none_time_count % (MCU_RESPONDS_SEND_CYCLE - 50) == 0)
		{
			// send_cmd_state = CMD_STATUS_START ;
			// current_cmd_data.cmd  = 	PRODUCT_INFO_CMD;
		}

		break;
	default:
		break;
	}
}

/*
mcu_cmd_send_task() 涂鸦协议命令发送状态机 启动命令发送时 调用的功能函数
在该函数中会根据 cmd  命令字类型进行命令处理
比如心跳命令
数据查询命令08
数据设置命令06 等等
*/

void static start_status_process(void)
{
	switch (current_cmd_data.cmd)
	{
	case STATE_QUERY_CMD:
		Response_Feature_Id_list.update_feature_num = 0;
		send_status_query();
#ifdef APP_TASK_DEBUG_OUT
		NRF_LOG_INFO("send_status_query");
#endif
		send_cmd_state = CMD_STATUS_SEND_IN;
		break;

	case HEAT_BEAT_CMD: // send heartbeat command
						//						 NRF_LOG_INFO("send heard cmd");
		send_heart_to_mcu();
		send_cmd_state = CMD_STATUS_SEND_IN;
		break;

	case PRODUCT_INFO_CMD: // red product information
		get_product_info_cmd();
		send_cmd_state = CMD_STATUS_SEND_IN;

		break;
	case DATA_QUERT_CMD:
		cmd_status_none_time_count = 0;

		if (current_cmd_data.dpID != DPID_CONNECT_STATE)
		{
			send_dpID_cmd(&current_cmd_data);
			send_cmd_state = CMD_STATUS_SEND_IN;
		}
		else
		{
			// 是App 连接状态命令
			user_connect_state = (current_cmd_data.data.byte_data == 1) ? true : false;

			if (!user_connect_state)
				CONNCET_BOND_LED_OFF();

			send_cmd_state = CMD_STATUS_OK;
			NRF_LOG_INFO("send_cmd_state = CMD_STATUS_OK");
		}

		break;

	default:
		break;
	}
	temp_time_tick = get_system_time_tick();
	time_cnt = 0;
}

/*
mcu_cmd_send_task() 涂鸦协议命令发送状态机 命令正在发送时 调用的功能函数
会进行命令应答超时判断，

*/

void static send_in_status_process(void)
{
	if (get_system_time_tick() == temp_time_tick)
		return; // 100ms 检查一次

	temp_time_tick = get_system_time_tick();

	// NRF_LOG_INFO("time_cnt==%d",time_cnt);
	if (++time_cnt >= TIME_OUT_NUM)
	{

		// 连续三次发送应答超时后会设置超时状态
		if (++send_count >= CMD_SEND_NUN)
		{
			send_cmd_state = CMD_STATUS_TIMEOUT;
		}
		else
		{
			// 不足三次时，继续启动发送
			send_cmd_state = CMD_STATUS_START;
			time_cnt = 0;
		}
	}
}

/*
mcu_cmd_send_task() 涂鸦协议命令发送状态机 出现命令发送应答超时状态时 调用的功能函数


*/

void static timeout_status_process(void)
{
	time_cnt = 0;
	send_count = 0;
	send_cmd_state = CMD_STATUS_NONE;

	switch (current_cmd_data.cmd)
	{
	case HEAT_BEAT_CMD:
		heart_send_status = SEND_AFTER_POWER_ON;
		break;

	case STATE_QUERY_CMD: // 08状态查询命令
		state_query_cmd_ack_timeout_cb();
		break;

	case DATA_QUERT_CMD: // 06设置命令
		data_query_cmd_ack_timeout_cb();
		break;

	default:
		break;
	}
}

/*
mcu_cmd_send_task() 涂鸦协议命令发送状态机 应答成功时 调用的功能函数


*/

void static ok_status_process(void)
{
	time_cnt = 0;
	send_count = 0;
	send_cmd_state = CMD_STATUS_NONE;

	switch (current_cmd_data.cmd)
	{
	case HEAT_BEAT_CMD:
		heart_send_status = SEND_AFTER_MCU_RESPONDS;
#ifdef APP_TASK_DEBUG_OUT
		NRF_LOG_INFO("hear_beat ok");
#endif
		break;

	case PRODUCT_INFO_CMD:
#ifdef APP_TASK_DEBUG_OUT
		NRF_LOG_INFO("product_info: %s", product_info);
#endif

		break;

	case STATE_QUERY_CMD: // 08状态查询命令
#ifdef APP_TASK_DEBUG_OUT
		NRF_LOG_INFO("state_query ok");
#endif
		state_query_cmd_ack_ok_cb();

		break;

	case DATA_QUERT_CMD: // 06设置命令
		current_cmd_data.dpID = 0;
		data_query_cmd_ack_ok_cb();
		break;

	} // switch(current_cmd_data.cmd)
}

/*
mcu_cmd_send_task() 涂鸦协议命令发送状态机 应答数据校验错误 调用的功能函数


*/

void static ack_check_error_status_process(void)
{
	time_cnt = 0;
	send_count = 0;
	send_cmd_state = CMD_STATUS_NONE;
#ifdef APP_TASK_DEBUG_OUT
	NRF_LOG_INFO("ack_check_error");
#endif

	switch (current_cmd_data.cmd)
	{
	case HEAT_BEAT_CMD:
		heart_send_status = SEND_AFTER_POWER_ON;
		break;

	case STATE_QUERY_CMD: // 08状态查询命令
		send_failed_to_app();
		break;

	case DATA_QUERT_CMD: // 06设置命令
		dpid_setting_status = DPID_SET_ACK_DATA_ERROR;
		break;

	default:
		break;
	}
}

/*
mcu_cmd_send_task() 涂鸦协议命令发送状态机管理


*/

void mcu_cmd_send_task(void)
{

	switch (send_cmd_state)
	{
	case CMD_STATUS_NONE: // 没有命令时的状态管理
		none_status_process();
		break;

	case CMD_STATUS_START: // 启动命令状态
		start_status_process();
		break;

	case CMD_STATUS_SEND_IN: // 命令发送中状态
		send_in_status_process();

		break;
	case CMD_STATUS_TIMEOUT: // 应答超时状态
		timeout_status_process();

		break;

	case CMD_STATUS_OK: // 有接收到正确应答
		ok_status_process();
		break;

	case CMD_STATUS_ACK_CHECK_ERROR: // 应答数据校验错误状态机
		ack_check_error_status_process();
		break;
	}
}

static uint32_t run_led_time_tick = 0;
static uint32_t run_led_time_count = 0;
static HEART_SEND_STATUS prev_heart_send_status = HEART_SEND_NONE;
/*
根据心跳状态，控制run led 灯的状态，心跳正常，led灯每秒闪烁一次，或者15s 闪烁一次
注意，根据正常状态下每隔15S发送一次心跳，因为如果UART断先，将要15S状态才会改变
同时，记录下与uart 的连接状态保存到 Get_Response_Pack.connction_status.attribute

*/
extern bool mt_is_ble_connected(void);

static void run_state_task(void)
{
	if (prev_heart_send_status != heart_send_status) // 状态变化时
	{
		prev_heart_send_status = heart_send_status;
		run_led_time_tick = get_system_time_tick();
		run_led_time_count = 0;
		RUN_LED_OFF();
		// CONNCET_BOND_LED_OFF() ;
		switch (heart_send_status)
		{
		case SEND_AFTER_POWER_ON:
			//	Get_Response_Pack.connction_status.attribute = 0 ;
			break;

		case SEND_AFTER_MCU_RESPONDS:
			//		Get_Response_Pack.connction_status.attribute = 1 ;

			break;
		}

		if (get_user_connect_state())
		{ // 将运行状态主动上传
			Get_Request_id[0] = Connect_Status_ID;
			send_feature_id_data_to_app(m_current_conn_handle, m_current_conn_handle, Get_Request_id, 1, ACTIVE_UPLOAD_CMD);
		}
	}

	switch (heart_send_status)
	{
	case HEART_SEND_NONE:
		run_led_time_tick = get_system_time_tick();
		run_led_time_count = 0;
		break;

	case SEND_AFTER_POWER_ON: // 没有心率应答，也就是控制器无回复,15s闪烁一次
		if (get_system_time_tick() == run_led_time_tick)
			return;
		run_led_time_tick = get_system_time_tick();
		run_led_time_count++;

		if (run_led_time_count >= 10) // 该状态正常保持1秒针记录下状态
		{
			if (Get_Response_Pack.connction_status.attribute != 0) // 记录下连接断开的状态，并主动上传
			{
				Get_Response_Pack.connction_status.attribute = 0;
#ifdef APP_TASK_DEBUG_OUT
				NRF_LOG_INFO("Controller disconnected");
#endif
			}
		}

		if (run_led_time_count >= 150 && run_led_time_count <= 160)
		{
			RUN_LED_ON();
		}
		else if (run_led_time_count > 160)
		{
			RUN_LED_OFF();
			run_led_time_count = 0;
		}

		break;

	case SEND_AFTER_MCU_RESPONDS: // 有心率数据应答，1s 闪烁一次
		if (get_system_time_tick() == run_led_time_tick)
			return;
		run_led_time_tick = get_system_time_tick();
		run_led_time_count++;
		//	 NRF_LOG_INFO("run_led_time_count =%d connect status=%d",run_led_time_count,Get_Response_Pack.connction_status.attribute);
		if (run_led_time_count >= 10) // 该状态正常保持1秒针记录下状态
		{
			if (Get_Response_Pack.connction_status.attribute != 1) // 记录下连接状态，并主动上传
			{
				Get_Response_Pack.connction_status.attribute = 1;

				NRF_LOG_INFO("Controller connected");

				start_status_query();
				//			NRF_LOG_INFO("start_status_query");
			}
		}

		if (run_led_time_count >= 10 && run_led_time_count <= 20)
		{
			RUN_LED_ON();
		}
		else if (run_led_time_count > 20)
		{
			RUN_LED_OFF();
			run_led_time_count = 0;
		}

		break;
	}
}
/*
 由于蓝牙与手机连接时，蓝牙会将连接状态发送给桑拿控制，桑拿控制器得到连接状态后会主动将数据状态传上来
							  因此上面并不需要发送状态查询命令


蓝牙刚刚连接时，检测UART连接状态，如果uart 没有连接，则延时200ms 后将连接状态发送给手机

*/

static void user_connect_after_processing(void)
{
	static uint32_t bluetooth_connect_timetick = 0;
	static uint32_t bluetooth_connect_time_count = 0;
	if (get_user_connect_state())
	{
		switch (bluetooth_connect_timetick)
		{
		case 0:
			bluetooth_connect_timetick = get_system_time_tick();
			break;
		default:
			if (bluetooth_connect_status)
				return;
			if (bluetooth_connect_timetick == get_system_time_tick())
				return;
			bluetooth_connect_timetick = get_system_time_tick();
			bluetooth_connect_time_count++;
			if (bluetooth_connect_time_count > 15)
			{

				if (Get_Response_Pack.connction_status.attribute == 0)
				{
#ifdef APP_TASK_DEBUG_OUT
					NRF_LOG_INFO("send uart no connect state to app");
#endif

					if (cts_send_state == TX_COMPLETE)
					{
						Get_Request_id[0] = Connect_Status_ID;
						send_feature_id_data_to_app(m_current_conn_handle, Get_Request_id, 1, ACTIVE_UPLOAD_CMD);
						bluetooth_connect_status = true;
					}
				}
				else
					bluetooth_connect_status = true;
			}
			break;
		}
	}
	else
	{

		// NRF_LOG_INFO("bluetooth_connect_status = false");
		bluetooth_connect_timetick = 0;
		bluetooth_connect_status = false;
		bluetooth_connect_time_count = 0;
		// CONNCET_BOND_LED_OFF();
	}
}

/*
桑拿控制器状态主动上传


*/

void proactively_upload_processing_procedure(void)
{
	// 	if(bluetooth_connect_status == true )
	if (mt_is_ble_connected() == true) // 蓝牙连接上时
	{
		if (upload_update_status == START_UPDATE) // 主动上传的feature id
		{
			//			 if(cts_send_state == TX_COMPLETE) {
			//					send_feature_id_data_to_app(Update_Feature_Id_list.feature_id,Update_Feature_Id_list.update_feature_num,ACTIVE_UPLOAD_CMD);
			//					#ifdef APP_TASK_DEBUG_OUT
			//					NRF_LOG_INFO("start update feature num =%d",Update_Feature_Id_list.update_feature_num);
			//					#endif
			//					Update_Feature_Id_list.update_feature_num =0 ;
			//					upload_update_status = UPDATE_NONE ;
			//			 }
			Update_Feature_Id_list.update_feature_num = 0;
			upload_update_status = UPDATE_NONE;
		}
	}
	else
	{											  // 蓝牙没有连接时
		if (upload_update_status == START_UPDATE) // 主动上传的feature id
		{
// send_feature_id_data_to_app(Update_Feature_Id_list.feature_id,Update_Feature_Id_list.update_feature_num,ACTIVE_UPLOAD_CMD);
#ifdef APP_TASK_DEBUG_OUT
			NRF_LOG_INFO("no connect app start update feature num =%d", Update_Feature_Id_list.update_feature_num);
#endif
			Update_Feature_Id_list.update_feature_num = 0;
			upload_update_status = UPDATE_NONE;
		}
	}
}

void app_task(void)
{

	task_check_input_io();	  // 检测外部输入IO
	run_state_task();		  // 根据涂鸦协议的心跳进行运行状态灯管理
	bluetooth_uart_service(); // 涂鸦协议,UART接收到的数据报文解析
	mcu_cmd_send_task();	  // 涂鸦协议命令发送状态机

	// 通过手机App 启动Set 命令的状态机管理
	switch (dpid_setting_status)
	{
	case DPID_SET_STATE_NONE:					   // 没有set 命令
		proactively_upload_processing_procedure(); // 状态主动上传处理 (session 版本中已经屏蔽，没有使用)
		user_connect_after_processing();		   // 根据uart 与桑拿控制器的连接状态，将连接状态上传給手机( session 版本中已经屏蔽，没有使用)

		break;
	case DPID_SET_PROCESSING: // set 命令进行中
		break;

	case DPID_SET_ACK_TIMEOUT:	  // set 请求 命令后应答超时
	case DPID_SET_ACK_DATA_ERROR: // set  请求命令应答错误
		device_Set_Response(false);
		dpid_setting_status = DPID_SET_STATE_NONE;
		break;

	case DPID_SET_ACK_OK: // set 命令 应答OK
		device_Set_Response(true);
		// App 进行set 命令后，收到了正确应答，返回设置请求OK 給手机，再会进行一个100-200ms 的延时等待状态，将控制器主动发送上来的状态数据发送給手机
		dpid_setting_status = DPID_SET_ACK_SEND_WAIT;
		send_data_app_delay_time_tick = get_system_time_tick();
		send_data_app_delay_time = 0;

		break;
	case DPID_SET_ACK_SEND_WAIT: // set 命令应答OK后的延时管理
		if (get_system_time_tick() == send_data_app_delay_time_tick)
			break;
		send_data_app_delay_time++;

		send_data_app_delay_time_tick = get_system_time_tick();

		if (send_data_app_delay_time == 1) // 100ms  过后，主动发送 控制器应答给蓝牙的dpid 数据
		{
			if (Response_Feature_Id_list.update_feature_num != 0)
			{
				//					  send_feature_id_data_to_app(Response_Feature_Id_list.feature_id,Response_Feature_Id_list.update_feature_num,ACTIVE_UPLOAD_CMD);

				// #ifdef APP_TASK_DEBUG_OUT
				NRF_LOG_INFO("Response_Feature_Id_list num =%d", Response_Feature_Id_list.update_feature_num);
				// #endif

				Response_Feature_Id_list.update_feature_num = 0;
			}
			else
			{
				dpid_setting_status = DPID_SET_STATE_NONE;
				send_data_app_delay_time = 0;
			}
		}
		else if (send_data_app_delay_time >= 2) // 200ms  后清除状态
		{
#ifdef APP_TASK_DEBUG_OUT
			NRF_LOG_INFO("dpid set clearn state to none");
#endif

			dpid_setting_status = DPID_SET_STATE_NONE;
			send_data_app_delay_time = 0;
		}
		break;

	default:
		break;
	}

	session_task_exec(); // session 任务管理
}

void custom_protocol_init(void)
{
	NRF_LOG_INFO("custom_protocol_init");
	mcu_protocol_init();
	// clear_recv_callbak();
	// tuya_cmd_send_task_ctor(&ao_tuya_cmd_send_task);
	// controller_connection_status_task_ctor(&ao_controller_connection_status_task);
}

#endif

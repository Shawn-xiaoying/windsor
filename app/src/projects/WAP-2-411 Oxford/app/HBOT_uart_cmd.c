/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"

#include <string.h>
#include "app_task.h"
#include "mt_serial.h"
// #include "wap-1-100-windsor-master-protocol.h"
// #include "app_cfg_cmd.h"
#include "HBOT_uart_cmd.h"

#include "air_conditione_communication.h"
#include "sensor_communication.h"
#include "app_cfg_cmd.h"
#include "fstorage.h"
#include "mt_ble_adv.h"

#define NRF_LOG_MODULE_NAME hbot_uart_modul
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

uint8_t uart_send_buf[50];
uint8_t m_cur_read_cmd = 0;

/*************************************************************************************************************
 * uart 接收命令处理
 * *********************************/
static bool wait_rev_cmd_ok = false; /*接收命名正确标志位*/

static void uart_revice_feedback_handle(uint8_t *p_data, uint8_t len)
{
    uint8_t slave_addr = 0, cmd = 0;
    uint16_t __attribute__((unused)) temp_crc = 0, rev_crc = 0;

    temp_crc = CalcCrc16(p_data, len);

    rev_crc = p_data[len - 2]; // rev rcr data
    rev_crc = (rev_crc << 8) | p_data[len - 1];

    slave_addr = p_data[0]; // 从机地址
    cmd = p_data[1];

#if 0
    if (temp_crc != rev_crc) // crc 校验    
    {
        wait_rev_cmd_ok = false;
        return;
    }

#endif

    switch (slave_addr)
    {
    case 0x18: /*设置和读取空调返回命令回复*/
        if (air_conditioner_cmd_hanlder(p_data, len) == true)
        {
            wait_rev_cmd_ok = true;
            break;
        }
        break;
    case 0x81: /*读取传感器命令回复*/
        if (cmd == READ_CMD)
        {
            uart_rev_sensor_data_handler(p_data, len);
            wait_rev_cmd_ok = true;
        }
        break;
    default:
        break;
    }
}

/************************************************************************
 * uart 接收数据函数
 * ***************/
void uart_receive_handler(uint8_t *p_data, uint16_t len)
{

    ASSERT(p_data != NULL);
    NRF_LOG_INFO("uart_receive_handler crc16=0x%x", CalcCrc16(p_data, len - 2));

    //    NRF_LOG_HEXDUMP_INFO(p_data,len);
    //   NRF_LOG_INFO("\r\n");

    uart_revice_feedback_handle(p_data, len);
}

/*********************************************************
 *app设置请求转换成 uart set cmd 处理
 *******************************************/
static uint8_t uart_write_state = READ_AIR_CONDITI_STATE;
static uint16_t set_feature_id_index = 0;
extern Feature_Id_List Set_Feature_Id_list;

Feature_Id_List* pSet_Feature_Id_list = &Set_Feature_Id_list ;
Set_Request_Data_Pack_t* pSettingData = &SettingData ;

void uart_switch_cmd_state(uint8_t set_state)
{
    uart_write_state = set_state;
    set_feature_id_index = 0;
}

void set_uart_wirte_sate(uint8_t set_state)
{
	uart_write_state = set_state;
}

uint8_t get_uart_write_state(void)
{
	 return uart_write_state ;
}

// 在falsh写入成功事件中会device_code_write_finished_proc(void)
void device_code_write_finished_proc(bool result)
{
    uint16_t device_code;
	  bluetooth_connection_status_task_t *bluetooth_connection_ownership_task; 
	 	bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
	
	
    if (result)
    {
        device_code = fstorge_read_device_code();
        if (SettingData.device_code == device_code)
        {
            update_adv_data_device_code(device_code);
					  
            return;
        }
        else // 写入后读到的值与设置值不相符
				{
					
						if (bluetooth_connection_ownership_task != NULL)
            device_Set_Response(bluetooth_connection_ownership_task, false);
						
						 bluetooth_connection_ownership_task->rv_lock = 0 ;
					
				}
    }
    else
		{
			  if (bluetooth_connection_ownership_task != NULL)
        device_Set_Response(	bluetooth_connection_ownership_task, false);
				
				 bluetooth_connection_ownership_task->rv_lock = 0 ;
			
		}

    uart_write_state = READ_AIR_CONDITI_STATE;
    set_feature_id_index = 0;
    Set_Feature_Id_list.update_feature_num=0;
}

/*************************************************
 * 周期性通过uart读取空调和传感器的数值和状态
 * 100ms 执行一次
 * *********************************************/
#define TMIE_OUT_10S (10000 / SYSTEM_TIME_UINT)
#define TIMEOUT_300MS (300 / SYSTEM_TIME_UINT)
#define FALSE_TIMES 3 // 命令返回错误次数

void task_uart_send_cmd_handle(void)
{
    static uint16_t time_cnt = 0, rev_cmd_false_times = 0;
    unsigned char feature_id;
    static uint32_t temp_time_tick = 0;
    bluetooth_connection_status_task_t *bluetooth_connection_ownership_task; 
	 	bluetooth_connection_ownership_task = find_ownership_bluetooth_connection_task(device_infor.ShortUserID);
	
    if (uart_write_state != SET_AIR_CONDITI_STATE)
    {
        /*100ms 进来一次*/
        if (get_system_time_tick() == temp_time_tick)
            return;
        temp_time_tick = get_system_time_tick();
    }

    switch (uart_write_state)
    {
        /*********************************** 设置空调 onoff ****************************************************/
    case SET_AIR_CONDITI_STATE: // 设置空调命令

        rev_cmd_false_times = 0;
        wait_rev_cmd_ok = false;
		    
		    if(pSet_Feature_Id_list->update_feature_num ==0)
				{
					NRF_LOG_INFO("update_feature_num ==0");
					uart_write_state = READ_AIR_CONDITI_STATE;
					break ;
				}
		
				 if (set_feature_id_index >= pSet_Feature_Id_list->update_feature_num)
        {
					  NRF_LOG_INFO("set cmd finished");
            set_feature_id_index = 0;
            pSet_Feature_Id_list->update_feature_num = 0;
            uart_write_state = READ_AIR_CONDITI_STATE;
						if(pSet_Feature_Id_list == &Set_Feature_Id_list)
						{ 
							if (bluetooth_connection_ownership_task != NULL)
							device_Set_Response(bluetooth_connection_ownership_task, true); /*设置返回命令成功，传输给app成功命名*/
							bluetooth_connection_ownership_task->rv_lock = 0 ;
						}else {

					//		pSet_Feature_Id_list = &Set_Feature_Id_list ;
					//	  pSettingData = &SettingData ;
							
						}
						 break;
        }
				
        feature_id = pSet_Feature_Id_list->feature_id[set_feature_id_index++];
				NRF_LOG_INFO("feature_id =%d set_feature_id_index=%d",feature_id,set_feature_id_index);
        switch (feature_id)
        {
        case Air_Conditioner_ID:
            uart_send_set_air_conditioner_OnOff(Get_Response_Pack.Air_Conditi.attribute); // 屏蔽开关空调
            uart_write_state = WAIT_SET_AIR_CONDITI_STATE;
            break;

        case Target_Temperature_ID:

            uart_send_set_air_conditioner_temper(pSettingData->AirConditi_Target_Temper);
            uart_write_state = WAIT_SET_AIR_CONDITI_TARGET_TEMP;

            break;

        case Remaining_Minutes_Timer_ID:

            break;
				case Oxygen_Concentrator_ID:
				//	 NRF_LOG_INFO("Oxygen_Concentrator_ID");
					 start_oxygen_ctrl_task();
				   uart_write_state = WAIT_Oxygen_Concentrator_CTRL ;
				    break ;
				case Air_Compressor_ID:
				//	  NRF_LOG_INFO("Oxygen_Concentrator_ID");
					  start_air_comp_ctrl_task();
				    uart_write_state = WAIT_Air_Compressor_CTRL ;
				    break ;

        case DEVICE_CODE_FEATURE_ID:
					   NRF_LOG_INFO("DEVICE_CODE_FEATURE_ID");
            if (fstorge_write_device_code(SettingData.device_code) == false) // 往flash 写入 device_code ,在falsh写入成功事件中会device_code_write_finished_proc(void)
            {

                uart_write_state = READ_AIR_CONDITI_STATE;
                set_feature_id_index = 0;
                Set_Feature_Id_list.update_feature_num = 0;
							  if (bluetooth_connection_ownership_task != NULL)
                device_Set_Response(bluetooth_connection_ownership_task, false); /*send 失败次数为3，返回给app 错误指令*/
								 bluetooth_connection_ownership_task->rv_lock = 0 ;
            }
            break;

        default:
            break;
        }
       

        break;
			
		case 		WAIT_Air_Compressor_CTRL:
						break ;		
    case 		WAIT_Oxygen_Concentrator_CTRL:
						break ;						
			
    case WAIT_SET_AIR_CONDITI_STATE: // 等待设置空调状态回复指超时

        if (wait_rev_cmd_ok == false)
        {
            if (time_cnt++ >= TIMEOUT_300MS)
            {
                time_cnt = 0;
                rev_cmd_false_times++;
                uart_send_set_air_conditioner_OnOff(Get_Response_Pack.Air_Conditi.attribute);
            }
            if (rev_cmd_false_times >= FALSE_TIMES) /*失败*/
            {
							  if(pSet_Feature_Id_list == &Set_Feature_Id_list) 
								{	
									if (bluetooth_connection_ownership_task != NULL)
									device_Set_Response(bluetooth_connection_ownership_task, false); /*send 失败次数为3，返回给app 错误指令*/
								
									bluetooth_connection_ownership_task->rv_lock = 0 ;
								
								
								}	else {
									
									//设置session 状态错误
								}
                Get_Response_Pack.Air_Conditi.attribute = 0;
								uart_write_state = SET_AIR_CONDITI_STATE; // SET_AIR_CONDITI_TARGET_TEMP;								
								
            }
        }
        else
        {
            //                device_Set_Response(true); /*设置返回命令成功，传输给app成功命名*/
            uart_write_state = SET_AIR_CONDITI_STATE;
        }
        break;
        /*********************************** 设置 空调 目标温度 ****************************************************/
        //        case SET_AIR_CONDITI_TARGET_TEMP://设置空调温度
        //             wait_rev_cmd_ok     = false;
        //             rev_cmd_false_times = 0;

        //             uart_send_set_air_conditioner_temper(Get_Response_Pack.AirConditi_Target_Temper);
        //             uart_write_state = WAIT_SET_AIR_CONDITI_TARGET_TEMP;
        //             break;
    case WAIT_SET_AIR_CONDITI_TARGET_TEMP: // 等待设置空调温度命名超时

        if (wait_rev_cmd_ok == false)
        {
            if (time_cnt++ >= TIMEOUT_300MS)
            {
                time_cnt = 0;
                rev_cmd_false_times++;
                uart_send_set_air_conditioner_temper(pSettingData->AirConditi_Target_Temper);
            }
            if (rev_cmd_false_times >= FALSE_TIMES) /*失败*/
            {
							  if(pSet_Feature_Id_list == &Set_Feature_Id_list) 
								{	
							  if (bluetooth_connection_ownership_task != NULL)
                device_Set_Response(bluetooth_connection_ownership_task, false); /*send 失败次数为3，返回给app 错误指令*/
								
								 bluetooth_connection_ownership_task->rv_lock = 0 ;
							  }else {
									
									
									//设置session 状态错误
								}
								
               uart_write_state = SET_AIR_CONDITI_STATE;
            }
        }
        else
        {
            //   device_Set_Response(true); /*设置返回命令成功，传输给app成功命名*/
            //   uart_write_state = READ_AIR_CONDITI_STATE;
            uart_write_state = SET_AIR_CONDITI_STATE;
        }
        break;

        /*********************************** 读取空调On Off状态 ****************************************************/
    case READ_AIR_CONDITI_STATE: /*send 读取当前空调状态*/
        wait_rev_cmd_ok = false;
        rev_cmd_false_times = 0;

        if (++time_cnt >= TMIE_OUT_10S) // 10S 读取一次
        {
            time_cnt = 0;
            m_cur_read_cmd = READ_AIR_CONDITI_STATE;

            uart_send_read_air_conditioner_state();
            uart_write_state = WAIT_READ_AIR_CONDITI_STATE;
        }
        break;
    case WAIT_READ_AIR_CONDITI_STATE:

        if (wait_rev_cmd_ok == false) /*失败*/
        {
            if (++time_cnt >= TIMEOUT_300MS)
            {
                time_cnt = 0;
                rev_cmd_false_times++;

                uart_send_read_air_conditioner_state(); // 200ms 再次传输
            }
            if (rev_cmd_false_times >= FALSE_TIMES)
            {
                time_cnt = 0;
                uart_write_state = READ_AIR_CONDITI_TEMP; /*读取空调温度*/
            }
        }
        else
        {
            uart_write_state = READ_AIR_CONDITI_TEMP; /*读取空调温度*/
        }
        break;
        /******************************* 读取空调 温度值  *********************************************************/
    case READ_AIR_CONDITI_TEMP: /*send 读取当前--空调温度*/
        wait_rev_cmd_ok = false;
        rev_cmd_false_times = 0;

        if (++time_cnt >= TMIE_OUT_10S) // 10S 读取一次
        {
            time_cnt = 0;
            m_cur_read_cmd = READ_AIR_CONDITI_TEMP;

            uart_send_read_air_conditioner_temper();
            uart_write_state = WAIT_READ_AIR_CONDITI_TEMP;
        }
        break;
    case WAIT_READ_AIR_CONDITI_TEMP:
        if (wait_rev_cmd_ok == false) /*失败*/
        {
            if (++time_cnt >= TIMEOUT_300MS)
            {
                time_cnt = 0;
                rev_cmd_false_times++;
                uart_send_read_air_conditioner_temper(); // 200ms 再次传输
            }
            if (rev_cmd_false_times >= FALSE_TIMES)
            {
                time_cnt = 0;
                uart_write_state = READ_AIR_CONDITI_Measured_TEMP; /*读取空调测量温度值*/
            }
        }
        else
        {
            uart_write_state = READ_AIR_CONDITI_Measured_TEMP; /*读取空调测量温度值*/
        }
        break;
        /******************************* 读取空调测量温度值  *********************************************************/
    case READ_AIR_CONDITI_Measured_TEMP: /*send 读取当前--空调温度*/
        wait_rev_cmd_ok = false;
        rev_cmd_false_times = 0;

        if (++time_cnt >= TMIE_OUT_10S) // 10S 读取一次
        {
            time_cnt = 0;
            m_cur_read_cmd = READ_AIR_CONDITI_Measured_TEMP;

            uart_send_read_air_conditioner_Measured_temper();
            uart_write_state = WAIT_SET_AIR_CONDITI_Measured_TEMP;
        }
        break;
    case WAIT_SET_AIR_CONDITI_Measured_TEMP:
        if (wait_rev_cmd_ok == false) /*失败*/
        {
            if (++time_cnt >= TIMEOUT_300MS)
            {
                time_cnt = 0;
                rev_cmd_false_times++;
                uart_send_read_air_conditioner_Measured_temper(); // 200ms 再次传输
            }
            if (rev_cmd_false_times >= FALSE_TIMES)
            {
                time_cnt = 0;
                uart_write_state = READ_HUM_SENSOR; /*读取湿度传感器*/
            }
        }
        else
        {
            uart_write_state = READ_HUM_SENSOR; /*读取湿度传感器*/
        }
        break;
        /******************************* 读取湿度传感器功能部分  *********************************************************/
    case READ_HUM_SENSOR: /* send read temp sensor*/
        wait_rev_cmd_ok = false;
        rev_cmd_false_times = 0;

        if (++time_cnt >= TMIE_OUT_10S) // 10S 读取一次
        {
            time_cnt = 0;
            uart_send_read_sensor_type(0x01); // 读取湿度
            uart_write_state = WAIT_READ_HUM_SENSOR;
        }
        break;
    case WAIT_READ_HUM_SENSOR:
        if (wait_rev_cmd_ok == false) /*失败*/
        {
            if (++time_cnt >= TIMEOUT_300MS)
            {
                time_cnt = 0;
                rev_cmd_false_times++;

                uart_send_read_sensor_type(0x01); // 读取湿度
            }
            if (rev_cmd_false_times >= FALSE_TIMES)
            {
                time_cnt = 0;
                uart_write_state = READ_ALL_SENSOR; /*读取湿度传感器*/
            }
        }
        else
        {
            uart_write_state = READ_ALL_SENSOR; /*读取湿度传感器*/
        }
        break;
        /******************************* 读取全部传感传感器功能部分  *********************************************************/
    case READ_ALL_SENSOR: /* read temp sensor*/
        wait_rev_cmd_ok = false;
        rev_cmd_false_times = 0;

        if (++time_cnt >= TMIE_OUT_10S) // 10S 读取一次
        {
            time_cnt = 0;
            uart_send_read_sensor_type(0x0A); // 读取十个传感器数据
            uart_write_state = WAIT_READ_ALL_SENSOR;
        }
        break;
    case WAIT_READ_ALL_SENSOR:
        if (wait_rev_cmd_ok == false) /*失败*/
        {
            if (++time_cnt >= TIMEOUT_300MS)
            {
                time_cnt = 0;
                rev_cmd_false_times++;

                uart_send_read_sensor_type(0x0A); // 读取十个传感器数据
            }
            if (rev_cmd_false_times >= FALSE_TIMES)
            {
                time_cnt = 0;
                uart_write_state = READ_AIR_CONDITI_STATE; /*读取空调状态*/
            }
        }
        else
        {
            uart_write_state = READ_AIR_CONDITI_STATE; /*读取空调状态*/
        }
        break;
    default:
        break;
    }
}

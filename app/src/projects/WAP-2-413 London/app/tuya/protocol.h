/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020,
**
**
**
*********************************************************************************/
/**
 * @file    protocol.h
 * @author  shawn
 * @version v1.0.0
 * @date    2020.12.16
 * @brief
 *                       *******非常重要，一定要看哦！！！********
 *          1. 用户在此文件中实现数据下发/上报功能
 *          2. DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
 *          3. 当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/******************************************************************************
                           用户相关信息配置
******************************************************************************/
/******************************************************************************
                       1:修改产品信息
******************************************************************************/
#define PRODUCT_KEY "kgspawn36rtdwby6" // 开发平台创建产品后生成的16位字符产品唯一标识

#define MCU_VER "1.0.0" // 用户的软件版本,用于MCU固件升级,MCU升级版本需修改

/*  模块工作方式选择,只能三选一,推荐使用防误触模式  */
// #define CONFIG_MODE     CONFIG_MODE_DEFAULT             //默认工作模式
// #define CONFIG_MODE     CONFIG_MODE_LOWPOWER            //安全模式 (低功耗配网方式)
#define CONFIG_MODE CONFIG_MODE_SPECIAL // 防误触模式(特殊配网方式)

/*  模块是否开启保持长连接的低功耗模式，没有该字段低功耗模式默认关闭  */
// #define LONG_CONN_LOWPOWER        0         //关闭低功耗模式
// #define LONG_CONN_LOWPOWER        1         //打开低功耗模式

#define SUPPORT_MCU_FIRM_UPDATE

/******************************************************************************
                         3:定义收发缓存:
                    如当前使用MCU的RAM不够,可修改为24
******************************************************************************/
#ifndef SUPPORT_MCU_FIRM_UPDATE
#define BLUETOOTH_UART_RECV_BUF_LMT 512 // 串口数据接收缓存区大小,如MCU的RAM不够,可缩小
#define BLUETOOTH_DATA_PROCESS_LMT 384  // 串口数据处理缓存区大小,根据用户DP数据大小量定,必须大于24
#else
#define BLUETOOTH_UART_RECV_BUF_LMT 512 // 串口数据接收缓存区大小,如MCU的RAM不够,可缩小

// 请在此处选择合适的串口数据处理缓存大小（根据上面MCU固件升级包选择的大小和是否开启天气服务来选择开启多大的缓存）
#define BLUETOOTH_DATA_PROCESS_LMT 384  // 串口数据处理缓存大小,如需MCU固件升级,若单包大小选择256,则缓存必须大于260,若开启天气服务,则需要更大
// #define BLUETOOTH_DATA_PROCESS_LMT           600             //串口数据处理缓存大小,如需MCU固件升级,若单包大小选择512,则缓存必须大于520,若开启天气服务,则需要更大
// #define BLUETOOTH_DATA_PROCESS_LMT           1200            //串口数据处理缓存大小,如需MCU固件升级,若单包大小选择1024,则缓存必须大于1030,若开启天气服务,则需要更大

#endif

#define BLUETOOTHR_UART_SEND_BUF_LMT 512 // 根据用户DP数据大小量定,必须大于48
/******************************************************************************
                        4:定义模块工作方式
模块自处理:
          bluetooth指示灯和bluetooth复位按钮接在bluetooth模块上(开启BLUETOOTH_CONTROL_SELF_MODE宏)
          并正确定义WF_STATE_KEY和WF_RESET_KEY
MCU自处理:
          bluetooth指示灯和bluetooth复位按钮接在MCU上(关闭BLUETOOTH_CONTROL_SELF_MODE宏)
          MCU在需要处理复位bluetooth的地方调用mcu_api.c文件内的mcu_reset_bluetooth()函数,并可调用mcu_get_reset_bluetooth_flag()函数返回复位bluetooth结果
          或调用设置bluetooth模式mcu_api.c文件内的mcu_set_bluetooth_mode(BLUETOOTH_CONFIG_E mode)函数,并可调用mcu_get_bluetooth_work_state()函数返回设置bluetooth结果
******************************************************************************/
// #define         BLUETOOTH_CONTROL_SELF_MODE                       //bluetooth自处理按键及LED指示灯;如为MCU外界按键/LED指示灯请关闭该宏
#ifdef BLUETOOTH_CONTROL_SELF_MODE // 模块自处理
#define WF_STATE_KEY 14            // bluetooth模块状态指示按键，请根据实际GPIO管脚设置
#define WF_RESERT_KEY 0            // bluetooth模块重置按键，请根据实际GPIO管脚设置
#endif

/******************************************************************************
                      5:MCU是否需要支持校时功能
如需要请开启该宏,并在Protocol.c文件内mcu_write_rtctime实现代码
mcu_write_rtctime内部有#err提示,完成函数后请删除该#err
mcu在bluetooth模块正确联网后可调用mcu_get_system_time()函数发起校时功能
******************************************************************************/
// #define         SUPPORT_MCU_RTC_CHECK                //开启校时功能

/******************************************************************************
                      6:MCU是否需要支持bluetooth功能测试
如需要请开启该宏,并且mcu在需要bluetooth功能测试处调用mcu_api.c文件内mcu_start_bluetoothtest
并在protocol.c文件bluetooth_test_result函数内查看测试结果,
bluetooth_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
#define BLUETOOTH_TEST_ENABLE // 开启BLUETOOTH产测功能（扫描指定路由）

/******************************************************************************
                      8:是否开启BLUETOOTH模组心跳关闭功能
如需要请开启该宏,调用mcu_api.c文件内bluetooth_heart_stop函数即可停止心跳
******************************************************************************/
// #define         BLUETOOTH_HEARTSTOP_ENABLE           //开启心跳停止功能

/******************************************************************************
                      10:MCU是否需要支持bluetooth功能测试(连接指定路由)
如需要请开启该宏,并且mcu在需要bluetooth功能测试处调用mcu_api.c文件内mcu_start_connect_bluetoothtest
并在protocol.c文件bluetooth_connect_test_result函数内查看测试结果,
bluetooth_connect_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
// #define         BLUETOOTH_CONNECT_TEST_ENABLE                //开启BLUETOOTH产测功能（连接指定路由）

/******************************************************************************
                      11:MCU是否需要开启获取当前BLUETOOTH联网状态功能
如需要请开启该宏,并且mcu在需要获取当前BLUETOOTH联网状态处调用mcu_api.c文件内mcu_get_bluetooth_connect_status
并在protocol.c文件bluetooth_test_result函数内查看结果,
bluetooth_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
// #define         GET_BLUETOOTH_STATUS_ENABLE                  //开启获取当前BLUETOOTH联网状态功能

/******************************************************************************
                      12:MCU是否需要开启获取模块mac地址功能
如需要请开启该宏,并且mcu在需要获取模块mac地址处调用mcu_api.c文件内mcu_get_module_mac
并在protocol.c文件mcu_get_mac函数内查看结果,
mcu_get_mac内部有#err提示,完成函数后请删除该#err
******************************************************************************/
// #define         GET_MODULE_MAC_ENABLE                   //开启获取模块mac地址功能

/******************************************************************************
                      13:MCU是否需要支持获取格林时间功能
如需要请开启该宏,并且mcu在需要获取格林时间处调用mcu_api.c文件内mcu_get_green_time
并在protocol.c文件mcu_get_greentime函数内查看结果,
mcu_get_greentime内部有#err提示,完成函数后请删除该#err
mcu在bluetooth模块正确联网后可调用mcu_get_green_time()函数发起校时功能
******************************************************************************/
// #define         SUPPORT_GREEN_TIME                //开启格林时间功能

/******************************************************************************
                      14:MCU是否需要开启同步状态上报功能
1) 此命令为同步指令，MCU 数据状态上报后，需要等待模块返回结果；
2) 每次发送模组都会有响应，BLUETOOTH 模组未响应前不可多次上报；
3) 网络不好，数据难以及时上报时，模块会在 5 后返回失败，MCU 需要等待大于 5 秒。
******************************************************************************/
// #define         MCU_DP_UPLOAD_SYN                   //开启同步状态上报功能

/******************************************************************************
                      20:MCU是否需要支持蓝牙相关功能
如需要请开启该宏,并且mcu在需要蓝牙相关功能处调用mcu_api.c文件内mcu_start_BLE_test
并在protocol.c文件BLE_test_result函数内查看测试结果,
BLE_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
// #define         BLE_RELATED_FUNCTION_ENABLE            //开启蓝牙相关功能

/******************************************************************************
                        1:dp数据点序列号重新定义
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**
******************************************************************************/
#define DPID_CONNECT_STATE 10000

#define DPID_INT_AC_POWER_SWITCH 1
#define DPID_INT_AC_ONOFF_SWITCH 2
#define DPID_SELECT_ATA 3
#define DPID_BIBS1_AIR_SWITCH  4
#define DPID_BIBS1_O2_SWITCH  5
#define DPID_BIBS2_AIR_SWITCH  6
#define DPID_BIBS2_O2_SWITCH  7
#define DPID_DOOR_POWER_SWITCH 8
#define DPID_DOOR_DETECTION 9
#define DPID_VALUE_CHAMBER_SWITCH 10

#define DPID_TOP_LIGHT_SWITCH 20
#define DPID_BOTTOM_LIGHT_SWITCH 21
#define DPID_TOP_LIGHT_ADJUST   22
#define DPID_BOTTOM_LIGHT_ADJUST   23

#define DPID_I_SENSE_A  30
#define DPID_I_SENSE_B  31

#define DPID_AIR_CONDITION_TEMP_SET   42
#define DPID_AIR_CONDITION_WATER_TEMP  56
#define DPID_AIR_CONDITION_WARING_CODE  58
#define DPID_AIR_CONDITION_REMOET_SWITCH 59


#define DPID_FCV_EXIT_SET_VALUE  70
#define DPID_FCV_ENTRY_SET_VALUE 100

#define DPID_ACP1_STATE_1 137
#define DPID_ACP1_STATE_2 138
#define DPID_ACP1_REMOTE_CMD 139
#define DPID_ACP1_WARING_CODE 155
#define DPID_ACP1_RUN_STATE 156
#define DPID_ACP1_FAULT_CODE 158


#define DPID_ACP2_STATE_1 337
#define DPID_ACP2_STATE_2 338
#define DPID_ACP2_REMOTE_CMD 339
#define DPID_ACP2_WARING_CODE 355
#define DPID_ACP2_RUN_STATE 356
#define DPID_ACP2_FAULT_CODE 358

#define DPID_O2_GENERATOR_1_RUN_STATE     460
#define DPID_02_GENERATOR_1_RUN_PRESSURE  461
#define DPID_02_GENERATOR_1_OXYGEN_CONCENTRATION  462  //ÑõÅ¨¶È
#define DPID_02_GENERATOR_1_TEMPERATURE   463
#define DPID_O2_GENERATOR_1_ALARM_STATE   466
#define DPID_O2_GENERATOR_1_ACP_RUN_STATE 467
#define DPID_O2_GENERATOR_1_DRAIN_VALUE_STATE 468

#define DPID_O2_GENERATOR_2_RUN_STATE    480
#define DPID_02_GENERATOR_2_RUN_PRESSURE  481
#define DPID_02_GENERATOR_2_OXYGEN_CONCENTRATION  482  //ÑõÅ¨¶È	 
#define DPID_02_GENERATOR_2_TEMPERATURE   483
#define DPID_O2_GENERATOR_2_ALARM_STATE  486
#define DPID_O2_GENERATOR_2_ACP_RUN_STATE 487
#define DPID_O2_GENERATOR_2_DRAIN_VALUE_STATE 488

/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart_transmit_output(unsigned char value);

/**
 * @brief  系统所有dp点信息上传,实现APP和muc数据同步
 * @param  Null
 * @return Null
 * @note   MCU必须实现该函数内数据上报功能
 */
void all_data_update(void);

/**
 * @brief  dp下发处理函数
 * @param[in] {dpid} dpid 序号
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {length} dp数据长度
 * @return dp处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   该函数用户不能修改
 */
unsigned char dp_upload_handle(unsigned char dpid, const unsigned char value[], unsigned short length);
unsigned char dp_upload_handle_have_cb(unsigned short dpid, unsigned char dp_type,const unsigned char value[], unsigned short length);

/**
 * @brief  获取所有dp命令总和
 * @param[in] Null
 * @return 下发命令总和
 * @note   该函数用户不能修改
 */
unsigned char get_upload_cmd_total(void);

/**
 * @brief  发送蓝牙连接状态到MCU
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void send_bluetooth_status_to_mcu(void);

/**
 * @brief  发送心跳到MCU
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void send_heart_to_mcu(void);

/**
 * @brief  得到产品信息
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void get_product_info_cmd(void);

/**
 * @brief  状态查询
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void send_status_query(void);

/**
 * @brief  重置蓝牙返回命令
 * @param[in] NULL
 * @return Null
 * @note   Null
 */

void ack_bluetooth_reset_cmd(void);

#endif

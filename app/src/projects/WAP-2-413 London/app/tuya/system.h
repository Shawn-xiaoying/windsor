/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020,
**
**                             
**
*********************************************************************************/
/**
 * @file    system.h
 * @author  shawn
 * @version v1.0.0
 * @date    2024.7.4
 * @brief   串口数据处理，用户无需关心该文件实现内容
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#ifdef SYSTEM_GLOBAL
  #define SYSTEM_EXTERN
#else
  #define SYSTEM_EXTERN   extern
#endif

//=============================================================================
//Byte order of the frame
//=============================================================================
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1        
#define         PROTOCOL_VERSION                2
#define         FRAME_TYPE                      3
#define         LENGTH_HIGH                     4
#define         LENGTH_LOW                      5
#define         DATA_START                      6

//=============================================================================
//Data frame type
//=============================================================================
#define         HEAT_BEAT_CMD                   0                               //心跳包
#define         PRODUCT_INFO_CMD                1                               //产品信息
#define         WORK_MODE_CMD                   2                               //查询MCU 设定的模块工作模式	
#define         BLUETOOTH_STATE_CMD                  3                               //bluetooth工作状态	
#define         BLUETOOTH_RESET_CMD                  4                               //重置bluetooth
#define         BLUETOOTH_MODE_CMD                   5                               //选择smartconfig/AP模式	
#define         DATA_QUERT_CMD                  6                               //命令下发
#define         STATE_UPLOAD_CMD                7                               //状态上报	 
#define         STATE_QUERY_CMD                 8                               //状态查询   
#define         UPDATE_START_CMD                0x0a                            //升级开始
#define         UPDATE_TRANS_CMD                0x0b                            //升级传输
#define         GET_ONLINE_TIME_CMD             0x0c                            //获取系统时间(格林威治时间)
#define         FACTORY_MODE_CMD                0x0d                            //进入产测模式    
#define         BLUETOOTH_TEST_CMD                   0x0e                            //BLUETOOTH功能测试
#define         GET_LOCAL_TIME_CMD              0x1c                            //获取本地时间
#define         WEATHER_OPEN_CMD                0x20                            //打开天气          
#define         WEATHER_DATA_CMD                0x21                            //天气数据
#define         STATE_UPLOAD_SYN_CMD            0x22                            //状态上报（同步）
#define         STATE_UPLOAD_SYN_RECV_CMD       0x23                            //状态上报结果通知（同步）
#define         HEAT_BEAT_STOP                  0x25                            //关闭BLUETOOTH模组心跳
#define         STREAM_TRANS_CMD                0x28                            //流数据传输
#define         GET_BLUETOOTH_STATUS_CMD             0x2b                            //获取当前BLUETOOTH联网状态
#define         BLUETOOTH_CONNECT_TEST_CMD           0x2c                            //BLUETOOTH功能测试(连接指定路由)
#define         GET_MAC_CMD                     0x2d                            //获取模块mac
#define         GET_IR_STATUS_CMD               0x2e                            //红外状态通知
#define         IR_TX_RX_TEST_CMD               0x2f                            //红外进入收发产测
#define         MAPS_STREAM_TRANS_CMD           0x30                            //流数据传输(支持多张地图)
#define         FILE_DOWNLOAD_START_CMD         0x31                            //文件下载启动
#define         FILE_DOWNLOAD_TRANS_CMD         0x32                            //文件下载数据传输
#define         MODULE_EXTEND_FUN_CMD           0x34                            //模块拓展服务
#define         BLE_TEST_CMD                    0x35                            //蓝牙功能性测试（扫描指定蓝牙信标）
#define         GET_VOICE_STATE_CMD             0x60                            //获取语音状态码
#define         MIC_SILENCE_CMD                 0x61                            //MIC静音设置
#define         SET_SPEAKER_VOLUME_CMD          0x62                            //speaker音量设置
#define         VOICE_TEST_CMD                  0x63                            //语音模组音频产测
#define         VOICE_AWAKEN_TEST_CMD           0x64                            //语音模组唤醒产测
#define         VOICE_EXTEND_FUN_CMD            0x65                            //语音模组扩展功能


//=============================================================================
#define BLUETOOTH_RX_VER              0x03                                            //MCU发送帧协议版本号
#define BLUETOOTH_TX_VER              0x00                                            //蓝牙发送帧协议版本号(默认)
#define PROTOCOL_HEAD           0x07                                            //固定协议头长度
#define FRAME_FIRST             0x55                                            //帧头第一字节
#define FRAME_SECOND            0xaa                                            //帧头第二字节
//============================================================================= 
SYSTEM_EXTERN volatile unsigned char bluetooth_data_process_buf[PROTOCOL_HEAD + BLUETOOTH_DATA_PROCESS_LMT];     //串口数据处理缓存
SYSTEM_EXTERN volatile unsigned char bluetooth_uart_rx_buf[PROTOCOL_HEAD + BLUETOOTH_UART_RECV_BUF_LMT];         //串口接收缓存
SYSTEM_EXTERN volatile unsigned char bluetooth_uart_tx_buf[PROTOCOL_HEAD + BLUETOOTHR_UART_SEND_BUF_LMT];        //串口发送缓存
//
SYSTEM_EXTERN volatile unsigned char *rx_buf_in;
SYSTEM_EXTERN volatile unsigned char *rx_buf_out;

SYSTEM_EXTERN volatile unsigned char stop_update_flag;                                                 //ENABLE:停止一切数据上传  DISABLE:恢复一切数据上传

#ifndef BLUETOOTH_CONTROL_SELF_MODE
SYSTEM_EXTERN volatile unsigned char reset_bluetooth_flag;                                                  //重置bluetooth标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN volatile unsigned char set_bluetoothmode_flag;                                                //设置bluetooth工作模式标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN volatile unsigned char bluetooth_work_state;                                                  //bluetooth模块当前工作状态
#endif

#ifdef BLUETOOTH_STREAM_ENABLE
SYSTEM_EXTERN volatile unsigned char stream_status;                                                             //流服务发包返回状态
SYSTEM_EXTERN volatile unsigned char maps_stream_status;                                                        //多地图流服务发包返回状态
#endif



SYSTEM_EXTERN volatile unsigned char  mcu_heart_reset_state;   //0x00 第一次，0x01 其他  在上电初始化函数中赋值一个ff,用来表示没有心跳包，在主程序中要进行心跳逻辑处理

#define PRODUCT_INFO_LENGTH 0x2a
SYSTEM_EXTERN volatile unsigned char	product_info[PRODUCT_INFO_LENGTH+1] ;


typedef enum {
	CMD_STATUS_NONE,
	CMD_STATUS_START, //start sending  启动命令发生
	CMD_STATUS_SEND_IN, //send in      命令发生中
	CMD_STATUS_TIMEOUT, //response timeout  没有应答，也就是超时状态
	CMD_STATUS_ACK_CHECK_ERROR, // check error  应答数据校验错误
	CMD_STATUS_OK               //应答正确
	
	
} SEND_CMD_STATUS ;

/*
定义涂鸦协议命令数据结构
*/

#define CMD_DATA_LENGTH 13

typedef struct{
unsigned char cmd ;      //命令字
unsigned short dpID;      //功能ID号
union {
signed char  byte_data;
signed short short_data;
signed int   int_data;
unsigned char	 buffer[CMD_DATA_LENGTH] ;
	
}data;                   //数据，包含4种不同的数据类型
	
}cmd_data_description;

SYSTEM_EXTERN  volatile  SEND_CMD_STATUS send_cmd_state ;  //涂鸦协议命令发生状态机状态
SYSTEM_EXTERN  volatile  cmd_data_description current_cmd_data ;  //用来描述一条命令


//heart send status  涂鸦命令心率管理状态
typedef enum {
HEART_SEND_NONE,	
SEND_AFTER_POWER_ON, 
SEND_AFTER_MCU_RESPONDS, 
	
	
}HEART_SEND_STATUS;

SYSTEM_EXTERN  volatile HEART_SEND_STATUS  heart_send_status ;


//主动上传状态定义
typedef enum {
UPDATE_NONE,
START_UPDATE ,	
UPDATE_DATA_CHECK_ERROR,	
}UPDATE_STATUS ;

SYSTEM_EXTERN  volatile UPDATE_STATUS  upload_update_status ;

//启动功能点设置
typedef enum{
DPID_SET_STATE_NONE,//状态无
DPID_SET_PROCESSING,	//进行中
DPID_SET_ACK_TIMEOUT, //没有应答，超时
DPID_SET_ACK_DATA_ERROR,//数据包有错误
DPID_SET_ACK_OK,      //应答正确	
DPID_SET_ACK_SEND_WAIT //发送应答数据给app 的延时等待
}DPID_SET_STATUS; 

SYSTEM_EXTERN  volatile DPID_SET_STATUS dpid_setting_status ;

typedef unsigned char (*dp_upload_fun_handle_callbak)(unsigned char dpid, const unsigned char value[], unsigned short length);

/**
 * @brief  写bluetooth_uart字节
 * @param[in] {dest} 缓存区其实地址
 * @param[in] {byte} 写入字节值
 * @return 写入完成后的总长度
 */
unsigned short set_bluetooth_uart_byte(unsigned short dest, unsigned char byte);

/**
 * @brief  写bluetooth_uart_buffer
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {len} 数据长度
 * @return 写入结束的缓存地址
 */
unsigned short set_bluetooth_uart_buffer(unsigned short dest, const unsigned char *src, unsigned short len);

/**
 * @brief  计算校验和
 * @param[in] {pack} 数据源指针
 * @param[in] {pack_len} 计算校验和长度
 * @return 校验和
 */
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);

/**
 * @brief  向bluetooth串口发送一帧数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {fr_ver} 帧版本
 * @param[in] {len} 数据长度
 * @return Null
 */
void bluetooth_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len);


/**
 * @brief  数据帧处理
 * @param[in] {offset} 数据起始位
 * @return Null
 */
void data_handle(unsigned short offset);

/**
 * @brief  判断串口接收缓存中是否有数据
 * @param  Null
 * @return 是否有数据
 */
unsigned char with_data_rxbuff(void);

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
unsigned char take_byte_rxbuff(void);

void send_dpID_cmd(volatile cmd_data_description* c_d_desc);
#endif
  
  

/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020,
**
**
**
*********************************************************************************/
/**
 * @file    system.c
 * @author  shawn
 * @version v1.0.0
 * @date    2024.7.4
 * @brief   串口数据处理，用户无需关心该文件实现内容
 */

#define SYSTEM_GLOBAL

#include "mcu.h"

#define NRF_LOG_MODULE_NAME system_log
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

//#define SYSTEM_DEBUG_OUT

extern const DOWNLOAD_CMD_S upload_cmd[];

/**
 * @brief  写bluetooth_uart字节
 * @param[in] {dest} 缓存区其实地址
 * @param[in] {byte} 写入字节值
 * @return 写入完成后的总长度
 */
unsigned short set_bluetooth_uart_byte(unsigned short dest, unsigned char byte)
{
    unsigned char *obj = (unsigned char *)bluetooth_uart_tx_buf + DATA_START + dest;

    *obj = byte;
    dest += 1;

    return dest;
}

/**
 * @brief  写bluetooth_uart_buffer
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {len} 数据长度
 * @return 写入结束的缓存地址
 */
unsigned short set_bluetooth_uart_buffer(unsigned short dest, const unsigned char *src, unsigned short len)
{
    unsigned char *obj = (unsigned char *)bluetooth_uart_tx_buf + DATA_START + dest;

    my_memcpy(obj, src, len);

    dest += len;
    return dest;
}

/**
 * @brief  计算校验和
 * @param[in] {pack} 数据源指针
 * @param[in] {pack_len} 计算校验和长度
 * @return 校验和
 */
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
    unsigned short i;
    unsigned char check_sum = 0;

    for (i = 0; i < pack_len; i++)
    {
        check_sum += *pack++;
    }

    return check_sum;
}

/**
 * @brief  串口发送一段数据
 * @param[in] {in} 发送缓存指针
 * @param[in] {len} 数据发送长度
 * @return Null
 */
static void bluetooth_uart_write_data(unsigned char *in, unsigned short len)
{
    if ((NULL == in) || (0 == len))
    {
        return;
    }

    while (len--)
    {
        uart_transmit_output(*in);
        in++;
    }
}

/**
 * @brief  向bluetooth串口发送一帧数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {fr_ver} 帧版本
 * @param[in] {len} 数据长度
 * @return Null
 */
void bluetooth_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len)
{
    unsigned char check_sum = 0;

    bluetooth_uart_tx_buf[HEAD_FIRST] = 0x55;
    bluetooth_uart_tx_buf[HEAD_SECOND] = 0xaa;
    bluetooth_uart_tx_buf[PROTOCOL_VERSION] = fr_ver;
    bluetooth_uart_tx_buf[FRAME_TYPE] = fr_type;
    bluetooth_uart_tx_buf[LENGTH_HIGH] = len >> 8;
    bluetooth_uart_tx_buf[LENGTH_LOW] = len & 0xff;

    len += PROTOCOL_HEAD;
    check_sum = get_check_sum((unsigned char *)bluetooth_uart_tx_buf, len - 1);
    bluetooth_uart_tx_buf[len - 1] = check_sum;

#ifdef SYSTEM_DEBUG_OUT
    if (fr_type != 0x2B)
        NRF_LOG_HEXDUMP_INFO((unsigned char *)bluetooth_uart_tx_buf, len);
#endif

    bluetooth_uart_write_data((unsigned char *)bluetooth_uart_tx_buf, len);
}

#if 0
/**
 * @brief  心跳包检测
 * @param  Null
 * @return Null
 */
static void heat_beat_check(void)
{
    unsigned char length = 0;
    static unsigned char mcu_reset_state = FALSE;
    
    if(FALSE == mcu_reset_state) {
        length = set_bluetooth_uart_byte(length, FALSE);
        mcu_reset_state = TRUE;
    }else {
        length = set_bluetooth_uart_byte(length, TRUE);
    }
    
    bluetooth_uart_write_frame(HEAT_BEAT_CMD, MCU_TX_VER, length);
}

#endif

/**
 * @brief  mcu查询mcu和bluetooth的工作模式
 * @param  Null
 * @return Null
 */
static void get_mcu_bluetooth_mode(void) __attribute__((unused));
static void get_mcu_bluetooth_mode(void)
{
#if 0
    unsigned char length = 0;

#ifdef BLUETOOTH_CONTROL_SELF_MODE // 模块自处理
    length = set_bluetooth_uart_byte(length, WF_STATE_KEY);
    length = set_bluetooth_uart_byte(length, WF_RESERT_KEY);
#else                                                           
    //No need to process data
#endif
    
    bluetooth_uart_write_frame(WORK_MODE_CMD, MCU_TX_VER, length);
#endif
}

/**
 * @brief  获取制定DPID在数组中的序号
 * @param[in] {dpid} dpid
 * @return dp序号
 */
static unsigned char get_upload_dpid_index(unsigned char dpid)
{
    unsigned char index;
    unsigned char total = get_upload_cmd_total();

    //	  NRF_LOG_INFO("upload cmd total=%d",total);
    for (index = 0; index < total; index++)
    {
        if (upload_cmd[index].dp_id == dpid)
        {
            break;
        }
    }
    //    NRF_LOG_INFO("get upload cmd index=%d",index);
    return index;
}

/**
 * @brief  状态上报数据处理
 * @param[in] {value} 上报数据源指针
 * @return 返回数据处理结果
 */
static unsigned char data_point_handle(const unsigned char value[])
{
    unsigned char dp_id, index;
    unsigned char dp_type;
    unsigned char ret;
    unsigned short dp_len;

    dp_id = value[0];
    dp_type = value[1];
    dp_len = value[2] * 0x100;
    dp_len += value[3];

    index = get_upload_dpid_index(dp_id);

    if (dp_type != upload_cmd[index].dp_type)
    {
        // 错误提示
#ifdef SYSTEM_DEBUG_OUT
        NRF_LOG_INFO("non-existen upload cmd: id= %d  dp_type=%d", dp_id, dp_type);
#endif
        return FALSE;
    }
    else
    {
        ret = dp_upload_handle(dp_id, value + 4, dp_len);
    }

    return ret;
}

/**
 * @brief  数据帧处理
 * @param[in] {offset} 数据起始位
 * @return Null
 */
void data_handle(unsigned short offset)
{

    unsigned short dp_len;

    unsigned char ret;
    unsigned short i, total_len;
    unsigned char cmd_type = bluetooth_data_process_buf[offset + FRAME_TYPE];

    NRF_LOG_INFO("cmd_type=%d  current cmd=%d", cmd_type, current_cmd_data.cmd);
    if (current_cmd_data.cmd != DATA_QUERT_CMD && send_cmd_state != CMD_STATUS_NONE)
    {

        if ((current_cmd_data.cmd == cmd_type) || (current_cmd_data.cmd == STATE_QUERY_CMD && cmd_type == STATE_UPLOAD_CMD))
        {
            send_cmd_state = CMD_STATUS_OK;
            NRF_LOG_INFO("send_cmd_state=CMD_STATUS_OK Current File: %s\nLine: %d", __FILE__, __LINE__);
        }
    }

    switch (cmd_type)
    {

    case HEAT_BEAT_CMD: // 心跳包
        mcu_heart_reset_state = bluetooth_data_process_buf[offset + DATA_START];

        break;

    case GET_BLUETOOTH_STATUS_CMD:      // 得到蓝牙连接状态命令
        send_bluetooth_status_to_mcu(); // 发送连接状态测试
        break;

    case PRODUCT_INFO_CMD: // 产品信息

        total_len = (bluetooth_data_process_buf[offset + LENGTH_HIGH] << 8) | bluetooth_data_process_buf[offset + LENGTH_LOW];
        if (total_len <= PRODUCT_INFO_LENGTH)
            my_memcpy((void *)product_info, (void *)&bluetooth_data_process_buf[offset + DATA_START], total_len);
        else
            NRF_LOG_ERROR("The length of product information is greater than 0x2a");
        break;

    case STATE_UPLOAD_CMD: // MCU状态上报
        total_len = (bluetooth_data_process_buf[offset + LENGTH_HIGH] << 8) | bluetooth_data_process_buf[offset + LENGTH_LOW];

        for (i = 0; i < total_len;)
        {
            dp_len = bluetooth_data_process_buf[offset + DATA_START + i + 2] * 0x100;
            dp_len += bluetooth_data_process_buf[offset + DATA_START + i + 3];
            //
            ret = data_point_handle((unsigned char *)bluetooth_data_process_buf + offset + DATA_START + i);

            if (SUCCESS == ret)
            {
                // 成功提示
            }
            else
            {
                // 错误提示
            }

            i += (dp_len + 4);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief  判断串口接收缓存中是否有数据
 * @param  Null
 * @return 是否有数据
 */
unsigned char with_data_rxbuff(void)
{
    if (rx_buf_in != rx_buf_out)
        return 1;
    else
        return 0;
}

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
unsigned char take_byte_rxbuff(void)
{
    unsigned char value;

    if (rx_buf_out != rx_buf_in)
    {
        // 有数据
        if (rx_buf_out >= (unsigned char *)(bluetooth_uart_rx_buf + sizeof(bluetooth_uart_rx_buf)))
        {
            // 数据已经到末尾
            rx_buf_out = (unsigned char *)(bluetooth_uart_rx_buf);
        }

        value = *rx_buf_out++;
    }

    return value;
}

/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020
**
**
**
*********************************************************************************/
/**
 * @file    mcu_api.c
 * @author  shawn
 * @version v1.0.0
 * @date    2024.7.4
 * @brief   用户需要主动调用的函数都在该文件内
 */

#include "mcu.h"
#include "app_cfg_cmd.h"
#include "dpid_map_c.h"

#define NRF_LOG_MODULE_NAME mcuapi
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

// #define MCU_API_DEBUG_OUT

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#define MCU_API_GLOBAL

extern uint32_t ble_ctcs_s_send_handler(uint8_t *p_data, uint16_t* len, uint16_t uuid);

/**
 * @brief  hex转bcd
 * @param[in] {Value_H} 高字节
 * @param[in] {Value_L} 低字节
 * @return 转换完成后数据
 */
unsigned char hex_to_bcd(unsigned char Value_H, unsigned char Value_L)
{
    unsigned char bcd_value;

    if ((Value_H >= '0') && (Value_H <= '9'))
        Value_H -= '0';
    else if ((Value_H >= 'A') && (Value_H <= 'F'))
        Value_H = Value_H - 'A' + 10;
    else if ((Value_H >= 'a') && (Value_H <= 'f'))
        Value_H = Value_H - 'a' + 10;

    bcd_value = Value_H & 0x0f;

    bcd_value <<= 4;
    if ((Value_L >= '0') && (Value_L <= '9'))
        Value_L -= '0';
    else if ((Value_L >= 'A') && (Value_L <= 'F'))
        Value_L = Value_L - 'a' + 10;
    else if ((Value_L >= 'a') && (Value_L <= 'f'))
        Value_L = Value_L - 'a' + 10;

    bcd_value |= Value_L & 0x0f;

    return bcd_value;
}

/**
 * @brief  求字符串长度
 * @param[in] {str} 字符串地址
 * @return 数据长度
 */
unsigned long my_strlen(unsigned char *str)
{
    unsigned long len = 0;
    if (str == NULL)
    {
        return 0;
    }

    for (len = 0; *str++ != '\0';)
    {
        len++;
    }

    return len;
}

/**
 * @brief  把src所指内存区域的前count个字节设置成字符c
 * @param[out] {src} 待设置的内存首地址
 * @param[in] {ch} 设置的字符
 * @param[in] {count} 设置的内存长度
 * @return 待设置的内存首地址
 */
void *my_memset(void *src, unsigned char ch, unsigned short count)
{
    unsigned char *tmp = (unsigned char *)src;

    if (src == NULL)
    {
        return NULL;
    }

    while (count--)
    {
        *tmp++ = ch;
    }

    return src;
}

/**
 * @brief  内存拷贝
 * @param[out] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {count} 拷贝数据个数
 * @return 数据处理完后的源地址
 */
void *my_memcpy(void *dest, const void *src, unsigned short count)
{
    unsigned char *pdest = (unsigned char *)dest;
    const unsigned char *psrc = (const unsigned char *)src;
    unsigned short i;

    if (dest == NULL || src == NULL)
    {
        return NULL;
    }

    if ((pdest <= psrc) || (pdest > psrc + count))
    {
        for (i = 0; i < count; i++)
        {
            pdest[i] = psrc[i];
        }
    }
    else
    {
        for (i = count; i > 0; i--)
        {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}

/**
 * @brief  字符串拷贝
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @return 数据处理完后的源地址
 */
char *my_strcpy(char *dest, const char *src)
{
    if ((NULL == dest) || (NULL == src))
    {
        return NULL;
    }

    char *p = dest;
    while (*src != '\0')
    {
        *dest++ = *src++;
    }
    *dest = '\0';
    return p;
}

/**
 * @brief  字符串比较
 * @param[in] {s1} 字符串 1
 * @param[in] {s2} 字符串 2
 * @return 大小比较值
 * -         0:s1=s2
 * -         <0:s1<s2
 * -         >0:s1>s2
 */
int my_strcmp(char *s1, char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/**
 * @brief  将int类型拆分四个字节
 * @param[in] {number} 4字节原数据
 * @param[out] {value} 处理完成后4字节数据
 * @return Null
 */
void int_to_byte(unsigned long number, unsigned char value[4])
{
    value[0] = number >> 24;
    value[1] = number >> 16;
    value[2] = number >> 8;
    value[3] = number & 0xff;
}

/**
 * @brief  将4字节合并为1个32bit变量
 * @param[in] {value} 4字节数组
 * @return 合并完成后的32bit变量
 */
unsigned long byte_to_int(const unsigned char value[4])
{
    unsigned long nubmer = 0;

    nubmer = (unsigned long)value[0];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[1];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[2];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[3];

    return nubmer;
}

/**
 * @brief  raw型dp数据下发
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char send_dp_raw_to_mcu(unsigned char dpid, const unsigned char value[], unsigned short len)
{
    unsigned short send_len = 0;

    //
    send_len = set_bluetooth_uart_byte(send_len, dpid);
    send_len = set_bluetooth_uart_byte(send_len, DP_TYPE_RAW);
    //
    send_len = set_bluetooth_uart_byte(send_len, len / 0x100);
    send_len = set_bluetooth_uart_byte(send_len, len % 0x100);
    //
    send_len = set_bluetooth_uart_buffer(send_len, (unsigned char *)value, len);

    bluetooth_uart_write_frame(DATA_QUERT_CMD, BLUETOOTH_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  bool型dp数据下发
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char send_dp_bool_to_mcu(unsigned char dpid, unsigned char value)
{
    unsigned short send_len = 0;

    send_len = set_bluetooth_uart_byte(send_len, dpid);
    send_len = set_bluetooth_uart_byte(send_len, DP_TYPE_BOOL);
    //
    send_len = set_bluetooth_uart_byte(send_len, 0);
    send_len = set_bluetooth_uart_byte(send_len, 1);
    //
    if (value == FALSE)
    {
        send_len = set_bluetooth_uart_byte(send_len, FALSE);
    }
    else
    {
        send_len = set_bluetooth_uart_byte(send_len, 1);
    }

    bluetooth_uart_write_frame(DATA_QUERT_CMD, BLUETOOTH_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  value型dp数据下发
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char send_dp_value_to_mcu(unsigned char dpid, unsigned long value)
{
    unsigned short send_len = 0;

#ifdef MCU_API_DEBUG_OUT
    NRF_LOG_INFO("value_to_mcu value=%d", value);
#endif

    send_len = set_bluetooth_uart_byte(send_len, dpid);
    send_len = set_bluetooth_uart_byte(send_len, DP_TYPE_VALUE);
    //
    send_len = set_bluetooth_uart_byte(send_len, 0);
    send_len = set_bluetooth_uart_byte(send_len, 4);
    //
    send_len = set_bluetooth_uart_byte(send_len, value >> 24);
    send_len = set_bluetooth_uart_byte(send_len, value >> 16);
    send_len = set_bluetooth_uart_byte(send_len, value >> 8);
    send_len = set_bluetooth_uart_byte(send_len, value & 0xff);

    bluetooth_uart_write_frame(DATA_QUERT_CMD, BLUETOOTH_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  string型dp数据下发
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char send_dp_string_to_mcu(unsigned char dpid, const unsigned char value[], unsigned short len)
{
    unsigned short send_len = 0;

    //
    send_len = set_bluetooth_uart_byte(send_len, dpid);
    send_len = set_bluetooth_uart_byte(send_len, DP_TYPE_STRING);
    //
    send_len = set_bluetooth_uart_byte(send_len, len / 0x100);
    send_len = set_bluetooth_uart_byte(send_len, len % 0x100);
    //
    send_len = set_bluetooth_uart_buffer(send_len, (unsigned char *)value, len);

    bluetooth_uart_write_frame(DATA_QUERT_CMD, BLUETOOTH_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  enum型dp数据下发
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char send_dp_enum_to_mcu(unsigned char dpid, unsigned char value)
{
    unsigned short send_len = 0;

    send_len = set_bluetooth_uart_byte(send_len, dpid);
    send_len = set_bluetooth_uart_byte(send_len, DP_TYPE_ENUM);
    //
    send_len = set_bluetooth_uart_byte(send_len, 0);
    send_len = set_bluetooth_uart_byte(send_len, 1);
    //
    send_len = set_bluetooth_uart_byte(send_len, value);

    bluetooth_uart_write_frame(DATA_QUERT_CMD, BLUETOOTH_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  fault型dp数据下发
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char send_dp_fault_to_mcu(unsigned char dpid, unsigned long value)
{
    unsigned short send_len = 0;

    send_len = set_bluetooth_uart_byte(send_len, dpid);
    send_len = set_bluetooth_uart_byte(send_len, DP_TYPE_BITMAP);
    //
    send_len = set_bluetooth_uart_byte(send_len, 0);

    if ((value | 0xff) == 0xff)
    {
        send_len = set_bluetooth_uart_byte(send_len, 1);
        send_len = set_bluetooth_uart_byte(send_len, value);
    }
    else if ((value | 0xffff) == 0xffff)
    {
        send_len = set_bluetooth_uart_byte(send_len, 2);
        send_len = set_bluetooth_uart_byte(send_len, value >> 8);
        send_len = set_bluetooth_uart_byte(send_len, value & 0xff);
    }
    else
    {
        send_len = set_bluetooth_uart_byte(send_len, 4);
        send_len = set_bluetooth_uart_byte(send_len, value >> 24);
        send_len = set_bluetooth_uart_byte(send_len, value >> 16);
        send_len = set_bluetooth_uart_byte(send_len, value >> 8);
        send_len = set_bluetooth_uart_byte(send_len, value & 0xff);
    }

    bluetooth_uart_write_frame(DATA_QUERT_CMD, BLUETOOTH_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  bluetooth获取bool型上传dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned char bluetooth_get_dp_upload_bool(const unsigned char value[], unsigned short len)
{
    return (value[0]);
}

/**
 * @brief  bluetooth获取enum型上传dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned char bluetooth_get_dp_upload_enum(const unsigned char value[], unsigned short len)
{
    return (value[0]);
}

/**
 * @brief  bluetooth获取value型上传dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned long bluetooth_get_dp_upload_value(const unsigned char value[], unsigned short len)
{
    return (byte_to_int(value));
}

/**
 * @brief  raw型dp数据上传到app
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_raw_update(unsigned char dpid, const unsigned char value[], unsigned short len)
{

#if 0
    unsigned short send_len = 0;
    
    if(stop_update_flag == ENABLE)
        return SUCCESS;
    //
    send_len = set_wifi_uart_byte(send_len,dpid);
    send_len = set_wifi_uart_byte(send_len,DP_TYPE_RAW);
    //
    send_len = set_wifi_uart_byte(send_len,len / 0x100);
    send_len = set_wifi_uart_byte(send_len,len % 0x100);
    //
    send_len = set_wifi_uart_buffer(send_len,(unsigned char *)value,len);
    
    wifi_uart_write_frame(STATE_UPLOAD_CMD,MCU_TX_VER,send_len);

#endif

    return SUCCESS;
}

/**
 * @brief  bool型dp数据上传到app
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */

// #define DEBUG_DATA_TO_APP

unsigned char mcu_dp_bool_update(unsigned char dpid, unsigned char value)
{
#ifdef DEBUG_DATA_TO_APP
    char string[25] = {0};
    // itoa(value, string,10);
    sprintf(string, "%d", value);
    ble_ctcs_s_send_handler((uint8_t *)string, strlen(string), CTCS_TRANSPORT_UUID);
#endif
    return SUCCESS;
}

/**
 * @brief  value型dp数据上传到app
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_value_update(unsigned char dpid, unsigned long value)
{
#ifdef DEBUG_DATA_TO_APP
    char string[25] = {0};
    sprintf(string, "%d", value);

    ble_ctcs_s_send_handler((uint8_t *)string, strlen(string), CTCS_TRANSPORT_UUID);
#endif

    return SUCCESS;
}

/**
 * @brief  string型dp数据上传到app
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_string_update(unsigned char dpid, const unsigned char value[], unsigned short len)
{
#ifdef DEBUG_DATA_TO_APP
    ble_ctcs_s_send_handler((unsigned char *)value, len, CTCS_TRANSPORT_UUID);
#endif

    return SUCCESS;
}

/**
 * @brief  enum型dp数据上传到app
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_enum_update(unsigned char dpid, unsigned char value)
{
#ifdef DEBUG_DATA_TO_APP
    char string[25] = {0};
    sprintf(string, "%d", value);

    ble_ctcs_s_send_handler((uint8_t *)string, strlen(string), CTCS_TRANSPORT_UUID);
#endif

    return SUCCESS;
}

/**
 * @brief  fault型dp数据上传到app
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_fault_update(unsigned char dpid, unsigned long value)
{
#if 0
    unsigned short send_len = 0;
     
    if(stop_update_flag == ENABLE)
        return SUCCESS;
    
    send_len = set_wifi_uart_byte(send_len,dpid);
    send_len = set_wifi_uart_byte(send_len,DP_TYPE_BITMAP);
    //
    send_len = set_wifi_uart_byte(send_len,0);
    
    if((value | 0xff) == 0xff) {
        send_len = set_wifi_uart_byte(send_len,1);
        send_len = set_wifi_uart_byte(send_len,value);
    }else if((value | 0xffff) == 0xffff) {
        send_len = set_wifi_uart_byte(send_len,2);
        send_len = set_wifi_uart_byte(send_len,value >> 8);
        send_len = set_wifi_uart_byte(send_len,value & 0xff);
    }else {
        send_len = set_wifi_uart_byte(send_len,4);
        send_len = set_wifi_uart_byte(send_len,value >> 24);
        send_len = set_wifi_uart_byte(send_len,value >> 16);
        send_len = set_wifi_uart_byte(send_len,value >> 8);
        send_len = set_wifi_uart_byte(send_len,value & 0xff);
    }    
    
    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

#endif
    return SUCCESS;
}

/**
 * @brief  串口接收数据暂存处理
 * @param[in] {value} 串口收到的1字节数据
 * @return Null
 * @note   在MCU串口处理函数中调用该函数,并将接收到的数据作为参数传入
 */
void uart_receive_input(unsigned char value)
{
    // #error "请在串口接收中断中调用uart_receive_input(value),串口数据由MCU_SDK处理,用户请勿再另行处理,完成后删除该行"

    if (1 == rx_buf_out - rx_buf_in)
    {
#ifdef MCU_API_DEBUG_OUT
        NRF_LOG_INFO("rv buff  full _1");
#endif

        // 串口接收缓存已满
    }
    else if ((rx_buf_in > rx_buf_out) && ((rx_buf_in - rx_buf_out) >= sizeof(bluetooth_uart_rx_buf)))
    {
#ifdef MCU_API_DEBUG_OUT
        NRF_LOG_INFO("rv buff  full _2");
#endif

        // 串口接收缓存已满
    }
    else
    {
        // 串口接收缓存未满
        if (rx_buf_in >= (unsigned char *)(bluetooth_uart_rx_buf + sizeof(bluetooth_uart_rx_buf)))
        {
            rx_buf_in = (unsigned char *)(bluetooth_uart_rx_buf);
        }

        *rx_buf_in++ = value;
    }
}

/**
 * @brief  串口接收多个字节数据暂存处理
 * @param[in] {value} 串口要接收的数据的源地址
 * @param[in] {data_len} 串口要接收的数据的数据长度
 * @return Null
 * @note   如需要支持一次多字节缓存，可调用该函数
 */

void uart_receive_handler(unsigned char value[], unsigned short data_len)
{
    // #error "请在需要一次缓存多个字节串口数据处调用此函数,串口数据由MCU_SDK处理,用户请勿再另行处理,完成后删除该行"

    unsigned short i = 0;
#ifdef MCU_API_DEBUG_OUT
    NRF_LOG_HEXDUMP_INFO(value, data_len);
#endif

    for (i = 0; i < data_len; i++)
    {
        uart_receive_input(value[i]);
    }
}

/**
 * @brief  bluetooth串口数据处理服务
 * @param  Null
 * @return Null
 * @note   在MCU主函数while循环中调用该函数
 */
void bluetooth_uart_service(void)
{

    // #error "请直接在main函数的while(1){}中添加bluetooth_uart_service(),调用该函数不要加任何条件判断,完成后删除该行"
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;

    while ((rx_in < sizeof(bluetooth_data_process_buf)) && with_data_rxbuff() > 0)
    {
        bluetooth_data_process_buf[rx_in++] = take_byte_rxbuff();
    }

    if (rx_in < PROTOCOL_HEAD)
        return;

    while ((rx_in - offset) >= PROTOCOL_HEAD)
    {

        if (bluetooth_data_process_buf[offset + HEAD_FIRST] != FRAME_FIRST)
        {
            offset++;

            continue;
        }

        if (bluetooth_data_process_buf[offset + HEAD_SECOND] != FRAME_SECOND)
        {
            offset++;

            continue;
        }

        if (bluetooth_data_process_buf[offset + PROTOCOL_VERSION] != BLUETOOTH_RX_VER)
        {
            offset += 2;
            continue;
        }

        rx_value_len = bluetooth_data_process_buf[offset + LENGTH_HIGH] * 0x100;
        rx_value_len += (bluetooth_data_process_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);
        if (rx_value_len > sizeof(bluetooth_data_process_buf) + PROTOCOL_HEAD)
        {
            offset += 3;

            continue;
        }

        if ((rx_in - offset) < rx_value_len)
        {
            break;
        }

        // 数据接收完成
        if (get_check_sum((unsigned char *)bluetooth_data_process_buf + offset, rx_value_len - 1) != bluetooth_data_process_buf[offset + rx_value_len - 1])
        {
            // 校验出错
            // printf("crc error (crc:0x%X  but data:0x%X)\r\n",get_check_sum((unsigned char *)bluetooth_data_process_buf + offset,rx_value_len - 1),bluetooth_data_process_buf[offset + rx_value_len - 1]);
            offset += 3;

            if (send_cmd_state != CMD_STATUS_NONE) // 发送命令的情况下记录校验错误
                send_cmd_state = CMD_STATUS_ACK_CHECK_ERROR;
            else
                upload_update_status = UPDATE_DATA_CHECK_ERROR;
            continue;
        }

        data_handle(offset);
        offset += rx_value_len;
    } // end while

    if (send_cmd_state == CMD_STATUS_NONE)
    { // 判断是否是设备主动上传的数据
        if (Update_Feature_Id_list.update_feature_num != 0)
        {
            //    NRF_LOG_INFO("while update_feature_num=%d",Update_Feature_Id_list.update_feature_num);
            //   if(upload_update_status != UPDATE_DATA_CHECK_ERROR)
            upload_update_status = START_UPDATE;
            //	 else
            //		upload_update_status = UPDATE_NONE ;
        }
        else
        {
            upload_update_status = UPDATE_NONE;
        }
    }

    rx_in -= offset;
    if (rx_in > 0)
    {
        my_memcpy((char *)bluetooth_data_process_buf, (const char *)bluetooth_data_process_buf + offset, rx_in);
    }
}

/**
 * @brief  协议串口初始化函数
 * @param  Null
 * @return Null
 * @note   在MCU初始化代码中调用该函数
 */
void mcu_protocol_init(void)
{
    //  #error " 请在main函数中添加mcu_protocol_init()完成bluetooth协议初始化,并删除该行"
    rx_buf_in = (unsigned char *)bluetooth_uart_rx_buf;
    rx_buf_out = (unsigned char *)bluetooth_uart_rx_buf;

    stop_update_flag = DISABLE;
    send_cmd_state = CMD_STATUS_NONE;
    heart_send_status = SEND_AFTER_POWER_ON;
    mcu_heart_reset_state = 0xff;
    upload_update_status = UPDATE_NONE;
    dpid_setting_status = DPID_SET_STATE_NONE;

    feature_dpid_setValueAdr_map_init();

#ifndef BLUETOOTH_CONTROL_SELF_MODE
    bluetooth_work_state = BLUETOOTH_SATE_UNKNOW;
#endif
}

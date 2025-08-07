/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#ifndef MT_CST_PROTOCOL_H__
#define MT_CST_PROTOCOL_H__

#include <stdint.h>
#include "ble_ctcs.h"

/* clang-format off */
#define OPEN_ASE         0
#define DEF_PASSCODE      "12345678"            /* 默认密码 */

#define SYSTEM_TIME_UINT 100   //100ms 


#define READ_STATU_CMD    {0x5a,0x03,0x00,0x00} /* 读取主板当前工作状态 */
#define SET_POWER_CMD     {0x5a,0x04,0x01,0x00,0x00} /* 设置主板开关机 */
#define SET_TEMP          {0x5a,0x05,0x02,0x00,0x00,0x00} /* 设置桑拿房温度 */
#define SET_TIME          {0x5a,0x05,0x03,0x00,0x00,0x00} /* 设置桑拿房时间 */
#define SET_LIGHT         {0x5a,0x04,0x00,0x00,0x00} /* 设置桑拿房 led 开关 */

#define DEF_PRODUCT_INFO   0x01 /* 默认蓝牙生产信息 */
#define DEF_HARDWARE_INFO  0x0b /* 默认蓝牙硬件信息 */
#define DEF_SOFTWARE_INFO  0x17 /* 默认蓝牙软件信息 */







#define ROOM_INFO_DATA_LEN 19  /* 串口回复 room 信息数据长度 */
/* clang-format on */

/***************************************
 * @brief 设备基本信息
 */
typedef struct
{
    uint8_t ShortUserID[5];
    uint8_t ChipID[8];
    uint32_t m_system_clock;
    uint8_t mac_addr[6];

} DEVICE_INFOR_Pack;

extern DEVICE_INFOR_Pack device_infor;

/**
 * @brief 大小端转换器.
 */
#define BIG_LITTLE_ENDIAN_CONVERTER(ptr, len)    \
    do                                           \
    {                                            \
        uint8_t tmp;                             \
        uint8_t *pointer = (ptr);                \
        for (uint16_t i = 0; i < (len) / 2; ++i) \
        {                                        \
            tmp = pointer[i];                    \
            pointer[i] = pointer[(len) - 1 - i]; \
            pointer[(len) - 1 - i] = tmp;        \
        }                                        \
    } while (0)

/**
 * @brief 计算 crc
 *
 * @param[in] p_buf 指向存放要计算 crc 的地址.
 * @param[in] len   计算 crc 的数据长度.
 *
 * @return[out] 输出计算好的 crc 数据.
 */
uint8_t GetCRC8(uint8_t *p_buf, uint8_t len);

/**
 * @brief 接收服务事件处理函数
 *
 * @param[in] p_evt 指向服务事件.
 */
void ctcs_data_handler(ble_ctcs_evt_t *p_evt);

/**
 * @brief 用户协议层初始化
 */
void mt_user_protocol_init(void);

uint32_t get_system_time_tick(void);
uint32_t get_system_time_second(void);
#endif

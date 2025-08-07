/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ble_gap.h"
#include "app_util.h"
#include "app_error.h"
#include "comm_lib.h"
#include <math.h>
#include <string.h>
#include "nrf52840.h"

const uint8_t CRC_Table[256] =
    {
        0, 49, 98, 83, 196, 245, 166, 151, 185, 136, 219, 234, 125, 76, 31, 46, 67, 114, 33,
        16, 135, 182, 229, 212, 250, 203, 152, 169, 62, 15, 92, 109, 134, 183, 228, 213, 66, 115,
        32, 17, 63, 14, 93, 108, 251, 202, 153, 168, 197, 244, 167, 150, 1, 48, 99, 82, 124,
        77, 30, 47, 184, 137, 218, 235, 61, 12, 95, 110, 249, 200, 155, 170, 132, 181, 230, 215,
        64, 113, 34, 19, 126, 79, 28, 45, 186, 139, 216, 233, 199, 246, 165, 148, 3, 50, 97,
        80, 187, 138, 217, 232, 127, 78, 29, 44, 2, 51, 96, 81, 198, 247, 164, 149, 248, 201,
        154, 171, 60, 13, 94, 111, 65, 112, 35, 18, 133, 180, 231, 214, 122, 75, 24, 41, 190,
        143, 220, 237, 195, 242, 161, 144, 7, 54, 101, 84, 57, 8, 91, 106, 253, 204, 159, 174,
        128, 177, 226, 211, 68, 117, 38, 23, 252, 205, 158, 175, 56, 9, 90, 107, 69, 116, 39,
        22, 129, 176, 227, 210, 191, 142, 221, 236, 123, 74, 25, 40, 6, 55, 100, 85, 194, 243,
        160, 145, 71, 118, 37, 20, 131, 178, 225, 208, 254, 207, 156, 173, 58, 11, 88, 105, 4,
        53, 102, 87, 192, 241, 162, 147, 189, 140, 223, 238, 121, 72, 27, 42, 193, 240, 163, 146,
        5, 52, 103, 86, 120, 73, 26, 43, 188, 141, 222, 239, 130, 179, 224, 209, 70, 119, 36,
        21, 59, 10, 89, 104, 255, 206, 157, 172};

/********************************************
 * crc8 查表法
 *********************************/
uint8_t GetCRC8(uint8_t *p_buf, uint8_t len)
{
    uint8_t crc = 0;
    uint32_t i;

    for (i = 0; i < len - 1; i++)
    {
        crc = CRC_Table[p_buf[i] ^ crc];
    }
    return crc;
}

/***************************************
 * crc16 modbus算法
 *********************************/
#define PLOY 0xA001
uint16_t CalcCrc16(unsigned char *in_data, int size)
{
    unsigned short crc = 0xffff;
    //	unsigned int flag;
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned char data_t;

    if (in_data == NULL)
    {
        return 0;
    }

    for (j = 0; j < size; j++)
    {
        data_t = *in_data++;
        crc = (data_t ^ (crc));

        for (i = 0; i < 8; i++)
        {
            if ((crc & 0x1) == 1)
            {
                crc = (crc >> 1) ^ PLOY;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/***********************************************
 * 获得芯片id码
 *********************************/
void get_chip_device_id(uint8_t *pdata, uint8_t *rev_len)
{
    uint8_t buf[10];
    uint32_t device_id[2];
    uint8_t len = 0;

    device_id[0] = NRF_FICR->DEVICEID[0];
    device_id[1] = NRF_FICR->DEVICEID[1];

    buf[len++] = device_id[0] >> 24;
    buf[len++] = device_id[0] >> 16;
    buf[len++] = device_id[0] >> 8; //
    buf[len++] = device_id[0];
    buf[len++] = device_id[1] >> 24;
    buf[len++] = device_id[1] >> 16;
    buf[len++] = device_id[1] >> 8; //
    buf[len++] = device_id[1];

    memcpy(pdata, buf, len);

    *rev_len = len;
}

// Function definition: Celsius to Fahrenheit
int celsius_to_fahrenheit(double celsius)
{
    double fahrenheit = 0;
    fahrenheit = celsius * 9.0 / 5 + 32.0;
    return (int)round(fahrenheit);
}

int fahrenheit_to_celsius(double fahrenheit)
{
    double celsius = 0;
    celsius = (fahrenheit - 32.0) * 5.0 / 9.0;
    return (int)round(celsius);
}

#define JSON_KEY_T1 "\"t1\""
#define JSON_KEY_T2 "\"t2\""

int parse_value(const char *json_str, const char *key, int8_t *output)
{
    if (!json_str || !key || !output)
        return -1;

    const char *key_pos = strstr(json_str, key);
    if (!key_pos)
        return -2;

    char *colon = strchr(key_pos, ':');
    if (!colon)
        return -3;

    char *value_start = colon + 1;
    if (*value_start != '"')
        return -4;

    char *end_quote = strchr(value_start + 1, '"');
    if (!end_quote)
        return -5;

    char num_str[32];
    strncpy(num_str, value_start + 1, end_quote - (value_start + 1));
    num_str[end_quote - (value_start + 1)] = '\0';

    char *end_ptr;
    long val = strtol(num_str, &end_ptr, 10);
    if (*end_ptr != '\0')
        return -6;

    if (val < INT8_MIN || val > INT8_MAX)
        return -7;

    *output = (int8_t)val;
    return 0;
}

int extract_temperatures(const char *json_str, int8_t *t1, int8_t *t2)
{

    int ret;
    if (!json_str || !t1 || !t2)
    {
        return -1;
    }

    ret = parse_value(json_str, JSON_KEY_T1, t1);
    if (ret != 0)
    {
        return ret;
    }

    ret = parse_value(json_str, JSON_KEY_T2, t2);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

// 测试代码
int test_extract_temperatrues()
{

    const char *json = "{\"t1\":\"22\",\"t2\":\"-22\"}";
    int8_t t1, t2;

    int result = extract_temperatures(json, &t1, &t2);
    if (result == 0)
    {
        printf("t1=%d, t2=%d ", t1, t2);
    }
    else
    {
        printf("Error: %d ", result);
    }

    // 测试错误情况
    const char *invalid_json = "{\"t1\":\"abc\",\"t2\":\"xyz\"}";
    result = extract_temperatures(invalid_json, &t1, &t2);
    printf("Test2 Error: %d ", result); // 预期输出非0错误码

    return 0;
}

json_error_t json_extract_value(const char *json_str, const char *key, char *output, int output_size)
{
    // 参数有效性检查
    if (!json_str || !key || !output || output_size == 0)
        return JSON_INVALID_PARAM;

    // 查找第一个键的位置
    const char *key_start = strchr(json_str, JSON_QUOTE);
    if (!key_start)
        return JSON_SYNTAX_ERROR;
    key_start++; // 跳过起始引号

    const char *key_end = strchr(key_start, JSON_QUOTE);
    if (!key_end)
        return JSON_SYNTAX_ERROR;

    // 计算键长度
    size_t key_len = key_end - key_start;

    // 比较键名
    if (strncmp(key_start, key, key_len) != 0)
        return KEY_NOT_FOUND;

    // 定位值起始位置
    const char *value_start = key_end + 1;
    while (*value_start && *value_start != JSON_DELIM)
        value_start++;
    if (*value_start != JSON_DELIM)
        return JSON_SYNTAX_ERROR;
    value_start += 2; // 跳过冒号和空格

    // 定位值结束位置
    const char *value_end = strchr(value_start, JSON_QUOTE);
    if (!value_end)
        return JSON_SYNTAX_ERROR;

    // 计算值长度
    size_t value_len = value_end - value_start;

    // 检查缓冲区容量
    if (value_len >= output_size)
        return BUFFER_OVERFLOW;

    // 复制结果并确保终止符
    strncpy(output, value_start, value_len);
    output[value_len] = '\0';

    return JSON_PARSE_OK;
}

// 测试用例
int test_json()
{
    const char *json = "{\"device\":\"sensor123\",\"status\":\"active\"}";
    char buffer[64];

    json_error_t result = json_extract_value(json, "device", buffer, sizeof(buffer));
    if (result == JSON_PARSE_OK)
    {
        printf("Extracted value: %s ", buffer);
    }
    else
    {
        printf("Error code: %d ", result);
    }

    // 测试错误情况
    result = json_extract_value("{\"temp\":25}", "humidity", buffer, sizeof(buffer));
    printf("Test error: %d ", result); // 预期输出 KEY_NOT_FOUND

    return 0;
}

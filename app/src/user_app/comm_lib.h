/* Copyright (C)
   All rights reserved. */

#ifndef COMM_LIB_H__
#define COMM_LIB_H__

#include <stdint.h>

#define JSON_START '{'
#define JSON_END '}'
#define JSON_DELIM ':'
#define JSON_QUOTE '"'

typedef enum
{
    JSON_PARSE_OK = 0,
    JSON_INVALID_PARAM = -1,
    JSON_SYNTAX_ERROR = -2,
    KEY_NOT_FOUND = -3,
    BUFFER_OVERFLOW = -4
} json_error_t;

uint16_t CalcCrc16(uint8_t *in_data, int size);
void get_chip_device_id(uint8_t *pdata, uint8_t *rev_len);
uint8_t GetCRC8(uint8_t *p_buf, uint8_t len);
int celsius_to_fahrenheit(double celsius);
int fahrenheit_to_celsius(double fahrenheit);
int parse_value(const char *json_str, const char *key, int8_t *output);
json_error_t json_extract_value(const char *json_str, const char *key, char *output, int output_size);
#endif

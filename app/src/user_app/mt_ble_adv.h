/* Copyright (C) Shenzhen Minew Technologies Co., Ltd
   All rights reserved. */

#ifndef MT_BLE_ADV_H__
#define MT_BLE_ADV_H__

/*
Byte0
MSB--LSB
000----- Reserved
-------X Bluetooth - 1: connected, 0: not connected
------X- Power - 1: on, 0: off
---XXX-- Session status
   000   0: ready(not configured),
   001   1: configured(have a session, not started)
   010   2: preparation,
   011   3: running,
   100   4: stopped
	 101   5: free (ownership has not been taken,ownerID ==0)

Byte1 Temperature, unsigned, unit: 1 degree

Byte2 Pressure, unsigned, unit: kPa

Byte3 Reserved

*/

// typedef enum {
// SESSION_NO_CONFIGURED,
// SESSION_RUN_UP,
// SESSION_RUNNING_1,
// SESSION_FINISHED
//
//
//
// }session_status_t ;
typedef enum
{
   NOT_CONNECTED,
   CONNECTED,

} connect_status_t;

typedef enum
{
   POWER_OFF,
   POWER_ON,

} power_status_t;

typedef struct
{
   unsigned int session_status : 3;
   unsigned int power_status : 1;
   unsigned int connected_status : 1;
   unsigned int : 3;

} __attribute__((packed, aligned(1))) device_state_on_off_t;

typedef union
{
   device_state_on_off_t on_off_st_value;
   unsigned char on_off_byte_value;

} __attribute__((packed, aligned(1))) device_state_on_off_u;

typedef struct
{
   device_state_on_off_u on_off_state;
   unsigned char temperature;
   unsigned char pressure;
   unsigned char reserve;

} __attribute__((packed, aligned(1))) device_state_t;

typedef struct
{
   unsigned char length;
   unsigned char type;
   unsigned char value[2];

} service_uiid_t;

typedef struct
{
   unsigned char length;
   unsigned char type;
   unsigned char value[7];

} short_name_t;

typedef struct
{
   unsigned char length;
   unsigned char type;
   unsigned char service_uiid[2];
   unsigned char device_code[2];
   device_state_t device_state;
   unsigned char chip_id[8];

} service_data_t;

typedef struct
{
   service_uiid_t service_uiid;
   short_name_t short_name;
   service_data_t service_data;
} adv_data_t;

void ble_adv_start(void);
void ble_adv_stop(void);
void advertisting_init(void);
void update_adv_date(unsigned char data);
void update_adv_data_device_state_of_connect(unsigned char state);
void update_adv_data_device_state_of_power(unsigned char state);
void update_adv_data_device_state_of_session(unsigned char state);
void update_adv_data_device_state_of_temperature(unsigned char value);
void update_adv_data_device_state_of_pressure(unsigned char value);
void update_adv_data_device_code(uint16_t device_code);

#endif

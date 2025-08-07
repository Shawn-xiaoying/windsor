#ifndef _WAP_1_100_WINDSOR_MASTER_PROTOCOL_H_
#define _WAP_1_100_WINDSOR_MASTER_PROTOCOL_H_

#define WINDSOR_MAX_BUF_LENGTH 300
#define MAX_FEATURE_NUM 40
#define WINDSOR_PROTOCOL_APP_TO_DEVICE_HEAD 0xA5
#define WINDSOR_PROTOCOL_DEVOCE_TO_APP_HEAD 0x5A
#define DEVICE_CODE_FEATURE_ID 0xFF
enum
{
    SET_ERROR = 0,
    SET_OK,
    SET_DEVICE_RESET

};

/**
 *   App Command Type
 */
enum
{
    INFOR_REQUESET = 0x20,
    INFOR_RESPONSE = 0x21,

    TAKE_OWNERSHIP_REQUEST = 0x40,
    TAKE_OWNERSHIP_RESPONSE = 0x41,

    RELEASE_OWNERSHIP_REQUEST = 0x42,
    RELEASE_OWNERSHIP_RESPONSE = 0x43,

    RELINQUISH_OWNERSHIP_REQUEST = 0x44,
    RELINQUISH_OWNERSHIP_RESPONSE = 0x45,

    GET_REQUESET = 0x50,
    GET_RESPONSE = 0x51,

    SET_REQUESET = 0x52,
    SET_RESPONSE = 0x53,

    ACTIVE_UPLOAD_CMD = 0x55,

    SET_LOG_PARAMES = 0x56,
    SET_LOG_PARAMES_RESPONSE = 0x57,

    GET_FEATURES = 0x56,
    GET_FEATURES_RESPONSE = 0x57,

    GET_LOG_FEATURES = 0x58,
    GET_LOG_FEATURES_RESPONSE = 0x59,

    START_STOP_LOGGING_REQUEST = 0x5A,
    START_STOP_LOGGING_RESPONSE = 0x5B,

    SET_SESSION = 0x60,
    SET_SESSION_RESPONSE = 0x61,

    GET_SESSION = 0x62,
    GET_SESSION_RESPONSE = 0x63,

    CONTROL_SESSION = 0x64,
    CONTROL_SESSION_RESPONSE = 0x65,

    SESSION_STATUS = 0x66,
    SESSION_STATUS_RESPONSE = 0x67,

    SESSION_BROADCAST = 0x69,

    SESSION_LOG = 0x6B,

    GET_LOGS = 0x6C,
    GET_LOGS_RESPONSE = 0x6D,

    CLEAR_SESSION = 0x6E,
    CLEAR_SESSION_RESPONSE = 0x6F,

    JOIN_SESSION_REQUEST = 0x70,
    JOIN_SESSION_RESPONSE = 0x71,

    QUIT_SESSION_REQUEST = 0x72,
    QUIT_SESSION_RESPONSE = 0x73
};

typedef struct
{
    uint8_t Head;
    uint8_t cmd;
    uint8_t Attr_MsgId;
    uint8_t ShortUserID[5];
    uint8_t TimeStamp[4];
    uint16_t Payload_Size;

} __attribute__((packed, aligned(1))) header_packet_t;

typedef struct
{
    uint8_t Head;
    uint8_t cmd;
    uint8_t Attr_MsgId;
    uint8_t ChipID[8];
    uint8_t TimeStamp[4];
    uint8_t Payload_Size[2];
} __attribute__((packed, aligned(1))) response_header_packet_t;

/*********************************************************
 App Get Requeset Cmd Packet
 */
typedef struct
{
    header_packet_t header_packet;
    uint8_t Feature_ID[MAX_FEATURE_NUM];

} __attribute__((packed, aligned(1))) Get_Request_Pack_t;

/*********************************************************
 * @brief Set Request Cmd Packet
 */
typedef struct
{
    header_packet_t header_packet;
    uint8_t id_data[WINDSOR_MAX_BUF_LENGTH - sizeof(header_packet_t)];

} __attribute__((packed, aligned(1))) Set_Request_Pack_t;

typedef struct
{
    uint8_t update_feature_num;
    uint8_t feature_id[MAX_FEATURE_NUM];

} __attribute__((packed, aligned(1))) Feature_Id_List;

/*********************************************************
 * @brief 1 ????1 attribute ???
 */
typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    int8_t attribute;
} __attribute__((packed, aligned(1))) Valu_1Bytes_1Attribute_Pack;

/*********************************************************
 * @brief 2 ????1 attribute ???
 */
typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    int16_t attribute;
} __attribute__((packed, aligned(1))) Valu_2Bytes_1Attribute_Pack;

typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    uint8_t attribute1;
    uint8_t attribute2;
} __attribute__((packed, aligned(1))) Valu_2Bytes_2Attribute_Pack;


typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    uint32_t attribute;

} __attribute__((packed, aligned(1))) Valu_4Bytes_1Attribute_Pack;


/*********************************************************
 * @brief 3 ????2 attribute ???
 */
typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    uint16_t attribute1;
    uint16_t attribute2;
    uint16_t attribute3;
} __attribute__((packed, aligned(1))) Valu_6Bytes_3Attribute_Pack;

typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    uint8_t attribute[6];

} __attribute__((packed, aligned(1))) Valu_6Bytes_6Attribute_Pack;

typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    uint16_t attribute1;
    uint8_t attribute2[5];

} __attribute__((packed, aligned(1))) Valu_7Bytes_6Attribute_Pack;

typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    uint16_t attribute[6];

} __attribute__((packed, aligned(1))) Valu_12Bytes_6Attribute_Pack;

/*********************************************************
 * @brief 12 ????1 attribute ???
 */
typedef struct
{
    uint8_t Feature_ID;
    uint8_t packet_size;
    uint8_t attribute[12];

} __attribute__((packed, aligned(1))) Valu_12Bytes_1Attribute_Pack;

typedef struct
{

    uint8_t feature_id;
    uint8_t value_length;

} feature_value_length_map_t;

extern uint8_t allRequestID_Num;
extern uint8_t m_send_buf[WINDSOR_MAX_BUF_LENGTH];
extern uint8_t m_rev_buf[WINDSOR_MAX_BUF_LENGTH];

extern uint8_t Get_Request_id[MAX_FEATURE_NUM];

#endif

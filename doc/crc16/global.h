#ifndef __user_type_def
#define __user_type_def

#pragma pack(push,1)


/******************************************************************Basic Features definition *********************************************************/
typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, E.g. nBytes must equals to 4 here, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[1];        //1 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_1Byte_Value_Def;

typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[2];        //2 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_2Bytes_Value_Def;

typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[3];        //3 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_3Bytes_Value_Def;

typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[4];        //4 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_4Bytes_Value_Def;

typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[5];        //5 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_5Bytes_Value_Def;

typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[6];        //6 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_6Bytes_Value_Def;

typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[7];        //7 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_7Bytes_Value_Def;

typedef struct{
    unsigned char       FeatureID;              //Feature ID
    unsigned char       nBytes;                 //Number of bytes of the whole Feature structure, nBytes = sizeof(the-structure)
    unsigned char       FeatureValue[8];        //8 byte(s) feature value to Feature ID
    unsigned char       nextFeature;            //0 = the last feature, !0 = other feature followed
}FeatureID_Follow_8Bytes_Value_Def;




/******************************************************************APP Header structure definition*********************************************************/
typedef struct{
unsigned char        Head;                              // 1  byte, APP start with 0x5A 
unsigned char        CMD;                               // 1  byte, 0:Info request, 2 Get Request, 4 Set Request
unsigned int short   MsgId;                             // 2  bytes, Message ID: from 0~ 65535 
unsigned char        MAC[6];  		                    // 6  bytes, Smart phone Bluetooth MAC address
unsigned char        IMEI[16];                          // 16 bytes, Smart phone IMEI
unsigned int long    TimeStamp;                         // 4  bytes, Smart phone time stamp
}APP_HeaderBlock_Def;


/******************************************************************BLE Header structure definition*********************************************************/
typedef struct{
unsigned char        Head;                              // 1  byte, Master board start with 0xA5
unsigned char        CMD;                               // 1  byte, 1:Info Response, 3 Get Response, 5 Set Response
unsigned int short   MsgId;                             // 2  bytes, Message ID: from 0~ 65535 
unsigned char        MAC[6];  		                    // 6  bytes, nrf52840 MAC address
unsigned char        ChipID[8];                         // 8  bytes, 8 bytes of unique chip ID of nRF52840
unsigned int long    TimeStamp;                         // 4  bytes, nRF52840 time stamp
}BLE_HeaderBlock_Def;



/******************************************************************Set Request package definition*********************************************************/
typedef struct{
    APP_HeaderBlock_Def                   APP_Header;                    //APP_Header
    unsigned char                         Payload_Flag;                  //0 = no payload, !0 = follow with payload
    struct{
        FeatureID_Follow_1Byte_Value_Def  HBOT_Air_Compressor;
        FeatureID_Follow_1Byte_Value_Def  HBOT_Oxygen_Concentrator;
        FeatureID_Follow_1Byte_Value_Def  HBOT_Air_Conditioner;
    }Payload;
}Set_Request_Pack_Def;



/******************************************************************Get Request package definition*********************************************************/
typedef struct{
    APP_HeaderBlock_Def                     APP_Header;                   // APP_Header
    unsigned char                           nFeatures;                    // 1  bytes, number of features to be followed after
    struct{
        unsigned char                       FeatureID[5];                 // Feature IDs to query
    }Payload; 
}Get_Request_Pack_Def;



/******************************************************************Get Response package definition*********************************************************/
typedef struct{
    BLE_HeaderBlock_Def                     BLE_Header;                   // BLE_Header
    unsigned char                           Payload_Flag;                  //0 = no payload, !0 = follow with payload
    struct{
        FeatureID_Follow_1Byte_Value_Def    HBOT_Air_Compressor;
        FeatureID_Follow_1Byte_Value_Def    HBOT_Oxygen_Concentrator;
        FeatureID_Follow_1Byte_Value_Def    HBOT_Air_Conditioner;
        FeatureID_Follow_3Bytes_Value_Def   HBOT_Humidity;
        FeatureID_Follow_7Bytes_Value_Def   HBOT_PM;
    }Payload;
}Get_Response_Pack_Def;




/******************************************************************Info Request package definition*********************************************************/

typedef struct{
    APP_HeaderBlock_Def APP_Header;
    struct empty{
    }Payload;
}Info_Request_Pack_Def;




/******************************************************************Info Response package definition*********************************************************/

typedef struct{
    BLE_HeaderBlock_Def BLE_Header;
    struct{
        unsigned char        ProductID;                 // 2  bytes, 0=Sauna, 1=HBOT, other: to be defined in the future
        unsigned char        HardwareVersion;           // Board hardware revision
        unsigned char        FirmwareVersion;  		    // Version
        unsigned char        FirmwareRevision;  		// Revision
        unsigned char        FirmwareBuild;  		    // Build
    }Payload;
}Info_Response_Pack_Def;



/******************************************************************Set Response package definition*********************************************************/

typedef struct{
    BLE_HeaderBlock_Def      BLE_Header;                // BLE_Header
    struct empty1{
    }Payload;
}Set_Response_Pack_Def;


typedef enum{
	ON = 0,
	OFF = 1,
	BLINK = 2,
	FADING = 3
}LED_TYPE_DEF;

typedef struct{
	LED_TYPE_DEF Power;
	LED_TYPE_DEF AT;
	LED_TYPE_DEF Speed;
	LED_TYPE_DEF Sleep;
	LED_TYPE_DEF WiFi;
	LED_TYPE_DEF UV;
	LED_TYPE_DEF Lock;
	LED_TYPE_DEF Filter;
}LED_STRUCT_TYPE_DEF;

#pragma pack(pop)

#endif

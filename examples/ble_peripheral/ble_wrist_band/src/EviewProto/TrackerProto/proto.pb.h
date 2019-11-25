

#ifndef PROTO_PB_H__
#define PROTO_PB_H__

#include "epbProto.pb.h"

typedef enum {
    COMMAND_ID_DATA 		= 0x01,
    COMMAND_ID_CONFIG		= 0x02,
    COMMAND_ID_SERVICES		= 0x03,
    COMMAND_ID_SYS_CONTROL	= 0x04,
    COMMAND_ID_MEMORY_READER    = 0x05,
    COMMAND_ID_DEBUG            = 0x06,
    COMMAND_ID_BLE_BASE         = 0x07,



    COMMAND_ID_FIRMWARE 	= 0x7E,
    COMMAND_ID_NEGATIVE 	= 0x7F,
    COMMAND_ID_FACOTRY_TEST     = 0x80,
}epb_command_id_t;


/**
 * COMMAND_ID_DATA 
 */
typedef enum { //epb_key_of_data_t
               //epb_key_of_data_t
    DATA_KEY_IMEI = 0x01,
    DATA_KEY_ALARM_CODE =      0x02,
    DATA_KEY_USER_ID, 

    DATA_KEY_TX_COMPLETED =    0x10,
    DATA_KEY_HISTORICAL_DATA = 0x11, //请求多次的数据。
    DATA_SINGLE_LOCATION = 0x12, //只下行请求一次的数据
    DATA_CONTIMUE_LOCATION = 0x13,


    DATA_KEY_GPS_LOC = 0x20,
    DATA_KEY_GSM_LOC = 0x21,
    DATA_KEY_WIFI_LOC = 0x22,
    DATA_KEY_BLE_LOC = 0x23,

    DATA_KEY_GENERAL_DATA = 0x24,
    DATA_KEY_GSM_CALL_RECENTS = 0x25,
    DATA_KEY_ALERT_DATE = 0x26,

    DATA_KEY_STEP_DATA = 0x30,

    DATA_KEY_ACTIVITY_DATA = 0x31,

    DATA_KEY_GPS_LATEST_LOC,
    DATA_KEY_GENERAL_FLAG,


    DATA_KEY_GPS_ALERT_NOTIFY = 0x40,
    DATA_KEY_GSM_ALERT_NOTIFY,

    DATA_KEY_MILEAGE_SET = 0x50,



    DATA_GSENSOR_RAW_DATA = 0xAC,
}epb_key_of_data_t;

typedef enum { //epb_key_of_service_t
    SERV_KEY_IMEI = 0X01,

    //心跳包
    SERV_KEY_KEEP_ALIVE 	= 0x10,
    SERV_KEY_ADDR_PARSE, //传输地址
    SERV_KEY_GET_TIMESTARMP, //向服务器获取时间。


    SERV_KEY_CONN_SERVER = 0x31,
    SERV_KEY_DISTANCE_ALERT,

}epb_key_of_service_t;

typedef enum { //epb_key_of_sys_control_t

    SYS_KEY_RESET_RECORD	= 0x10,
    SYS_KEY_FACTORY_RECOVERY, //0x11
    SYS_KEY_DEVICE_RESET, //0x12
    SYS_KEY_FIND_ME,
    SYS_KEY_POWER_OFF,
    SYS_KEY_LED_BLANK,
    SYS_KEY_BLE_SCAN_MODE,


}epb_key_of_sys_control_t;


typedef enum { //epb_key_of_debug_t

    DEBUG_KEY_SAVE	= 0x10,

    DEBUG_KEY_SERVER_PARSE_ADDRESS,




    DEBUG_KEY_RECORD = 0X20,
    DEBUG_KEY_RECORD_RAM,
    DEBUG_KEY_STACK,
    DEBUG_KEY_HEAP,
    DEBUG_KEY_FLASH_LOG,
    DEBUG_KEY_FLASH_LOG_CONTROL,
    DEBUG_KEY_SMS,


    DEBUG_KEY_STATUS = 0x30,
    DEBUG_KEY_AGPS_WRITE,
    DEBUG_KEY_ALERT,
    DEBUG_KEY_SIMGPS,
    DEBUG_KEY_ONLY_AT,


    DEBUG_KEY_LOG_SWITCH = 0x50,
    DEBUG_KEY_LOG_GPS_RX,
    DEBUG_KEY_LOG_GPS_TX,
    DEBUG_KEY_LOG_GSM_RX,
    DEBUG_KEY_LOG_GSM_TX,
    DEBUG_KEY_LOG_DEBUG,
    DEBUG_KEY_LOG_SWITCH_DEBUG,


    DEBUG_KEY_DOWN_SIM800_START = 0x60,
    DEBUG_KEY_DOWN_SIM800_FILE,
    DEBUG_KEY_DOWN_SIM800_END,

    DEBUG_KEY_FS_OPEN = 0X70,
    DEBUG_KEY_FS_CLOSE,
    DEBUG_KEY_FS_WRITE,
    DEBUG_KEY_FS_READ,
    DEBUG_KEY_FS_FILE_STAT,
    DEBUG_KEY_FS_SEEK,
    DEBUG_KEY_FS_REMOVE,
    DEBUG_KEY_FS_SIZE,
    DEBUG_KEY_FS_DIR_READ,
    DEBUG_KEY_FS_MK_DIR,
    DEBUG_KEY_FS_TRAVERSE,
}epb_key_of_debug_t;


typedef enum {

    MEMO_KEY_INNER_RAM = 0x10,
    MEMO_KEY_EXTERNAL_FLASH,
    MEMO_KEY_MEMS,
}epb_key_of_memory_reader_t;

typedef enum {

    LOG_KEY_CONTROL = 0x10,
    LOG_KEY_INFO,
}epb_key_of_log_t;



typedef enum { //epb_key_of_factory_test_t

    FACTORY_KEY_BASE = 0,
    FACTORY_KEY_START_TEST,
    FACTORY_KEY_STOP_TEST,

    FACTORY_KEY_GPS_SNR,
    FACTORY_KEY_GSM_ECHO,
    FACTORY_KEY_GSM_CSQ,
    FACTORY_KEY_MEMS_ORIENT,  //return MEMS orient
    FACTORY_KEY_FLASH_IDENTIFIER, //return FLASH identifier


    //以上的协议是旧的，暂时不作删除


    FACTORY_KEY_OPEN_GPS = 0x20,
    FACTORY_KEY_CLOSE_GPS,
    FACTORY_KEY_GET_GPS_UART, //获取GPS是否有数据
    FACTORY_KEY_TEST_LED_MOTO, //LED与马达的测试
    FACTORY_KEY_GET_GSM_INFO, //IMEI,ICCID,CSQ

    FACTORY_KEY_SET_GSM_CSQ, //发送查询CSQ命令。
    FACTORY_KEY_TEST_GSM_MIC, //MIC 与 speeker测试
    FACTORY_KEY_GET_MEMS_ID, //LIS3DH ID
    FACTORY_KEY_GET_FLASH_ID, //获取FLASH ID
    FACTORY_KEY_GET_BAT_VALUE, //获取BATTERY 的电压，以及充电IO

    FACTORY_KEY_FIRST_SET, //相当于进入测试模式，把要发的数据一起打包发上来。
    FACTORY_KEY_GET_BLE_MAC, //获取BLE MAC
    FACTORY_KEY_GET_WIFI_LOC, //获取BLE MAC



    FACTORY_KEY_DOWN_MP3 = 0X40, //相当于进入测试模式，把要发的数据一起打包发上来。


}epb_key_of_factory_test_t;

typedef enum { //epb_key_of_sys_control_t

    //底座类型
    BLE_BASE_BASE_TYPE	= 0x10,
    BLE_BASE_MUSIC_PLAY,
    BLE_BASE_MUSIC_CTRL,
    BLE_BASE_DEV_TYPE, //设备类型
    
    //手环使用key
    BLE_WB_SOS_KEY_ALARM = 0X30, //chen
    BLE_WB_READ_INFO_DATA,
    BLE_WB_READ_SPORT_DATA,
    BLE_WB_UPDATE_UTC_SECONDS,
    BLE_WB_READ_FW_INFO,
    
}epb_key_of_ble_base_t;
typedef struct {
    uint16_t  size;
    uint8_t *p_data;
    msg_packet_t head;
}PacketInfo_t;





#endif

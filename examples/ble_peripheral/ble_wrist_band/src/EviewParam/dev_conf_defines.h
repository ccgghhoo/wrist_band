


#ifndef DEV_CONF_DEFINES_H__
#define DEV_CONF_DEFINES_H__


#include <stdint.h>
#include "halParam.h"




/**
 */


#define CONFIG_MAGIC_CODE 	0xDEF1C0DE // device config code

#define DEV_SETTING_VERSION 5


#define DEV_IMEI_DATA_SIZE 	15
#define DEV_ICCID_DATA_SIZE 20

#define MODULE_NUMBER_SIZE		4
#define FIRMWARE_VERSION_SIZE	4







#define ALERT_GEO_NUMBERS				4


#define DEV_IMEI_DATA_SIZE 	15
#define DEV_IMSI_DATA_SIZE 	20
#define DEV_ICCID_DATA_SIZE 20
#define DEV_GSM_MODULE_MAX_SIZE  20
#define MODULE_NUMBER_SIZE		4
#define FIRMWARE_VERSION_SIZE	4



#define SYS_GPRS_GPRS_USER_ID_MAX 32

#define CLOCKS_NUM  4

#define CLOCKS_ENABLE       0x80
#define CLOCKS_NUM_MASK     0x7F


#define DEV_CONFIG_DATA_SIZE_MAX		sizeof(dev_config_t) + ( 25 * 2 )
#define DEV_CONFIG_DATA_LEN				(sizeof(dev_config_t) - 12 )


typedef enum {
    BUTTONS_TRIGGER_MODE_LONG = 0,
    BUTTONS_TRIGGER_MODE_DOUBLE_CLICK = 1,

}BUTTONS_TRIGGER_MODE_EN;


typedef enum {
    BUTTONS_OP_DO_NOTHINGS = 0,


    BUTTONS_OP_DIAL_1ST_NUMBER = 1,
    BUTTONS_OP_DIAL_2ND_NUMBER,
    BUTTONS_OP_DIAL_3RD_NUMBER,
    BUTTONS_OP_DIAL_4TH_NUMBER,
    BUTTONS_OP_DIAL_5TH_NUMBER,
    BUTTONS_OP_DIAL_6TH_NUMBER,
    BUTTONS_OP_DIAL_7TH_NUMBER,
    BUTTONS_OP_DIAL_8TH_NUMBER,
    BUTTONS_OP_DIAL_9TH_NUMBER,
    BUTTONS_OP_DIAL_10TH_NUMBER,

    BUTTONS_OP_SOS_ALERT = 15,
    BUTTONS_OP_POWER_OFF = 0xFE,
}BUTTONS_OP_CODE;

typedef enum {
    BUTTONS_EVT_CLICK = 0,
    BUTTONS_EVT_DOUBLE_CLICK,
    BUTTONS_EVT_SHORT_HOLD,
    BUTTONS_EVT_LONG_HOLD,
    BUTTONS_EVT_END,
}ButtonsEvt_en;

typedef enum {
    BUTTONS_IDX_SOS = 0,
    BUTTONS_IDX_KEY_L,
    BUTTONS_IDX_KEY_R,
}ButtonsIdex_en;

#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language=extended
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

//=================================================
//

typedef struct {
    char imei[DEV_IMEI_DATA_SIZE+1];
    char imsi[DEV_IMSI_DATA_SIZE+1];
    char iccid[DEV_ICCID_DATA_SIZE+1];
    char gsm_modlue[DEV_GSM_MODULE_MAX_SIZE+1];
    uint32_t module;
}dev_property_t;

typedef struct {
    union {
        struct {
            uint8_t index:7;
            uint8_t enable:1;
        };
        uint8_t flag;
    };
    uint8_t 	hour;
    uint8_t 	minute;
    uint8_t     weekday;

    uint8_t     ring_time;   // active time
    uint8_t     ring_type;
    uint8_t     reserved[2];
}alarm_clock_t; //8B

//=================================================
// Button
typedef struct { //button_sense_t
    uint16_t 	click;
    uint16_t 	double_click;
    // uint16_t 	short_hold;
    uint16_t 	long_hold;
}button_sense_t;
typedef struct { //button_action_t
    uint8_t     task[8][4];
}button_action_t;


#define BUTTON_FB_NONE 0
#define BUTTON_FB_MOTO 1
#define BUTTON_FB_VOICE 2
#define BUTTON_FB_MOTO_VOICE 3


#define BUTTON_MODE_LONGPRESS 0
#define BUTTON_MODE_DOUBLECLICK 1
typedef struct {
    union {
        struct {
            uint32_t feedback:2; //00:no feedback 01:moto 10:voice 11moto and voice
            uint32_t time:7; // unit 0.1s
            uint32_t task:4;
            uint32_t triggerMode:1;
            uint32_t reserved14:1;
            uint32_t enable:1;
        };
        uint16_t value;
    };
    uint16_t reserved16;
}button_operation_t;




typedef struct { //button_settings_t
    button_operation_t sos;
    button_operation_t call_1;
    button_operation_t call_2;
}button_settings_t;

//=================================================
// Settings
typedef union { //sys_mode_t
    struct {
        /**
         * Mode = 1/2/3, value is not used 
         * Mode = 4, value is GSM/GPS work interval 
         * Mode = 5, value is GSM/GPS work time
         * 
         * @author hzhmcu (2018/5/7)
         */
        uint32_t	time:	24;

        uint32_t 	mode:	8;
    };
    uint32_t value;
}sys_mode_t;


typedef union {
    struct {
        uint8_t start_hour;
        uint8_t start_minute;
        uint8_t end_hour;
        uint8_t end_minute;
    };
    struct
    {
        uint16_t start_hm;
        uint16_t stop_hm;
    };
    uint32_t value;
}sys_work_time_t;
typedef struct {
    union
    {
        uint8_t flag;
        struct
        {
            uint8_t index:7;
            uint8_t enable:1;
        };
    };
    union
    {
        struct
        {
            uint8_t start_minute;
            uint8_t start_hour;
            uint8_t end_minute;
            uint8_t end_hour;
        };
        struct
        {
            uint16_t start_hm;
            uint16_t stop_hm;
        };

        //这是大端模式，不可以直接赋值。
        // uint32_t value;
    };
    uint8_t reserve[3];
}system_no_disturb_t; //8


typedef union { //sys_time_zone_t
    int8_t 	offset;
    uint32_t value;
}sys_time_zone_t;
typedef union { //sys_password_t
    struct {
        uint32_t 	number:	31;
        uint32_t 	enable:	1;
    };
    uint32_t value;
}sys_password_t;
typedef union {
    struct { //sys_switchs_t
        uint32_t leds:1;
        uint32_t buzzer:1;
        uint32_t motor:1;
        uint32_t GSMLoc:1;
        uint32_t WifiLoc:1;
        uint32_t SosSpeaker:1;
        uint32_t XSpeaker:1;
        uint32_t BLEOftenConnection:1;
        uint32_t BLELoc:1;
        uint32_t reserved: 21;
        uint32_t autoUpdate:1; //是否允许自动更新FW
        uint32_t AGPS:	1;

    };
    uint32_t value;
}sys_switchs_t;
typedef union {
    struct { //sys_volume_t
        uint8_t 	ringtone;
        uint8_t 	mic;
        uint8_t 	speaker;
    };
    uint32_t value;
}sys_volume_t;

typedef struct {
    uint32_t value;
}sys_musicswitch_t;

#define URL_MAX 40
typedef struct {
    char url[URL_MAX];
}sys_gps_url_t;

typedef struct {
    char url[URL_MAX];
}sys_wifi_lsb_url_t;

#define BLE_LOC_DES 32
typedef struct {

    int32_t latitude;
    int32_t longitude;
    char name[BLE_LOC_DES];
}sys_ble_loc_t;
#define WHITE_LIST_MAX 10

typedef struct {
    union {
        struct {
            uint8_t index:7;
            uint8_t enable:1;
        };
        uint8_t flag;
    };
    uint8_t mac[6];
}sys_ble_whitelist_t;



typedef struct { //sys_settings_t
    uint32_t initMileage;
    sys_mode_t	mode;
    alarm_clock_t alarms[4];
    system_no_disturb_t no_disturb;
    sys_password_t password;
    sys_time_zone_t zone;
    sys_switchs_t switches;
    sys_volume_t volume;
    sys_musicswitch_t musicswitch;
    sys_gps_url_t gps_url;
    sys_wifi_lsb_url_t wifi_lsb_url;
    sys_ble_whitelist_t  whitelist[WHITE_LIST_MAX];

}sys_settings_t;

//=================================================
typedef struct { //phone_contact_t
    union {
        struct {
            uint8_t idx:	4;
            uint8_t no_card:1;
            uint8_t	phone:	1;
            uint8_t sms:	1;
            uint8_t enable:	1;
        };
        uint8_t 	value;
    };

    char 	number[19];
}phone_contact_t;


#define PREFIX_MAX  100


typedef struct { //phone_sms_prefix_t
    union {
        struct {
            uint8_t rvs:7;
            uint8_t enable:1;
        };
        uint8_t flag;
    };
    uint8_t 	size;
    char 	text[PREFIX_MAX];
}phone_sms_prefix_t;
typedef union { //phone_sos_t
    struct {
        uint32_t holding_time: 16;
        uint32_t rings_time: 8;
        uint32_t loops: 8;
    };
    uint32_t value;
}phone_sos_t;

typedef union {
    struct { //phone_swithes_t
        uint32_t 	auto_answer_rings_delay: 7; //多少次响铃后自动应答。
        uint32_t 	auto_answer_enable: 1; //enable

        uint32_t 	call_set_only_by_auth_number:	1; //only authorized number 白名单
        uint32_t 	hangup_manual_not_allow: 1; //允许挂断
        uint32_t 	monitor_environment_enable: 1; //自动接听，没的speak。
        uint32_t 	sms_set_only_by_auth_number: 1; //短信白名单，SMS设置命令。

        uint32_t 	reserved: 20;
    };
    uint32_t value;
}phone_swithes_t;
typedef struct { //phone_settings_t
    phone_contact_t	contacts[10];
    phone_sms_prefix_t sms_prefix;
    phone_sos_t SOS;
    phone_swithes_t	switches;
}phone_settings_t;
//=================================================
// GPRS
typedef struct { //gprs_apn_t
    struct {
        uint32_t flag:31;
        uint32_t enable:1;
    };
    uint8_t apnLen;
    char apns[31];
    uint8_t userNameLen;
    char username[15];
    uint8_t passwordLen;
    char password[15];

}gprs_apn_t;
typedef struct { //gprs_server_t
    union {
        struct {
            uint8_t isUDP:1;
            uint8_t rsv:6;
            uint8_t enable:1;
        };
        uint8_t flag;
    };
    uint16_t port;
    char ip[32];
    uint8_t reserved;
}gprs_server_t;
typedef struct { //gprs_time_t
    union {
        struct {
            uint32_t heartbeat:31;
            uint32_t hb_enable:1;
        };
        uint32_t heartbeatValue;
    };
    uint32_t upload;
    uint32_t lazy_upload;
}gprs_time_t;

typedef struct
{
    uint16_t interval;
    uint16_t time;
}gprs_continue_loc_t;



typedef struct
{
    uint8_t len;
    uint8_t data[SYS_GPRS_GPRS_USER_ID_MAX];
}gprs_user_id_t;

typedef struct { //gprs_settings_t
    gprs_apn_t apn;
    gprs_server_t server;
    gprs_time_t interval;
    gprs_continue_loc_t continue_loc;
    gprs_user_id_t userid;
}gprs_settings_t;
//=================================================
typedef union { //alert_power_t
    struct {
        uint32_t threshold:8;
        uint32_t low_voice_threshold:8;
        uint32_t RVS:12;
        uint32_t low_voice:1;
        uint32_t low:1;
        uint32_t on:1;
        uint32_t off:1;
    };
    uint32_t value;
}alert_power_t;


/* 
   Index:0-3,最多可以设置4个GEO，
Type=0  points只能为1，第一个经纬度点为圆心,radius为半径，100-60000m。
Type=1  points>=3 && <=8，经纬度为多边形顶点，radius无效。

 */
typedef struct { //alert_geo_t
    union {
        struct {
            uint32_t idx:4;
            uint32_t count:4; // count of data long&lat

            uint32_t enable:1;
            uint32_t dir: 1;
            uint32_t type:1;  // 0 use 圆radius , 1 use fence
            uint32_t reserved: 5;

            uint32_t radius:16;
        };
        uint32_t value;
    };
    union
    {
        int32_t data[16];   // 8 lines at most
        struct
        {
            int32_t latitude; //*1000 0000
            int32_t longitude; //*1000 0000
        }loc[8];
    };
}alert_geo_t;
typedef union { //alert_motion_t
    struct {
        uint32_t setup_time: 16; //static time to setup alertion
        uint32_t action_time: 14; // motion time to generate alert
        uint32_t dial:1; //是否打电话
        uint32_t enable:1;
    };
    uint32_t value;
}alert_motion_t;
typedef union { //alert_static_t
    struct {
        uint32_t threshold: 30;
        uint32_t dial:1; //是否打电话
        uint32_t enable: 1;
    };
    uint32_t value;
}alert_static_t;


typedef union { //alert_speed_t
    struct {
        //单位是KM/H 以前是M/H
        uint32_t threshold:15;
        uint32_t enable: 1;
    };
    uint32_t value;
}alert_speed_t;

typedef union { //alert_tilt_t
    struct {
        uint32_t time: 16;
        uint32_t angle: 8;
        uint32_t reserved:6;
        uint32_t dial:1; //是否打电话
        uint32_t enable: 1;
    };
    uint32_t value;
}alert_tilt_t;

typedef union { //alert_falldown_t
    struct {
        uint8_t level: 4;
        uint8_t reserve:2;
        uint8_t dial:1;
        uint8_t enable:1;
    };
    uint8_t value;
}alert_falldown_t;


typedef struct {
    alert_power_t power;
    alert_geo_t geos[ALERT_GEO_NUMBERS];
    alert_motion_t motion;
    alert_static_t actionless;
    alert_speed_t speed;
    alert_tilt_t tilt;
    alert_falldown_t falldown;
}alert_settings_t;



typedef struct {
    uint16_t scan_time;
    uint16_t sleep_time;
}ble_scan_time_t;

typedef struct {
    uint16_t time;
}ble_scan_beacon_time_t;

typedef struct {
    uint8_t flag;
    uint16_t company_id;
    uint8_t uuid[16];
}ble_beacon_t;

typedef struct {
    ble_scan_time_t scan_time;
    ble_scan_beacon_time_t scan_beacon_time;
    ble_beacon_t beacon;
}ble_settings_t;

typedef struct {
//	uint32_t 		magic;
//	uint16_t 		crc;
//	uint16_t 		length;
    conf_file_header_t	head;
    uint32_t 		version;

    sys_settings_t	system;
    button_settings_t buttons;
    phone_settings_t phone;
    gprs_settings_t gprs;

    alert_settings_t alert;
    ble_settings_t ble; 

    uint32_t rvs;
}dev_config_t;







#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
// leave anonymous unions enabled
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

typedef int32_t (*dev_settings_update_handler_t)(uint32_t key, void *param);



extern dev_property_t dev_property;
extern dev_config_t dev_settings;
extern dev_settings_update_handler_t m_dev_cb;
#include "dev_config.verify.h"


#endif

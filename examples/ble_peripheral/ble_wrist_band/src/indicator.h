#ifndef __INDICATOR_H
#define __INDICATOR_H
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ALERT_TYPE_BASE = 0,

    ALERT_TYPE_POWER_ON,
    ALERT_TYPE_POWER_OFF,

    //green
    ALERT_TYPE_GSM_STOP,
    ALERT_TYPE_AT_NO_ANSWER,
    ALERT_TYPE_SIM_CARD_NOT_INSERT,
    ALERT_TYPE_GSM_NOT_CREG, 
    ALERT_TYPE_GSM_NORMAL_WORK,
    ALERT_TYPE_GPRS_CONNECTED,
    ALERT_TYPE_GPRS_DISCONNECTED,

    ALERT_TYPE_CALL_START,
    ALERT_TYPE_CALL_STOP,


//BLUE

    ALERT_TYPE_GPS_STOP,
    ALERT_TYPE_GPS_NO_NMEA_DATA,
    ALERT_TYPE_GPS_NO_LOCATION,
    ALERT_TYPE_GPS_HAS_LOCATION,
    ALERT_TYPE_GPS_APGS_WORK,
    ALERT_TYPE_GPS_APGS_STOP,

    ALERT_TYPE_BLE_LOC,
    ALERT_TYPE_WIFI_LOC,







    ALERT_TYPE_KEY_CLICK, //
    ALERT_TYPE_KEY_PRESS, //
    ALERT_TYPE_KEY_RELEASE, //
    ALERT_TYPE_IN_CHARGING, //
    ALERT_TYPE_DISCHARGE, //
    ALERT_TYPE_CHARGE_FULL, //

    ALERT_TYPE_KEY_CALL, //


    ALERT_TYPE_BATTERY_POWER_LOW, //
    ALERT_TYPE_BATTERY_POWER_NORMAL, //
    ALERT_TYPE_BLE_CONNECTED, //
    ALERT_TYPE_BLE_DISCONNECTED, //
    ALERT_TYPE_BLE_DATA_XFER, //
    
    ALERT_TYPE_BLE_ADV,       //chen
    ALERT_TYPE_BLE_ADV_END,    




    ALERT_TYPE_SMS_GET,
    ALERT_TYPE_PHONE_CALL,
    ALERT_TYPE_PHONE_DIAL,
    ALERT_TYPE_PHONE_ACTIVE,
    ALERT_TYPE_PHONE_CALL_DISCONNECT,
    ALERT_BUTTON_LONG_HOLD,

    ALERT_SHUTDOWN_LONG_HOLD,
    ALERT_TYPE_FINDME,
    ALERT_TYPE_FINDME_CANNEL,
    ALERT_TYPE_TEST,

}indicator_type_enum_t;

/**
 * 通知的事件
 * 
 * @author hzhmcu (2018/9/22)
 * 
 * @param evt 
 */
void Indicator_Evt(indicator_type_enum_t evt);
/**
 * 初始化 
 * 把所有的IO进行初始化 
 * 并用初始化一个100ms的定时器 
 * 
 * @author hzhmcu (2018/9/21)
 */
void Indicator_Init(void);
/**
 * 总开关。
 * 
 * @author hzhmcu (2018/9/22)
 * 
 * @param ind bit 1 LED bit2 motor  可以与运算
 * @param enable true代表打开
 */
void Indicator_Enable(uint32_t ind, bool enable);

#define Indicator_SetALL(enable)     Indicator_Enable(0xFFFFFFFF, enable)
#define Indicator_SetLED(enable)     Indicator_Enable(1, enable)
#define Indicator_SetMotor(enable)   Indicator_Enable(2, enable)
#define Indicator_SetBuzzer(enable)  Indicator_Enable(4, enable)

void Alert_AllOn();
void Alert_PowerOffIndicator();
void Alert_AllOff();


#endif


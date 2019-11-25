


#ifndef __DATE_TIME_H
#define __DATE_TIME_H


#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    RTC_NO_CHANGE = 0,
    RTC_CHANGE_IN_SECOND = 0x01,
    RTC_CHANGE_IN_MINUTE = 0x02,
    RTC_CHANGE_IN_HOUR = 0x04,
    RTC_CHANGE_IN_DAY = 0x08,
    RTC_CHANGE_IN_MONTH = 0x10,
    RTC_CHANGE_IN_YEAR = 0x20,

    RTC_CHANGE_IN_NEW = 0x40,
}rtc_ret_t;

#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language=extended
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

typedef union {
    struct {
        uint32_t		second	:	6;
        uint32_t 		minute	:	6;
        uint32_t 		hour	:	5;
        uint32_t 		day		:	5;
        uint32_t 		month	: 	4;
        uint32_t 		year	:	6;
    };
    uint32_t 		value;
}datetime_t;
typedef enum {
    UPDATE_TIME_BY_INVALID = 0,
    UPDATE_TIME_BY_GSM,
    UPDATE_TIME_BY_GPS,
    UPDATE_TIME_BY_BLE,
    UPDATE_TIME_BY_GPRS,
    UPDATE_TIME_BY_ZONE,
}ByUpdateTime;



#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
// leave anonymous unions enabled
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif


bool DateTime_IsNewTick(void);

bool DateTime_IsValid(void);

datetime_t DateTime_GetDateTime(void);

uint32_t RunTime_GetValue(void);

bool RunTime_InRange(uint32_t start, uint32_t range);
bool UTC_InRange(uint32_t startUTC, uint32_t range);

uint32_t app_rtc_drv_get_rtc0(void);

uint8_t DateTime_GetFlags(void);

uint8_t DateTime_GetWeekday(void);

uint8_t DateTime_GetHour(void);

uint8_t DateTime_GetMinute(void);
uint8_t DateTime_GetSecond(void);

void DateTime_IncreaseHandler(void);

datetime_t DateTime_CalcMinutesOffset(datetime_t datetime, int16_t minutes);

void DateTime_Update(datetime_t datetime, ByUpdateTime way);





datetime_t  DateTime_UTC2DateTime(uint32_t utc);

uint32_t UTC_GetValue();

//for debug
ByUpdateTime DebugTime_GetWay();
uint32_t DebugTime_GetLiveTime();
uint32_t  DebugTime_GetUpdateTime(int32_t zone_offset);

uint32_t DateTime_SetZone(int32_t zone_offset);

int DateTime_Init();
/**
 * 是否向服务器请求同步时间
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @return bool true 需要请求，false不需要请求
 */
bool DateTime_IsRequestUpDate();

void  UTC_SetValue(uint32_t utc) ;
bool UTC_IsValid(void);
#endif


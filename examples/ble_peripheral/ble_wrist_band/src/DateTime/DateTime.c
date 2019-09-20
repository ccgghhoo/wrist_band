
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include "HalRtc.h"
#include "DateTime.h"
#include "LibConfig.h"
//#include "dev_config.get.h"
#include "app_evt.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"



typedef struct
{
    datetime_t curTime; //当前时间
    uint32_t updateTimeUtc; //更新时间。
    uint32_t liveTick; //多时间没有更新
    int32_t lstRequest; //发请求网络数据
    bool IsTimeValid; //当前的时间是否有效
    ByUpdateTime way; //更新的原因
    int32_t zone_offset; //时区
    uint32_t utc;
}DataTimeInfo_t;

#define MAX_UPDATA_TCP_LIMIT (5*60)//2MIN 不再发出请求
#define MAX_UPDATA_SECOND   (72*60*60)//72 小时

#define IS_LEAP_YEAR( x ) 		(((x)%4==0 && (x)%100!= 0)  || ((x)%400== 0))



volatile uint32_t 				second_ticks;

static DataTimeInfo_t sg_sDataTimeInfo = {
    .curTime = {
        .year = 18,
        .month = 7,
        .day = 1,
        .hour = 8,
        .minute = 0,
        .second = 0,
    },
    .lstRequest = MAX_UPDATA_TCP_LIMIT,
};


static volatile uint32_t 		ticks_in_run;

static uint8_t 					m_flags;
uint8_t                 weekday;

static const uint8_t daysOfMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };




__INLINE uint32_t app_rtc_drv_get_rtc0(void)  //app timer used rtc1 not rtc0
{
    return NRF_RTC0->COUNTER;
}
/**
 * 获取当前Run time 
 * 主要可以用作系统超时。 
 * 因为此变量不会清零 
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE uint32_t 
 */
__INLINE uint32_t RunTime_GetValue(void)
{
    return ticks_in_run;
}

/**
 * RTC run tick 的比较
 * 
 * @author hzhmcu (2018/7/23)
 * 
 * @param start 这一定要是Runtick提取出来的时间
 * @param range 
 * 
 * @return __INLINE bool 
 */
__INLINE bool RunTime_InRange(uint32_t start, uint32_t range)
{
    return (start + range)>  ticks_in_run;
}


/** 
 * 获取UTC的时间 
 * 这是0时区的。 
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @return uint32_t 
 */
uint32_t UTC_GetValue()
{
    if (sg_sDataTimeInfo.IsTimeValid == false) return 0;
    return sg_sDataTimeInfo.utc;
}


/**
 * UTC 时间比较
 * 
 * @author hzhmcu (2018/7/23)
 * 
 * @param start  这一定要是UTC提取出来的时间
 * @param range 
 * 
 * @return __INLINE bool 
 */
__INLINE bool UTC_InRange(uint32_t startUTC, uint32_t range)
{
    return (startUTC + range)>  UTC_GetValue();
}
/**
 * 是否有新的Tick
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE bool 
 */
__INLINE bool DateTime_IsNewTick(void)
{
    return second_ticks > 0;
}
/**
 * DateTime 是否有效
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE bool 
 */
__INLINE bool DateTime_IsValid(void)
{
    return sg_sDataTimeInfo.IsTimeValid;
}

/**
 * 获得Datetime的时间
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE uint32_t 
 */
__INLINE datetime_t DateTime_GetDateTime(void)
{
    return sg_sDataTimeInfo.curTime;
}



/**
 * DateTime Get Flags
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @return __INLINE uint8_t 
 */
__INLINE uint8_t DateTime_GetFlags(void)
{
    uint8_t tmp = m_flags;
    m_flags = 0;
    return tmp;
}





/**
 * 获取周几
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE uint8_t 
 */
__INLINE uint8_t DateTime_GetWeekday(void)
{
    return weekday;
}



/**
 * 获取Hour
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE uint8_t 
 */
__INLINE uint8_t DateTime_GetHour(void)
{
    return sg_sDataTimeInfo.curTime.hour;
}

/**
 * 获取Minute
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE uint8_t 
 */
__INLINE uint8_t DateTime_GetMinute(void)
{
    return sg_sDataTimeInfo.curTime.minute;
}
/**
 * 获取Minute
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param void 
 * 
 * @return __INLINE uint8_t 
 */
__INLINE uint8_t DateTime_GetSecond(void)
{
    return sg_sDataTimeInfo.curTime.second;
}
/**
 * 计算周几。
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param year 
 * @param month 
 * @param day 
 * 
 * @return int 
 */
static int DateTime_CalcWeekday(uint32_t year, uint8_t month, uint8_t day)
{
    if (month == 1 || month == 2)
    {
        month += 12;
        year--;
    }
    int day_in_week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    return day_in_week;
}



/**
 * 秒中断
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param pcontext 
 */
void Second_TimeoutCallBack(void *p)
{
    //NRF_LOG_INFO("a second reached!!");
    set_app_evt(APP_EVT_1S); //chen
    sg_sDataTimeInfo.utc++;
    second_ticks++;
    ticks_in_run++;
    sg_sDataTimeInfo.lstRequest++;

}
/**
 * 转换成UTC
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param time 
 * @param zone_offset 
 * 
 * @return uint32_t 
 */
static uint32_t DateTime_DateTime2UTC(datetime_t time, int32_t zone_offset)
{
    time_t timep;
    struct tm dt;
    dt.tm_year = time.year + 2000 - 1900;
    dt.tm_mon = time.month - 1;
    dt.tm_mday = time.day;
    dt.tm_hour = time.hour;
    dt.tm_min = time.minute;
    dt.tm_sec = time.second;
    dt.tm_isdst =  0;
    timep = mktime(&dt); //传入本地时间

    //
    timep -= ((zone_offset)*15 * 60);
    return timep;
}
/**
 * DateTime Increase 处理
 * 
 * @author hzhmcu (2018/7/31)
 */
void DateTime_IncreaseHandler(void)
{
    if (DateTime_IsValid() == false)
    {
        second_ticks = 0;
        return;
    }

    if (second_ticks > 0)
    {
        sg_sDataTimeInfo.liveTick++;

        uint32_t ticks = second_ticks;
        second_ticks = 0;
        if (ticks > 0)
        {
            //SECOND 发生了变化
            m_flags |= RTC_CHANGE_IN_SECOND;
            if (sg_sDataTimeInfo.curTime.second + ticks < 60)
            {
                sg_sDataTimeInfo.curTime.second += ticks;
                return;
            }

            sg_sDataTimeInfo.curTime.second = (sg_sDataTimeInfo.curTime.second + ticks - 60);
            sg_sDataTimeInfo.curTime.minute++;


            //分钟发生了变化
            m_flags |= RTC_CHANGE_IN_MINUTE;
            if (sg_sDataTimeInfo.curTime.minute <= 59)
            {
                return;
            }

            sg_sDataTimeInfo.curTime.minute = 0;
            sg_sDataTimeInfo.curTime.hour++;


            //小时发生了变化
            m_flags |=  RTC_CHANGE_IN_HOUR;
            if (sg_sDataTimeInfo.curTime.hour < 24)
            {
                return;
            }

            sg_sDataTimeInfo.curTime.hour = 0;
            uint8_t day = sg_sDataTimeInfo.curTime.day;
            day++;
            //会溢出 最大31
            //sg_sDataTimeInfo.curTime.day++;
            //day 发生了变化
            m_flags |=  RTC_CHANGE_IN_DAY;
            if (sg_sDataTimeInfo.curTime.month == 2)
            {
                if (IS_LEAP_YEAR((2000 + sg_sDataTimeInfo.curTime.year)))
                {
                    if (day <= 29)
                    {
                        sg_sDataTimeInfo.curTime.day = day;
                        return;
                    }
                }
                else
                {
                    if (day <= daysOfMonth[sg_sDataTimeInfo.curTime.month])
                    {
                        sg_sDataTimeInfo.curTime.day = day;
                        return;
                    }
                }
            }
            else
            {
                if (day <= daysOfMonth[sg_sDataTimeInfo.curTime.month])
                {
                    sg_sDataTimeInfo.curTime.day = day;
                    return;
                }
            }

            sg_sDataTimeInfo.curTime.day = 1;
            sg_sDataTimeInfo.curTime.month++;
            //月发生了变化
            m_flags |= RTC_CHANGE_IN_MONTH;
            if (sg_sDataTimeInfo.curTime.month <= 12)
            {
                return;
            }

            m_flags |= RTC_CHANGE_IN_YEAR;
            sg_sDataTimeInfo.curTime.year++;
            sg_sDataTimeInfo.curTime.month = 1;
            return;
        }

    }
}

/**
 * 读算分钟偏移
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param datetime 计算前的时间
 * @param minutes 偏移的时间
 * 
 * @return datetime_t 计算后的时间
 */
datetime_t DateTime_CalcMinutesOffset(datetime_t datetime, int16_t minutes)
{
    // +/- 12 hours max
    if (minutes > 720 || minutes < -720)
    {
        return datetime;
    }

    datetime_t tmp_dt = datetime;
    uint8_t offset_of_hours, offset_of_minutes;

    if (minutes > 0) // +minutes
    {
        offset_of_hours = minutes / 60;
        offset_of_minutes = minutes % 60;

        if ((tmp_dt.minute + offset_of_minutes) > 59)
        {
            offset_of_hours += 1;
            tmp_dt.minute = tmp_dt.minute + offset_of_minutes - 60;
        }
        else
        {
            tmp_dt.minute = tmp_dt.minute + offset_of_minutes;
        }

        if ((tmp_dt.hour + offset_of_hours) > 23)
        {
            tmp_dt.hour = tmp_dt.hour + offset_of_hours - 24;
            tmp_dt.day++;

            if (tmp_dt.month == 2)
            {
                if (IS_LEAP_YEAR(tmp_dt.year))
                {
                    if (tmp_dt.day <= 29)
                    {
                        return tmp_dt;
                    }
                }
                else
                {
                    if (tmp_dt.day <= 28)
                    {
                        return tmp_dt;
                    }
                }
            }
            else
            {
                if (tmp_dt.day < daysOfMonth[tmp_dt.month])
                {
                    return tmp_dt;
                }
            }
            tmp_dt.month++;
            tmp_dt.day = 1;
            if (tmp_dt.month > 12)
            {
                tmp_dt.month = 1;
                tmp_dt.year += 1;
            }
        }
        else
        {
            tmp_dt.hour += offset_of_hours;
        }
    }
    else // -minutes
    {
        minutes = ~minutes + 1;
        offset_of_hours = minutes / 60;
        offset_of_minutes = minutes % 60;

        if (offset_of_minutes  >  tmp_dt.minute)
        {
            offset_of_hours += 1;
            tmp_dt.minute = 60 + tmp_dt.minute - offset_of_minutes;
        }
        else
        {
            tmp_dt.minute -= offset_of_minutes;
        }

        if (offset_of_hours > tmp_dt.hour) //day change
        {
            tmp_dt.hour = 24 + tmp_dt.hour - offset_of_hours;

            tmp_dt.day--;
            if (tmp_dt.day == 0) // last month
            {
                tmp_dt.month--;
                if (tmp_dt.month == 0) // last year
                {
                    tmp_dt.year--;
                    tmp_dt.month = 12;  //
                    tmp_dt.day = 31; // last day of in year
                }
                else
                {
                    if (tmp_dt.month == 2)
                    {
                        if (IS_LEAP_YEAR(tmp_dt.year))
                        {
                            tmp_dt.day = 29;
                        }
                        else
                        {
                            tmp_dt.day = 28;
                        }
                    }
                    else
                    {
                        tmp_dt.day = daysOfMonth[tmp_dt.month];
                    }
                }
            }
        }
        else //day not change
        {
            tmp_dt.hour -= offset_of_hours;
        }
    }
    return tmp_dt;
}




/**
 * 设置时区，只改变时区，不改变UTC
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param zone_offset 
 * 
 * @return uint32_t 
 */
uint32_t DateTime_SetZone(int32_t zone_offset)
{
    datetime_t datetime = { .value = 0 };
    DateTime_Update(datetime, UPDATE_TIME_BY_ZONE);
    return 0;
}




/**
 * 更新时间 0时区的时间
 * 
 * @author hzhmcu (2018/7/19)
 * 
 * @param datetime  是UTC的时间，没有时区。
 * @param way 
 */
void DateTime_Update(datetime_t datetime, ByUpdateTime way)
{
    int16_t minutes = dev_config_get_timezone_offset() * 15;
    m_flags |= RTC_CHANGE_IN_NEW; 
    //时区不更新时间
    if (UPDATE_TIME_BY_ZONE != way)
    {
        //utc的时间
        uint32_t utc = DateTime_DateTime2UTC(datetime, 0);
        sg_sDataTimeInfo.IsTimeValid = true;
        second_ticks = 0; // clear time tick
        sg_sDataTimeInfo.utc = utc;
        sg_sDataTimeInfo.liveTick = 0;
        datetime = DateTime_CalcMinutesOffset(datetime, minutes);
    }
    else
    {
        datetime =  DateTime_UTC2DateTime(sg_sDataTimeInfo.utc);
        datetime = DateTime_CalcMinutesOffset(datetime, minutes);

        datetime.value = datetime.value;
    }

    sg_sDataTimeInfo.updateTimeUtc = sg_sDataTimeInfo.utc;
    sg_sDataTimeInfo.way = way;


    if (UPDATE_TIME_BY_ZONE == way) return;
    uint32_t value = datetime.value;

    if ((sg_sDataTimeInfo.curTime.value ^ value) > 0x03F)
    {
        //m_flags |= RTC_CHANGE_IN_MINUTE;
    }

    if (((sg_sDataTimeInfo.curTime.value ^ value) & 0xFFFE0000UL) > 0)
    {
        sg_sDataTimeInfo.curTime.value = value;
        weekday = (uint8_t)DateTime_CalcWeekday(sg_sDataTimeInfo.curTime.year + 2000, sg_sDataTimeInfo.curTime.month, sg_sDataTimeInfo.curTime.day);
    }
    else
    {
        sg_sDataTimeInfo.curTime.value = value;
    }
}

/**
 * 是否向服务器请求同步时间
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @return bool true 需要请求，false不需要请求
 */
bool DateTime_IsRequestUpDate()
{
    if (sg_sDataTimeInfo.lstRequest < MAX_UPDATA_TCP_LIMIT)
    {
        return false;
    }
    if (sg_sDataTimeInfo.IsTimeValid == false)
    {
        sg_sDataTimeInfo.lstRequest = 0;
        return true;
    }
    if (sg_sDataTimeInfo.liveTick > MAX_UPDATA_SECOND)
    {
        sg_sDataTimeInfo.lstRequest = 0;
        return true;
    }
    return false;
}







/*
NOTICE: int32_t seconds = dev_config_get_timezone_offset() * 900;
utc += seconds;
*/
datetime_t DateTime_UTC2DateTime(uint32_t utc)
{
    datetime_t datatime;

    time_t time_seconds = utc;
    struct tm *now_time = localtime(&time_seconds);

    datatime.year = now_time->tm_year + 1900 - 2000;
    datatime.month = now_time->tm_mon + 1;
    datatime.day = now_time->tm_mday;
    datatime.hour = now_time->tm_hour;
    datatime.minute = now_time->tm_min;
    datatime.second = now_time->tm_sec;

    return datatime;
}

/*
以下的代码仅作调试 
 
*/


/**
 *  
 *  
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @return ByUpdateTime 
 */
ByUpdateTime DebugTime_GetWay()
{
    return sg_sDataTimeInfo.way;
}

/**
 * 请当前的时间
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @return uint32_t 
 */
uint32_t DebugTime_GetLiveTime()
{
    return sg_sDataTimeInfo.liveTick;
}

/**
 * 获取更新的时间
 * 
 * @author hzhmcu (2018/7/31)
 * 
 * @param zone_offset 
 * 
 * @return uint32_t 
 */
uint32_t  DebugTime_GetUpdateTime(int32_t zone_offset)
{
    return sg_sDataTimeInfo.updateTimeUtc + (zone_offset * 15 * 60);
}



/**
 * 初始化
 * 
 * @author hzhmcu (2018/8/1)
 * 
 * @return int 
 */
int DateTime_Init()
{
    HalRTC_Init(Second_TimeoutCallBack);
    ticks_in_run = 0;
    second_ticks = 0;
    return 0;
}



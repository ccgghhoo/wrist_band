

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "alarm_clock.h"
//alarm_clock_t *alarmclocks[4];
alarms_t alarms;

bool            is_alarms_initialize = false;

/**
 * 每分钟检测一次。
 * 
 * @author hzhmcu (2018/7/30)
 * 
 * @param hour 当前的小时
 * @param minute 当前的分钟
 * @param weekday 周几
 */
void alarmclock_run(uint8_t hour, uint8_t minute, uint8_t weekday)
{
  if (!is_alarms_initialize) return;

  for (uint32_t i = 0; i < CLOCKS_NUM; i++)
  {
    if ((alarms.clocks[i]->flag & CLOCKS_ENABLE) == 0) continue;

    if ((alarms.clocks[i]->flag & CLOCKS_NUM_MASK) != i) continue;

    if ((alarms.clocks[i]->weekday & (1 << weekday)) == 0) continue;

    if (alarms.clocks[i]->hour == hour && alarms.clocks[i]->minute == minute)
    {
      if (alarms.cb != 0)
      {
        alarms.cb(alarms.clocks[i]->ring_time,alarms.clocks[i]->ring_type);
      }
      return;
    }
  }
}

/**
 * 初始化 闹钟 
 * 
 * @author hzhmcu (2018/7/30)
 * 
 * @param p_alarmclock 
 */
void alarmclock_init(alarms_init_t *p_alarmclock)
{
  if (is_alarms_initialize) return;
  int i = 0;
  if (p_alarmclock == NULL) return;
  for (i = 0; i < CLOCKS_NUM; i++)
  {
   if( p_alarmclock->clocks[i] == NULL)  return;
  }
  for (i = 0; i < CLOCKS_NUM; i++)
  {
    alarms.clocks[i] = p_alarmclock->clocks[i];
  }
  alarms.cb = p_alarmclock->cb;
  is_alarms_initialize = true;
}


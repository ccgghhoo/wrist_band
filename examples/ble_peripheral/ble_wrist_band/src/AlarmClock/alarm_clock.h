

#ifndef ALARM_CLOCK_H__
#define ALARM_CLOCK_H__

#include <stdint.h>
#include <stdbool.h>
#include "dev_conf_defines.h"

#if defined(__CC_ARM)
    #pragma push
    #pragma anon_unions
#elif defined(__ICCARM__)
    #pragma language=extended
#elif defined(__GNUC__)
    // anonymous unions are enabled by default
#endif


typedef void (*clock_activate_handle_t)(uint8_t ringtime, uint8_t ring_type);

typedef struct {
  alarm_clock_t *clocks[CLOCKS_NUM];
  clock_activate_handle_t cb;
}alarms_t;

typedef struct {
  alarm_clock_t *clocks[CLOCKS_NUM];
  clock_activate_handle_t cb;
}alarms_init_t;

#if defined(__CC_ARM)
    #pragma pop
#elif defined(__ICCARM__)
    // leave anonymous unions enabled
#elif defined(__GNUC__)
    // anonymous unions are enabled by default
#endif

/**
 * ÿ���Ӽ��һ�Ρ�
 * 
 * @author hzhmcu (2018/7/30)
 * 
 * @param hour ��ǰ��Сʱ
 * @param minute ��ǰ�ķ���
 * @param weekday �ܼ�
 */
void alarmclock_run(uint8_t hour, uint8_t minute, uint8_t weekday);
/**
 * ��ʼ�� ���� 
 * 
 * @author hzhmcu (2018/7/30)
 * 
 * @param p_alarmclock 
 */
void alarmclock_init(alarms_init_t *p_alarmclock);

#endif

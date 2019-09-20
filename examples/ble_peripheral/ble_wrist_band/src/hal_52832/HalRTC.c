#include <stdint.h>
#include "nrf.h"
#include "nrf_timer.h"
#include "app_timer.h"
#define APP_TIMER_INTERVAL		APP_TIMER_TICKS(1000)

APP_TIMER_DEF(RTC_TIMER);
void (*pfnSecondCb)(void* p)=NULL;

static void Second_TimeoutHandler(void *p)
{
  pfnSecondCb(p);
}


/**
 * Hal RTC 初始化 
 * 
 * @author hzhmcu (2018/8/1)
 * 
 * @param fnSecondCb 每一秒中断的回调函数 
 */
void HalRTC_Init(void(*fnSecondCb)(void* p))
{
  uint32_t err_code = app_timer_create(&RTC_TIMER,
                                       APP_TIMER_MODE_REPEATED,
                                       Second_TimeoutHandler);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }

  err_code = app_timer_start(RTC_TIMER, APP_TIMER_INTERVAL, NULL);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }

  pfnSecondCb = fnSecondCb;

}


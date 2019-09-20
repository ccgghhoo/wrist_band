#ifndef __HAL_RTC_H
#define __HAL_RTC_H

/**
 * Hal RTC 初始化 
 * 
 * @author hzhmcu (2018/8/1)
 * 
 * @param fnSecondCb 每一秒中断的回调函数 
 */
void HalRTC_Init(void(*fnSecondCb)(void *p));
#endif

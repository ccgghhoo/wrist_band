#ifndef __HAL_RTC_H
#define __HAL_RTC_H

/**
 * Hal RTC ��ʼ�� 
 * 
 * @author hzhmcu (2018/8/1)
 * 
 * @param fnSecondCb ÿһ���жϵĻص����� 
 */
void HalRTC_Init(void(*fnSecondCb)(void *p));
#endif

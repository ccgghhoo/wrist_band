#ifndef __HAL_EXIT_H
#define __HAL_EXIT_H
#include "Hal.h"
typedef enum {
    EXIT_PUSH_NO,
    EXIT_PUSH_UP,
    EXIT_PUSH_DOWN,
}ExitPush_en;
/**
 * 初始化EXIT。 
 * 只可以初始化一次 
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param cb 
 * @param pin 
 * 
 * @return bool 
 */
bool HalExit_Init(void (*cb)(uint32_t pin));
/**
 * 注册Button中断
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param pin 
 * @param push 
 * 
 * @return int 
 */
int HalExit_Register(uint32_t pin, ExitPush_en push);
    /**
 * 使能Button中断
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param pin 
 * @param isEnable 
 * 
 * @return int 
 */
int HalExit_Enable(uint32_t pin, bool isEnable);
/**
 * 读Pin
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param pin 
 * 
 * @return bool 
 */
bool HalExit_Read(uint32_t pin);
#endif


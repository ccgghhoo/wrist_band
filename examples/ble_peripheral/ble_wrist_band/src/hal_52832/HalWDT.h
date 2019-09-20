
#ifndef __HAL_WDT_H
#define __HAL_WDT_H


/**
 * 开门狗 初始化
 *  
 * 2s 一定要喂狗 
 * @author hzhmcu (2018/8/1)
 *  
 *  
 *  
 * @param void 
 */
void HalWDT_Init(void);
/**
 * 喂狗
 * 
 * @author hzhmcu (2018/8/1)
 * 
 * @param void 
 */
void HalWDT_Reload(void);


#endif 

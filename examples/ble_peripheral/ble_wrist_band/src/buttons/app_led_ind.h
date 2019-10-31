
/******************************************************************************

*******************************************************************************/
#ifndef __PWM_CTRL__H
#define __PWM_CTRL__H

#include <stdbool.h>
#include "sdk_errors.h"


typedef struct 
 {
  
   union
   {
     struct{
       uint8_t led_cycle_flag;
       uint8_t led_cycle_cnt;
     };
     uint16_t led_cycle;
   };
        
    uint8_t led_time_set;       //设置的on/off时间 从不同模式对应表格中读取
    uint8_t led_time_cnt;       //用来计时 以确定是否到达设置值  单位为定时器的timeout时间 
    uint8_t led_db_item_index;  //对应表格中第几个元素
    uint8_t led_ind_mode;
    //uint8_t led_cycle_cnt_set;  //
    const uint8_t *led_mode_db_set; 
}led_ind_t;

typedef enum
{
    LED_FUN_OFF=0,
    LED_FUN_ON,
    LED_FUN_BLE_ADVERTISING,
    LED_FUN_BLE_CONNECTED,
    LED_FUN_CHARGING,
    LED_FUN_CHARG_FULL,         
    
    
}ind_fun_t;

#define  LED_FUN_FLAG_OFF               (1<<0)
#define  LED_FUN_FLAG_ON                (1<<1)
#define  LED_FUN_FLAG_CHARGING          (1<<2)
#define  LED_FUN_FLAG_CHARG_FULL        (1<<3)  
#define  LED_FUN_FLAG_ADVERTISING        (1<<4)
#define  LED_FUN_FLAG_CONNECTED         (1<<5)



 
//void led_mode_set(uint8_t led_id ,  uint8_t mode, uint8_t  cycles);
void led_ind_init(void);
//void led_ind_all_off(void);
void  app_led_indicate_set(ind_fun_t  ind_mode);

#endif /* */




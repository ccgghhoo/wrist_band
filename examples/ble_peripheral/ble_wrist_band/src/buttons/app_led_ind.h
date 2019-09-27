
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
        
    uint8_t led_time_set;       //���õ�on/offʱ�� �Ӳ�ͬģʽ��Ӧ����ж�ȡ
    uint8_t led_time_cnt;       //������ʱ ��ȷ���Ƿ񵽴�����ֵ  ��λΪ��ʱ����timeoutʱ�� 
    uint8_t led_db_item_index;  //��Ӧ����еڼ���Ԫ��
    uint8_t led_ind_mode;
    //uint8_t led_cycle_cnt_set;  //
    const uint8_t *led_mode_db_set; 
}led_ind_t;

typedef enum
{
    LED_MODE_OFF=0,
    LED_MODE_ON,
    LED_MODE_BLE_ADVERTISING,
    LED_MODE_BLE_CONNECTED,
    LED_MODE_CHARGING,
    LED_MODE_CHARG_FULL,         
    
    
}ind_mode_t;

 
//void led_mode_set(uint8_t led_id ,  uint8_t mode, uint8_t  cycles);
void led_ind_init(void);
//void led_ind_all_off(void);
void  app_led_indicate_set(ind_mode_t  ind_mode);

#endif /* */




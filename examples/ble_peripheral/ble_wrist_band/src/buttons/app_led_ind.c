/*
*
*
*/
#include "bsp.h"
#include "nrf_drv_clock.h"
#include "app_timer.h"
#include "app_led_ind.h"
#include "nrf_log.h"


APP_TIMER_DEF(m_timer_100ms);

#define   LED_IND_PERIOD   APP_TIMER_TICKS(100)  //100MS

//#define  LEDS_NUMBER          4
//#define  LED_MODE_ID_RED      0
//#define  LED_MODE_ID_GREEN    1
//#define  LED_MODE_ID_BLUE     2
//#define  LED_MODE_ID_MOTOR    3

#define  LED_MODE_FLAG_OFF        0//off
#define  LED_MODE_FLAG_ON         1//on always
#define  LED_MODE_FLAG_CYCLE      2//cycle for ever
//#define  LED_MODE_FLAG_CYCLE_SET       3//one time 
#define  LED_MODE_FLAG_CYCLE_SET  4//can be set up 

#define  MODE_BLE_ADVERTISING    		4
#define  MODE_BLE_CONNECTED      		8
#define  MODE_CHARGING                  1
#define  MODE_CHARG_FULL                0



const uint8_t  led_ind_mode_0_db[]={LED_MODE_FLAG_OFF,0};                       
const uint8_t  led_ind_mode_1_db[]={LED_MODE_FLAG_ON,0};  

const uint8_t  led_ind_mode_2_db[]={LED_MODE_FLAG_CYCLE, 5, 25,0};              
const uint8_t  led_ind_mode_3_db[]={LED_MODE_FLAG_CYCLE,1,29,0};
const uint8_t  led_ind_mode_4_db[]={LED_MODE_FLAG_CYCLE,1,3,1,25,0};
const uint8_t  led_ind_mode_5_db[]={LED_MODE_FLAG_CYCLE,3,27,0};
const uint8_t  led_ind_mode_6_db[]={LED_MODE_FLAG_CYCLE,1,29,0};
const uint8_t  led_ind_mode_7_db[]={LED_MODE_FLAG_CYCLE,10,20,0};

const uint8_t  led_ind_mode_8_db[]={LED_MODE_FLAG_CYCLE_SET,1,10,0};                    
const uint8_t  led_ind_mode_9_db[]={LED_MODE_FLAG_CYCLE_SET,1,3,0};
const uint8_t  led_ind_mode_10_db[]={LED_MODE_FLAG_CYCLE_SET,1,3,1,3,0};
const uint8_t  led_ind_mode_11_db[]={LED_MODE_FLAG_CYCLE_SET,1,3,1,3,1,3,0};
const uint8_t  led_ind_mode_12_db[]={LED_MODE_FLAG_CYCLE_SET,1,3,1,3,1,3,1,3,1,3,0};

const uint8_t * p_led_db[] =
{
    led_ind_mode_0_db,
    led_ind_mode_1_db,
    led_ind_mode_2_db,
    led_ind_mode_3_db,
    led_ind_mode_4_db,
    led_ind_mode_5_db,
    led_ind_mode_6_db,
    led_ind_mode_7_db,
    led_ind_mode_8_db,
    led_ind_mode_9_db,
    led_ind_mode_10_db,
    led_ind_mode_11_db,
    led_ind_mode_12_db,
};

static led_ind_t   m_led_ind[LEDS_NUMBER] ;
static uint8_t     m_100ms_timer_started = 0;


static void led_ind_mode_process(void);
static void led_ind_all_off(void);


static void timer_100ms_handler(void * p_context)
{  
	   
    led_ind_mode_process();  
          
    for(uint32_t i=0; i<LEDS_NUMBER; i++)
    {
    		if(m_led_ind[i]. led_cycle_flag>=1)
    		{
        		return;
    		}
    }
        
    led_ind_all_off();
    m_100ms_timer_started = 0;
    uint32_t err_code;
    err_code = app_timer_stop(m_timer_100ms);
    
    APP_ERROR_CHECK(err_code);    
    
              
}

void led_ind_init(void)
{
    uint32_t err_code;
    err_code = app_timer_create(&m_timer_100ms, APP_TIMER_MODE_REPEATED, timer_100ms_handler);
    APP_ERROR_CHECK(err_code);
    
    m_100ms_timer_started = 0;
    
	led_ind_all_off();
    
}


void led_mode_set(uint8_t led_id , uint8_t mode, uint8_t  cycles)
{
      uint32_t err_code;
      
      bsp_board_led_off(led_id);
           
      m_led_ind[led_id].led_db_item_index=0;
      m_led_ind[led_id].led_time_cnt = 0;
      m_led_ind[led_id].led_ind_mode = mode;
      m_led_ind[led_id].led_mode_db_set=p_led_db[mode];
      m_led_ind[led_id].led_cycle_flag=m_led_ind[led_id].led_mode_db_set[0];
      if(m_led_ind[led_id].led_cycle_flag==LED_MODE_FLAG_CYCLE_SET)
      {
          m_led_ind[led_id].led_cycle_cnt = cycles;
      }
      else
      {
          m_led_ind[led_id].led_cycle_cnt = 0;
      }
      m_led_ind[led_id].led_time_set = m_led_ind[led_id].led_mode_db_set[1]; 
            
      if(m_100ms_timer_started==0)
      {
          err_code = app_timer_start(m_timer_100ms, LED_IND_PERIOD, NULL);                
          APP_ERROR_CHECK(err_code);
      }
      
      //led_ind_mode_process();
           
}

static void led_ind_all_off(void)
{
     for(uint32_t i=0; i<LEDS_NUMBER; i++)
    {
		led_mode_set(i, LED_MODE_OFF, 0);
    }
    led_ind_mode_process();
}

static void led_ind_mode_process(void)
{
	for(uint32_t i=0;  i<LEDS_NUMBER; i++)
	{
        if(m_led_ind[i].led_cycle_flag==LED_MODE_FLAG_OFF)  //OFF ALWAYS
        { 
            bsp_board_led_off(i);
        }
        else if(m_led_ind[i].led_cycle_flag==LED_MODE_FLAG_ON)//on always
        {  
            bsp_board_led_on(i);        
        }
        else  
        {  
            if(m_led_ind[i].led_db_item_index == 0 && m_led_ind[i].led_time_cnt==0)
            {
                bsp_board_led_on(i); 
                m_led_ind[i].led_db_item_index=1;
                m_led_ind[i].led_time_set = m_led_ind[i].led_mode_db_set[1];
                
            }
            else
            {
                m_led_ind[i].led_time_cnt++;
                if(m_led_ind[i].led_time_cnt==m_led_ind[i].led_time_set)
                {
                    m_led_ind[i].led_db_item_index++;
                    bsp_board_led_invert(i);
                    //nrf_gpio_pin_write(i,nrf_gpio_pin_out_read(BSP_RED_LED)?0:1);
                    m_led_ind[i].led_time_set = m_led_ind[i].led_mode_db_set[m_led_ind[i].led_db_item_index];
                    //NRF_LOG_INFO("red timeset:%d reddb index: %d" ,m_led_ind[i].led_time_set , m_led_ind[i].led_db_item_index);
                    m_led_ind[i].led_time_cnt=0;
                    if(m_led_ind[i].led_time_set ==0)//0 is the last of array
                    {
                        if(m_led_ind[i].led_cycle_flag==LED_MODE_FLAG_CYCLE)
                        {
                            m_led_ind[i].led_db_item_index=0;
                            m_led_ind[i].led_time_cnt = 0;
                            bsp_board_led_off(i);
                        }
//                        else if(m_led_ind[i].led_cycle_flag==LED_MODE_FLAG_CYCLE_ONCE)
//                        {
//                            m_led_ind[i].led_db_item_index=0;
//                            m_led_ind[i].led_time_cnt = 0;
//                            bsp_board_led_off(i);
//                            m_led_ind[i].led_cycle_flag=0;
//                        }
                        else if(m_led_ind[i].led_cycle_flag==LED_MODE_FLAG_CYCLE_SET)
                        {
                            if(m_led_ind[i].led_cycle_cnt)m_led_ind[i].led_cycle_cnt--;
                              
                            if(m_led_ind[i].led_cycle_cnt)
                            {
                                m_led_ind[i].led_db_item_index=0;
                                m_led_ind[i].led_time_cnt--;
                                bsp_board_led_off(i);
                            }                            
                            else
                            {
                                m_led_ind[i].led_db_item_index=0;
                                m_led_ind[i].led_time_cnt = 0;
                                bsp_board_led_off(i);
                                m_led_ind[i].led_cycle_flag=0;
                            }
                        }
                    }                                            
                }                    
            }                
        } 
     }
            
} 



void  app_led_indicate_set(ind_mode_t  ind_mode)
{
  
    switch(ind_mode)
    {
     case LED_MODE_BLE_ADVERTISING:
        led_mode_set(LED_MODE_ID_GREEN,  MODE_BLE_ADVERTISING, 0);
        break;
               
     case LED_MODE_BLE_CONNECTED:
        led_mode_set(LED_MODE_ID_GREEN, MODE_BLE_CONNECTED, 10);
        break;
        
     case LED_MODE_CHARGING:
        led_mode_set(LED_MODE_ID_GREEN, MODE_CHARGING, 0);
        break;   
      
    }
  
  
}













#include "app_init.h"



uint32_t  m_app_evt=0;

void clear_app_evt(uint32_t evt)
{
    m_app_evt&=~evt;
}

void set_app_evt(uint32_t evt)
{
    m_app_evt|=evt;
}

bool check_app_evt(uint32_t evt)
{
    return (m_app_evt&evt)? 1:0;
}


void app_evt_poll(void)
{
  if(check_app_evt(0xffffffff)==0) return;
  
  
  if(check_app_evt(APP_EVT_1S))
  {
      uint32_t m_user_time_senconds=RunTime_GetValue();
      
      
      clear_app_evt(APP_EVT_1S);
      if(m_user_time_senconds%60==0)
      {
          battery_level_cal();
          batt_voltage_get();      
      }
      
      if(m_user_time_senconds%300==0)
      {
          history_data_save();//for test
      }
      
      uint8_t ret = DateTime_GetFlags();
      
      if ((ret & RTC_CHANGE_IN_DAY) > 0)       //new day
      {
           app_fds_new_day_handle();        
      }
      
            
      batt_charging_check();
      if(batt_state_changed())      
      {       
          batt_clear_adv_update_flag();
          update_adv_data();
      }
      
      
      //md_motion_or_static_alert_judge();
      
      if(check_app_evt(APP_EVT_DFU_RESET))
      {
          static uint32_t  dfu_reset_cnt=0;
          if(dfu_reset_cnt++>8)
          {
              dfu_reset_cnt=0;
              clear_app_evt(APP_EVT_DFU_RESET);
              NVIC_SystemReset();
          }
      }                           
  }
  
  
  if(check_app_evt(APP_EVT_PROTOCOL_DATA_HANDLE))
  {
    
  }
  
  
  
}




//end of  file
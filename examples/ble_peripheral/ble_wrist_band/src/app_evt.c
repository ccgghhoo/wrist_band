
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
      uint32_t m_user_time_senconds= UTC_GetValue();//RunTime_GetValue();           
      clear_app_evt(APP_EVT_1S);
            
      if(check_app_evt(APP_EVT_SOS_ALARM))
      {
          ble_sos_key_send();          //event will be cleared only when the central respond to band
      }
      
      if(RunTime_GetValue()>30 && (m_user_time_senconds&0x01))//
      {
          ble_sport_data_send();          
      }
      
      
#ifdef DEGREE
      //NRF_LOG_INFO("DGEGREE");
      md_module_period_one_second();
     
      if(m_user_time_senconds%300==0)//300
      {
          //if(UTC_IsValid())  //时间有效才存储
          {
                md_module_period_save();//
          }
      }
      
#endif      
      //read_file_data_test();  //test OK
      uint8_t ret = DateTime_GetFlags();
      
      if ((ret & RTC_CHANGE_IN_DAY) > 0)       //new day
      //if ((ret & RTC_CHANGE_IN_HOUR) > 0) 
      //if ((ret & RTC_CHANGE_IN_MINUTE) > 0)
      {          
           app_fds_new_day_handle();        
      }
      
            
      batt_charging_check();
      batt_status_get();            
      if(m_user_time_senconds%60==0)
      {
          battery_level_cal();
          batt_voltage_get();      
      }             
      if(batt_state_changed())      
      {       
          batt_clear_adv_update_flag();
          update_adv_data();
      }
      
//         AxesRaw_t raw_data;     // for test
//         LIS3DH_GetAccAxesRaw(&raw_data);      
//         NRF_LOG_INFO("x=%d y=%d z=%d", raw_data.X/16, raw_data.Y/16, raw_data.Z/16);
         
         
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
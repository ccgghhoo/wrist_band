
#include "app_init.h"




int32_t DeviceReset_CallBack()
{
    set_app_evt(APP_EVT_DFU_RESET);
    //NVIC_SystemReset();
    return 0;
}

void  app_proto_init(void)
{
    //Eview Proto 协议的初始化
    Proto_Init();
    //初始化DFU回调函数
    pApiDFU->Init(objDFU, DeviceReset_CallBack);
}

void  hal_init(void)
{
    timers_init();
    buttons_leds_init();
    Indicator_Init();
	DateTime_Init();
    md_init();
    batt_adc_init();
    
        
}



void  app_init(void)
{  
     hal_init();
     
     ble_role_init();
     
	 app_proto_init();
     
     app_fds_init();
	 
     //app_wdt_init();  //wdt is inited in bootloader ,so this call is invalid 
     
}

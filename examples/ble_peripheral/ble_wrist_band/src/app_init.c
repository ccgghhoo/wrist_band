#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "proto.h"
#include "dfu.h"
#include "nrf_delay.h"
#include "app_evt.h"
#include "DateTime.h"


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
	DateTime_Init();
}



void  app_init(void)
{
	app_proto_init();
	 hal_init();
}

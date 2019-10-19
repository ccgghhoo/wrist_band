

#include "nrf.h"
#include "app_wdt.h"

#define TIMEOUT_VALUE		(32768 << 4) 	// 16seconds 

void app_wdt_reload(void)
{
	NRF_WDT->RR[0] = WDT_RR_RR_Reload; 
	//NRF_WDT->RR[0] = TIMEOUT_VALUE; 
}


void app_wdt_init(void)
{
	NRF_WDT->CRV			= TIMEOUT_VALUE; 
	NRF_WDT->RREN 			= ( WDT_RREN_RR0_Enabled << WDT_RREN_RR0_Pos ); 
	//NRF_WDT->CONFIG 		= ( WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos ) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos ); 
	NRF_WDT->CONFIG 		= ( WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos ) | ( WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos ); 
	NRF_WDT->RR[0]			= WDT_RR_RR_Reload; 
	
	NRF_WDT->TASKS_START	= 1; 
}

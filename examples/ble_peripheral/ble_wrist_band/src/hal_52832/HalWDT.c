#include "nrf.h"
#include "halwdt.h"
#define HAL_WDT 1
#define TIMEOUT_VALUE		(32768 << 2) 	// 2seconds

/**
 * 喂狗
 * 
 * @author hzhmcu (2018/8/1)
 * 
 * @param void 
 */
void HalWDT_Reload(void)
{
#if HAL_WDT
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;
  NRF_WDT->RR[0] = TIMEOUT_VALUE;
#endif
}


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
void HalWDT_Init(void)
{
#if HAL_WDT
  NRF_WDT->CRV			= TIMEOUT_VALUE;
  NRF_WDT->RREN 			= (WDT_RREN_RR0_Enabled << WDT_RREN_RR0_Pos);
  NRF_WDT->CONFIG 		= (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);

  NRF_WDT->RR[0] = WDT_RR_RR_Reload;

  NRF_WDT->TASKS_START	= 1;

  HalWDT_Reload();
#endif
}

#ifndef __HAL_BATTERY_H
#define __HAL_BATTERY_H
#include <stdint.h>
#include <stdbool.h>
typedef struct{
	void (*cb)( uint32_t voltage ); 
  int (*ChargePin)(bool isCharge);
}battery_init_t; 


void HalBattery_Init(battery_init_t * p_battery_obj); 

void HalBattery_Start(void); 

void HalBattery_Stop(void); 
void HalBattery_SetCharge(bool enable);
bool HalBattery_IsCharge();
void HalBattery_Loop();
#endif 

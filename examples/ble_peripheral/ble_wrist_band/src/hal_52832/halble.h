#ifndef __HAL_BLE_H
#define __HAL_BLE_H

#include "app_nus.h"
//#include "advertising.h"


int HalBLE_SetChargeBaseLoc(uint32_t lat, uint32_t lon);
int HalBLE_SecondLoop();


#endif


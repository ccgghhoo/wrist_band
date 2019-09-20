#ifndef __SCAN_ADV_H
#define __SCAN_ADV_H
#include "LibConfig.h"
#include "ble_gap.h"
int32_t ScanAdv_Init(uint32_t scan_time, uint32_t scan_priod,
                     void (*start_scan)(), void (*stop_scan)());
int32_t ScanAdv_Start();
int32_t ScanAdv_Stop();
int32_t ScanAdv_CallBack(ble_gap_evt_adv_report_t const *pAdv);
int32_t ScanAdv_SetTime(uint32_t scan_time, uint32_t scan_priod);
#endif


#ifndef __HAL_BLEC_H
#define __HAL_BLEC_H
#if (FEATURE_BLEC)
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
typedef enum {
    HalBLECNotifAct_NONE,
    HalBLECNotifAct_Adversting,
    HalBLECNotifAct_Connected,
    HalBLECNotifAct_Disconnected,
    HalBLECNotifAct_ReceiveData,
    HalBLECNotifAct_RssiChanged,
}HalBLECNotifAct_en;

typedef struct {
    uint16_t conn_handle;
    uint8_t *p_data;
    uint16_t data_len;
}HalReceiveData_t;
typedef void (*pfnHalBLECNotif_t)(HalBLECNotifAct_en act, void *param);

void HalBLEC_RegisterNotif(pfnHalBLECNotif_t notif);
int HalBLEC_Send(uint16_t conn, uint8_t *data, uint16_t len);
void HalBLEC_Init();
void HalBLEC_ScanStart();
void HalBLEC_ScanStop();
int HalBLEC_Connect(uint8_t const mac[6]);
#endif
#endif


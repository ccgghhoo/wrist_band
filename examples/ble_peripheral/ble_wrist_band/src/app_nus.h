

#ifndef APP_NUS_H__
#define APP_NUS_H__

#include <stdint.h>

#include "ble_nus.h"

void app_nus_task_loop(void); 

void app_nus_init(void);

void app_nus_tx_data_put(uint8_t *data, uint16_t len);

void app_nus_on_ble_evt( ble_evt_t * p_ble_evt); 

void app_nus_res_handler(void);

void app_nus_log_ble(uint8_t *p_data, uint16_t len); //chen

bool BLE_IsConnect();

uint32_t app_nus_send_data(uint8_t * data, uint16_t * len);
void ble_sos_key_send(void);
void ble_sport_data_send(void);
void ble_utc_time_req_send(void);
#endif 


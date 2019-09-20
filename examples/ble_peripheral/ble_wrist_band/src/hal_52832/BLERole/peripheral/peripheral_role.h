

#ifndef PERIPHERAL_ROLE_H__
#define PERIPHERAL_ROLE_H__


#include <stdint.h>
#include <stdbool.h>

#include "advertising.h"


#define BA_BATT_LOW                 0x0001
#define BA_BATT_IN_CHARGING         0x0002
#define BA_BLE_CONNECTED            0x0004
#define BA_TCP_CONNECTED            0x0008

#define BA_IN_FALLDOWN_ALERT        0x0010 
#define BA_IN_STATIC_ALERT          0x0020 
#define BA_IN_TILT_ALERT            0x0040 
#define BA_IN_MOTION_ALERT          0x0080 

#define BA_BLE_HAS_DATA             0x0100
#define BA_BLE_OFTEN_CONNECT        0x0200



#define BA_IN_ALERTS_RESET          ( BA_IN_FALLDOWN_ALERT | BA_IN_STATIC_ALERT | BA_IN_TILT_ALERT | BA_IN_MOTION_ALERT )

typedef struct{
    uint8_t change; 
    uint8_t version; 
    uint8_t battery; 
    uint16_t state; 
    uint16_t company_id; 
    uint16_t crc; 
    
    uint32_t module_id; 
    
}bl_user_data_t; 

void ble_slow_connection(void);

void ble_fast_connection(void);

void bl_user_data_change(bl_user_data_type type, uint16_t value, bool enable); 

void ble_stop(void); 

void ble_stack_init(void);

void ble_role_init(void);

uint16_t ble_get_conn_handle(void);

void ble_manual_setup_adv(void); 
uint16_t GetBLESetLen();
#endif 


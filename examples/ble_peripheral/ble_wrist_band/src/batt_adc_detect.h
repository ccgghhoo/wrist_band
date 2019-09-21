
/******************************************************************************

*******************************************************************************/
#ifndef __BATT_ADC_DETECT__H
#define __BATT_ADC_DETECT__H

#include <stdint.h>

extern  uint8_t                      m_batt_level;

void batt_adc_init(void);
void batt_start_measure(void);
int16_t batt_voltage_get(void);
int16_t batt_adc_value_get(void);

uint32_t batt_voltage_av_get(void);
uint8_t battery_level_cal(void);
uint8_t batt_level_get(void);

bool batt_level_changed(void);
void batt_clear_adv_update_flag(void);

#endif /* */




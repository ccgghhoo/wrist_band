

#ifndef DFU_SETTINGS_H__
#define DFU_SETTINGS_H__
#include "dfu.h"

#include <stdint.h>

uint32_t DFUSetting_Save(dfu_settings_t * s_dfu_settings,const uint8_t *init_command, uint16_t size, void (*callback)(uint32_t reson));
#endif 


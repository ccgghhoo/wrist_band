

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "libHeap.h"

#include "dfu.types.h"

#include "crc32.h"

#include "app_flash_drv.h"

uint32_t PortDFUSetting_Save(dfu_settings_t *s_dfu_settings, void (*callback)(uint32_t reson));





uint32_t DFUSetting_Save(dfu_settings_t * s_dfu_settings,const uint8_t *init_command, uint16_t size, void (*callback)(uint32_t reson))
{
 
  if (s_dfu_settings == NULL)
  {
    return -1;
  }
  memcpy(s_dfu_settings, (uint32_t *)DFU_SETTINGS_ADDR_START, sizeof(dfu_settings_t));
  s_dfu_settings->bank_1.bank_code = NRF_DFU_BANK_READY_APP;//NRF_DFU_BANK_READY_EXAPP;
  s_dfu_settings->settings_version = NRF_DFU_SETTINGS_VERSION;
  s_dfu_settings->progress.command_size = size;
  s_dfu_settings->progress.command_crc = crc32_compute(init_command, size, NULL);
  s_dfu_settings->progress.firmware_image_offset = FIRMWARE_START_ADDR; // Flash Start Address
  memcpy(s_dfu_settings->init_command, init_command, size);
  
  s_dfu_settings->crc = crc32_compute((uint8_t *)s_dfu_settings + 4, sizeof(dfu_settings_t) - 4 - sizeof(s_dfu_settings->init_command), NULL);
  
  if (PortDFUSetting_Save(s_dfu_settings, callback) != 0)
  {
    
  }
  return 0;
}



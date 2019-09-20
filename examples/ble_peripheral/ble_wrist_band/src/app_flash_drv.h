

#ifndef APP_FLASH_DRV_H__
#define APP_FLASH_DRV_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "nrf_log.h"

#define FLASH_SECTOR_SIZE_MASK		(0x1000 - 1)
#define FLASH_SECTOR_NUM_MASK		(~FLASH_SECTOR_SIZE_MASK)
#define FLASH_SECTOR_SIZE			4096 

#define FLASH_PAGE_SIZE				256 
#define FLASH_PAGE_SIZE_MASK		0xff

#define DFU_CHIP_FLASH_SIZE         512*1024
#define DFU_BL_SETTING_SIZE         2*4*1024
#define DFU_BL_SETTING_START_ADDR   (DFU_CHIP_FLASH_SIZE - DFU_BL_SETTING_SIZE)
#define DFU_BL_SETTING_END_ADDR     (DFU_BL_SETTING_START_ADDR + DFU_BL_SETTING_SIZE)
#define DFU_MBR_PARAM_SIZE          4*1024
#define DFU_MBR_START_ADDR          0 
#define DFU_MBR_SIZE                4*1024
//extern uint32_t Image$$ER_IROM1$$Base;
//#define DFU_SOFTDEVICE_END_ADDR     Image$$ER_IROM1$$Base//124*1024 //152 0X26000
#if 1
#define DFU_SOFTDEVICE_END_ADDR     100*1024//124*1024 //0X1f000
#else
#define DFU_SOFTDEVICE_END_ADDR     152*1024 //0X26000
#endif

#define DFU_BOOTLOADER_SIZE         44*1024//24*1024
#define DFU_BOOTLOADER_START_ADDR   0x72000//(DFU_CHIP_FLASH_SIZE-DFU_SOFTDEVICE_END_ADDR-DFU_BL_SETTING_SIZE-DFU_MBR_PARAM_SIZE-DFU_BOOTLOADER_SIZE) 
#define DFU_APP0_START_ADDR         (DFU_SOFTDEVICE_END_ADDR) //SD.HEX=(MBR+SD)
#define DFU_APP1_START_ADDR         4096*((DFU_APP0_START_ADDR+(DFU_BOOTLOADER_START_ADDR - DFU_APP0_START_ADDR)/2)/4096) //308k

#define DFU_FLASH_START_ADDR		DFU_APP1_START_ADDR   //=45000           
#define DFU_FLASH_END_ADDR			DFU_BOOTLOADER_START_ADDR
#define DFU_APP0_SIZE               (DFU_APP1_START_ADDR-DFU_APP0_START_ADDR)
#define DFU_APP1_SIZE               (DFU_FLASH_END_ADDR-DFU_FLASH_START_ADDR) //0X27000=156k

#define FIRMWARE_START_ADDR         DFU_FLASH_START_ADDR  
extern nrf_fstorage_t app_flash_fs_config;



void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage);

bool flash_write_completed(void);
bool flash_write_data(uint32_t addr, uint8_t *data, uint16_t len);

bool flash_read_data(uint32_t addr, uint8_t *data, uint16_t len);

bool flash_erase_sector(uint32_t addr); 

void app_dfu_flash_init(void);



#endif 

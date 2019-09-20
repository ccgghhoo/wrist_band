#ifndef __DFU_SETTING_PORT_H
#define __DFU_SETTING_PORT_H

#include "nrf.h"
#include "nrf_mbr.h"
#include "nrf_sdm.h"

#if defined(NRF51)
  #define CODE_PAGE_SIZE            (PAGE_SIZE_IN_WORDS * sizeof(uint32_t))
#elif defined(NRF52) || defined(NRF52840_XXAA)
  #define CODE_PAGE_SIZE            (MBR_PAGE_SIZE_IN_WORDS * sizeof(uint32_t))
#else
  #error "Architecture not set."
#endif

#define DFU_SETTINGS_ADDR_START	0x7E000
#define DFU_SETTINGS_ADDR_END	0x7F000


#define DFU_FIRMWARE_START_ADDR		0x7F000


#define DFU_APP_MAX_SIZE	(182 * 1024 )

#define INIT_COMMAND_MAX_SIZE       256     /**< Maximum size of the init command stored in dfu_settings. */


#include "app_flash_drv.h"

#include "nrf_fstorage.h"
//#include "fstorage.h"
/*
---------------------------------------------------------
0x7F000-0x7FFFF Device Factory Configuration (4K)		                |
---------------------------------------------------------
0x7E000-0x7EFFF DFU Settings (4K)						|
---------------------------------------------------------
0x7D000-0x7DFFF MBR Settings (4K)						|
---------------------------------------------------------
0x76000-0x7CFFF Bootloader(28K)							|
---------------------------------------------------------
0x18000-0x75FFF Application (376K) 
// include 
0x74000-0x75FFF Device Configuration(8K)
0x73000-0x73FFF Reserved for Meshtech(4K)

0x18000-0x72000 Application(364K)
dual bank mode - only accept 182K application 

---------------------------------------------------------
0x00000-0x17FFFF Softdevice(96K)						|
---------------------------------------------------------
*/


#endif

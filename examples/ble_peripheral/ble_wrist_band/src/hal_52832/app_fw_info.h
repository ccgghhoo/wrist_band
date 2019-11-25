

#ifndef FW_H__
#define FW_H__

#include <stdint.h>
#include "nrf_sdm.h"
#include "app_flash_drv.h"

// offset 8K, change if IROM1 start has been change 
#define APP_FW_START_ADDRESS			DFU_APP0_START_ADDR///0x1F000///26000


// ************* Do not modify from here!!!!! *******************
//	Important: allow to fetch information of firmware if defined 
// **************************************************************
#define APP_FW_INFO_START_ADDRESS	( APP_FW_START_ADDRESS + 0x2000 ) 
//#define APP_INFO_MAGIC				( APP_FW_INFO_START_ADDRESS + 0)
//#define APP_INFO_FW_VERSION			( APP_INFO_MAGIC + 4 ) 
//#define APP_INFO_FW_SIZE			( APP_INFO_FW_VERSION + 4 ) 
//#define APP_INFO_HW					( APP_INFO_FW_SIZE + 4 ) 
//#define APP_INFO_SD_API_REV			( APP_INFO_HW + 4 ) 
//#define APP_INFO_DATE				( APP_INFO_SD_API_REV + 4 ) 
//#define APP_INFO_TIME				( APP_INFO_DATE + 12 )
//#define APP_INFO_END				( APP_INFO_TIME + 8 ) 


#if 1
#define SD_MAJOR_VERSION_112  (6)
#define SD_MINOR_VERSION_112  (1)
#define SD_BUGFIX_VERSION_112 (0)
#define APP_FW_SD_FW_ID	       0x00b0
#else
#define SD_MAJOR_VERSION  (6)
#define SD_MINOR_VERSION  (1)
#define SD_BUGFIX_VERSION (0)
#define APP_FW_SD_FW_ID	  0x00af 
#endif

#define SD_VARIANT_ID_112 112
#define SD_VERSION_112 (SD_MAJOR_VERSION_112 * 1000000 + SD_MINOR_VERSION_112 * 1000 + SD_BUGFIX_VERSION_112)
#define APP_FW_SD_VERSION		SD_VERSION_112
 

#define APP_FW_SIZE				DFU_APP0_SIZE//(0x3C000 - 0x1000)
#define APP_FW_MAGIC_CODE		0xA991C0DE	//APPL CODE  
#define APP_FW_DATE				__DATE__
#define APP_FW_TIME				__TIME__ 


typedef struct{
	uint32_t magic; 
	uint32_t fw_version;
	uint32_t fw_size; 
	uint32_t hw_version;
	uint32_t sd_version; 
	uint32_t sd_fw_id; 		// actually, it's a 16-bit value 
	
	uint8_t date[12];
	uint8_t time[8]; 
}app_info_t; 


// firmware Version ( Major.Minor.Revsion.Inner ) 
#define APP_FW_MAJOR				0x00
#define APP_FW_MINOR				0x00
#define APP_FW_REV					0x00
#define APP_FW_INNER				0x02

#define FW_REVISION			        (( APP_FW_MAJOR << 24 ) | ( APP_FW_MINOR << 16 ) | ( APP_FW_REV << 8 ) | APP_FW_INNER )

#define FW_REVISION_STRING			"v0.1"

//#define APP_FW_HW_VER			    (0x90)


extern const app_info_t appl_info;

#endif 
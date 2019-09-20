

#ifndef DFU_FLASH_H__
#define DFU_FLASH_H__


#include <stdint.h>
#include <stdbool.h>

typedef enum{
	DFU_STEP_CHECK_BUSY = 0, 
	DFU_STEP_CHECK_SECTOR, 
	DFU_STEP_CHECK_BEFORE_WRITE, 
	
	DFU_STEP_WRITE,
	DFU_STEP_WAIT_COMPLETED, 
//	DFU_FLASH_VERIFY
}dfu_flash_step_t; 

enum{
	DFU_RET_SUCCESS = 0, 
	DFU_RET_WRITE_COMPLETED, 
};

enum{
	DFU_FLAG_IDLE		= 0, 
	DFU_FLAG_EXECUTING = 0x01, 
};

void dfu_flash_init(void); 

uint32_t dfu_flash_crc_compute( uint32_t addr, uint32_t size ); 

void dfu_flash_process(void); 

bool dfu_flash_write_update(uint32_t write_offset, const uint8_t* p_data, uint16_t size ); 

#endif 

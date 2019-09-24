
// flash --- app flash driver 


#include <stdint.h>
#include <stdbool.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "crc16.h"
#include "nrf_fstorage_sd.h"
//#include "app_spi_drv.h"
#include "app_flash_drv.h"
#include "app_nus.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


//static  uint32_t 				m_flash_in_used_count ; 
static  uint32_t                m_write_flash_result = 0; 



static void fs_storage_callback(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");        
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            //NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",p_evt->len, p_evt->addr);
             m_write_flash_result = 1;             
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            //NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",p_evt->len, p_evt->addr);
            m_write_flash_result = 2;             
        } break;

        default:
        	
            break;
    }    
       
}

NRF_FSTORAGE_DEF(nrf_fstorage_t app_flash_fs_config) =
{
    .evt_handler = fs_storage_callback,
    .start_addr = DFU_FLASH_START_ADDR,
    .end_addr   = DFU_FLASH_END_ADDR,
};


void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage))
    {
        (void) sd_app_evt_wait();
    }
}




bool flash_write_completed(void)
{
//    if(!nrf_fstorage_is_busy(&app_flash_fs_config))
//    {
//       app_nus_res_handler();
//    }
    
    return !nrf_fstorage_is_busy(&app_flash_fs_config);
}

bool flash_write_data(uint32_t addr, uint8_t *data, uint16_t len)
{
    //m_write_flash_result = 0xffffffff; 
    uint32_t ret_code = nrf_fstorage_write(&app_flash_fs_config,
                              addr,
                              data,
                              len,
                              NULL);
//    wait_for_flash_ready(&app_flash_fs_config);  //wait for write completed
    return ret_code;
}

bool flash_read_data(uint32_t addr, uint8_t *data, uint16_t len)
{
	//memcpy(data , (uint8_t*)addr, len);
    uint32_t ret_code = nrf_fstorage_read(&app_flash_fs_config,
                             addr,
                             data,
                             len);
    return ret_code;                         
}

bool flash_erase_sector(uint32_t addr)
{
    //m_write_flash_result = 0xffffffff; 
    uint32_t ret_code = nrf_fstorage_erase(&app_flash_fs_config,
                              addr,
                              1,                 //1*4096
                              NULL);
//    wait_for_flash_ready(&app_flash_fs_config);
    return ret_code;
}

void app_dfu_flash_init(void)
{
    
    ret_code_t rc;
    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;    
    rc = nrf_fstorage_init(&app_flash_fs_config, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);  //chen	

}

void flash_op_test(void)
{
     NRF_LOG_INFO("erase flash test starting !!");
    uint32_t addr=(DFU_APP1_START_ADDR);
    m_write_flash_result = 0;          
    uint32_t ret_code = nrf_fstorage_erase(&app_flash_fs_config,
                              addr,
                              (DFU_FLASH_END_ADDR-DFU_FLASH_START_ADDR)/4096,   //total page
                              NULL);
    NRF_LOG_INFO("erase flash test end  result:%d :%d !!", ret_code, nrf_fstorage_is_busy(&app_flash_fs_config));
    //while(m_write_flash_result == 0);
    wait_for_flash_ready(&app_flash_fs_config);
    
    m_write_flash_result = 0;
    uint8_t buff[]={"123456781234"};
    ret_code = flash_write_data(DFU_APP1_START_ADDR, buff, 12);
       
    NRF_LOG_INFO("write flash result:%d :%d !!", ret_code, nrf_fstorage_is_busy(&app_flash_fs_config));
    //while(m_write_flash_result == 0);
    wait_for_flash_ready(&app_flash_fs_config);
    
    NRF_LOG_INFO("write flash completed:%d !!" ,m_write_flash_result);
    
    uint8_t readbuff[20];
    flash_read_data(DFU_APP1_START_ADDR, readbuff, 20);
    
    for(uint32_t i=0; i<20; i++)
    {
        NRF_LOG_INFO("read flash:%d !!", readbuff[i]);
    }
    
    
}














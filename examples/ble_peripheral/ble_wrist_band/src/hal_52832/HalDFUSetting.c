/**
 * 1.支持带SoftDevice与不带SoftDevice的操作 
 * 2.是在fstorage上做了一次操作。 3. 参数保存?
 * 3. 参数保存在 
 *  #define DFU_SETTINGS_ADDR_START	0x7E000
 *	#define DFU_SETTINGS_ADDR_END	0x7F000
 *  
 *  
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nrf_delay.h"

#include "LibHeap.h"
#include "dfu.types.h"
#include "dfu.settings.h"
#include "crc32.h"

#include "nrf_fstorage_sd.h"
#include "app_flash_drv.h"
#include "app_scheduler.h"


//static dfu_settings_t *s_dfu_settings;


static void fs_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> dfu_setting_fs : ERROR while executing an fstorage operation.");        
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> dfu_setting_fs: wrote %d bytes at address 0x%x.",p_evt->len, p_evt->addr);
                        
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> dfu_setting_fs: erased %d page from address 0x%x.",p_evt->len, p_evt->addr);
                        
        } break;

        default:
        	
            break;
    }    
       
}

NRF_FSTORAGE_DEF(nrf_fstorage_t fs_dfu_config) =
{
    .evt_handler = fs_evt_handler,
    .start_addr = DFU_SETTINGS_ADDR_START,
    .end_addr   = DFU_SETTINGS_ADDR_END,
};

void dfu_setting_flash_init(void)
{
    ret_code_t rc;
    nrf_fstorage_api_t * p_fs_api;
    p_fs_api = &nrf_fstorage_sd;    
    rc = nrf_fstorage_init(&fs_dfu_config, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
    
}


uint32_t PortDFUSetting_Save(dfu_settings_t *s_dfu_settings, void (*callback)(uint32_t reson))
{
  
//    s_dfu_settings = ( dfu_settings_t *) APP_MALLOC( DFU_SAVE_SETTING_BUFFER, sizeof( dfu_settings_t ) ); 
//      
//	if( s_dfu_settings == NULL ){
//		return NRF_ERROR_NO_MEM; 
//	}
//    
//    memcpy( s_dfu_settings, (uint32_t*)fs_dfu_config.start_addr, sizeof(dfu_settings_t ));     
   
    uint32_t ret_code = nrf_fstorage_erase(&fs_dfu_config,
                              fs_dfu_config.start_addr,     //0X7F000 ,
                              1,                            //1*4096
                              NULL);
    if(ret_code!=NRF_SUCCESS)
    {
        return NRF_ERROR_BUSY;
    }
    
    wait_for_flash_ready(&fs_dfu_config);
    
    ret_code = nrf_fstorage_write(&fs_dfu_config,
                                       fs_dfu_config.start_addr,//0X7E000 ,
                                       s_dfu_settings,
                                       sizeof(dfu_settings_t),
                                       NULL);
    
	if(ret_code!=NRF_SUCCESS)
    {
        NRF_LOG_INFO("dfu_setting_fs  write fail !!!!");         
        return NRF_ERROR_BUSY;
        
    }
    
    wait_for_flash_ready(&fs_dfu_config);
    
    if(callback!=NULL)
    {
        NRF_LOG_INFO("dfu_setting_fs  reset device ,, dfu sucessed !!!!");   
        callback(0);
    }
    
	return NRF_SUCCESS; 
 
  
  
  
  
  
/*  
  
  ret_code_t err_code = FS_SUCCESS;

  wait_for_pending();

  flash_operation_pending = true;
  m_callback = callback;

  do
  {
    wait_for_queue();

    // Not setting the callback function because ERASE is required before STORE
    // Only report completion on successful STORE.
    err_code = dfu_flash_erase((uint32_t *)fs_dfu_config.p_start_addr, 1, NULL);

  } while (err_code == FS_ERR_QUEUE_FULL);

  if (err_code != FS_SUCCESS)
  {
//        NRF_LOG_ERROR("Erasing from flash memory failed.\r\n");
    flash_operation_pending = false;
    return NRF_ERROR_INTERNAL;
  }


  do
  {
    wait_for_queue();

    err_code = dfu_flash_store((uint32_t *)fs_dfu_config.p_start_addr,
                               (uint32_t *)s_dfu_settings,
                               sizeof(dfu_settings_t) / 4,
                               dfu_settings_write_callback);

  } while (err_code == FS_ERR_QUEUE_FULL);

  return FS_SUCCESS;

*/
}





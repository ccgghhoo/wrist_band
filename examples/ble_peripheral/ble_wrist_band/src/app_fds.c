#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "fds.h"
#include "app_fds.h"
#include "nrf_log.h"

/* Flag to check fds initialization. */
static bool volatile m_fds_initialized;
static bool volatile m_fds_write_completed=true;
static fds_record_desc_t m_history_data_desc = {0};
uint8_t  m_history_data[32]={1,2,3,4,5,6,7,8,9,0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,};

/* A record containing dummy configuration data. */
static fds_record_t const m_history_data_record =
{
    .file_id                 = HISTORY_FILE,
    .key                     = HISTORY_REC_KEY,
    .data.p_data             = &m_history_data,
    /* The length of a record is always expressed in 4-byte units (words). */
    .data.length_words = (sizeof(m_history_data) + 3) / sizeof(uint32_t),
};



static void app_fds_evt_handler(fds_evt_t const * p_evt)
{
    switch (p_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_evt->result == FDS_SUCCESS)
            {
                m_fds_initialized = true;
            }
            break;

        case FDS_EVT_WRITE:
        {
            if (p_evt->result == FDS_SUCCESS)
            {
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->write.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->write.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->write.record_key);
                m_fds_write_completed = true;
            }
        } break;

        case FDS_EVT_DEL_RECORD:
        {
            if (p_evt->result == FDS_SUCCESS)
            {
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->del.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->del.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->del.record_key);
            }
            //m_delete_all.pending = false;
        } break;

        default:
            break;
    }
}
	 
void app_fds_init(void)
{	
	ret_code_t rc;
    /* Register first to receive an event when initialization is complete. */
    (void) fds_register(app_fds_evt_handler);

    NRF_LOG_INFO("Initializing fds...");

    rc = fds_init();
    APP_ERROR_CHECK(rc);

    /* Wait for fds to initialize. */
    while((!m_fds_initialized));
	
	fds_stat_t stat = {0};

    rc = fds_stat(&stat);
    APP_ERROR_CHECK(rc);

    NRF_LOG_INFO("Found %d valid records.", stat.valid_records);
    NRF_LOG_INFO("Found %d dirty records (ready to be garbage collected).", stat.dirty_records);
    
    if(stat.dirty_records!=0)
    {
      fds_gc();  //回收垃圾
    }
    
    fds_find_token_t  tok  = {0};
    //for(uint32_t i=0; i<20;i++)  //for test 
    {
      rc = fds_record_find(HISTORY_FILE, HISTORY_REC_KEY, &m_history_data_desc, &tok);
      if (rc != FDS_SUCCESS)
      {
		NRF_LOG_INFO("Config file not found");
      }
      else
      {
        NRF_LOG_INFO("found in page= %x  at page_address=%x ", tok.page, tok.p_addr);
        for(uint32_t i=0; i<32; i++)
        {
          //NRF_LOG_INFO("data=%x ", *((uint8_t*)tok.p_addr+12+i));   //hearder length=12  
          //NRF_LOG_INFO("data=%x ", *((uint8_t*)m_history_data_desc.p_record+12+i))
        }
        NRF_LOG_INFO("desc_precord= %x ", m_history_data_desc.p_record);
        //tok.p_addr == m_history_data_desc.p_record 这两个地址是相等的
      }
    }
	
} 

void  history_data_save(void)
{
  
  
    if(!m_fds_write_completed) return;

      m_fds_write_completed = false;
      ret_code_t rc;      
      
	  rc = fds_record_write(&m_history_data_desc, &m_history_data_record);//save data one by one  not overwrite
      //rc = fds_record_update(&desc, &m_history_data_record);  //overwrite the last data
      NRF_LOG_INFO("flash write result= %x ", rc);
	  if(rc == FDS_ERR_NO_SPACE_IN_FLASH)
      {        
        fds_file_delete(HISTORY_FILE);
        //m_history_data_desc.record_id = 0;
        //m_history_data_desc.p_record =      //开始存放的地址
        fds_gc();
        return;
      }
     APP_ERROR_CHECK(rc);	
      

      
}


    
     

















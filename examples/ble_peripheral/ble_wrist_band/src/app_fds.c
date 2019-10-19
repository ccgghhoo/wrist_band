
#include "app_init.h"

/* Flag to check fds initialization. */
static bool volatile m_fds_initialized;
static bool volatile m_fds_write_completed=true;
static fds_record_desc_t m_history_data_desc = {0};
uint8_t  m_history_data[32]={1,2,3,4,5,6,7,8,9,0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,};
uint8_t  m_day_index =0;

/*
进行大数据存储时 可以用file_id来表示哪一天  比如file_id=1代表第一天的数据 =2代表第二天的数据，依次类推
这样就可以根据file_id找到对应天数写入，读出，或者清除.

可以用key_id代表不同类型的数据，比如不同的key id分别对应配置信息数据，计步数据，活跃度数据，心率数据等等
*/

// Function to convert day index to file IDs.
static uint16_t day_id_to_file_id(uint8_t  day_index)
{
    return (uint16_t)(day_index + APP_FILE_ID_BASE);
}


/* A record containing  configuration data. */
static fds_record_t  m_sport_level_record =
{
    .file_id                 = APP_FILE_ID_BASE,
    .key                     = APP_REC_KEY_ID_BASE,
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
        rc = fds_record_find(APP_FILE_ID_BASE, APP_REC_KEY_ID_BASE, &m_history_data_desc, &tok);
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
            //tok.p_addr == m_history_data_desc.p_record 这两个地址是相等的 指的是实际物理地址
            //tok.page  这个页指的是虚拟页号 相对于所配置的存储区域来说的 不是整个flash上的页号
        }
    }
	
} 

void  history_data_save(void)
{
    
#if 1  
    if(!m_fds_write_completed) return;
    
    m_fds_write_completed = false;
    ret_code_t rc;
    
    rc = fds_record_write(NULL/*&m_history_data_desc*/, &m_sport_level_record);//save data one by one  not overwrite
    //rc = fds_record_update(&desc, &m_sport_level_record);  //overwrite the last data
    NRF_LOG_INFO("flash write result= %x ", rc);
    if(rc == FDS_ERR_NO_SPACE_IN_FLASH)
    {        
        fds_file_delete(m_sport_level_record.file_id);
        fds_gc();
        return;
    }
    APP_ERROR_CHECK(rc);	
    
#endif
    
}

void app_fds_new_day_handle(void)
{    
    m_day_index++;
    if(m_day_index>=MAX_FLASH_SAVE_DAY)
    {
        m_day_index = 0;              
    }
    
    m_sport_level_record.file_id = day_id_to_file_id(m_day_index);
    
    fds_file_delete(m_sport_level_record.file_id);
    fds_gc();
    
}




















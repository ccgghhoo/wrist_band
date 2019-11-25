
#include "app_init.h"

#define  ONE_HOUR_DATA_NUM    96       // 8bytes/5mins

/* Flag to check fds initialization. */
static bool volatile m_fds_initialized;
static bool volatile m_fds_write_completed=true;
static bool volatile m_delete_file_id_completed=true;
static fds_record_desc_t m_sport_record_desc = {0};
static find_file_t  m_find_token={0};


uint8_t  m_save_data_buff[ONE_HOUR_DATA_NUM]={0} ,m_save_data_cnt=0;
uint8_t  m_read_data_buff[ONE_HOUR_DATA_NUM]={0};
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
    .key                     = APP_REC_KEY_ID_SPORT_LVL,
    .data.p_data             = NULL,
    /* The length of a record is always expressed in 4-byte units (words). */
    .data.length_words = 0, //(sizeof(m_history_data) + 3) / sizeof(uint32_t),
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
                NRF_LOG_INFO("DEL BY RECORDID SUCCESS !!");
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->del.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->del.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->del.record_key);
            }
            //m_delete_all.pending = false;
        } break;
        
        case FDS_EVT_DEL_FILE:
        {
            if (p_evt->result == FDS_SUCCESS)
            {
                NRF_LOG_INFO("DEL BY FILEID SUCCESS !!");
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->del.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->del.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->del.record_key);
            }
            m_delete_file_id_completed = true;
        } break;
        
        case FDS_EVT_GC:
        if (p_evt->result == FDS_SUCCESS)
        {
            NRF_LOG_INFO("gc successed !!");
        }
        
        break;
        
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
        //fds_gc();  //回收垃圾
    }
    
    fds_find_token_t  tok  = {0};
    //for(uint32_t i=0; i<20;i++)  //for test 
    {
        rc = fds_record_find(APP_FILE_ID_BASE, APP_REC_KEY_ID_BASE, &m_sport_record_desc, &tok);
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
                //NRF_LOG_INFO("data=%x ", *((uint8_t*)m_sport_record_desc.p_record+12+i))
            }
            NRF_LOG_INFO("desc_precord= %x ", m_sport_record_desc.p_record);
            //tok.p_addr == m_sport_record_desc.p_record 这两个地址是相等的 指的是实际物理地址
            //tok.page  这个页指的是虚拟页号 相对于所配置的存储区域来说的 不是整个flash上的页号
        }
    }
	
} 

void  sport_level_data_save(uint32_t *pdata, uint16_t len)
{
    
#if 1  
            
    ret_code_t rc;
    
    if(m_save_data_cnt+len<=sizeof(m_save_data_buff))
    {
        memcpy(&m_save_data_buff[m_save_data_cnt], pdata, len);
        m_save_data_cnt+=len;
    }
    else
    {        
        m_save_data_cnt = sizeof(m_save_data_buff);//this package is lost
    }
    
    if(m_save_data_cnt==sizeof(m_save_data_buff))
    {        
        m_sport_level_record.data.p_data = m_save_data_buff;
        m_sport_level_record.data.length_words = (sizeof(m_save_data_buff) + 3) / sizeof(uint32_t);
    }
    else
    {
        return;
    }
    
//    m_sport_level_record.data.p_data = pdata;
//    m_sport_level_record.data.length_words = (len + 3) / sizeof(uint32_t); //align(4)
    if(m_fds_write_completed)
    {
        m_fds_write_completed = false;
        rc = fds_record_write(NULL/*&m_sport_record_desc*/, &m_sport_level_record);//save data one by one  not overwrite
        //rc = fds_record_update(NULL, &m_sport_level_record);  //overwrite the last data
        NRF_LOG_INFO("flash write result= %x ", rc);
        if(rc != FDS_SUCCESS)
        {
            m_fds_write_completed = true;  //不成功  下次重新写
        }
        else
        {
            m_save_data_cnt = 0; //
        }
        
        if(rc == FDS_ERR_NO_SPACE_IN_FLASH)
        {           
            fds_gc();
            return;
        }
        
        APP_ERROR_CHECK(rc);
    }	
    
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
    
    NRF_LOG_INFO("new day file_id= %x ",m_sport_level_record.file_id);
    
    m_delete_file_id_completed = false;
    fds_file_delete(m_sport_level_record.file_id);
    
    while(m_delete_file_id_completed==false);
    ret_code_t rc;    
    fds_stat_t stat = {0};   
    rc = fds_stat(&stat);
    APP_ERROR_CHECK(rc);
    
    NRF_LOG_INFO("Found %d valid records.", stat.valid_records);
    NRF_LOG_INFO("Found %d dirty records (ready to be garbage collected).", stat.dirty_records);
    //fds_gc();
    
}

void reset_find_token(void)
{
    m_find_token.b_find_begin = 0;
    m_find_token.b_find_end = 0;
    m_find_token.ftoken.page =0;
    m_find_token.ftoken.p_addr = 0;
    m_find_token.read_day_index = 0;
    m_find_token.delay_s = 0;
}


void  sport_level_data_read(uint32_t *readDataBuff, uint16_t  *ReadLength, uint8_t whichDay)
{       
    if(m_find_token.b_find_begin == 0 || m_find_token.read_day_index != whichDay)
    {
        reset_find_token();
        m_find_token.read_day_index = whichDay;
        m_find_token.b_find_begin = 1;
    }
    
    m_find_token.file_id = day_id_to_file_id(m_find_token.read_day_index);
    
    if(fds_record_find_in_file(m_find_token.file_id,&m_find_token.fdesc,&m_find_token.ftoken) == FDS_SUCCESS)
    {
        fds_header_t const * p_header = (fds_header_t*)m_find_token.ftoken.p_addr;
        *ReadLength = sizeof(uint32_t)*p_header->length_words;
        memcpy(readDataBuff, m_find_token.ftoken.p_addr+3, sizeof(uint32_t)*p_header->length_words);       
    }
    else
    {
        reset_find_token();
        *ReadLength = 0;
        m_find_token.b_find_end = 1;
    }                                                                         
}


bool  find_read_sport_record(uint32_t *readDataBuff, uint16_t  *ReadLength, uint8_t whichDay)
{ 
    
    if(m_find_token.b_find_end)
    {
        if(++m_find_token.delay_s>3620) //
        {
            reset_find_token();
        }
        else
        {
            return false;
        } 
    }
    
    if(m_find_token.b_find_begin == 0)
    {
        reset_find_token();
        m_find_token.b_find_begin = 1;
        m_find_token.retry_cnt = 0;        
    }
    
    //m_find_token.file_id = day_id_to_file_id(m_find_token.read_day_index);    
    //if(fds_record_find_in_file(m_find_token.file_id,&m_find_token.fdesc,&m_find_token.ftoken) == FDS_SUCCESS)
    if(fds_record_find_by_key(APP_REC_KEY_ID_SPORT_LVL,&m_find_token.fdesc,&m_find_token.ftoken) == FDS_SUCCESS)
    {
        fds_header_t const * p_header = (fds_header_t*)m_find_token.ftoken.p_addr;
        *ReadLength = sizeof(uint32_t)*p_header->length_words;
        readDataBuff = (uint32_t *)m_read_data_buff;
        memcpy(readDataBuff, m_find_token.ftoken.p_addr+3, sizeof(uint32_t)*p_header->length_words);          
        //fds_record_delete(&m_find_token.fdesc);
        return true;
    }
    else
    {
        reset_find_token();
        *ReadLength = 0;       
//        m_find_token.read_day_index++;
//        if(m_find_token.read_day_index>=MAX_FLASH_SAVE_DAY)
        {
            m_find_token.retry_cnt ++;
//            m_find_token.read_day_index = 0;
            if(m_find_token.retry_cnt>=2)
            {
                m_find_token.b_find_end = 1;
            }
            
        }
        return false;
    }   
   
}

bool del_current_sport_record(void)
{    
    uint32_t ret = fds_record_delete(&m_find_token.fdesc);
    return ret==0;
}





void read_file_data_test(void)
{
    uint8_t temp[128] = {0};
    uint16_t read_len =0;           
    sport_level_data_read((uint32_t*)&temp[0],  &read_len, m_day_index);  //read_len = 96bytes/hour

    NRF_LOG_INFO("reciverd bytes=%d %d" ,read_len, m_day_index);  
    if(read_len!=0)
    {         
        NRF_LOG_HEXDUMP_INFO((uint8_t *)&temp, read_len/2);  //长度好像不能超过80字节
        NRF_LOG_HEXDUMP_INFO((uint8_t *)&temp[read_len/2], read_len/2);
    }
    else
    {
        m_day_index++;
        if(m_day_index>4)
        {
            m_day_index=0;
        }
    }
}


















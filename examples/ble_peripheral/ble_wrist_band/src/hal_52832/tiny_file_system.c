#include "tiny_file_system.h"
#include "fstorage.h"
#include "crc16.h"
//#include "Qfile_error_handler.h"
#include "nrf_delay.h"
#include "app_util_platform.h"

#define USER_ERROR_CHECK(...)

#if 1
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define APP_LOG									NRF_LOG_INFO
#else
#define APP_LOG(...)
#endif 

#if 1
#define __sCritical_region_enter__				CRITICAL_REGION_ENTER()
#define __sCritical_region_exit__		    	CRITICAL_REGION_EXIT()
#else
#define __sCritical_region_enter__
#define __sCritical_region_exit__
#endif

#define S_TINY_FS_RECORD_ID						( 0xC0ED )
#define CONF_PAGE_SIZE   						( 0x1000 )
#define S_REC_INVALID_ADDR        				( 0xFFFFFFFFUL ) 

//#define S_DEGREE_RECORD_HEAD_SIZE       (2 * CONF_PAGE_SIZE) // data size  = 2 page
//#define S_DEGREE_RECORD_HEAD_MARK       (S_DEGREE_RECORD_HEAD_SIZE - 1)

enum
{
	UPDATE_WP_EVENT = 1,
	UPDATE_RP_EVENT,
};

typedef struct
{
	uint32_t wp;
	uint32_t rp;
} rom_op_pointer_t;

typedef struct
{
	uint16_t crc;
	uint16_t id;
    uint32_t reserve;// for test, data erase time, point erase time.
	rom_op_pointer_t op_pointer;
} rom_op_record_t;

typedef struct
{
	uint32_t 	start_addr;	// nums of sector
	uint32_t 	end_addr; 			
} rom_addr_config_t;

static void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result);
static void sUpdate_record_pointer(uint8_t event);
static void sConfig_default_record_pointer(void);

FS_REGISTER_CFG(fs_config_t fs_tiny_fs_system_record) =
{
    .callback  = fs_evt_handler,
    .num_pages = 3,
    // We register with the highest priority in order to be assigned
    // the pages with the highest memory address (closest to the bootloader).

    .priority  = 0xEF,
};

static rom_addr_config_t	 m_rec_data_config 	  = {	.start_addr = S_REC_INVALID_ADDR, 	.end_addr = S_REC_INVALID_ADDR};
static rom_addr_config_t	 m_rec_pointer_config = {	.start_addr = S_REC_INVALID_ADDR, 	.end_addr = S_REC_INVALID_ADDR};

static uint32_t                         m_op_pointer_read_weak = 0;
static rom_op_pointer_t      m_op_pointer;
static bool					 m_record_pointer_update = false;
static bool					 m_degree_data_update    = false;

static rom_op_record_t 		 m_op_record_head_clear, m_op_record_head;

static uint32_t              m_point_head_addr       = S_REC_INVALID_ADDR;
static uint32_t              m_last_write_address    = 0;
static uint8_t               m_fstorage_op_mutex     = 0;
//static uint32_t              m_last_point_address = 0;

static bool 				 m_is_initalize_flag     = false;
static bool                  flash_operation_pending = false; // barrier for reading flash

static Tinyfs_file_data_t    m_tinyfs_file_data;
static uint16_t              m_data_erase_times = 0, m_pointer_erase_times = 0;
static void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result)
{
    switch (evt->id)
    {
        case FS_EVT_STORE:
		{
			if( result == FS_SUCCESS )
			{
				if (true == m_degree_data_update)
				{
					m_degree_data_update = false; 		//clear the write the user data event flag.
					flash_operation_pending = false; 	//clear the pending flag
					sUpdate_record_pointer(UPDATE_WP_EVENT);

					Tinyfs_file_data_t *op_record_header = NULL;
					op_record_header = (Tinyfs_file_data_t *)m_last_write_address;
					
                    //check 
					uint16_t Crc16 = crc16_compute( (uint8_t *)&m_tinyfs_file_data.file_data, sizeof(m_tinyfs_file_data.file_data), 0); 
					if ( (op_record_header->file_id == S_TINY_FS_RECORD_ID) &&
						(op_record_header->file_crc == Crc16) &&
						(0 == memcmp(op_record_header->file_data, m_tinyfs_file_data.file_data, sizeof(Tinyfs_file_data_t))) )
					{
						APP_LOG("[evt] WRITE OK!\r\n");
					}
                    else
                    {
                        ;//Reach here, have no idea now!
                    }
				}
                if ( true == m_record_pointer_update )
                {
					__sCritical_region_enter__;
					-- m_fstorage_op_mutex ;
					__sCritical_region_exit__;
					if ( 0 == m_fstorage_op_mutex )
					{
						APP_LOG("[evt] UPDATE POINT OK.\r\n");
						
						m_record_pointer_update = false; //clear the record pointer update event flag.
						flash_operation_pending = false; //clear the pending flag
					}
                }
			}
			else
			{
				flash_operation_pending = false;
				m_fstorage_op_mutex 	= 0;
			}
			break;
		}
        case FS_EVT_ERASE:
		{
			if( result == FS_SUCCESS )
			{
				APP_LOG("[evt] ERASE OK.\r\n");
			}
			else
			{
				flash_operation_pending = false;
				m_fstorage_op_mutex 	= 0;
			}
			break;
		}
		case FS_ERR_INTERNAL:
		{
			APP_LOG("[evt] FS_ERR EVENT.\r\n");
            sConfig_default_record_pointer();
            
			flash_operation_pending = false;
			m_fstorage_op_mutex 	= 0;
			break;
		}
    }
}


static bool sCheck_op_data_addr_before_use_it(uint32_t op_addr) 		//data area addr range.
{
    if ( (op_addr >= m_rec_data_config.start_addr) && 
		(op_addr < m_rec_data_config.end_addr)     &&
		(0 == (op_addr & (0x1000 - 1)) % sizeof(Tinyfs_file_data_t) )	//alian the addr
	)
    {
        return true;
    }
    return false;
}


static bool sCheck_op_pointer_addr_before_use_it(uint32_t op_addr)		//pointer addr range.
{
    if ( (op_addr >= m_rec_pointer_config.start_addr) && 
		(op_addr < m_rec_pointer_config.end_addr)     &&
		(0 == (op_addr & (0x1000 - 1)) % sizeof(rom_op_record_t)) )		//alian the addr
    {
        return true;
    }
    return false;
}


static void sWait_for_queue(void)
{
    static uint32_t timeouts = 0;
    while ( fs_queue_is_full())
    {
        nrf_delay_ms(100);
        if ( ++timeouts >= 15) //wait max time = 1500ms
        {
            break;
        }
    }
}


static void sWait_for_pending(void)
{
    static uint32_t timeouts = 0;
    while (flash_operation_pending == true )
    {
        nrf_delay_ms(100);
        if ( ++timeouts >= 15) //wait max time = 1500ms
        {
            break;
        }        
    }
}


bool Tinyfs_push_new_record(uint8_t *p_dest, uint16_t input_size)
{
    if ( false == m_is_initalize_flag )
    {
		USER_ERROR_CHECK(ERROR_CODE_PARAMETER);
        return false;
    }
    
	//check 
	if ( (NULL == p_dest) || (0 == input_size) || (input_size > sizeof(m_tinyfs_file_data.file_data)) )
	{
		USER_ERROR_CHECK(ERROR_CODE_PARAMETER);
		return false;
	}

	//check before use the pointer.
    if ( false == sCheck_op_data_addr_before_use_it(m_op_pointer.wp) )
	{
		sConfig_default_record_pointer();
		
		APP_LOG("[CONF] operation_addr vaild.\r\n");
		return false;
	}
	uint32_t operation_addr = (uint32_t)m_op_pointer.wp;
	
    sWait_for_pending();
    flash_operation_pending = true;

	//is new sector address. must erase the sector.
	if ( 0 == operation_addr % CONF_PAGE_SIZE )
	{
        sWait_for_queue();

		uint32_t err_code = fs_erase( & fs_tiny_fs_system_record, (const uint32_t *)operation_addr, 1, NULL ); 
		if( err_code != FS_SUCCESS )
		{
            USER_ERROR_CHECK(err_code);
			APP_LOG("[CONF]: erase error = 0x%X\r\n", err_code ); 
		}
		APP_LOG("[CONF]: erase page:0x%02x\r\n", operation_addr);
		++ m_data_erase_times;
	}
	//write data.
    sWait_for_queue();
	memset(m_tinyfs_file_data.file_data, 0x00, sizeof(m_tinyfs_file_data.file_data));
	uint16_t copy_size = (input_size / sizeof(uint32_t)) * sizeof(uint32_t);
	memcpy(m_tinyfs_file_data.file_data, p_dest, copy_size);
	
	m_tinyfs_file_data.file_id  = S_TINY_FS_RECORD_ID;
	uint16_t Crc16 = crc16_compute( (uint8_t *)&m_tinyfs_file_data.file_data, sizeof(m_tinyfs_file_data.file_data), 0); 
	m_tinyfs_file_data.file_crc = Crc16;

	const uint32_t *p_data = (uint32_t *)&m_tinyfs_file_data;
	uint32_t err_code = fs_store( & fs_tiny_fs_system_record, (const uint32_t *)operation_addr, p_data, sizeof(Tinyfs_file_data_t)/sizeof(uint32_t), NULL ); 
	if ( err_code != FS_SUCCESS) 
    {
        USER_ERROR_CHECK(err_code);
		APP_LOG("[CONF]: store error = 0x%X\r\n", err_code ); 
    }

	m_last_write_address = operation_addr;
	//update the pointer record.
	m_op_pointer.wp += sizeof(Tinyfs_file_data_t);
	if ( m_op_pointer.wp >= m_rec_data_config.end_addr )
	{
		m_op_pointer.wp = m_rec_data_config.start_addr;
	}
	
	//for verify
	m_degree_data_update = true;

    return true;
}


static void sConfig_default_record_pointer(void)
{
	m_op_pointer.wp = m_rec_data_config.start_addr;
	m_op_pointer.rp = m_rec_data_config.start_addr;
}


static uint32_t sPoint_current_sector(uint32_t addr)
{
	if ( (addr >= m_rec_data_config.start_addr) && 
		(addr < (m_rec_data_config.start_addr + CONF_PAGE_SIZE)) )		
	{
		return m_rec_data_config.start_addr;
	}
	else if ( (addr >= (m_rec_data_config.start_addr + CONF_PAGE_SIZE)) && 
		 (addr < (m_rec_data_config.start_addr + 2*CONF_PAGE_SIZE)) )
	{
		return m_rec_data_config.start_addr + CONF_PAGE_SIZE;
	}
	else
	{
		USER_ERROR_CHECK(ERROR_CODE_PARAMETER);
		return S_REC_INVALID_ADDR;
	}
}


uint16_t Tinyfs_pop_exist_record(uint8_t *p_dest, uint16_t max_size)
{
#if 0
	//verify
	rom_op_record_t *op_record_header = NULL;
	op_record_header = (rom_op_record_t *)(m_last_point_address);
	if ( op_record_header->id == S_TINY_FS_RECORD_ID 
		 
//                        op_record_header->op_pointer.rp == m_op_pointer.rp &&
//                        op_record_header->op_pointer.wp == m_op_pointer.wp
        )
	{
		APP_LOG("[check] CHECK OK!\r\n");	
	}
	else
	{
		sConfig_default_record_pointer();
        return 0;
	}
#endif

	//assert
    if ( false == m_is_initalize_flag )
    {
		USER_ERROR_CHECK(ERROR_CODE_PARAMETER);
        return 0;
    }
    if ( (NULL == p_dest) || (max_size > TINY_FS_READ_MAX_SIZE) )
    {
		USER_ERROR_CHECK(ERROR_CODE_PARAMETER);
        return 0;
    }

	//check before use the pointer.
    if ( false == sCheck_op_data_addr_before_use_it(m_op_pointer.rp) )
	{
		sConfig_default_record_pointer();
		
		APP_LOG("[CONF] operation_addr vaild.\r\n");
		return 0;
	}
	if ( m_op_pointer.rp == m_op_pointer.wp )
	{
		APP_LOG("[CONF] have no data in queue.\r\n");
		return 0;
	}
	//check the wp and rp
	uint32_t wp_sector = sPoint_current_sector(m_op_pointer.wp);
	uint32_t rp_sector = sPoint_current_sector(m_op_pointer.rp);
    
	if ( (wp_sector == rp_sector) &&  
		(m_op_pointer.rp > m_op_pointer.wp) )
	{
		sConfig_default_record_pointer();
		APP_LOG("[CONF] operation_addr vaild.\r\n");
       
		return 0;
	}

    
	//copy the data.
    uint16_t one_point_size    = sizeof(Tinyfs_file_data_t);
    uint16_t numbers_byte_cocy = 0;

    sWait_for_pending();
    __sCritical_region_enter__;	
    flash_operation_pending = true;
    __sCritical_region_exit__;
    
    m_op_pointer_read_weak = m_op_pointer.rp;
    
    Tinyfs_file_data_t *p_read_addr  = (Tinyfs_file_data_t *)m_op_pointer_read_weak;
    Tinyfs_file_data_t *p_write_addr = (Tinyfs_file_data_t *)p_dest;
    
//	uint32_t operation_addr = (uint32_t)m_op_pointer_read_weak;
    for ( ; ; )
    {
        if ( (m_op_pointer_read_weak == m_op_pointer.wp) || (numbers_byte_cocy >= max_size) )
        {
            break;
        }
        
        //check before use the pointer.
        if ( false == sCheck_op_data_addr_before_use_it(m_op_pointer_read_weak) )
        {
            sConfig_default_record_pointer();
            break;
        }
        
		//check the file record.
		uint16_t Crc16 = crc16_compute( (uint8_t *)&p_read_addr->file_data, sizeof(p_read_addr->file_data), 0); 
		if ( (S_TINY_FS_RECORD_ID == p_read_addr->file_id) && 
			(Crc16 == p_read_addr->file_crc))
		{
			// copy.
			*p_write_addr ++   = *p_read_addr;

			numbers_byte_cocy += one_point_size; 
		}
        
		//update the rp now.
		__sCritical_region_enter__;
		m_op_pointer_read_weak    += one_point_size;
		if ( m_op_pointer_read_weak >= m_rec_data_config.end_addr )
		{
			 m_op_pointer_read_weak = m_rec_data_config.start_addr;
		}
		__sCritical_region_exit__;

        
		//next read addr.
		p_read_addr  = (Tinyfs_file_data_t *)m_op_pointer_read_weak;
    }
//	sUpdate_record_pointer(UPDATE_RP_EVENT);
	APP_LOG("[] m_op_pointer wp =  0x%02x\r\n", (uint32_t)m_op_pointer.wp);
	APP_LOG("[] m_op_pointer rp =  0x%02x\r\n", (uint32_t)m_op_pointer.rp);	
	
	APP_LOG("[] m_data_erase_times    =  0x%02x\r\n", (uint16_t)m_data_erase_times);	
	APP_LOG("[] m_pointer_erase_times =  0x%02x\r\n", (uint16_t)m_pointer_erase_times);	
    
	__sCritical_region_enter__;	
    flash_operation_pending = false;
	__sCritical_region_exit__;

    return (numbers_byte_cocy);
}


void Tinyfs_update_read_pointer(bool report_flag) //¸üÐÂ¶ÁÖ¸Õë
{
    if ( report_flag )
    {
        if ( false == sCheck_op_data_addr_before_use_it(m_op_pointer_read_weak) ) //check before use the pointer.
        {
            sConfig_default_record_pointer();
        }
        m_op_pointer.rp = m_op_pointer_read_weak;
    }
}

bool Tingfs_queue_is_empty(void)
{
    if ( m_op_pointer.rp == m_op_pointer.wp )
    {
        return true;
    }
    return false;
}


static void sUpdate_record_pointer(uint8_t event)//update the recover pointer
{
	switch ( event )
	{
		case UPDATE_WP_EVENT:
		{
		//	APP_LOG("update wp event.\r\n");
			uint32_t wp_sector = sPoint_current_sector(m_op_pointer.wp);
			uint32_t rp_sector = sPoint_current_sector(m_op_pointer.rp);

			if ( (S_REC_INVALID_ADDR == wp_sector) || (S_REC_INVALID_ADDR == rp_sector) )
			{
				sConfig_default_record_pointer();
			}
			if ( (0 == (m_op_pointer.wp % CONF_PAGE_SIZE)) && 	// current sector is new sector
                (wp_sector == rp_sector)                    &&
                (wp_sector != S_REC_INVALID_ADDR) )				
			{
                __sCritical_region_enter__;
				m_op_pointer.rp = wp_sector + CONF_PAGE_SIZE;//point next sector
				if ( m_op_pointer.rp >= m_rec_data_config.end_addr )
				{
					m_op_pointer.rp = m_rec_data_config.start_addr;
				}
                __sCritical_region_exit__;
			}
			break;
		}
		case UPDATE_RP_EVENT:
		{
		//	APP_LOG("update rp event.\r\n");

			break;
		}
		default:
		{
			return ;
		}
	}
    
    //check the m_point_head_addr
    if ( false == sCheck_op_pointer_addr_before_use_it(m_point_head_addr) )
    {
        m_point_head_addr = m_rec_pointer_config.start_addr;
    }
    sWait_for_pending();
    flash_operation_pending = true;
    m_fstorage_op_mutex 	= 0;
	
	if ( m_point_head_addr == S_REC_INVALID_ADDR ||
		m_point_head_addr == m_rec_pointer_config.start_addr )
	{
		//erase	the sector
        sWait_for_queue();
        
		uint32_t addr = (uint32_t)m_rec_pointer_config.start_addr; 
		uint32_t err_code = fs_erase( & fs_tiny_fs_system_record, (const uint32_t *)addr, 1, NULL ); 
		if( err_code != FS_SUCCESS )
	    {
            USER_ERROR_CHECK(err_code);
			APP_LOG("[CONF]: erase error = 0x%X\r\n", err_code ); 
	    }
		++ m_pointer_erase_times;
		m_point_head_addr = m_rec_pointer_config.start_addr;
	}

	//delte old record data.
	if ( m_point_head_addr >= (m_rec_pointer_config.start_addr + sizeof(rom_op_record_t)) ) 
	{
		uint32_t *p_data = (uint32_t*)&m_op_record_head_clear;
		memset(p_data, 0x00, sizeof(rom_op_record_t));

		uint32_t write_address = (uint32_t)(m_point_head_addr - sizeof(rom_op_record_t));
		uint16_t write_words   = sizeof(rom_op_record_t) / sizeof(uint32_t);
		
        __sCritical_region_enter__;
		++ m_fstorage_op_mutex;
		__sCritical_region_exit__;
		
		m_record_pointer_update = true;
        sWait_for_queue();
		
		uint32_t err_code = fs_store( & fs_tiny_fs_system_record, (const uint32_t *)write_address, p_data, write_words, NULL ); 
		if( err_code != FS_SUCCESS) 
	    {
            USER_ERROR_CHECK(err_code);
			APP_LOG("[CONF]: store error = 0x%X\r\n", err_code ); 
	    }
	}
	
	//write the new record data.
	m_op_record_head.op_pointer = m_op_pointer;
	m_op_record_head.id         = S_TINY_FS_RECORD_ID;
	m_op_record_head.crc		= crc16_compute( (uint8_t *)&m_op_pointer, sizeof(rom_op_pointer_t), 0);
	m_op_record_head.reserve    = m_data_erase_times << 16 | m_pointer_erase_times;
	
	APP_LOG("[before write]: m_point_head_addr = 0x%X\r\n", m_point_head_addr ); 
//	APP_LOG("[before write] m_op_pointer wp =  0x%02x\r\n", (uint32_t)m_op_pointer.wp);
//	APP_LOG("[before write] m_op_pointer rp =  0x%02x\r\n", (uint32_t)m_op_pointer.rp);	
	uint32_t *p_data 		= (uint32_t*)&m_op_record_head; 
	uint32_t write_address  = (uint32_t)(m_point_head_addr);
	uint16_t write_words    = sizeof(rom_op_record_t) / sizeof(uint32_t);
    
	__sCritical_region_enter__;
	++ m_fstorage_op_mutex;
	__sCritical_region_exit__;
	
	m_record_pointer_update = true;
    sWait_for_queue();
	
	uint32_t err_code = fs_store( & fs_tiny_fs_system_record, (const uint32_t *)write_address, p_data, write_words, NULL ); 
	if( err_code != FS_SUCCESS) 
    {
    	APP_LOG("write_address = 0x%02x\r\n", write_address);
        USER_ERROR_CHECK(err_code);
		APP_LOG("[CONF]: store error = 0x%X\r\n", err_code ); 
    }

//    m_last_point_address	= m_point_head_addr;

	//update the m_point_head_addr 
    __sCritical_region_enter__;
	m_point_head_addr += sizeof(rom_op_record_t);
	if ( m_point_head_addr >= m_rec_pointer_config.end_addr )
	{
		m_point_head_addr = m_rec_pointer_config.start_addr;
	}
    __sCritical_region_exit__;

    return;
}


static bool sHeader_is_valid(rom_op_record_t const * const p_header)
{
    // assert
    if ( NULL == p_header )
    {
		USER_ERROR_CHECK(ERROR_CODE_PARAMETER);
        return false;
    }
    uint16_t Crc16 = crc16_compute( (uint8_t *)&p_header->op_pointer, sizeof(rom_op_pointer_t), 0); 

	return ( (p_header->id == S_TINY_FS_RECORD_ID) && (Crc16 == p_header->crc) );
//    return ((p_header->id    != S_TINY_FS_RECORD_ID) &&
//            (p_header->tl.record_key != FDS_RECORD_KEY_DIRTY));
}


static void sRecover_record_pointer(void)
{
	uint32_t rec_next_addr = m_rec_pointer_config.start_addr;
	
	uint16_t record_one_size  = sizeof(rom_op_record_t);
	uint16_t record_nums_size = ((m_rec_pointer_config.end_addr - m_rec_pointer_config.start_addr)/record_one_size);
	
	rom_op_record_t *op_record_header = NULL;
    m_point_head_addr = S_REC_INVALID_ADDR;
    
	for ( uint16_t i = 0; i < record_nums_size; ++ i )
	{
		op_record_header = (rom_op_record_t *)rec_next_addr;
		if ( sHeader_is_valid(op_record_header) )
		{
            __sCritical_region_enter__;
			m_op_pointer = 	op_record_header->op_pointer;
			m_point_head_addr = rec_next_addr + record_one_size;
			__sCritical_region_exit__;

            if ( m_point_head_addr >= m_rec_pointer_config.end_addr )
			{
				m_point_head_addr = m_rec_pointer_config.start_addr;
			}
			
			m_data_erase_times    = op_record_header->reserve >> 16 ;
			m_pointer_erase_times = op_record_header->reserve;
			
            APP_LOG("have find the header......index = %d\r\n", i);
			break;
		}
		else
		{
            __sCritical_region_enter__;
            rec_next_addr += record_one_size;//find the header next address area.
            __sCritical_region_exit__;
        }
	}	
	
	APP_LOG("m_op_pointer wp =  0x%02x\r\n", (uint32_t)m_op_pointer.wp);
	APP_LOG("m_op_pointer rp =  0x%02x\r\n", (uint32_t)m_op_pointer.rp);
	
}


bool Tinyfs_init(void)
{
	if( 0 != (4096 % sizeof(Tinyfs_file_data_t)) )
	{
		USER_ERROR_CHECK(ERROR_CODE_PARAMETER);
		return false;
	}
	
	m_rec_data_config.start_addr 	= (uint32_t)fs_tiny_fs_system_record.p_start_addr;
	m_rec_data_config.end_addr  	= (uint32_t)fs_tiny_fs_system_record.p_start_addr + 2* CONF_PAGE_SIZE;

	m_rec_pointer_config.start_addr = (uint32_t)fs_tiny_fs_system_record.p_start_addr + 2* CONF_PAGE_SIZE;
	m_rec_pointer_config.end_addr   = (uint32_t)fs_tiny_fs_system_record.p_end_addr;

	sConfig_default_record_pointer();
	sRecover_record_pointer();
   
    flash_operation_pending = false;
    m_is_initalize_flag     = true;
    
    return true;
}



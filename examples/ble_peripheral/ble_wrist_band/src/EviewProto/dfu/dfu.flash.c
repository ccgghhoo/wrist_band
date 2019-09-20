/**
 * 保存在外部Flash当中。
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "LibHeap.h"

#include "dfu.flash.h"
#include "app_flash_drv.h"  //chen
#include "crc32.h"
//#include "HalWDT.h"
#if 0
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define DFU_FLASH_LOG			NRF_LOG_INFO
#else
#define DFU_FLASH_LOG(...)
#endif
static bool isWriteFlash = false;

#define DFU_WRITE_FIFO_SIZE		32
#define DFU_WRITE_FIFO_MASK		( DFU_WRITE_FIFO_SIZE - 1 )

typedef struct {
    uint8_t *pdata;
    uint32_t 	addr;
    uint32_t 	size;
}dfu_flash_write_data_t;

typedef struct {
    dfu_flash_write_data_t		fifo[DFU_WRITE_FIFO_SIZE];
    uint32_t 	rp; //已经写入的索引
    int32_t 	count; //剩余几个还没有写。
    uint32_t write_offset; //当前写的offset，主要是写flash跨页。

    dfu_flash_step_t state; //flash 操作的状态

    uint32_t flag; ////标志是否在有数据在处理Flash.
}dfu_flash_write_t;

static dfu_flash_write_t m_dfu_write;



/**
 * Ram当中是否还有数据还没有写入Flash.
 * 
 * @author hzhmc (2018/5/6)
 * 
 * @param void 
 * 
 * @return bool 
 */
static bool dfu_flash_op_advance(void)
{
    return (m_dfu_write.count > 0);
}

/**
 * 处理写的操作。
 * 
 * @author hzhmc (2018/5/6)
 */
static void dfu_flash_write_execute(void)
{
//	m_dfu_write.flash = DFU_FLAG_EXECUTING;

    dfu_flash_write_data_t  *p_dfu_write = &m_dfu_write.fifo[m_dfu_write.rp & DFU_WRITE_FIFO_MASK];
    uint32_t ret = DFU_RET_SUCCESS;

    switch (m_dfu_write.state)
    {
    case DFU_STEP_CHECK_BUSY: //wait flash is writable
        if (!flash_write_completed()) break;

        m_dfu_write.state = DFU_STEP_CHECK_SECTOR;
        m_dfu_write.write_offset = 0;
        DFU_FLASH_LOG("[DFU]: idle\r\n");

    case DFU_STEP_CHECK_SECTOR: // check sector is start address
        if ((p_dfu_write->addr & FLASH_SECTOR_SIZE_MASK) == 0)
        {
            flash_erase_sector(p_dfu_write->addr);
            m_dfu_write.state = DFU_STEP_WAIT_COMPLETED;
            DFU_FLASH_LOG("[DFU]: wait write completed\r\n");
            break;
        }

        m_dfu_write.state = DFU_STEP_CHECK_BEFORE_WRITE;

    case DFU_STEP_CHECK_BEFORE_WRITE:
        if (!flash_write_completed()) break;

        m_dfu_write.state = DFU_STEP_WRITE;

    case DFU_STEP_WRITE:
        {
            //DFU_FLASH_LOG("[DFU]: write = 0x%X, data = 0x%X\r\n", p_dfu_write->addr, (uint32_t)p_dfu_write->pdata);
            uint32_t remains = p_dfu_write->size - m_dfu_write.write_offset;

//			if( p_dfu_write->addr == 0x65100 ){
//				__NOP();
//			}

            if (remains > FLASH_PAGE_SIZE)
            {
                flash_write_data(p_dfu_write->addr + m_dfu_write.write_offset, p_dfu_write->pdata + m_dfu_write.write_offset, FLASH_PAGE_SIZE);
                m_dfu_write.write_offset += FLASH_PAGE_SIZE;
            }
            else
            {
                flash_write_data(p_dfu_write->addr + m_dfu_write.write_offset, p_dfu_write->pdata + m_dfu_write.write_offset, remains);
                m_dfu_write.write_offset = p_dfu_write->size;

            }
            m_dfu_write.state = DFU_STEP_WAIT_COMPLETED;

        }
        break;

    case DFU_STEP_WAIT_COMPLETED:
        if (flash_write_completed())
        {
            if (m_dfu_write.write_offset < p_dfu_write->size)
            {
                m_dfu_write.state = DFU_STEP_CHECK_BEFORE_WRITE;
            }
            else
            {
                APP_FREE(p_dfu_write->pdata);

                m_dfu_write.write_offset = 0;
                m_dfu_write.rp++;
                //以防数据有溢出 hzhmcu
                m_dfu_write.rp = m_dfu_write.rp & DFU_WRITE_FIFO_MASK;

                //
                m_dfu_write.count--;
                //越界处理。
                if (m_dfu_write.count < 0)
                {
                    m_dfu_write.count = 0;
                }

                ret = DFU_RET_WRITE_COMPLETED;
                DFU_FLASH_LOG("[DFU]: write completed, remain=%d \r\n", m_dfu_write.count);
            }
        }
        break;

    default:
        break;
    }


    //if ret == DFU_RET_WRITE_COMPLETED
    if (ret != DFU_RET_SUCCESS)
    {
        // queue next write operation
        if (dfu_flash_op_advance())
        {
            m_dfu_write.state = DFU_STEP_CHECK_BUSY;
        }
        else
        {
            isWriteFlash = false;
            //flash_enter_power_down_mode();
            m_dfu_write.flag = DFU_FLAG_IDLE;
            m_dfu_write.state = DFU_STEP_CHECK_BUSY;
        }
    }
}

/**
 * Flash处理函数。
 * 
 * @author hzhmc (2018/5/6)
 * 
 * @param void 
 */
void dfu_flash_process(void)
{
    if (m_dfu_write.flag == DFU_FLAG_IDLE) return;

    dfu_flash_write_execute();
    
}

/**
 * 写Flash
 * 
 * @author hzhmc (2018/5/6)
 * 
 * @param write_offset 
 * @param p_data 
 * @param size 
 * 
 * @return bool 
 */
bool dfu_flash_write_update(uint32_t write_offset, const uint8_t *p_data, uint16_t size)
{
    if (isWriteFlash == false)
    {
        //flash_release_power_down_mode();
        isWriteFlash = true;
    }
    //如果当前写的个数，少于最大值
    if (m_dfu_write.count < DFU_WRITE_FIFO_SIZE)
    {
        //计算写的索引
        uint32_t idx = (m_dfu_write.count + m_dfu_write.rp) & DFU_WRITE_FIFO_MASK;

        //数据进复制。
        m_dfu_write.fifo[idx].pdata = (uint8_t *)APP_MALLOC(DFU_WRITE_FLASH_TEXT, size);
        if (m_dfu_write.fifo[idx].pdata == NULL)
        {
            return false;
        }
        memcpy(m_dfu_write.fifo[idx].pdata, p_data, size);

        m_dfu_write.fifo[idx].addr = write_offset;
        m_dfu_write.fifo[idx].size = size;

        m_dfu_write.count++;

        DFU_FLASH_LOG("[DFU]: addr = 0x%X, count = %d\r\n", write_offset, m_dfu_write.count);

        if (m_dfu_write.flag == DFU_FLAG_IDLE)
        {
            m_dfu_write.flag = DFU_FLAG_EXECUTING;
            dfu_flash_process();
        }
        return true;
    }
    else
    {
        DFU_FLASH_LOG("[DFU]: dfu_flash_write_update fail, no fifo to use!!  \r\n");
        return false;
    }
}

/**
 * 计算CRC32的值
 * 
 * @author hzhmc (2018/5/6)
 * 
 * @param addr 
 * @param size 
 * 
 * @return uint32_t 
 */
uint32_t dfu_flash_crc_compute(uint32_t addr, uint32_t size)
{
    //flash_release_power_down_mode();
    uint32_t crc32 = 0;
    uint32_t offset = 0;

    uint8_t temp[FLASH_PAGE_SIZE];
    
    
    
     while(m_dfu_write.count!=0)
     {
        dfu_flash_process();
     }
    
    
    DFU_FLASH_LOG("flash crc begin!!\r\n");
    DFU_FLASH_LOG("firmware start address =0x%X\r\n", addr);
    DFU_FLASH_LOG("firmware size =0x%X\r\n", size);
    while (offset < size)
    {
        if ((offset + FLASH_PAGE_SIZE) < size)
        {
            flash_read_data(addr + offset, temp, FLASH_PAGE_SIZE);
            crc32 = crc32_compute(temp, FLASH_PAGE_SIZE, &crc32);
            offset += FLASH_PAGE_SIZE;
        }
        else
        {
            uint32_t bytes = size - offset;
            flash_read_data(addr + offset, temp, bytes);
            crc32 = crc32_compute(temp, bytes, &crc32);
            offset = size;
        }
        //DFU_FLASH_LOG("address =0x%X\r\n", offset);
    }
    DFU_FLASH_LOG("flash crc ended!!\r\n");
    DFU_FLASH_LOG("CRC32 =0x%X offset=0x%X\r\n", crc32, offset);
    
    return crc32;
}


/**
 * 释放所用的资源。
 * 
 * @author hzhmc (2018/5/6)
 */
void dfu_flash_reset(void)
{
    for (uint32_t i = 0; i < m_dfu_write.count; i++)
    {
        uint32_t rp = (m_dfu_write.rp & DFU_WRITE_FIFO_MASK);
        APP_FREE(m_dfu_write.fifo[rp].pdata);

        m_dfu_write.rp++;
        m_dfu_write.count--;
    }
}


/**
 * Flash的初始化
 * 
 * @author hzhmc (2018/5/6)
 * 
 * @param void 
 */
void dfu_flash_init(void)
{
    m_dfu_write.state = DFU_STEP_CHECK_BUSY;
    m_dfu_write.flag = DFU_FLAG_IDLE;
//	flash_enter_power_down_enable( false );
    
    app_dfu_flash_init();      //chen
    dfu_setting_flash_init();  //chen
}



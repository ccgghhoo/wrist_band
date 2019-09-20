// Header:
// File Name: 	Configuration File Operation
// Author:		Carlton
// Date:		2017.06.12


#include "fstorage.h"


#include "crc16.h"
#include "HalParam.h"
#if 0
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define DEBUG				NRF_LOG_DEBUG
#else
#define DEBUG(...)
#endif

#define CONF_PAGE_SIZE   (4096)

pfnWriteCompleted writeCompleted = NULL;
static void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result);
static void fs_evt_backup_handler(fs_evt_t const *const evt, fs_ret_t result);

FS_REGISTER_CFG(fs_config_t fs_conf_file) =
{
    .callback  = fs_evt_handler,
    .num_pages = 1,
    .priority  = 0xF0
};

FS_REGISTER_CFG(fs_config_t fs_backup_conf_file) =
{
    .callback  = fs_evt_backup_handler,
    .num_pages = 1,
    .priority  = 0xF0
};


static void fs_evt_backup_handler(fs_evt_t const *const evt, fs_ret_t result)
{
    switch (evt->id)
    {
    case FS_EVT_STORE:
        writeCompleted(CONF_FILE_BACKUP, result);

        break;
    case FS_EVT_ERASE:
        break;
    }

}

static void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result)
{
    switch (evt->id)
    {
    case FS_EVT_STORE:
        writeCompleted(CONF_FILE_NORMAL, result);

        break;
    case FS_EVT_ERASE:
        break;
    }

}



void HalParam_Write(CONF_FILE_TYPE_ENUM type, const uint32_t *data, uint16_t len)
{
    fs_config_t  * p_config = NULL;


    if (type == CONF_FILE_NORMAL) p_config = &fs_conf_file;
    else if (type == CONF_FILE_BACKUP) p_config = &fs_backup_conf_file;
    else return;
    
    uint32_t addr = (uint32_t)p_config->p_start_addr;

    uint32_t err_code = fs_erase(p_config, (const uint32_t *)addr, 1, NULL);
    if (err_code != FS_SUCCESS)
    {
        DEBUG("[CONF]: erase error = 0x%X\r\n", err_code);
    }
    err_code = fs_store(p_config, (const uint32_t *)addr, data, len / sizeof(uint32_t), NULL);
    if (err_code != FS_SUCCESS)
    {
        DEBUG("[CONF]: storage error = 0x%X\r\n", err_code);
    }


}



uint8_t* HalParam_ReadAddr(CONF_FILE_TYPE_ENUM type)
{

    uint32_t addr = NULL;

    if (type >= CONF_FILE_NUMS) return NULL;

    if (type == CONF_FILE_NORMAL) addr = (uint32_t)fs_conf_file.p_start_addr;
    else if (type == CONF_FILE_BACKUP) addr = (uint32_t)fs_backup_conf_file.p_start_addr;

    else addr = 0x7f000;   //specify file
    return (uint8_t *)addr;
}



void HalParam_Init(pfnWriteCompleted cb)
{
    writeCompleted = cb;
    (void)fs_init();
}

////*********************************************

/////////////////////////////////////////////////////

#include "app_fw_info.h"
#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#if defined ( __CC_ARM )
const app_info_t appl_info __attribute__((at((uint32_t)APP_FW_INFO_START_ADDRESS))) =
{
    .magic = APP_FW_MAGIC_CODE,
    .fw_version = FW_REVISION,
    .fw_size = APP_FW_SIZE,
    .hw_version = APP_FW_HW_VER,
    .sd_version = APP_FW_SD_VERSION,
    .sd_fw_id = APP_FW_SD_FW_ID,
    .date = APP_FW_DATE,
    .time = APP_FW_TIME,
};

#elif defined(__ICCARM__)
const app_info_t appl_info @APP_FW_INFO_START_ADDRESS =
{
    .magic = APP_FW_MAGIC_CODE,
    .fw_version = FW_REVISION,
    .fw_size = APP_FW_SIZE,
    .hw_version = APP_FW_HW_VER,
    .sd_version = APP_FW_SD_VERSION,
    .sd_fw_id = APP_FW_SD_FW_ID,
    .date = APP_FW_DATE,
    .time = APP_FW_TIME,
};

#else
const app_info_t appl_info =
{
    .magic = APP_FW_MAGIC_CODE,
    .fw_version = PROPERTY_FIRMWARE_VER,
    .fw_size = APP_FW_SIZE,
    .hw_version = APP_FW_HW_VER,
    .sd_version = APP_FW_SD_VERSION,
    .sd_fw_id = APP_FW_SD_FW_ID,
    .date = APP_FW_DATE,
    .time = APP_FW_TIME,
};

#endif

void cpy_fw(void)
{
    app_info_t test_info;
    test_info = appl_info;
}






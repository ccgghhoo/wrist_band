
#ifndef __HAL_PARAM_H
#define __HAL_PARAM_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    CONF_FILE_NORMAL,
    CONF_FILE_BACKUP,
    CONF_FILE_FACTORY,

    CONF_FILE_NUMS,
}CONF_FILE_TYPE_ENUM;

typedef struct {
    uint16_t 	file_id;
    uint16_t 	len;
    uint16_t 	crc;
    uint16_t 	key_id;
}conf_file_header_t;

void HalParam_Write(CONF_FILE_TYPE_ENUM type, const uint32_t *data, uint16_t len);

uint32_t* HalParam_Read(CONF_FILE_TYPE_ENUM type);
uint8_t* HalParam_ReadAddr(CONF_FILE_TYPE_ENUM type);
typedef void (*pfnWriteCompleted)(CONF_FILE_TYPE_ENUM type, int32_t result);
void HalParam_Init(pfnWriteCompleted cb);
#endif

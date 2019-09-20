

#ifndef CRC16_H__
#define CRC16_H__

#include <stdint.h>


uint16_t crc16_compute(const uint8_t *data, uint32_t len, uint32_t init);

#endif 

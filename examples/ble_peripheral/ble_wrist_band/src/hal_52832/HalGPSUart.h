#ifndef __HAL_GPS_UART_H
#define __HAL_GPS_UART_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
bool HalGPSTxUart_Open();
bool HalGPSTxUart_Close();
bool HalGPSUart_Close();
bool HalGPSUart_Open(uint32_t baudRate);
void HalGPS_Init(void (*readcb)(uint8_t data));
bool HalGPSTxUart_Put(uint8_t *buf,uint32_t len);
#endif


#ifndef __HAL_SIM_UART_H
#define __HAL_SIM_UART_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
int32_t Uart_Init(uint32_t rxPin, uint32_t txPin, void (*cb)(uint8_t data));
int32_t Uart_Stop();
int32_t Uart_Start(); 
#endif


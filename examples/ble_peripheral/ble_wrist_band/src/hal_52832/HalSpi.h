/****************************************************************************
 * Copyright (c) 2016 YYYT AS. All Rights Reserved.
 *
 */

#ifndef __HAL_SPI_H
#define __HAL_SPI_H


void HalSpi0_Enable(void);

void HalSpi0_Disable(void);

unsigned char HalSpi0_Xfer(unsigned char byte);

void HalSpi0_Init(void);


#endif 

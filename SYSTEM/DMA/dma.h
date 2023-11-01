#ifndef __DMA_H
#define __DMA_H
#include "common.h"

void bsp_InitAdcDMA(void);

uint16_t get_adc_value(uint8_t ch);
#endif

#ifndef __UART2_H
#define __UART2_H

#include "common.h"



void uart2_init(u32 bound);
void compressor_set(uint16_t rpm, uint8_t en, uint8_t dir, uint8_t stop);
void compressor_read(void);
void uart2_printf(u8 *buf, u16 len);
#endif

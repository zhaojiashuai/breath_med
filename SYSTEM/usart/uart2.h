#ifndef __UART2_H
#define __UART2_H

#include "common.h"



void uart2_init(u32 bound);
void compressor_set(int16_t dat, int16_t speed,int16_t fan);
#endif

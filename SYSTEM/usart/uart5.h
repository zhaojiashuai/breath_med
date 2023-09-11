#ifndef __UART5_H
#define __UART5_H
#include "common.h"

void uart5_init(u32 bound);
void uart5_printf(u8 *buf, u8 len);

#endif

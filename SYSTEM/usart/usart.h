
#ifndef __USART_H
#define __USART_H
#include "common.h"
void uart_init(u32 bound);
void uart1_printf(u8 *buf, u16 len);
#endif

#ifndef __GPIO_H
#define __GPIO_H
#include "common.h"

// LED�˿ڶ���
#define LED0 PFout(9)  // DS0
#define LED1 PFout(10) // DS1

void gpio_Init(void); // ��ʼ��
#endif

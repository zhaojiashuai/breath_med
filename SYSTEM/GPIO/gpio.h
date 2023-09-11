#ifndef __GPIO_H
#define __GPIO_H
#include "common.h"

// LED端口定义
#define LED0 PFout(9)  // DS0
#define LED1 PFout(10) // DS1

void gpio_Init(void); // 初始化
#endif

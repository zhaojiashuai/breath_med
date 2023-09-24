#ifndef __GPIO_H
#define __GPIO_H
#include "common.h"

// LED端口定义
#define LED PCout(13)  // LED0

void gpio_init(void); // 初始化
#endif

#ifndef __GPIO_H
#define __GPIO_H
#include "common.h"

// LED端口定义
#define LED PCout(13)  // LED0

#define IO1(a)  GPIO_WriteBit(GPIOC,GPIO_Pin_0,a);
#define IO2(a)  GPIO_WriteBit(GPIOC,GPIO_Pin_1,a);
#define IO3(a)  GPIO_WriteBit(GPIOC,GPIO_Pin_2,a);
#define IO4(a)  GPIO_WriteBit(GPIOC,GPIO_Pin_3,a);

void gpio_init(void); // 初始化
#endif

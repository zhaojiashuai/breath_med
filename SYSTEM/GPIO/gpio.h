#ifndef __GPIO_H
#define __GPIO_H
#include "common.h"

// LED端口定义
#define LED PCout(13) // LED0

#define TEST_IO1 PCout(0)   //A
#define TEST_IO2 PCout(1)   //B 
 
#define FAN_DIR_CW      PCout(2)  = 1
#define FAN_DIR_CCW     PCout(2)  = 0

#define FAN_BREAK_ON PCout(3) = 0  
#define FAN_BREAK_OFF PCout(3) = 1 

void gpio_init(void); // 初始化

#endif

#ifndef __GPIO_H
#define __GPIO_H
#include "common.h"

// LED�˿ڶ���
#define LED PCout(13) // LED0

#define TEST_IO1 PCout(0)   //A
#define TEST_IO2 PCout(1)   //B
#define FAN_DIR PCout(2)   
#define FAN_BREAK PCout(3)   

void gpio_init(void); // ��ʼ��

#endif

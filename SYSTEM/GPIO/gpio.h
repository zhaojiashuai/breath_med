#ifndef __GPIO_H
#define __GPIO_H
#include "common.h"

// LED端口定义
#define LED PCout(13) // LED0

#define IO1(a) GPIO_WriteBit(GPIOC, GPIO_Pin_0, a)
#define IO2(a) GPIO_WriteBit(GPIOC, GPIO_Pin_1, a)
#define IO3(a) GPIO_WriteBit(GPIOC, GPIO_Pin_2, a)
#define IO4(a) GPIO_WriteBit(GPIOC, GPIO_Pin_3, a)

#define TEST_IO1 PCout(0)   //A
#define TEST_IO2 PCout(1)   //B
#define TEST_IO3 PCout(2)   //PWM1
#define TEST_IO4 PCout(3)   //PWM2

void gpio_init(void); // 初始化
void pwm_io1_out(void);
void pwm_io2_out(void);
#endif

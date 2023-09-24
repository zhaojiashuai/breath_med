#include "gpio.h"

void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOF时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // LEDIO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // 上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);                  // 初始化GPIO

    GPIO_SetBits(GPIOC, GPIO_Pin_13); // GPIOC13设置高，灯灭
}


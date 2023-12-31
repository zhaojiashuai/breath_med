#include "gpio.h"

void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOC时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_13; // LEDIO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                                                  // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                 // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                             // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                                   // 上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);                                                         // 初始化GPIO

    GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}

void pwm_io1_out(void)
{
    static uint8_t count = 0;
    if (count++ < 50)
    {
        TEST_IO3 = 1;
    }
    else
    {
        TEST_IO3 = 0;
    }
    if (count >= 100)
    {
        count = 0;
    }
}

void pwm_io2_out(void)
{
    static uint8_t count = 0;
    if (count++ < 50)
    {
        TEST_IO4 = 1;
    }
    else
    {
        TEST_IO4 = 0;
    }
    if (count >= 100)
    {
        count = 0;
    }
}

#include "gpio.h"

void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOC端口的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 配置GPIO_InitStructure结构体，设置要初始化的GPIO端口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_13; // LED和IO端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                                                  // 设置为通用输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                 // 设置为推挽输出类型
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                             // 设置GPIO速度为100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                                   // 设置上拉电阻

    // 根据GPIO_InitStructure的配置初始化GPIOC端口
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 将GPIOC端口的第0、1、2、3脚置高电平
    GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);

    // 设置风扇方向为顺时针
    FAN_DIR_CW;
    // 关闭风扇制动
    FAN_BREAK_OFF;
}

#include "EXIT.h"

// 外部中断0服务程序
void EXTI0_IRQHandler(void)
{
    // CODE
    EXTI_ClearITPendingBit(EXTI_Line0); // 清除LINE0上的中断标志位
}

// 外部中断初始化程序
// 初始化PE2~4,PA0为中断输入.
void EXTIX_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // 使能GPIOA,GPIOE时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;          // PB0 对应引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       // 普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;     // 下拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // 初始化GPIOB0

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // 使能SYSCFG时钟

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0); // PA0 连接到中断线0

    /* 配置EXTI_Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;             // LINE0
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // 中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // 使能LINE0
    EXTI_Init(&EXTI_InitStructure);                        // 配置

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;             // 外部中断0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        // 子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              // 使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);                              // 配置
}

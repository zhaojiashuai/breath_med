#include "EXIT.h"

void EXTI0_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line0); 
}


/**
 * @brief 外部中断初始化函数
 * 
 * 这个函数用于配置STM32的外部中断EXTI，以及相关的NVIC（嵌套向量中断控制器）和GPIO。
 * 它配置了一个GPIO引脚作为外部中断的输入，并设置了中断的触发条件、优先级等。
 * 
 * @param void
 * @return void
 */
void EXTIX_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; // NVIC初始化结构体
    EXTI_InitTypeDef EXTI_InitStructure; // EXTI初始化结构体

    GPIO_InitTypeDef GPIO_InitStructure; // GPIO初始化结构体

    // 使能GPIOB时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 

    // 配置GPIOB0引脚为输入模式，下拉，速度为100MHz
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;     
    GPIO_Init(GPIOB, &GPIO_InitStructure);             

    // 使能SYSCFG时钟，用于配置EXTI线
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 

    // 将GPIOB0连接到EXTI线0
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0); 

    // 配置EXTI线0为中断模式，上升沿触发
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;             
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              
    EXTI_Init(&EXTI_InitStructure);                        

    // 配置NVIC，设置EXTI0中断的优先级，并使能中断
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;             
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              
    NVIC_Init(&NVIC_InitStructure);                              
}


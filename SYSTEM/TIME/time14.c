#include "time14.h"

// TIM14 PWM部分初始化
// PWM输出初始化
// arr：自动重装值
// psc：时钟预分频数
void TIM14_PWM_Init(u32 arr, u32 psc)
{
    // 此部分需手动修改IO口设置

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); // TIM14时钟使能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); // 使能PORTF时钟

    GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14); // GPIOF9复用为定时器14

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          // GPIOF9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOF, &GPIO_InitStructure);             // 初始化PF9

    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure); // 初始化定时器14

    // 初始化TIM14 Channel1 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // 选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      // 输出极性:TIM输出比较极性低
    TIM_OC1Init(TIM14, &TIM_OCInitStructure);                     // 根据T指定的参数初始化外设TIM1 4OC1

    TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable); // 使能TIM14在CCR1上的预装载寄存器

    TIM_ARRPreloadConfig(TIM14, ENABLE); // ARPE使能

    TIM_Cmd(TIM14, ENABLE); // 使能TIM14
}

void TIM2_Configuration(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 42000 - 1; // 根据需要调整分频系数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 2000 - 1; // 根据需要调整计数周期
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        // 在这里编写你想执行的代码
        // ...
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}



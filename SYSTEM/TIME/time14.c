#include "time14.h"

/**
 * 配置定时器2以产生1ms的周期中断
 *
 * 定时器频率计算公式: 定时器频率 = 系统时钟频率 / (预分频值 + 1)
 * 定时周期计算公式: 定时周期 = 自动重装载值 / 定时器频率
 */
void TIM2_Configuration(void) // 1ms
{
    // 使能TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 定时器时基单元初始化结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 设置时钟分频器，这里以84MHz的系统时钟为例
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // 预分频值，84MHz时钟下，分频为83
    // 设置计数模式为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 设置自动重装载值为1000，即1ms的周期
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
    // 设置时钟分频为不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 设置重复计数器为0，即不重复
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    // 根据TIM_TimeBaseStructure配置定时器时基单元
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 清除TIM2更新中断标志位
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    // 配置TIM2更新中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // NVIC初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;
    // 配置TIM2中断优先级和子优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道

    // 根据NVIC_InitStructure配置NVIC
    NVIC_Init(&NVIC_InitStructure);

    // 使能TIM2定时器
    TIM_Cmd(TIM2, ENABLE);
}

uint32_t time_cnt = 0;
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        time_cnt++;
        timing_task();
        task_time();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

/**
 * 配置定时器4以产生10us的周期中断
 *
 * 定时器频率计算公式: 定时器频率 = 系统时钟频率 / (预分频值 + 1)
 * 定时周期计算公式: 定时周期 = 自动重装载值 / 定时器频率
 */
void TIM4_Configuration(void) // 10us
{
    // 使能TIM4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 定时器时基单元初始化结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 设置时钟分频器，这里以84MHz的系统时钟为例
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // 预分频值，84MHz时钟下，分频为83
    // 设置计数模式为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 设置自动重装载值为10，即10us的周期
    TIM_TimeBaseStructure.TIM_Period = 10 - 1;
    // 设置时钟分频为不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 设置重复计数器为0，即不重复
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    // 根据TIM_TimeBaseStructure配置定时器时基单元
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 清除TIM4更新中断标志位
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    // 配置TIM4更新中断
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    // NVIC初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;
    // 配置TIM4中断优先级和子优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道

    // 根据NVIC_InitStructure配置NVIC
    NVIC_Init(&NVIC_InitStructure);

    // 使能TIM4定时器
    TIM_Cmd(TIM4, ENABLE);
}


void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

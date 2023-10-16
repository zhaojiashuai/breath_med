#include "uart4.h"

void uart4_init(u32 bound)
{
    // 使能UART4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    // 配置串口引脚
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 将引脚设置为串口功能
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

    // 配置UART4参数
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4, &USART_InitStruct);

    // 使能串口接收中断
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

    // 使能串口
    USART_Cmd(UART4, ENABLE);

    // 配置串口中断优先级
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

// buf:发送区首地址
// len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void uart4_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环发送数据
    {
        while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
            ; // 循环发送,直到发送完毕
        USART_SendData(UART4, buf[t]);
    }
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
        ;
}

void UART4_IRQHandler(void)
{
    u8 res;
    // static u8  cnt;
    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(UART4, USART_IT_RXNE); // 清除中断标志位
        res = USART_ReceiveData(UART4);                // 读取接收到的数据
        USART_SendData(UART4, res);
    }
}

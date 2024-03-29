#include "uart3.h"

oxygen_t output = {0};

void uart3_init(u32 bound)
{
    // 使能USART3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // 配置串口引脚
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 将引脚设置为串口功能
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    // 配置USART3参数
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStruct);

    // 使能串口接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // 使能串口
    USART_Cmd(USART3, ENABLE);

    // 配置串口中断优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}

// buf:发送区首地址
// len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void uart3_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环发送数据
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ; // 循环发送,直到发送完毕
        USART_SendData(USART3, buf[t]);
    }
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        ;
}

void USART3_IRQHandler(void)
{
    static u8 cnt = 0;
    static u8 uart3[100] = {0};
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); // 清除中断标志位
        uart3[cnt++] = USART_ReceiveData(USART3);       // 读取接收到的数据
        if (uart3[0] != 0x16||cnt>90)
        {
            cnt = 0;
        }
        else
        {
            if (cnt >= 11)
            {
                output.oxygen = uart3[3] * 256 + uart3[4];
                output.flow = uart3[5] * 256 + uart3[6];
                output.temp = uart3[7] * 256 + uart3[8];
                cnt = 0;
            }
        }
    }
}

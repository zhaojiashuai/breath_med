#include "uart5.h"

oxygen_t  input = {0};

void uart5_init(u32 bound)
{
    // 使能UART5时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

    // 配置串口引脚
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    // 将引脚设置为串口功能
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

    // 配置UART5参数
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART5, &USART_InitStruct);

    // 使能串口接收中断
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

    // 使能串口
    USART_Cmd(UART5, ENABLE);

    // 配置串口中断优先级
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

// buf:发送区首地址
// len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void uart5_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环发送数据
    {
        while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
            ; // 循环发送,直到发送完毕
        USART_SendData(UART5, buf[t]);
    }
    while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
        ;
}

void UART5_IRQHandler(void)//16 09 01 01 F4 00 64 00 D2 00 00 B5
{
    static u8 cnt=0;
    static u8 uart5[100]={0};
    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(UART5, USART_IT_RXNE); // 清除中断标志位
        uart5[cnt++] = USART_ReceiveData(UART5);                // 读取接收到的数据
        if(uart5[0]!=0x16)
        {
            cnt = 0;
        }
        else
        {
            if(cnt>=11)
            {
                input.oxygen = uart5[3]*256+uart5[4];
                input.flow = uart5[5]*256+uart5[6];
                input.temp = uart5[7]*256+uart5[8];
                cnt = 0;
            }
        }
    }
}

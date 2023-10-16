#include "uart4.h"

void uart4_init(u32 bound)
{
    // ʹ��UART4ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    // ���ô�������
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // ����������Ϊ���ڹ���
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

    // ����UART4����
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4, &USART_InitStruct);

    // ʹ�ܴ��ڽ����ж�
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

    // ʹ�ܴ���
    USART_Cmd(UART4, ENABLE);

    // ���ô����ж����ȼ�
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

// buf:�������׵�ַ
// len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void uart4_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // ѭ����������
    {
        while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
            ; // ѭ������,ֱ���������
        USART_SendData(UART4, buf[t]);
    }
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
        ;
}

void UART4_IRQHandler(void)
{
    u8 res;
    // static u8  cnt;
    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) // ���յ�����
    {
        USART_ClearITPendingBit(UART4, USART_IT_RXNE); // ����жϱ�־λ
        res = USART_ReceiveData(UART4);                // ��ȡ���յ�������
        USART_SendData(UART4, res);
    }
}

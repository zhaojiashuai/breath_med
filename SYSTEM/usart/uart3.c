#include "uart3.h"

void uart3_init(u32 bound)
{
    // ʹ��USART3ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // ���ô�������
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // ����������Ϊ���ڹ���
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    // ����USART3����
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStruct);

    // ʹ�ܴ��ڽ����ж�
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // ʹ�ܴ���
    USART_Cmd(USART3, ENABLE);

    // ���ô����ж����ȼ�
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

// buf:�������׵�ַ
// len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void uart3_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // ѭ����������
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ; // ѭ������,ֱ���������
        USART_SendData(USART3, buf[t]);
    }
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        ;
}

void USART3_IRQHandler(void)
{
    u8 res;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // ���յ�����
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); // ����жϱ�־λ
        res = USART_ReceiveData(USART3);                // ��ȡ���յ�������
        USART_SendData(USART3, res);
    }
}
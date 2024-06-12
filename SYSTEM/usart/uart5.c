#include "uart5.h"

oxygen_t  input = {0};

void uart5_init(u32 bound)
{
    // ʹ��UART5ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

    // ���ô�������
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

    // ����������Ϊ���ڹ���
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

    // ����UART5����
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART5, &USART_InitStruct);

    // ʹ�ܴ��ڽ����ж�
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

    // ʹ�ܴ���
    USART_Cmd(UART5, ENABLE);

    // ���ô����ж����ȼ�
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
}

// buf:�������׵�ַ
// len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void uart5_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // ѭ����������
    {
        while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
            ; // ѭ������,ֱ���������
        USART_SendData(UART5, buf[t]);
    }
    while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET)
        ;
}

void UART5_IRQHandler(void)//16 09 01 01 F4 00 64 00 D2 00 00 B5
{
    static u8 cnt=0;
    static u8 uart5[100]={0};
    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // ���յ�����
    {
        USART_ClearITPendingBit(UART5, USART_IT_RXNE); // ����жϱ�־λ
        uart5[cnt++] = USART_ReceiveData(UART5);                // ��ȡ���յ�������
        if(uart5[0]!=0x16||cnt>90)
        {
            cnt = 0;
        }
        else
        {
            if(cnt>=11)
            {
                uart5_printf(uart5,cnt);
                input.oxygen = (uart5[3]<<8)+uart5[4];
                input.flow = (uart5[5]<<8)+uart5[6];
                input.temp = (uart5[7]<<8)+uart5[8];
                cnt = 0;
            }
        }
    }
}

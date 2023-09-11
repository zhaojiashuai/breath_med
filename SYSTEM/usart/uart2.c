#include "uart2.h"

void uart2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // ʹ��GPIOA,Dʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ʹ��USART2ʱ��

 	// ����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);  // GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // GPIOA3����ΪUSART2

	// USART2�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// ��ʼ��PA2��PA3   

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);  // ��λ����2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE); // ֹͣ��λ

    USART_InitStructure.USART_BaudRate = bound;                                     // ����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 8λ���ݳ���
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             /// ��żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); // ��ʼ������

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;         // ʹ�ܴ���2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // �����ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                           // ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // �����ж�

    USART_Cmd(USART2, ENABLE); // ʹ�ܴ���
}

// buf:�������׵�ַ
// len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void uart2_printf(u8 *buf, u16 len)
{
    u16 t;
    for (t = 0; t < len; t++) // ѭ����������
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
            ;
        USART_SendData(USART2, buf[t]);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;
}

void USART2_IRQHandler(void)
{
    // u8 res;
    // static u8  cnt;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // ���յ�����
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); // ����жϱ�־λ
        // res = USART_ReceiveData(USART2);                // ��ȡ���յ�������
    }
}

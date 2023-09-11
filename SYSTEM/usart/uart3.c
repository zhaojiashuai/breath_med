#include "uart3.h"

void uart3_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // ʹ��GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ʹ��USART3ʱ��

 	// ����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);  // GPIOB10����ΪUSART3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); // GPIOB11����ΪUSART3

	// USART3�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // GPIOB10��GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_Init(GPIOB, &GPIO_InitStructure);					// ��ʼ��PB10��PB11   

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);  // ��λ����2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE); // ֹͣ��λ

    USART_InitStructure.USART_BaudRate = bound;                                     // ����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 8λ���ݳ���
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             /// ��żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ

    USART_Init(USART3, &USART_InitStructure); // ��ʼ������

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;         // ʹ�ܴ���2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // �����ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                           // ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // �����ж�

    USART_Cmd(USART3, ENABLE); // ʹ�ܴ���
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
    // static u8  cnt;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // ���յ�����
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); // ����жϱ�־λ
        res = USART_ReceiveData(USART3);                // ��ȡ���յ�������
        USART_SendData(USART3, res);
    }
}

#include "uart5.h"

void uart5_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);                        // ʹ��UART5��GPIOBʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE); // ʹ��GPIOC/Dʱ��

  	// ����5��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);  // GPIOC12����ΪUSART5
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5); // GPIOD2����ΪUSART5

	// USART5�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // GPIOC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_Init(GPIOC, &GPIO_InitStructure);					// ��ʼ��PC12   

	// USART5�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_Init(GPIOD, &GPIO_InitStructure);					// ��ʼ��PD2         

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;        // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART5, &USART_InitStructure);

    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // �����ж�
    USART_Cmd(UART5, ENABLE);                     // ʹ�ܴ���
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

void UART5_IRQHandler(void)
{
    u8 res;
    // static u8  cnt;
    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // ���յ�����
    {
        USART_ClearITPendingBit(UART5, USART_IT_RXNE); // ����жϱ�־λ
        res = USART_ReceiveData(UART5);                // ��ȡ���յ�������
        USART_SendData(UART5, res);
    }
}

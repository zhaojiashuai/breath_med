/*
 * @Author: PLUS-ZHAO
 * @Date: 2023-09-03 22:18:59
 * @LastEditors: PLUS-ZHAO
 * @LastEditTime: 2023-09-03 22:41:02
 * @FilePath: \Template\SYSTEM\usart\uart4.c
 * @Description:
 *
 * Copyright (c) 2023 by ${PLUS-ZHAO}, All Rights Reserved.
 */
#include "uart4.h"

void uart4_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); // for UART4

  	// ����4��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);  // GPIOC10����ΪUSART4
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); // GPIOC11����ΪUSART4

	// USART4�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // GPIOC10��GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_Init(GPIOC, &GPIO_InitStructure);					// ��ʼ��PC10��PC11      

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // ����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // ��У��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ
    USART_Init(UART4, &USART_InitStructure);                                        // ���ô��ڲ�������

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;          // ʹ�ܴ���2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;        // �����ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                           // ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    /* Enable USART1 Receive and Transmit interrupts */
    USART_ClearFlag(UART4, USART_IT_RXNE);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // ʹ�ܽ����ж�
    // USART_ITConfig(UART4, USART_IT_TXE, ENABLE);                        //ʹ�ܷ��ͻ�����ж�

    /* Enable the UART4 */
    USART_Cmd(UART4, ENABLE);
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

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

  	// 串口4对应引脚复用映射
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);  // GPIOC10复用为USART4
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); // GPIOC11复用为USART4

	// USART4端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // GPIOC10与GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);					// 初始化PC10，PC11      

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式
    USART_Init(UART4, &USART_InitStructure);                                        // 配置串口参数函数

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;          // 使能串口2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;        // 从优先级2级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);                           // 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    /* Enable USART1 Receive and Transmit interrupts */
    USART_ClearFlag(UART4, USART_IT_RXNE);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // 使能接收中断
    // USART_ITConfig(UART4, USART_IT_TXE, ENABLE);                        //使能发送缓冲空中断

    /* Enable the UART4 */
    USART_Cmd(UART4, ENABLE);
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

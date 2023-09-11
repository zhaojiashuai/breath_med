#include "uart5.h"

void uart5_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);                        // 使能UART5，GPIOB时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE); // 使能GPIOC/D时钟

  	// 串口5对应引脚复用映射
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);  // GPIOC12复用为USART5
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5); // GPIOD2复用为USART5

	// USART5端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // GPIOC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);					// 初始化PC12   

	// USART5端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);					// 初始化PD2         

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART5, &USART_InitStructure);

    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // 开启中断
    USART_Cmd(UART5, ENABLE);                     // 使能串口
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

void UART5_IRQHandler(void)
{
    u8 res;
    // static u8  cnt;
    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(UART5, USART_IT_RXNE); // 清除中断标志位
        res = USART_ReceiveData(UART5);                // 读取接收到的数据
        USART_SendData(UART5, res);
    }
}

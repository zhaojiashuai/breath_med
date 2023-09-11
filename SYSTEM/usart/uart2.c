#include "uart2.h"

void uart2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // 使能GPIOA,D时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 使能USART2时钟

 	// 串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);  // GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // GPIOA3复用为USART2

	// USART2端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 初始化PA2，PA3   

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);  // 复位串口2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE); // 停止复位

    USART_InitStructure.USART_BaudRate = bound;                                     // 波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 8位数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             /// 奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式

    USART_Init(USART2, &USART_InitStructure); // 初始化串口

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;         // 使能串口2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // 从优先级2级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);                           // 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启中断

    USART_Cmd(USART2, ENABLE); // 使能串口
}

// buf:发送区首地址
// len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void uart2_printf(u8 *buf, u16 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环发送数据
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
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清除中断标志位
        // res = USART_ReceiveData(USART2);                // 读取接收到的数据
    }
}

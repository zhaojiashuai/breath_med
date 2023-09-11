#include "uart3.h"

void uart3_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // 使能USART3时钟

 	// 串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);  // GPIOB10复用为USART3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); // GPIOB11复用为USART3

	// USART3端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // GPIOB10与GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);					// 初始化PB10，PB11   

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);  // 复位串口2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE); // 停止复位

    USART_InitStructure.USART_BaudRate = bound;                                     // 波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 8位数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             /// 奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式

    USART_Init(USART3, &USART_InitStructure); // 初始化串口

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;         // 使能串口2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 从优先级2级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);                           // 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启中断

    USART_Cmd(USART3, ENABLE); // 使能串口
}

// buf:发送区首地址
// len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void uart3_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环发送数据
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ; // 循环发送,直到发送完毕
        USART_SendData(USART3, buf[t]);
    }
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        ;
}

void USART3_IRQHandler(void)
{
    u8 res;
    // static u8  cnt;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); // 清除中断标志位
        res = USART_ReceiveData(USART3);                // 读取接收到的数据
        USART_SendData(USART3, res);
    }
}

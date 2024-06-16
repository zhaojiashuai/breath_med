#include "uart4.h"

/**
 * 初始化UART4串行通信接口
 *
 * 该函数配置GPIOC端口和UART4模块，使其能够进行串行通信。
 * 参数bound指定波特率，例如9600。
 */
void uart4_init(u32 bound)
{
    // 使能UART4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    // 初始化GPIOC引脚，用于UART4的TX和RX
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // UART4的TX和RX引脚
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // 复用功能模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz速度
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // 浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 将GPIOC的10和11引脚配置为UART4复用功能
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

    // 初始化UART4模块
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound; // 设置波特率
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; // 8位数据长度
    USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1位停止位
    USART_InitStruct.USART_Parity = USART_Parity_No; // 无奇偶校验
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_Init(UART4, &USART_InitStruct); // 初始化UART4

    // 使能UART4接收中断
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

    // 使能UART4
    USART_Cmd(UART4, ENABLE); // 使能UART4

    // 配置NVIC，使能UART4中断，并设置优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; // UART4中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC
}


/**
 * 通过UART4发送字符串
 *
 * 该函数使用UART4发送一个字符串，直到发送完成。
 * 参数buf指向要发送的字符串。
 * 参数len指定要发送的字符串长度。
 */
void uart4_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环遍历字符串中的每个字符
    {
        while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
            ; // 等待发送完成标志位被清除
        USART_SendData(UART4, buf[t]); // 发送当前字符
    }
    // 再次循环，确保最后一个字符发送完成后，发送完成标志位被清除
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET)
        ;
}


/**
 * UART4中断服务程序
 *
 * 该函数处理UART4的接收中断请求。
 * 当接收到数据时，它会清除中断标志位，并将数据存储到缓冲区。
 * 然后，它会调用modbus_slave_deal处理接收到的数据。
 */
void UART4_IRQHandler(void)
{
    u8 res; // 用于存储接收到的数据
    // static u8 cnt; // 静态变量，用于计数接收到的字节数

    // 如果接收到新数据
    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        // 清除接收中断标志位
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
        // 读取接收到的数据
        res = USART_ReceiveData(UART4);
        // 调用modbus_slave_deal处理接收到的数据
        modbus_slave_deal(res, 0x01);
    }
}


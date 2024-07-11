#include "uart3.h"

oxygen_t output = {0};

/**
 * 初始化USART3串行通信接口
 *
 * 该函数配置GPIOB端口和USART3模块，使其能够进行串行通信。
 * 参数bound指定波特率，例如9600。
 */
void uart3_init(u32 bound)
{
    // 使能USART3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    // 使能GPIOB时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    // 初始化GPIOB引脚，用于USART3的TX和RX
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // USART3的TX和RX引脚
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // 复用功能模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz速度
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // 浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 将GPIOB的10和11引脚配置为USART3复用功能
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    // 初始化USART3模块
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = bound; // 设置波特率
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; // 8位数据长度
    USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1位停止位
    USART_InitStruct.USART_Parity = USART_Parity_No; // 无奇偶校验
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_Init(USART3, &USART_InitStruct); // 初始化USART3

    // 使能USART3接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // 使能USART3
    USART_Cmd(USART3, ENABLE); // 使能USART3

    // 配置NVIC，使能USART3中断，并设置优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; // USART3中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC
}


/**
 * 通过USART3发送字符串
 *
 * 该函数使用USART3发送一个字符串，直到发送完成。
 * 参数buf指向要发送的字符串。
 * 参数len指定要发送的字符串长度。
 */
void uart3_printf(u8 *buf, u8 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环遍历字符串中的每个字符
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ; // 等待发送完成标志位被清除
        USART_SendData(USART3, buf[t]); // 发送当前字符
    }
    // 再次循环，确保最后一个字符发送完成后，发送完成标志位被清除
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        ;
}


/**
 * USART3中断服务程序
 *
 * 该函数处理USART3的接收中断请求。
 * 当接收到数据时，它会清除中断标志位，并将数据存储到缓冲区。
 * 然后，它会检查数据的起始标志和长度，以及数据的格式。
 * 如果格式正确，它会调用uart3_printf发送数据，并更新输出变量。
 */
void USART3_IRQHandler(void)
{
    static u8 cnt = 0; // 静态变量，用于计数接收到的字节数
    static u8 uart3[100] = {0}; // 静态缓冲区，用于存储接收到的数据

    // 如果接收到新数据
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        // 清除接收中断标志位
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        // 读取接收到的数据并存储到缓冲区
        uart3[cnt++] = USART_ReceiveData(USART3);

        // 检查起始标志和长度
        if (uart3[0] != 0x16 || cnt > 90)
        {
            // 如果不符合起始标志或长度超过限制，重置计数器
            cnt = 0;
        }
        else
        {
            // 如果长度足够
            if (cnt >= 11)
            {
                // 调用uart3_printf发送数据
                uart3_printf(uart3, cnt);
                // 更新输出变量
                output.oxygen = (uart3[3] << 8) + uart3[4];
                output.flow = (uart3[5] << 8) + uart3[6];
                output.temp = (uart3[7] << 8) + uart3[8];
                // 重置计数器
                cnt = 0;
            }
        }
    }
}

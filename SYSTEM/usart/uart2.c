#include "uart2.h"
compressor_t machine = {0};
/**
 * 初始化USART2串行通信接口
 *
 * 该函数配置GPIOA端口和USART2模块，使其能够进行串行通信。
 * 参数bound指定波特率，例如9600。
 */
void uart2_init(u32 bound)
{
    // 使能GPIOA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // 使能USART2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 初始化GPIOA引脚，用于USART2的TX和RX
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // USART2的TX和RX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz速度
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // 上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 将GPIOA的2和3引脚配置为USART2复用功能
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // 初始化USART2模块
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound; // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USART2, &USART_InitStructure); // 初始化USART2

    // 初始化NVIC，配置USART2中断优先级和子优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; // USART2中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC

    // 使能USART2接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // 使能USART2
    USART_Cmd(USART2, ENABLE); // 使能USART2
}


/**
 * 通过USART2发送字符串
 *
 * 该函数使用USART2发送一个字符串，直到发送完成。
 * 参数buf指向要发送的字符串。
 * 参数len指定要发送的字符串长度。
 */
void uart2_printf(u8 *buf, u16 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环遍历字符串中的每个字符
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
            ; // 等待发送完成标志位被清除
        USART_SendData(USART2, buf[t]); // 发送当前字符
    }
    // 再次循环，确保最后一个字符发送完成后，发送完成标志位被清除
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;
}


void USART2_SendString(char *str)
{
    while (*str)
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            ;
        USART_SendData(USART2, *str++);
    }
}

/**
 * USART2中断服务程序
 *
 * 该函数处理USART2的接收中断请求。
 * 当接收到数据时，它会清除中断标志位，并将数据存储到缓冲区。
 * 然后，它会检查数据的起始标志和长度，以及数据的CRC校验。
 * 如果校验通过，它会更新机器的状态变量。
 */
void USART2_IRQHandler(void)
{
    uint16_t crc16 = 0;
    static u8 cnt = 0; // 静态变量，用于计数接收到的字节数
    static u8 uart2[100] = {0}; // 静态缓冲区，用于存储接收到的数据

    // 如果接收到新数据
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        // 清除接收中断标志位
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        // 读取接收到的数据并存储到缓冲区
        uart2[cnt++] = USART_ReceiveData(USART2);

        // 检查起始标志和长度
        if (uart2[0] != 0x01 || cnt > 90)
        {
            // 如果不符合起始标志或长度超过限制，重置计数器
            cnt = 0;
        }
        else
        {
            // 如果长度不够，返回
            if (cnt < 2)
                return;

            // 检查长度和起始标志
            if (uart2[1] == 0x06)
            {
                // 如果长度足够
                if (cnt >= 20)
                {
                    // 计算CRC16校验和
                    crc16 = uart2[18] | (uart2[19] << 8);
                    // 检查CRC校验
                    if (crc16 == get_crc16(uart2, 18))
                    {
                        // 如果校验通过，更新机器状态变量
                        machine.rpm = (uart2[5] | (uart2[6] << 8));
                        machine.err_code = 0;
                        machine.err_code |= (uart2[7] & 0x01) | ((uart2[7 + 1] & 0x01) << 1) | ((uart2[7 + 2] & 0x01) << 2) | ((uart2[7 + 3] & 0x01) << 3) | ((uart2[7 + 4] & 0x01) << 4) | ((uart2[7 + 5] & 0x01) << 5) | ((uart2[7 + 6] & 0x01) << 6) | ((uart2[7 + 7] & 0x01) << 7) | ((uart2[7 + 8] & 0x01) << 8);
                    }
                    // 重置计数器
                    cnt = 0;
                }
            }
            else
            {
                // 如果长度不符合预期，重置计数器
                cnt = 0;
            }
        }
    }
}

/*压缩机参数获取*/
static void compressor_read(void)
{
    uint8_t len = 0;
    uint8_t send[] = {0x01, 0x06, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x4d};
    len = sizeof(send) / sizeof(send[0]);
    uart2_printf(send, len);
}

/*压缩机风机收发设定*/
void compressor_set(uint16_t rpm, uint8_t en, uint8_t dir, uint8_t stop)
{
    static uint8_t flag = 0;
    uint8_t len = 0, i, j;
    uint8_t send[] = {0x01, 0x03, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x55};
    len = sizeof(send) / sizeof(send[0]);
    if (flag++ % 2 == 1) 
    {
        compressor_read();
        return;
    }
    send[3] = rpm;
    send[4] = rpm >> 8;
    send[7] = en;
    send[8] = dir;
    send[9] = stop;
    calculate_CRC(send, (len - 2), &i, &j);
    send[len - 2] = i;
    send[len - 1] = j;
    uart2_printf(send, len);
}

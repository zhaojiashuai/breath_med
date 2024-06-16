#include "sys.h"
#include "usart.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h" //ucos ʹ��
#endif

#if 1
#pragma import(__use_no_semihosting)

struct __FILE
{
	int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
	x = x;
}

int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; 
	USART1->DR = (u8)ch;
	return ch;
}
#endif


/**
 * 初始化USART1串行通信接口
 *
 * 该函数配置GPIOA端口和USART1模块，使其能够进行串行通信。
 * 参数bound指定波特率，例如9600。
 */
void uart_init(u32 bound)
{
    // GPIO端口初始化结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    // USART初始化结构体
    USART_InitTypeDef USART_InitStructure;
    // NVIC初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIOA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // 使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // USART1初始化
    USART_InitStructure.USART_BaudRate = bound; // 设置波特率
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); // GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); // GPIOA10复用为USART1

    // USART1端口初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // GPIOA9和GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz速度
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // 上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA

    // USART1参数初始化
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USART1, &USART_InitStructure); // 初始化USART1

    // 使能USART1
    USART_Cmd(USART1, ENABLE); // 使能USART1
    // 清除发送完成标志位（如果需要）
    // USART_ClearFlag(USART1, USART_FLAG_TC);
    // 使能USART1接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能USART1接收中断

    // USART1 NVIC配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // USART1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC
}

/**
 * 通过USART1发送字符串
 *
 * 该函数使用USART1发送一个字符串，直到发送完成。
 * 参数buf指向要发送的字符串。
 * 参数len指定要发送的字符串长度。
 */
void uart1_printf(u8 *buf, u16 len)
{
    u16 t;
    for (t = 0; t < len; t++) // 循环遍历字符串中的每个字符
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
            ; // 等待发送完成标志位被清除
        USART_SendData(USART1, buf[t]); // 发送当前字符
    }
    // 再次循环，确保最后一个字符发送完成后，发送完成标志位被清除
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
}


/**
 * USART1中断服务程序
 *
 * 该函数处理USART1的接收中断请求。
 * 当接收到数据时，它会清除中断标志位，并将数据存储到缓冲区。
 * 然后，它会调用modbus_slave_deal处理接收到的数据。
 */
void USART1_IRQHandler(void)
{
    u8 Res; // 用于存储接收到的数据

    // 如果接收到新数据
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // 清除接收中断标志位
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        // 读取接收到的数据
        Res = USART_ReceiveData(USART1); // 或者使用 USART1->DR
        // 调用modbus_slave_deal处理接收到的数据
        modbus_slave_deal(Res, 0x01);
    }
}


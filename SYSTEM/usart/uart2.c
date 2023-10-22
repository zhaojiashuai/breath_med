#include "uart2.h"
compressor_t machine = {0};
void uart2_init(u32 bound)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART2, ENABLE);
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

void USART2_SendString(char *str)
{
    while (*str)
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            ;
        USART_SendData(USART2, *str++);
    }
}

void USART2_IRQHandler(void)
{
    static u8 cnt = 0;
    static u8 uart2[100] = {0};
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清除中断标志位
        uart2[cnt++] = USART_ReceiveData(USART2);       // 读取接收到的数据
        if (uart2[0] != 0xAA)
        {
            cnt = 0;
        }
        else
        {
            if (cnt >= 15)
            {
                if (uart2[cnt - 1] == 0x55)
                {
                    machine.speed = uart2[2] | (uart2[3] >> 8);
                    machine.current = uart2[4] | (uart2[5] >> 8);
                    machine.volatge = uart2[6] | (uart2[7] >> 8);
                    machine.err_code = uart2[9];
                    machine.ambient = uart2[10];
                    machine.gas_temp = uart2[11];
                }
                cnt = 0;
            }
        }
    }
}

uint8_t calculateChecksum(uint8_t *data, int size)
{
    uint8_t checksum = 0;

    for (int i = 0; i < size; i++)
    {
        checksum += data[i];
    }

    return ~(checksum) + 1;
}

/*压缩机发送控制指令*/
void compressor_set(int16_t dat, int16_t speed,int16_t fan)
{
    uint8_t send[] = {0xAA, 0x00, 0x01, 0xB8, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x55};
    send[2] = dat;
    send[3] = speed;
    send[4] = speed >> 8;
    send[7] = fan;
    send[14] = calculateChecksum(&send[1], 13);
    uart2_printf(send, 16);
}

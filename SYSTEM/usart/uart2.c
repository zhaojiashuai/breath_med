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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
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
    uint16_t crc16 = 0;
    static u8 cnt = 0;
    static u8 uart2[100] = {0};
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收到数据
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清除中断标志位
        uart2[cnt++] = USART_ReceiveData(USART2);       // 读取接收到的数据
        if (uart2[0] != 0x01 || cnt > 90)
        {
            cnt = 0;
        }
        else
        {
            if (cnt < 2)
                return;
            if (uart2[1] == 0x06)
            {
                if (cnt >= 20)
                {
                    crc16 = uart2[18] | (uart2[19] << 8);
                    if (crc16 == get_crc16(uart2, 18))
                    {
                        machine.rpm = (uart2[5] | (uart2[6] << 8));
                        machine.err_code = 0;
                        machine.err_code |= (uart2[7] & 0x01) | ((uart2[7 + 1] & 0x01) << 1) | ((uart2[7 + 2] & 0x01) << 2) | ((uart2[7 + 3] & 0x01) << 3) | ((uart2[7 + 4] & 0x01) << 4) | ((uart2[7 + 5] & 0x01) << 5) | ((uart2[7 + 6] & 0x01) << 6) | ((uart2[7 + 7] & 0x01) << 7) | ((uart2[7 + 8] & 0x01) << 8);
                    }
                    cnt = 0;
                }
            }
            else
            {
                cnt = 0;
            }
        }
    }
}

/*压缩机发送读取状态指令*/
static void compressor_read(void)
{
    uint8_t len = 0;
    uint8_t send[] = {0x01, 0x06, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x4d};
    len = sizeof(send) / sizeof(send[0]);
    uart2_printf(send, len);
}

/*压缩机发送控制指令和读取指令*/
void compressor_set(uint16_t rpm, uint8_t en, uint8_t dir, uint8_t stop)
{
    static uint8_t flag = 0;
    uint8_t len = 0, i, j;
    uint8_t send[] = {0x01, 0x03, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x55};
    len = sizeof(send) / sizeof(send[0]);
    if (flag++ % 2 == 1) // 分开发送一次发送设定一次发送读取指令
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

#include "gpio.h"

void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // ʹ��GPIOCʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_13; // LEDIO��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                                                  // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                                 // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                             // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                                   // ����
    GPIO_Init(GPIOC, &GPIO_InitStructure);                                                         // ��ʼ��GPIO

    GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}

void pwm_io1_out(void)
{
    static uint8_t count = 0;
    if (count++ < 50)
    {
        TEST_IO3 = 1;
    }
    else
    {
        TEST_IO3 = 0;
    }
    if (count >= 100)
    {
        count = 0;
    }
}

void pwm_io2_out(void)
{
    static uint8_t count = 0;
    if (count++ < 50)
    {
        TEST_IO4 = 1;
    }
    else
    {
        TEST_IO4 = 0;
    }
    if (count >= 100)
    {
        count = 0;
    }
}

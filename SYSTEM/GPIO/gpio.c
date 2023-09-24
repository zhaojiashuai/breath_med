#include "gpio.h"

void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // ʹ��GPIOFʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // LEDIO��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // ����
    GPIO_Init(GPIOC, &GPIO_InitStructure);                  // ��ʼ��GPIO

    GPIO_SetBits(GPIOC, GPIO_Pin_13); // GPIOC13���øߣ�����
}


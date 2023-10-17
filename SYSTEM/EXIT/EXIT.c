#include "EXIT.h"

// �ⲿ�ж�0�������
void EXTI0_IRQHandler(void)
{
    // CODE
    EXTI_ClearITPendingBit(EXTI_Line0); // ���LINE0�ϵ��жϱ�־λ
}

// �ⲿ�жϳ�ʼ������
// ��ʼ��PE2~4,PA0Ϊ�ж�����.
void EXTIX_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // ʹ��GPIOA,GPIOEʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;          // PB0 ��Ӧ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       // ��ͨ����ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;     // ����
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // ��ʼ��GPIOB0

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // ʹ��SYSCFGʱ��

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0); // PA0 ���ӵ��ж���0

    /* ����EXTI_Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;             // LINE0
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // �ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // �����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // ʹ��LINE0
    EXTI_Init(&EXTI_InitStructure);                        // ����

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;             // �ⲿ�ж�0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        // �����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              // ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                              // ����
}

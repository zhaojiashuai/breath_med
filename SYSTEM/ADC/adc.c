#include "adc.h"

// ��ʼ��ADC
void Adc_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // ʹ��ADC1ʱ��

    // �ȳ�ʼ��ADC1ͨ��5 IO��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // PA5 ͨ��5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;     // ģ������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // ����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // ��ʼ��

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);  // ADC1��λ
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); // ��λ����

	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;/*DMAʧ��*/
	ADC_CommonInitStructure.ADC_Mode          = ADC_Mode_Independent;/*����ģʽ*/
	ADC_CommonInitStructure.ADC_Prescaler     = ADC_Prescaler_Div4;/*APB2��4��Ƶ ��84/4=21M*/
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;/*���������׶ε���ʱ5��ʱ��*/
	ADC_CommonInit(&ADC_CommonInitStructure);/*��ʼ��*//*��ʼ��ADC1*/
	ADC_InitStructure.ADC_Resolution  = ADC_Resolution_12b;/*12λģʽ*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;/*ɨ��ģʽ*/
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;/*����ת��*/
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;/*��ֹ������� ʹ���������*/
	ADC_InitStructure.ADC_DataAlign    = ADC_DataAlign_Right;/*�Ҷ���*/
	ADC_InitStructure.ADC_NbrOfConversion = 3;/*2ͨ�� 1*/
	ADC_Init(ADC1,&ADC_InitStructure);/*��ʼ��*/
	ADC_Cmd(ADC1,ENABLE);/*����ת��*/
	//ADC_Init(ADC2,&ADC_InitStructure);/*��ʼ��*/
	//ADC_Cmd(ADC2,ENABLE);/*����ת��*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_480Cycles);/*���ù���ͨ��2 һ������ ����ʱ�� */     // 1����
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,2,ADC_SampleTime_480Cycles);/*���ù���ͨ��2 һ������ ����ʱ�� */    //1����
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,3,ADC_SampleTime_480Cycles);/*���ù���ͨ��3 �������� ����ʱ�� */   //1����
	//ADC_RegularChannelConfig(ADC2,ADC_Channel_6,4,ADC_SampleTime_480Cycles);/*���ù���ͨ��2 һ������ ����ʱ�� */
	ADC_SoftwareStartConv(ADC1);/*�������ת��*/
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//Դ���ݱ仯ʱ����DMA����ADC_DMACmd(ADC1,ENABLE);//ʹ��ADC����
	//ADC_SoftwareStartConv(ADC2);/*�������ת��*/
	//ADC_DMARequestAfterLastTransferCmd(ADC2,ENABLE);//Դ���ݱ仯ʱ����DMA����ADC_DMACmd(ADC1,ENABLE);//ʹ��ADC����
	ADC_DMACmd(ADC1,ENABLE);//ʹ��ADC����
}

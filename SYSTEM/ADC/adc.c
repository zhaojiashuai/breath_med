#include "adc.h"

// ��ʼ��ADC
void Adc_Init(void)
{ 
      GPIO_InitTypeDef GPIO_InitStructure  ;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ��
	// �ȳ�ʼ��ADC1ͨ��5 IO��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // PA4��5��6��7 ͨ��4��5��6��7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;									 // ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;								 // ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);											 // ��ʼ��
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // ʹ��ADC1ʱ��

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);  // ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); // ��λ����

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;					  // ����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles; // ���������׶�֮����ӳ�20��ʱ�� ����ʱ���Ƿ���Ҫ�Ľ�һ��
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;		  // DMAʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;					  // Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);									  // ��ʼ��

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;						// 12λģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;								// ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							// ����ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // ��ֹ������⣬ʹ����������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;		// �ⲿ����ͨ����������ʹ��������������ֵ��㸳ֵ����
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						// �Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 4;									// 1��ת���ڹ��������� Ҳ����ֻת����������1
	ADC_Init(ADC1, &ADC_InitStructure);											// ADC��ʼ��

	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_56Cycles); /*���ù���ͨ��2 һ������ ����ʱ�� */ // 1����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_56Cycles); /*���ù���ͨ��3 �������� ����ʱ�� */ // 1����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_56Cycles); /*���ù���ͨ��2 һ������ ����ʱ�� */ // 1����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_56Cycles); /*���ù���ͨ��2 һ������ ����ʱ�� */ // 1����

	// ʹ��DMA���� after last transfer (Single-ADC mode)
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	/*************ʹ��ADC DMA*************/
	ADC_DMACmd(ADC1, ENABLE);
	/*************ʹ��ADC******************/
	ADC_Cmd(ADC1, ENABLE);
	// ��ʼadcת������������
	ADC_SoftwareStartConv(ADC1);
}

#include "adc.h"

// 初始化ADC
void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOA时钟
	// 先初始化ADC1通道5 IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // PA4、5、6、7 通道4、5、6、7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;									 // 模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;								 // 不带上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);											 // 初始化
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1时钟

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);  // ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); // 复位结束

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;					  // 独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles; // 两个采样阶段之间的延迟20个时钟 采样时间是否需要改进一下
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;		  // DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;					  // 预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);									  // 初始化

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;						// 12位模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;								// 扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							// 连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;		// 外部触发通道，本例子使用软件触发，此值随便赋值即可
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						// 右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 4;									// 1个转换在规则序列中 也就是只转换规则序列1
	ADC_Init(ADC1, &ADC_InitStructure);											// ADC初始化

	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_56Cycles); /*设置规则通道2 一个序列 采样时间 */ // 1可以
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_56Cycles); /*设置规则通道3 二个序列 采样时间 */ // 1可以
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_56Cycles); /*设置规则通道2 一个序列 采样时间 */ // 1可以
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_56Cycles); /*设置规则通道2 一个序列 采样时间 */ // 1可以

	// 使能DMA请求 after last transfer (Single-ADC mode)
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	/*************使能ADC DMA*************/
	ADC_DMACmd(ADC1, ENABLE);
	/*************使能ADC******************/
	ADC_Cmd(ADC1, ENABLE);
	// 开始adc转换，软件触发
	ADC_SoftwareStartConv(ADC1);
}

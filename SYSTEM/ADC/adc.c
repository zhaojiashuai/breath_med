#include "adc.h"

// 初始化ADC
void Adc_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // 使能ADC1时钟

    // 先初始化ADC1通道5 IO口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        // PA5 通道5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;     // 模拟输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 不带上下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);           // 初始化

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);  // ADC1复位
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE); // 复位结束

	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;/*DMA失能*/
	ADC_CommonInitStructure.ADC_Mode          = ADC_Mode_Independent;/*独立模式*/
	ADC_CommonInitStructure.ADC_Prescaler     = ADC_Prescaler_Div4;/*APB2的4分频 即84/4=21M*/
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;/*两个采样阶段的延时5个时钟*/
	ADC_CommonInit(&ADC_CommonInitStructure);/*初始化*//*初始化ADC1*/
	ADC_InitStructure.ADC_Resolution  = ADC_Resolution_12b;/*12位模式*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;/*扫描模式*/
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;/*连续转换*/
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;/*禁止触发检测 使用软件触发*/
	ADC_InitStructure.ADC_DataAlign    = ADC_DataAlign_Right;/*右对齐*/
	ADC_InitStructure.ADC_NbrOfConversion = 3;/*2通道 1*/
	ADC_Init(ADC1,&ADC_InitStructure);/*初始化*/
	ADC_Cmd(ADC1,ENABLE);/*开启转换*/
	//ADC_Init(ADC2,&ADC_InitStructure);/*初始化*/
	//ADC_Cmd(ADC2,ENABLE);/*开启转换*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_480Cycles);/*设置规则通道2 一个序列 采样时间 */     // 1可以
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,2,ADC_SampleTime_480Cycles);/*设置规则通道2 一个序列 采样时间 */    //1可以
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,3,ADC_SampleTime_480Cycles);/*设置规则通道3 二个序列 采样时间 */   //1可以
	//ADC_RegularChannelConfig(ADC2,ADC_Channel_6,4,ADC_SampleTime_480Cycles);/*设置规则通道2 一个序列 采样时间 */
	ADC_SoftwareStartConv(ADC1);/*启动软件转换*/
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//源数据变化时开启DMA传输ADC_DMACmd(ADC1,ENABLE);//使能ADC传输
	//ADC_SoftwareStartConv(ADC2);/*启动软件转换*/
	//ADC_DMARequestAfterLastTransferCmd(ADC2,ENABLE);//源数据变化时开启DMA传输ADC_DMACmd(ADC1,ENABLE);//使能ADC传输
	ADC_DMACmd(ADC1,ENABLE);//使能ADC传输
}

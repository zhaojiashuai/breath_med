#include "adc.h"

// 初始化ADC
void Adc_Init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使能GPIOA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // 初始化ADC1通道5的IO口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // PA4,5,6,7 对应通道4,5,6,7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                                     // 模拟输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                                  // 不上拉也不下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                            // 初始化
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    // 使能ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // ADC1复位
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);  
    // 复位结束
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
    // ADC通用初始化设置
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                        // 独立模式
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;   // 两个采样阶段之间的延迟20个周期
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;         // DMA访问模式禁用
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                      // 预分频器4分频, ADCCLK=PCLK2/4=84/4=21Mhz, ADC时钟不要超过36Mhz
    ADC_CommonInit(&ADC_CommonInitStructure);                                       // 初始化
    // ADC初始化设置
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                           // 12位分辨率
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                     // 扫描转换模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                               // 连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;      // 禁止外部触发转换边缘
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;            // 外部触发转换事件
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                           // 数据右对齐
    ADC_InitStructure.ADC_NbrOfConversion = 4;                                       // 转换序列长度为4
    ADC_Init(ADC1, &ADC_InitStructure);                                              // ADC初始化
    // 配置规则通道
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_56Cycles);       // 配置ADC1的规则通道4，第1个转换，采样时间为56周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_56Cycles);       // 配置ADC1的规则通道5，第2个转换，采样时间为56周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_56Cycles);       // 配置ADC1的规则通道6，第3个转换，采样时间为56周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_56Cycles);       // 配置ADC1的规则通道7，第4个转换，采样时间为56周期
    // 启用DMA请求
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    // 启用ADC的DMA功能
    ADC_DMACmd(ADC1, ENABLE);
    // 启用ADC
    ADC_Cmd(ADC1, ENABLE);
    // 开始软件转换
    ADC_SoftwareStartConv(ADC1);
}


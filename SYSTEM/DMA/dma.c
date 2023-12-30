#include "dma.h"

uint16_t AdcValues[10];

void bsp_InitAdcDMA(void)
{
    // 配置DMA传输
    DMA_InitTypeDef DMA_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)AdcValues;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory; // 确定方向很重要
    DMA_InitStruct.DMA_BufferSize = 4;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_InitStruct.DMA_Channel = DMA_Channel_0;

    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
    DMA_Cmd(DMA2_Stream0, ENABLE);
}

uint16_t get_adc_value(uint8_t ch)
{
    if (ch > 4)
    {
        return 0;
    }
    return AdcValues[ch];
}

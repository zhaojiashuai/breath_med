#include "dma.h"

uint16_t AdcValues[10];

void bsp_InitAdcDMA(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); /*DMA2��ʱ��ʹ��*/
    while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE)
        ;                                                                       /*�ȴ�DMA��������*/
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;                              /*DMAͨ��0*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_BASE + 0x4C;      /*�����ַ*/
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)AdcValues;                /*��ȡ����ַ*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     /*��������赽�ڴ�*/
    DMA_InitStructure.DMA_BufferSize = 4;                                       /*���ݴ��������Ϊ1*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            /*��ַ������*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     /*��ַ����*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; /*���ݳ��Ȱ���*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         /*���ݳ��Ȱ���*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         /*�����ȼ�*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             /*ѭ��ģʽ*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      /*��ֹFIFO*/
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;           /*FIFO��ֵ*/
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 /*���δ���*/
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         /*���δ���*/
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);                                 /**/
    DMA_Cmd(DMA2_Stream0, ENABLE);                                              // ����DMA����
}

void getadcs(void)
{
    while (DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) != RESET) /*�������*/
    {
        DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0); /*����*/
    }

    delay_ms(300);
}



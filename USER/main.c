#include "common.h"


RCC_ClocksTypeDef clk = {0};
int main(void)
{
	delay_init(168);
	RCC_GetClocksFreq(&clk);
	gpio_init();
	bsp_InitAdcDMA();
	Adc_Init();
	TIM3_Int_Init(500-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.   
	TIM2_Configuration(); //
	TIM4_Configuration();
	// EXTIX_Init(); 
	uart_init(9600);
	uart2_init(9600);
	uart3_init(9600);
	uart4_init(9600);
	uart5_init(9600);
	data_init();//��ʼ��������ʾ	
	while (1)
	{
		task_run();
	}
}

#include "common.h"

RCC_ClocksTypeDef clk = {0};
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ����ϵͳ�ж����ȼ�����2
	delay_init(168);
	delay_ms(2000); // �ȴ�ϵͳ�ϵ���ӳ٣��ȴ���Ļ�ϵ���ͨѶ
	RCC_GetClocksFreq(&clk);
	gpio_init();
	bsp_InitAdcDMA();
	Adc_Init();
	Init__ADS1115();
	TIM3_Int_Init(500 - 1, 84 - 1); // 84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.
	TIM2_Configuration();			//
	TIM4_Configuration();
	uart_init(9600);
	uart2_init(9600);
	uart3_init(9600);
	uart4_init(9600);
	uart5_init(9600);
	data_init(); // ��ʼ��������ʾ
	while (1)
	{
		task_run();
	}
}

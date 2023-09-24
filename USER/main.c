#include "common.h"
#include "gpio.h"
#include "dma.h"
#include "adc.h"

RCC_ClocksTypeDef clk = {0};
int main(void)
{
	delay_init(168);
	RCC_GetClocksFreq(&clk);
	gpio_init();
	uart_init(115200);
	bsp_InitAdcDMA();
	Adc_Init();
	init_task();

	while (1)
	{
		run_tasks();
	}
}

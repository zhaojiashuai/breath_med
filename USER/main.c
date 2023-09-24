#include "common.h"
#include "gpio.h"


int main(void)
{
	delay_init(168);
	gpio_init();
	uart_init(115200);
	init_task();

	while (1)
	{
		run_tasks();
	}
}

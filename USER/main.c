#include "common.h"



int main(void)
{
	delay_init(168);
	uart_init(115200);
	init_task();

	while (1)
	{
		run_tasks();
	}
}

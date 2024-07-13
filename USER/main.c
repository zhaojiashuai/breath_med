#include "common.h"

RCC_ClocksTypeDef clk = {0};
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置系统中断优先级分组为2
	delay_init(168); // 初始化延时函数，参数168通常代表系统时钟为168MHz
	RCC_GetClocksFreq(&clk); // 获取系统时钟频率
	gpio_init(); // 初始化通用输入输出（GPIO）
	bsp_InitAdcDMA(); // 初始化模数转换器（ADC）的DMA功能
	Adc_Init(); // 初始化ADC
	Init__ADS1115(); // 初始化ADS1115，可能是一个外部ADC模块
	TIM3_Int_Init(100 - 1, 84 - 1); // 初始化定时器3，设置预分频为84-1，自动重装载值为100-1，生成10KHz的PWM信号
	TIM2_Configuration(); // 配置定时器2
	TIM4_Configuration(); // 配置定时器4 
	uart_init(9600); // 初始化USART，设置波特率为9600
	uart2_init(9600); // 初始化USART2，设置波特率为9600
	uart3_init(9600); // 初始化USART3，设置波特率为9600
	uart4_init(115200); // 初始化USART4，设置波特率为115200
	uart5_init(9600); // 初始化USART5，设置波特率为9600
	delay_ms(2000); // 等待系统上电延时，等待屏幕通信
	data_init(); // 初始化数据显示，可能用于LCD或OLED屏幕等

	while (1)
	{
		adc_Cal();
		task_scheduler();
	}
}

#ifndef __COMMON_H
#define __COMMON_H
#include "sys.h"
#include "delay.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "usart.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "uart5.h"
#include "task.h"
#include "gpio.h"
#include "dma.h"
#include "adc.h"
#include "EXIT.h"
#include "time3.h"
#include "time14.h"
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_rcc.h"

#define BUFFER_SIZE 10

typedef struct common
{
    uint16_t buf[BUFFER_SIZE];
    uint16_t index;
} filter_t, *filter_ptr;

int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

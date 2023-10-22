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

typedef struct sensor
{
    int16_t oxygen;
    int16_t flow;
    int16_t temp;
} oxygen_t, *oxygen_ptr;

typedef struct device
{
    int16_t set_speed;
    int16_t set_switch;
    int16_t set_fan;
    int16_t speed;
    int16_t current;
    int16_t volatge;
    int16_t err_code;
    int16_t ambient;  // ÊÒÎÂ
    int16_t gas_temp; // ÆøÌåÎÂ¶È
} compressor_t, *compressor_ptr;

int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

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
#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_rcc.h"

#define BUFFER_SIZE 10

typedef struct common
{
    uint16_t buf[BUFFER_SIZE];
    uint16_t index;
} filter_t, *filter_ptr;

typedef struct sensor // 氧气传感器信息
{
    int16_t oxygen;
    int16_t flow;
    int16_t temp;
} oxygen_t, *oxygen_ptr;

typedef struct device // 压缩机信息
{
    int16_t set_speed;
    int16_t set_switch;
    int16_t set_fan;
    int16_t speed;
    int16_t current;
    int16_t volatge;
    int16_t err_code;
    int16_t ambient;  // 室温
    int16_t gas_temp; // 气体温度
} compressor_t, *compressor_ptr;

typedef struct display
{
    uint16_t oxygen1;        // 氧气浓度进气
    uint16_t set_oxygen1;    // 设定氧气浓度进气
    uint16_t oxygen2;        // 氧气浓度出气
    uint16_t set_oxygen2;    // 设定氧气浓度出气
    uint16_t flow1;          // 流量进气
    uint16_t set_flow1;      // 设定流量进气
    uint16_t flow2;          // 流量进气
    uint16_t set_flow2;      // 设定流量进气
    uint16_t breath_pre;     // 呼吸压力
    uint16_t set_breath_pre; // 设定呼吸压力
    float tidal_val;         // 潮气量
    float breath_frq;        // 呼吸频率
    float breath_rat;        // 吸呼比
} display_t, *display_ptr;

typedef struct parameters
{
    int8_t breath_stat;           // 呼吸状态
    int8_t last_breath_stat;      // 上次呼吸状态
    uint16_t breath_in_n_time[5]; // 上5次吸气努力时间
    uint16_t breath_out_time[5];  // 上5次呼气时间
    uint16_t breath_in_time[5];   // 上5次吸气时间
    uint16_t in_count;            // 吸相计数
    uint16_t out_count;           // 呼相计数
    uint16_t stat_time;           // 当前状态保持时间
    uint16_t last_stat_time;      // 上一状态保持时间
    uint16_t average_time;        // 前五次平均吸气努力时间
    uint16_t sample_rat;          // 采样率-任务触发频率
    float breath_flow;            // 呼吸流量
    float last_breath_flow;       // 呼吸流量
    float flow_k;                 // 流量导数
    float breath_in[5];           // 上五次吸气量
    float breath_out[5];          // 上五次呼气量
    float breath_in_pre;          // 吸相压力
    float breath_out_pre;         // 呼相压力
} parameters_t, *parameters_ptr;

int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

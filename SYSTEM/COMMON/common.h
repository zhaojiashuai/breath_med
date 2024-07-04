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
#include "flash.h"
#include "modbusmaster.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stm32f4xx_rcc.h"
#include "ADS1115.h"

#define BUFFER_SIZE 11

typedef struct common
{
    uint16_t buf[BUFFER_SIZE];
    uint16_t index;
} filter_t, *filter_ptr;

typedef struct sensor // ������������Ϣ
{
    int16_t oxygen;
    int16_t flow;
    int16_t temp;
    int16_t set_flow;
} oxygen_t, *oxygen_ptr;

typedef struct
{
    double Setpoint;
    double Input;
    double Output;
    double LastError;
    double Integral;
    double Timestamp;
    double Kp;
    double Ki;
    double Kd;
} PIDController;

typedef struct device // ѹ������Ϣ
{
    uint8_t en;
    uint8_t dir;
    uint8_t stop;
    uint16_t err_code;
    uint16_t set_rpm;
    uint16_t rpm;
} compressor_t, *compressor_ptr;

typedef enum 
{
    concentrator_oxygen,
    concentrator_flow,
    Compressor_speed,
    Compressor_err,
    Compressor_setspeed,
    mixed_oxygen,
    mixed_setoxygen,
    breath_pressure,
    breath_offset,
    set_xi_pre,
    set_hu_pre,
    set_pre,
    xiqivalue_1,
    xiqivalue_2,
    xiqivalue_3,
    xiqivalue_4,
    xiqivalue_5,
    huqivalue_1,
    huqivalue_2,
    huqivalue_3,
    huqivalue_4,
    huqivalue_5,
    huxi_flow,
    huxi_offset,
    chaoqi_value,
    huxi_freq,
    huxi_ratio,
    debug_mode,
    p_value_out,
    fan_out,
    relay_plus,
    relay_cycle,
    Compressor_kp,
    Compressor_ki,
    Compressor_kd,
    mixed_kp,
    mixed_ki,
    mixed_kd,
    pressure_kp,
    pressure_ki,
    pressure_kd,
    breath_stat,
    cal_stat,
    pre_x1,
    pre_y1,
    pre_x2,
    pre_y2,
    flow_x1,
    flow_y1,
    flow_x2,
    flow_y2,
    all_num,   
} modbus;



int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

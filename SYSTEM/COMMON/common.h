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
#include "modbusmaster.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    int16_t set_flow;
} oxygen_t, *oxygen_ptr;

typedef struct
{
    double Output;      // 输出值
    double Err_P;       // P偏差
    double Err_I;       // I偏差
    double Err_D;       // D偏差
    double Kp;          // 比例增益
    double Ki;          // 积分增益
    double Kd;          // 微分增益
    double LastError;   // 上一次的偏差
    double L_LastError; // 上上一次的偏差
} PIDController;

typedef struct device // 压缩机信息
{
    uint8_t en;
    uint8_t dir;
    uint8_t stop;
    uint16_t err_code;
    uint16_t set_rpm;
    uint16_t rpm;
} compressor_t, *compressor_ptr;

typedef struct display
{
    int8_t  breath_stat;
    int16_t berath_value;     // 呼吸流量
    int16_t last_berath_value;     // 上一次呼吸流量
    uint16_t breath_pre;     // 呼吸压力   
    uint16_t last_breath_pre;     // 上一次呼吸压力    
    uint16_t breath_frq;        // 呼吸频率
    uint16_t breath_rat;        // 吸呼比
    int32_t breath_count;       //呼吸流量积分
    float k_pre;//斜率
    float b_pre;//偏移
    float k_valu;//斜率
    float b_valu;//偏移    
} display_t, *display_ptr;

typedef enum 
{
    concentrator_oxygen = 0,//制氧机氧气浓度------------压缩机调节使用
    concentrator_flow,//制氧机流量
    Compressor_speed,//压缩机速度
    Compressor_err,//压缩机错误码
    Compressor_setspeed,//压缩机设定速度
    mixed_oxygen,//混氧浓度
    mixed_setoxygen,//混氧设定浓度---------比例阀调节使用
    breath_pressure,//呼吸压力-----------压力传感器采集所得
    breath_offset,//呼吸压力偏移-----------压力传感器采集所得
    set_xi_pre,//设定吸相压力
    set_hu_pre,//设定呼相压力
    set_pre,//设定跟随压力
    xiqivalue_1,//吸气量1
    xiqivalue_2,//吸气量2
    xiqivalue_3,//吸气量3
    xiqivalue_4,//吸气量4
    xiqivalue_5,//吸气量5
    huqivalue_1,//呼气量1
    huqivalue_2,//呼气量2
    huqivalue_3,//呼气量3
    huqivalue_4,//呼气量4
    huqivalue_5,//呼气量5
    huxi_flow,//呼吸流量
    huxi_offset,//呼吸流量偏移
    chaoqi_value,//潮气量----------就是吸气量
    huxi_freq,//呼吸频率
    huxi_ratio,//呼吸比
    debug_mode,//调试模式
    p_value_out,//比例阀输出
    fan_out,//风机输出
    relay_plus,//阀序1时间
    relay_cycle,//阀序2时间
    Compressor_kp,//制氧机PID参数
    Compressor_ki,//制氧机PID参数
    Compressor_kd,//制氧机PID参数
    mixed_kp,//混氧PID参数
    mixed_ki,//混氧PID参数
    mixed_kd,//混氧PID参数
    pressure_kp,//压力PID参数
    pressure_ki,//压力PID参数
    pressure_kd,//压力PID参数
    breath_stat,//呼吸模式
    cal_stat,//采集的状态
    pre_x1,//压力标定X1值
    pre_y1,//压力标定Y1值
    pre_x2,//压力标定X2值
    pre_y2,//压力标定Y1值
    flow_x1,//流量标定X1值
    flow_y1,//流量标定Y1值
    flow_x2,//流量标定X2值
    flow_y2,//流量标定Y2值    
}modbus;


int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

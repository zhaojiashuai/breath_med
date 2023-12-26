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
    uint8_t relay1;
    uint8_t relay2;
    uint8_t relay3;
    uint8_t relay4;
    int8_t  breath_stat;
    int16_t breath_pre;     // 呼吸压力  
    int16_t xi_value;     // 吸气量
    int16_t hu_value;     // 呼气量
    int16_t berath_value;     // 呼吸流量
    uint16_t qiti_pre;     // 气体压力
    uint16_t set_breath_pre; // 设定呼吸压力
    uint16_t set_p_value; // 设定比例阀输出
    uint16_t set_fan_out; // 设定无刷风机输出
    uint16_t tidal_val;         // 潮气量
    uint16_t breath_frq;        // 呼吸频率
    uint16_t breath_rat;        // 吸呼比
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

typedef enum 
{
    concentrator_oxygen = 0,//制氧机氧气浓度
    concentrator_flow,//制氧机流量
    concentrator_setflow,//制氧机流量设定
    Compressor_setswitch,//压缩机开关
    Compressor_setfun,//压缩机风扇开关
    Compressor_speed,//压缩机速度
    Compressor_setspeed,//压缩机设定速度
    Compressor_cur,//压缩机电流
    Compressor_vol,//压缩机电压
    Compressor_err,//压缩机错误码
    Compressor_ambient,//压缩机室温
    Compressor_gas_temp,//压缩机气体温度
    concentrator_mode,//制氧机模式，闭环-开环
    mixed_oxygen,//混氧浓度
    mixed_flow,//混氧浓度
    mixed_setoxygen,//混氧设定浓度
    xi_pre,//吸相压力
    hu_pre,//呼相压力
    set_pre,//设定压力
    xiqi_value,//吸气量
    huqi_value,//呼气量
    huxi_flow,//呼吸流量
    chaoqi_value,//潮气量
    huxi_freq,//呼吸频率
    huxi_ratio,//呼吸比
    debug_mode,//调试模式
    p_value_out,//比例阀输出
    fan_out,//风机输出
    relay1,//继电器1
    relay2,//继电器2
    relay3,//继电器3
    relay4,//继电器4
    Compressor_kp,//制氧机PID参数KP
    Compressor_ki,//制氧机PID参数KI
    Compressor_kd,//制氧机PID参数KD
    mixed_kp,//混氧PID参数
    mixed_ki,//混氧PID参数
    mixed_kd,//混氧PID参数
    pressure_kp,//压力PID参数
    pressure_ki,//压力PID参数
    pressure_kd,//压力PID参数
    breath_pressure,//呼吸压力
}modbus;


int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

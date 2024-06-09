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

#define BUFFER_SIZE 10

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

typedef struct display
{
    int8_t  breath_stat;
    int16_t berath_value;     // ��������
    int16_t last_berath_value;     // ��һ�κ�������
    uint16_t breath_pre;     // ����ѹ��   
    uint16_t last_breath_pre;     // ��һ�κ���ѹ��    
    uint16_t breath_frq;        // ����Ƶ��
    uint16_t breath_rat;        // ������
    int32_t breath_count;       //������������
    float k_pre;//б��
    float b_pre;//ƫ��
    float k_valu;//б��
    float b_valu;//ƫ��    
} display_t, *display_ptr;

typedef enum 
{
    concentrator_oxygen = 0,//����������Ũ��------------ѹ��������ʹ��
    concentrator_flow,//����������
    Compressor_speed,//ѹ�����ٶ�
    Compressor_err,//ѹ����������
    Compressor_setspeed,//ѹ�����趨�ٶ�
    mixed_oxygen,//����Ũ��
    mixed_setoxygen,//�����趨Ũ��---------����������ʹ��
    breath_pressure,//����ѹ��-----------ѹ���������ɼ�����
    breath_offset,//����ѹ��ƫ��-----------ѹ���������ɼ�����
    set_xi_pre,//�趨����ѹ��
    set_hu_pre,//�趨����ѹ��
    set_pre,//�趨����ѹ��
    xiqivalue_1,//������1
    xiqivalue_2,//������2
    xiqivalue_3,//������3
    xiqivalue_4,//������4
    xiqivalue_5,//������5
    huqivalue_1,//������1
    huqivalue_2,//������2
    huqivalue_3,//������3
    huqivalue_4,//������4
    huqivalue_5,//������5
    huxi_flow,//��������
    huxi_offset,//��������ƫ��
    chaoqi_value,//������----------����������
    huxi_freq,//����Ƶ��
    huxi_ratio,//������
    debug_mode,//����ģʽ
    p_value_out,//���������
    fan_out,//������
    relay_plus,//����1ʱ��
    relay_cycle,//����2ʱ��
    Compressor_kp,//������PID����
    Compressor_ki,//������PID����
    Compressor_kd,//������PID����
    mixed_kp,//����PID����
    mixed_ki,//����PID����
    mixed_kd,//����PID����
    pressure_kp,//ѹ��PID����
    pressure_ki,//ѹ��PID����
    pressure_kd,//ѹ��PID����
    breath_stat,//����ģʽ
    cal_stat,//�ɼ���״̬
    pre_x1,//ѹ���궨X1ֵ--�洢״̬��ʼֵ
    pre_y1,//ѹ���궨Y1ֵ
    pre_x2,//ѹ���궨X2ֵ
    pre_y2,//ѹ���궨Y1ֵ
    flow_x1,//�����궨X1ֵ
    flow_y1,//�����궨Y1ֵ
    flow_x2,//�����궨X2ֵ
    flow_y2,//�����궨Y2ֵ 
    all_num,   
}modbus;


int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

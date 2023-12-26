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

typedef struct sensor // ������������Ϣ
{
    int16_t oxygen;
    int16_t flow;
    int16_t temp;
    int16_t set_flow;
} oxygen_t, *oxygen_ptr;

typedef struct
{
    double Output;      // ���ֵ
    double Err_P;       // Pƫ��
    double Err_I;       // Iƫ��
    double Err_D;       // Dƫ��
    double Kp;          // ��������
    double Ki;          // ��������
    double Kd;          // ΢������
    double LastError;   // ��һ�ε�ƫ��
    double L_LastError; // ����һ�ε�ƫ��
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
    uint8_t relay1;
    uint8_t relay2;
    uint8_t relay3;
    uint8_t relay4;
    int8_t  breath_stat;
    int16_t breath_pre;     // ����ѹ��  
    int16_t xi_value;     // ������
    int16_t hu_value;     // ������
    int16_t berath_value;     // ��������
    uint16_t qiti_pre;     // ����ѹ��
    uint16_t set_breath_pre; // �趨����ѹ��
    uint16_t set_p_value; // �趨���������
    uint16_t set_fan_out; // �趨��ˢ������
    uint16_t tidal_val;         // ������
    uint16_t breath_frq;        // ����Ƶ��
    uint16_t breath_rat;        // ������
} display_t, *display_ptr;

typedef struct parameters
{
    int8_t breath_stat;           // ����״̬
    int8_t last_breath_stat;      // �ϴκ���״̬
    uint16_t breath_in_n_time[5]; // ��5������Ŭ��ʱ��
    uint16_t breath_out_time[5];  // ��5�κ���ʱ��
    uint16_t breath_in_time[5];   // ��5������ʱ��
    uint16_t in_count;            // �������
    uint16_t out_count;           // �������
    uint16_t stat_time;           // ��ǰ״̬����ʱ��
    uint16_t last_stat_time;      // ��һ״̬����ʱ��
    uint16_t average_time;        // ǰ���ƽ������Ŭ��ʱ��
    uint16_t sample_rat;          // ������-���񴥷�Ƶ��
    float breath_flow;            // ��������
    float last_breath_flow;       // ��������
    float flow_k;                 // ��������
    float breath_in[5];           // �����������
    float breath_out[5];          // ����κ�����
    float breath_in_pre;          // ����ѹ��
    float breath_out_pre;         // ����ѹ��
} parameters_t, *parameters_ptr;

typedef enum 
{
    concentrator_oxygen = 0,//����������Ũ��
    concentrator_flow,//����������
    concentrator_setflow,//�����������趨
    Compressor_setswitch,//ѹ��������
    Compressor_setfun,//ѹ�������ȿ���
    Compressor_speed,//ѹ�����ٶ�
    Compressor_setspeed,//ѹ�����趨�ٶ�
    Compressor_cur,//ѹ��������
    Compressor_vol,//ѹ������ѹ
    Compressor_err,//ѹ����������
    Compressor_ambient,//ѹ��������
    Compressor_gas_temp,//ѹ���������¶�
    concentrator_mode,//������ģʽ���ջ�-����
    mixed_oxygen,//����Ũ��
    mixed_flow,//����Ũ��
    mixed_setoxygen,//�����趨Ũ��
    xi_pre,//����ѹ��
    hu_pre,//����ѹ��
    set_pre,//�趨ѹ��
    xiqi_value,//������
    huqi_value,//������
    huxi_flow,//��������
    chaoqi_value,//������
    huxi_freq,//����Ƶ��
    huxi_ratio,//������
    debug_mode,//����ģʽ
    p_value_out,//���������
    fan_out,//������
    relay1,//�̵���1
    relay2,//�̵���2
    relay3,//�̵���3
    relay4,//�̵���4
    Compressor_kp,//������PID����KP
    Compressor_ki,//������PID����KI
    Compressor_kd,//������PID����KD
    mixed_kp,//����PID����
    mixed_ki,//����PID����
    mixed_kd,//����PID����
    pressure_kp,//ѹ��PID����
    pressure_ki,//ѹ��PID����
    pressure_kd,//ѹ��PID����
    breath_pressure,//����ѹ��
}modbus;


int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

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

typedef struct sensor // ������������Ϣ
{
    int16_t oxygen;
    int16_t flow;
    int16_t temp;
} oxygen_t, *oxygen_ptr;

typedef struct device // ѹ������Ϣ
{
    int16_t set_speed;
    int16_t set_switch;
    int16_t set_fan;
    int16_t speed;
    int16_t current;
    int16_t volatge;
    int16_t err_code;
    int16_t ambient;  // ����
    int16_t gas_temp; // �����¶�
} compressor_t, *compressor_ptr;

typedef struct display
{
    uint16_t oxygen1;        // ����Ũ�Ƚ���
    uint16_t set_oxygen1;    // �趨����Ũ�Ƚ���
    uint16_t oxygen2;        // ����Ũ�ȳ���
    uint16_t set_oxygen2;    // �趨����Ũ�ȳ���
    uint16_t flow1;          // ��������
    uint16_t set_flow1;      // �趨��������
    uint16_t flow2;          // ��������
    uint16_t set_flow2;      // �趨��������
    uint16_t breath_pre;     // ����ѹ��
    uint16_t set_breath_pre; // �趨����ѹ��
    float tidal_val;         // ������
    float breath_frq;        // ����Ƶ��
    float breath_rat;        // ������
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

int clampFilter(int value, int minValue, int maxValue);
float sliding_average_filter(filter_t *data, float input);
float median_filter(filter_t *data, float input);
#endif

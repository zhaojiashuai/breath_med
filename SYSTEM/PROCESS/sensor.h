#ifndef __SENSOR_H
#define __SENSOR_H
#include "common.h"

typedef struct display
{
    int16_t berath_value;     
    uint16_t breath_pre;       
    uint16_t breath_frq;        
    uint16_t breath_rat;        
    uint16_t breath_chaoqi;
    float k_pre;
    float b_pre;
    float k_valu;
    float b_valu;
} display_t, *display_ptr;

extern uint16_t modbus_dis[128];
extern display_t sensor;
void set_sensor_value(void);
void datatrans_deal(void);
void closed_loop_control(void);
void data_init(void);
void timing_task(void);
void print_task(void);
void write_flash(void);
void adc_Cal(void);
void display_trans(void);
#endif

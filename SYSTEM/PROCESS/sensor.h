#ifndef __SENSOR_H
#define __SENSOR_H
#include "common.h"

extern uint16_t modbus_dis[128];

void set_sensor_value(void);
void datatrans_deal(void);
void closed_loop_control(void);
void data_init(void);
void timing_task(void);
void print_task(void);
void write_flash(void);
#endif

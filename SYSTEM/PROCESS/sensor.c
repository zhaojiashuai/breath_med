#include "sensor.h"

display_t sensor = {0};

void get_sensor_value(void)
{
    float f = 0.0;
    f = (float)get_adc_value(0);
    f = f/4096*3.3;
    sensor.breath_pre = (uint16_t)f;
    

}
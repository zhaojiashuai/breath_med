#include "sensor.h"

display_t sensor = {0};

void get_sensor_value(void)
{
    double f = 0;
    f = (double)get_adc_value(0);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 16 - 3.2;
        sensor.breath_pre = (uint16_t)(f * 1000);
    }

    f = (double)get_adc_value(1);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 2 / 5 - 0.58;
        sensor.set_breath_pre = (uint16_t)(f * 1000);
    }

    f = (double)get_adc_value(2);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 16 - 3.2;
        sensor.set_flow2 = (uint16_t)(f * 1000);
    }

    f = (double)get_adc_value(3);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 2 / 5 - 0.58;
        sensor.flow2 = (uint16_t)(f * 1000);
    }
}

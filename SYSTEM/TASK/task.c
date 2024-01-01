#include "task.h"
#include "common.h"

uint8_t task_tic = 0;

void task_run(void)
{
    static uint32_t task_time = 0;
    if (task_tic == 0)
        return;
    task_tic = 0;
    task_time++;
    if (task_time % 5 == 0) // 5ms
    {
        set_sensor_value();
        closed_loop_control();
    }
    if (task_time % 10 == 0) // 10ms
    {
    }
    if (task_time % 20 == 0) // 20ms
    {
    }
    if (task_time % 100 == 0) // 100ms
    {
        datatrans_deal();
    }
    if (task_time % 500 == 0) // 500ms
    {
        LED = !LED;
        print_task();
    }
}

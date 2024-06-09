#include "task.h"
#include "common.h"

#define     TASK_5MS        0X01
#define     TASK_10MS        0X02
#define     TASK_100MS        0X04
#define     TASK_1000MS        0X08

uint32_t task_flag = 0;

void task_time(uint32_t cnt)
{
    if(cnt%5==0)
    {
        task_flag|=TASK_5MS;
    }
    if(cnt%10==0)
    {
        task_flag|=TASK_10MS;
    }  
    if(cnt%100==0)
    {
        task_flag|=TASK_100MS;
         
    }   
    if(cnt%1000==0)
    {
        task_flag|=TASK_1000MS;
    }         
}

void task_run(void)
{
    if(task_flag&TASK_5MS)
    {
        task_flag&=~TASK_5MS;
        set_sensor_value();
    }
    if(task_flag&TASK_10MS)
    {
        task_flag&=~TASK_10MS;
        adc_Cal();
    } 
    if(task_flag&TASK_100MS)
    {
        task_flag&=~TASK_100MS;
        datatrans_deal();
        closed_loop_control();          
    }  
    if(task_flag&TASK_1000MS)
    {
        task_flag&=~TASK_1000MS;
        write_flash();        
        LED = !LED;
        print_task();         
    }         
}

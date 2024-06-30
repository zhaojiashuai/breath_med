#include "task.h"
#include "common.h"

// 任务执行函数
void task_5ms(void) {
    // 执行5ms任务
    set_sensor_value();
}

void task_10ms(void) {
    // 执行10ms任务
    soft_process();
    breath_value_Cal();
    data_send();    
}

void task_20ms(void) {
    // 执行20ms任务
    display_trans();
}

void task_100ms(void) {
    // 执行100ms任务
    datatrans_deal();
    closed_loop_control();     
}

void task_1000ms(void) {
    // 执行1000ms任务
    write_flash();        
    LED = !LED;
    print_task();     
}

// 定义任务结构体
typedef struct {
    uint32_t period;       // 任务执行周期
    void (*task)(void);    // 任务回调函数
    uint32_t cnt;           // 任务执行的间隔时间
} Task;
 
// 任务列表
Task tasks[] = {
    { 5, task_5ms, 0 },
    { 10, task_10ms, 0 },
    { 20, task_20ms, 0 },
    { 100, task_100ms, 0 },
    { 1000, task_1000ms, 0 }
};

void task_time(void)
{
    for (int i = 0; i < (sizeof(tasks) / sizeof(Task)); i++) {
        tasks[i].cnt++;       // 执行任务
    }
}

// 任务调度函数
void task_scheduler( void ) {
    for (int i = 0; i < (sizeof(tasks) / sizeof(Task)); i++) {
        if ( tasks[i].cnt / tasks[i].period > 0) {
            tasks[i].task();       // 执行任务
            tasks[i].cnt = 0;
        }
    }
}


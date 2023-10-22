#include "task.h"
#include "common.h"

typedef void (*TaskFunction)(); // 定义任务函数指针类型

// 任务结构体
typedef struct
{
    TaskFunction func;         // 任务函数指针
    unsigned int interval;     // 任务执行间隔时间
    unsigned int elapsed_time; // 任务已经过去的时间
} Task;

Task tasks[10];    // 定义任务数组
int num_tasks = 0; // 当前任务数量

void add_task(TaskFunction func, unsigned int interval)
{
    if (num_tasks >= sizeof(tasks) / sizeof(tasks[0]))
    {
        printf("Too many tasks, cannot add more!\n");
        return;
    }

    tasks[num_tasks].func = func;
    tasks[num_tasks].interval = interval;
    tasks[num_tasks].elapsed_time = 0;
    num_tasks++;
}

void remove_task(int index)
{
    int i = 0;
    if (index >= num_tasks)
    {
        printf("Invalid task index!\n");
        return;
    }

    for (i = index; i < num_tasks - 1; i++)
    {
        tasks[i] = tasks[i + 1];
    }

    num_tasks--;
}

void run_tasks(void)
{
    int i = 0;
    for (i = 0; i < num_tasks; i++)
    {
        tasks[i].elapsed_time++;
        delay_us(10);
        if (tasks[i].elapsed_time >= tasks[i].interval)
        {
            tasks[i].func();
            tasks[i].elapsed_time = 0;
        }
    }
}

void task1(void)
{
    compressor_set(1,1,1);
    printf("Task 1 executed!\n");
}

void task2(void)
{
    LED = !LED;
    printf("Task 2 executed!\n");
}

void init_task(void)
{
    add_task(task1, 10000);
    add_task(task2, 50000);
}

////////////////////////////////////////////////////////////example///////////////////////////////////////////////////////////////////////////////
// 以下是示例任务函数
// void task1()
// {
//     printf("Task 1 executed!\n");
// }

// void task2()
// {
//     printf("Task 2 executed!\n");
// }

// int main()
// {
//     add_task(task1, 100);
//     add_task(task2, 200);

//     while (1)
//     {
//         run_tasks(); // 开始任务调度
//     }

//     return 0;
// }

// 这个任务调度程序使用了一个任务数组来存储任务信息，每个任务包括任务函数指针、执行间隔时间和已经过去的时间。通过调用add_task函数可以添加新的任务，通过调用remove_task函数可以删除任务。任务调度主循环run_tasks会不断遍历任务数组，判断任务是否需要执行，并执行相应的任务函数。

// 在上面的示例中，我们定义了两个简单的任务函数task1和task2，分别打印出一段文本。通过调用add_task函数将这两个任务添加进任务数组，并指定每个任务的执行间隔时间。最后，在main函数中调用run_tasks函数开始任务调度。

// 你可以根据实际需求修改任务函数的内容，并使用add_task和remove_task函数来动态调整任务。请注意，这只是一个简单的示例，实际应用中可能需要考虑更多的细节和安全性问题。

#include "task.h"
#include "common.h"

typedef void (*TaskFunction)(); // ����������ָ������

// ����ṹ��
typedef struct
{
    TaskFunction func;         // ������ָ��
    unsigned int interval;     // ����ִ�м��ʱ��
    unsigned int elapsed_time; // �����Ѿ���ȥ��ʱ��
} Task;

Task tasks[10];    // ������������
int num_tasks = 0; // ��ǰ��������

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

extern display_t sensor;
void task1(void)
{
    get_sensor_value();
    // printf("Task 1 executed!\n");
}

void task2(void)
{
    LED = !LED;
    set_sensor_value();
    // printf("Task 2 executed!\n");
}

void task3(void)
{
    uint8_t buff[] = {"test example\r\n"};
    uart3_printf(buff, strlen((const char *)buff));
    uart4_printf(buff, strlen((const char *)buff));
    uart5_printf(buff, strlen((const char *)buff));
    // printf("Task 3 executed!\n");
}

void init_task(void)
{
    add_task(task1, 700);  // 7ms
    add_task(task2, 10100); // 101ms
    add_task(task3, 100010); // 1000ms
}

////////////////////////////////////////////////////////////example///////////////////////////////////////////////////////////////////////////////
// ������ʾ��������
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
//         run_tasks(); // ��ʼ�������
//     }

//     return 0;
// }

// ���������ȳ���ʹ����һ�������������洢������Ϣ��ÿ���������������ָ�롢ִ�м��ʱ����Ѿ���ȥ��ʱ�䡣ͨ������add_task������������µ�����ͨ������remove_task��������ɾ���������������ѭ��run_tasks�᲻�ϱ����������飬�ж������Ƿ���Ҫִ�У���ִ����Ӧ����������

// �������ʾ���У����Ƕ����������򵥵�������task1��task2���ֱ��ӡ��һ���ı���ͨ������add_task������������������ӽ��������飬��ָ��ÿ�������ִ�м��ʱ�䡣�����main�����е���run_tasks������ʼ������ȡ�

// ����Ը���ʵ�������޸������������ݣ���ʹ��add_task��remove_task��������̬����������ע�⣬��ֻ��һ���򵥵�ʾ����ʵ��Ӧ���п�����Ҫ���Ǹ����ϸ�ںͰ�ȫ�����⡣

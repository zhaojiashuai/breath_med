#include "common.h"

// 限幅滤波
int clampFilter(int value, int minValue, int maxValue)
{
    if (value < minValue)
    {
        return minValue;
    }
    else if (value > maxValue)
    {
        return maxValue;
    }
    else
    {
        return value;
    }
}

// 滑动平均滤波函数
float sliding_average_filter(filter_t *data, float input)
{
    float sum = 0;
    // 更新滑动窗口缓存
    data->buf[data->index] = input;
    data->index = (data->index + 1) % BUFFER_SIZE;

    // 计算滑动平均值
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        sum += data->buf[i];
    }

    float average = sum / BUFFER_SIZE;
    return average;
}

// 中值滤波函数
float median_filter(filter_t *data, float input)
{
    // 更新缓存
    data->buf[data->index] = input;
    data->index = (data->index + 1) % BUFFER_SIZE;

    // 冒泡排序
    for (int i = 0; i < BUFFER_SIZE - 1; i++)
    {
        for (int j = 0; j < BUFFER_SIZE - i - 1; j++)
        {
            if (data->buf[j] > data->buf[j + 1])
            {
                float temp = data->buf[j];
                data->buf[j] = data->buf[j + 1];
                data->buf[j + 1] = temp;
            }
        }
    }

    // 取中值
    float median;
    if (BUFFER_SIZE % 2 == 0)
    {
        int middle_index = BUFFER_SIZE / 2;
        median = (data->buf[middle_index - 1] + data->buf[middle_index]) / 2.0;
    }
    else
    {
        int middle_index = BUFFER_SIZE / 2;
        median = data->buf[middle_index];
    }

    return median;
}

#include "common.h"

/**
 * @brief 将值限制在指定范围内
 * 
 * 这个函数用于确保输入值不会超出指定的最小值和最大值范围。
 * 如果输入值小于最小值，则返回最小值；如果输入值大于最大值，则返回最大值；
 * 如果输入值在范围内，则直接返回输入值。
 * 
 * @param value 需要被限制的输入值
 * @param minValue 允许的最小值
 * @param maxValue 允许的最大值
 * @return int 被限制在指定范围内的值
 */
int clampFilter(int value, int minValue, int maxValue)
{
    if (value < minValue)
    {
        // 如果输入值小于最小值，返回最小值
        return minValue;
    }
    else if (value > maxValue)
    {
        // 如果输入值大于最大值，返回最大值
        return maxValue;
    }
    else
    {
        // 如果输入值在指定范围内，直接返回输入值
        return value;
    }
}



/**
 * @brief 对输入数据进行滑动平均滤波
 * 
 * 这个函数接受一个指向滤波器结构的指针和一个新的输入值。
 * 它将新的输入值存储在缓冲区中，并更新索引值，然后计算缓冲区中所有值的平均值。
 * 这个平均值作为滤波器的输出返回。
 * 
 * @param data 指向滤波器结构的指针，该结构包含一个浮点数数组和当前索引值
 * @param input 新的输入值，将被加入到滑动平均滤波器中
 * @return float 缓冲区中所有值的平均值
 */
float sliding_average_filter(filter_t *data, float input)
{
    float sum = 0; // 初始化求和变量

    // 将新的输入值存储在缓冲区中的当前索引位置
    data->buf[data->index] = input;
    // 更新索引值，如果到达缓冲区末尾则循环回到开始位置
    data->index = (data->index + 1) % BUFFER_SIZE;

    // 计算缓冲区中所有值的总和
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        sum += data->buf[i];
    }

    // 计算平均值
    float average = sum / BUFFER_SIZE;
    // 返回平均值作为滤波器的输出
    return average;
}


/**
 * @brief 中值滤波器
 * 
 * 这个函数实现了中值滤波器，它将新的输入值存储在缓冲区中，并对缓冲区中的值进行排序，
 * 然后返回中间位置的值作为滤波器的输出。中值滤波器是一种常用的去除噪声的方法，
 * 特别适合于去除脉冲噪声和尖峰噪声。
 * 
 * @param data 指向滤波器结构的指针，该结构包含一个浮点数数组和当前索引值
 * @param input 新的输入值，将被加入到中值滤波器中
 * @return float 缓冲区中所有值排序后的中值
 */
float median_filter(filter_t *data, float input)
{
    // 更新缓冲区，将新的输入值存储在当前索引位置
    data->buf[data->index] = input;
    // 更新索引值，如果到达缓冲区末尾则循环回到开始位置
    data->index = (data->index + 1) % BUFFER_SIZE;

    // 使用冒泡排序算法对缓冲区中的值进行排序
    for (int i = 0; i < BUFFER_SIZE - 1; i++)
    {
        for (int j = 0; j < BUFFER_SIZE - i - 1; j++)
        {
            if (data->buf[j] > data->buf[j + 1])
            {
                // 如果当前值大于下一个值，交换它们
                float temp = data->buf[j];
                data->buf[j] = data->buf[j + 1];
                data->buf[j + 1] = temp;
            }
        }
    }

    // 计算并返回中值
    float median;
    if (BUFFER_SIZE % 2 == 0)
    {
        // 如果缓冲区大小是偶数，中值是中间两个数的平均值
        int middle_index = BUFFER_SIZE / 2;
        median = (data->buf[middle_index - 1] + data->buf[middle_index]) / 2.0;
    }
    else
    {
        // 如果缓冲区大小是奇数，中值是中间的数
        int middle_index = BUFFER_SIZE / 2;
        median = data->buf[middle_index];
    }

    return median;
}

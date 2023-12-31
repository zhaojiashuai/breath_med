#include "sensor.h"

display_t sensor = {0};
uint16_t modbus_dis[50];

extern oxygen_t input;
extern oxygen_t output;
extern compressor_t machine;
void get_sensor_value(void)
{
    static filter_t breath_pre = {0}, berath_value = {0};
    double f = 0;
    f = (double)get_adc_value(0);
    f = sliding_average_filter(&breath_pre, f);
    f = f / 4096 * 3.3;
    f = f * 16000 - 3200;
    if (f > 40000)
    {
        f = 40000;
    }
    else if (f <= 0)
    {
        f = 0;
    }
    sensor.breath_pre = (uint16_t)f + 1000 - modbus_dis[breath_offset]; // 增加偏移提高系统稳定性

    f = (double)get_adc_value(1);
    f = sliding_average_filter(&berath_value, f);
    f = f / 4096 * 3.3;
    f = f * 5.0 / 3.3;
    f = 1.975442640 * pow(f, 7) - 34.3990880 * pow(f, 6) + 240.94350 * pow(f, 5) - 868.19 * pow(f, 4) + 1715.71913742 * pow(f, 3) - 1855.499169 * pow(f, 2) + 1105.51131 * f - 401.78595;
    if (f > 150)
    {
        f = 150;
    }
    else if (f < -150)
    {
        f = -150;
    }
    sensor.berath_value = (uint16_t)(f * 10) + 1000 - modbus_dis[huxi_offset]; // 增加偏移提高系统稳定性
    if (sensor.berath_value > 3)
    {
        sensor.breath_stat = 1;
    }
    else if (sensor.berath_value < 0)
    {
        sensor.breath_stat = -1;
    }
}
/*阀序控制逻辑*/
void valve_control(void)
{
    static uint16_t count = 0;
    if (modbus_dis[relay_plus] >= modbus_dis[relay_cycle]) // 设定错误
    {
        TEST_IO1 = 0;
        TEST_IO2 = 0;
        return;
    }
    if (count++ < modbus_dis[relay_plus])
    {
        TEST_IO1 = 1;
        TEST_IO2 = 0;
    }
    else
    {
        TEST_IO1 = 1;
        TEST_IO2 = 1;
    }
    if (count >= modbus_dis[relay_cycle])
    {
        count = 0;
    }
}

/*周期获取流量积分*/
void get_breathcount(void)
{
    static int8_t last_stat = 0;
    if (last_stat != sensor.breath_stat)
    {
        sensor.breath_count = 0;
        last_stat = sensor.breath_stat;
    }
    if (sensor.berath_value != 0)
        sensor.breath_count += sensor.berath_value;
}
/*定时器的任务累计和处理任务--1ms*/
void timing_task(void)
{
    valve_control();
    get_breathcount();
}

void get_breathvalue(uint32_t time)
{
    int32_t value = 0;
    if (sensor.breath_stat == 1) // 吸气状态计算呼气积分量
    {
        value = -sensor.breath_count * time / 1000;
        modbus_dis[huqivalue_5] = modbus_dis[huqivalue_4];
        modbus_dis[huqivalue_4] = modbus_dis[huqivalue_3];
        modbus_dis[huqivalue_3] = modbus_dis[huqivalue_2];
        modbus_dis[huqivalue_2] = modbus_dis[huqivalue_1];
        modbus_dis[huqivalue_1] = value;
    }
    else if (sensor.breath_stat == -1) // 呼气状态计算吸气积分
    {
        value = sensor.breath_count * time / 1000;
        modbus_dis[xiqivalue_5] = modbus_dis[xiqivalue_4];
        modbus_dis[xiqivalue_4] = modbus_dis[xiqivalue_3];
        modbus_dis[xiqivalue_3] = modbus_dis[xiqivalue_2];
        modbus_dis[xiqivalue_2] = modbus_dis[xiqivalue_1];
        modbus_dis[xiqivalue_1] = value;
    }
}

void breath_Deal(void)
{
    static int8_t last_stat = 0;
    static uint16_t count = 0;
    static uint32_t in_time, out_time;
    if (last_stat != sensor.breath_stat)
    {
        last_stat = sensor.breath_stat;
        if (sensor.breath_stat == 1)
        {
            out_time = g_count;
            get_breathvalue(out_time);
            g_count = 0;
        }
        else if (sensor.breath_stat == -1)
        {
            in_time = g_count;
            get_breathvalue(in_time);
            g_count = 0;
        }
    }
    if (sensor.breath_stat == 0) // 7ms
    {
        if (count++ > 500) // 3.5s没有呼吸的状态
        {
            count = 500;
            in_time = 0;
            out_time = 0;
            sensor.berath_value = 0;
        }
    }
    else
    {
        count = 0;
    }

    sensor.breath_frq = 60000 / (in_time + out_time); // 呼吸频率
    sensor.breath_rat = in_time * 100 / out_time;     // 吸呼比
}

void display_trans(void)
{
    modbus_dis[concentrator_oxygen] = input.oxygen;
    modbus_dis[concentrator_flow] = input.flow;
    modbus_dis[Compressor_speed] = machine.rpm;
    modbus_dis[Compressor_err] = machine.err_code;

    modbus_dis[mixed_oxygen] = output.oxygen;

    modbus_dis[huxi_freq] = sensor.breath_frq;
    modbus_dis[huxi_ratio] = sensor.breath_rat;

    modbus_dis[huxi_flow] = sensor.berath_value; ////为了解决传输中不能出现负数的问题
    modbus_dis[breath_pressure] = sensor.breath_pre;
    modbus_dis[chaoqi_value] = modbus_dis[xiqivalue_1];
    modbus_slave_parse(modbus_dis);
    // 解析到之后把数据设定到指定结构体之中
}

void datatrans_deal(void)
{
    compressor_set(modbus_dis[Compressor_setspeed], 0x01, 0x01, 0x00);
    compressor_read();
    display_trans();
}

void set_sensor_value(void)
{
    get_sensor_value();
    breath_Deal();
    TIM_SetCompare1(TIM3, modbus_dis[fan_out]);     // 设定无刷风机
    TIM_SetCompare2(TIM3, modbus_dis[p_value_out]); // 设定比例阀输出
}

// PID控制器更新函数
uint16_t PID_cal(PIDController *pid, double setpoint, double input, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    uint16_t out;
    pid->Kp = Kp / 1000000;
    pid->Ki = Ki / 1000000;
    pid->Kd = Kd / 1000000;
    // 计算偏差
    pid->Err_P = setpoint - input;

    pid->Err_I = pid->Err_P - pid->LastError;
    pid->Err_D = pid->Err_P - 2 * pid->LastError + pid->L_LastError;
    pid->Output += pid->Kp * pid->Err_P + pid->Ki * pid->Err_I + pid->Kd * pid->Err_D;
    if (setpoint == 0 || pid->Output <= 0)
    {
        pid->Output = 0;
        pid->LastError = 0.0;
        pid->L_LastError = 0.0;
    }
    pid->L_LastError = pid->LastError;
    pid->LastError = pid->Err_P;
    out = (uint16_t)pid->Output;
    return out;
}

void Compressor_closed(void)
{
    // PIDController Compressor = {0};
}

void mixed_closed(void)
{
    PIDController mixed = {0};
    modbus_dis[p_value_out] = PID_cal(&mixed, modbus_dis[mixed_setoxygen], output.oxygen, modbus_dis[mixed_kp], modbus_dis[mixed_ki], modbus_dis[mixed_kd]);
}

void pressure_closed(void)
{
    PIDController pressure = {0};
    modbus_dis[fan_out] = PID_cal(&pressure, modbus_dis[set_pre], sensor.breath_pre, modbus_dis[mixed_kp], modbus_dis[mixed_ki], modbus_dis[mixed_kd]);
}
void closed_loop_control(void)
{
    if (modbus_dis[debug_mode])
        return;
    Compressor_closed();
    mixed_closed();
    pressure_closed();
}

void data_init(void)
{
    modbus_dis[breath_offset] = 1000;
    modbus_dis[huxi_offset] = 1072;
    modbus_dis[relay_plus] = 3200;
    modbus_dis[relay_cycle] = 3700;
}

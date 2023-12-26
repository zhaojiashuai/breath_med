#include "sensor.h"

display_t sensor = {0};
uint16_t modbus_dis[50];

extern oxygen_t input;
extern oxygen_t output;
extern compressor_t machine;
void get_sensor_value(void)
{
    double f = 0;
    f = (double)get_adc_value(0);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 16000 - 3200;
        sensor.qiti_pre = (int16_t)f;
    }
    else
    {
        sensor.qiti_pre = 0;
    }

    f = (double)get_adc_value(1);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 400 - 580;
        sensor.breath_pre = (int16_t)f;
    }
    else
    {
        sensor.breath_pre = 0;
    }

    if (abs(sensor.breath_pre) < 10)
    {
        sensor.breath_stat = 0;
    }
    else if (sensor.breath_pre > 10)
    {
        sensor.breath_stat = 1;
    }
    else if (sensor.breath_pre < -10)
    {
        sensor.breath_stat = -1;
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
            g_count = 0;
        }
        else if (sensor.breath_stat == -1)
        {
            in_time = g_count;
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
            sensor.xi_value = 0;
            sensor.hu_value = 0;
            sensor.berath_value = 0;
        }
    }
    else
    {
        count = 0;
    }

    sensor.breath_frq = 60000 / (in_time + out_time); // 呼吸频率
    sensor.breath_rat = in_time * 100 / out_time;     // 吸呼比

    if (sensor.breath_stat == 1) // 吸气量--呼吸流量
    {
        sensor.xi_value = output.flow;
        sensor.berath_value = output.flow;
    }
    else if (sensor.breath_stat == -1) // 呼气量--呼吸流量
    {
        sensor.hu_value = output.flow;
        sensor.berath_value = -output.flow;
    }
}

void display_trans(void)
{
    // modbus_dis[Compressor_speed] = machine.speed;
    // modbus_dis[Compressor_cur] = machine.current;
    // modbus_dis[Compressor_vol] = machine.volatge;
    // modbus_dis[Compressor_err] = machine.err_code;
    // modbus_dis[Compressor_ambient] = machine.ambient;
    // modbus_dis[Compressor_gas_temp] = machine.gas_temp;

    modbus_dis[concentrator_oxygen] = input.oxygen;
    modbus_dis[concentrator_flow] = input.flow;
    modbus_dis[mixed_oxygen] = output.oxygen;
    modbus_dis[mixed_flow] = output.flow;

    modbus_dis[huxi_freq] = sensor.breath_frq;
    modbus_dis[huxi_ratio] = sensor.breath_rat;

    modbus_dis[xiqi_value] = sensor.xi_value;
    modbus_dis[huqi_value] = sensor.hu_value;
    modbus_dis[huxi_flow] = sensor.berath_value; ////为了解决传输中不能出现负数的问题
    modbus_dis[chaoqi_value] = sensor.xi_value;

    (sensor.breath_pre > 0) ? (modbus_dis[xi_pre] = sensor.breath_pre) : (modbus_dis[hu_pre] = sensor.breath_pre); // 为了解决传输中不能出现负数的问题

    modbus_dis[breath_pressure] = sensor.qiti_pre;

    modbus_slave_parse(modbus_dis);
    // 解析到之后把数据设定到指定结构体之中
    // machine.set_fan = modbus_dis[Compressor_setfun];
    // machine.set_switch = modbus_dis[Compressor_setswitch];
    // machine.set_speed = modbus_dis[Compressor_setspeed];
    sensor.set_p_value = modbus_dis[p_value_out];
    sensor.set_fan_out = modbus_dis[fan_out];

    sensor.relay1 = modbus_dis[relay1];
    sensor.relay2 = modbus_dis[relay2];
    sensor.relay3 = modbus_dis[relay3];
    sensor.relay4 = modbus_dis[relay4];
    if (modbus_dis[debug_mode])
    {
    }
    (sensor.relay1 & 0x01) ? (TEST_IO1 = 1) : (TEST_IO1 = 0);
    (sensor.relay2 & 0x01) ? (TEST_IO2 = 1) : (TEST_IO2 = 0);
    (sensor.relay3 & 0x01) ? (TEST_IO3 = 1) : (TEST_IO3 = 0);
    (sensor.relay4 & 0x01) ? (TEST_IO4 = 1) : (TEST_IO4 = 0);
}

void datatrans_deal(void)
{
    compressor_set(1000,0x01,0x01,0x00);
    compressor_read();
    display_trans();
}

void set_sensor_value(void)
{
    get_sensor_value();
    breath_Deal();
    TIM_SetCompare1(TIM3, sensor.set_fan_out); // 设定无刷风机
    TIM_SetCompare2(TIM3, sensor.set_p_value); // 设定比例阀输出
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
    sensor.set_p_value = PID_cal(&mixed, modbus_dis[mixed_setoxygen], output.oxygen, modbus_dis[mixed_kp], modbus_dis[mixed_ki], modbus_dis[mixed_kd]);
}

void pressure_closed(void)
{
    PIDController pressure = {0};
    sensor.set_fan_out = PID_cal(&pressure, modbus_dis[set_pre], sensor.qiti_pre, modbus_dis[mixed_kp], modbus_dis[mixed_ki], modbus_dis[mixed_kd]);
}
void closed_loop_control(void)
{
    if (modbus_dis[debug_mode])
        return;
    Compressor_closed();
    mixed_closed();
    pressure_closed();
}

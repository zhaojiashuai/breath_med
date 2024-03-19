#include "sensor.h"

display_t sensor = {0};
uint16_t modbus_dis[128];

extern oxygen_t input;
extern oxygen_t output;
extern compressor_t machine;

void zhjw_test(void)
{
    input.oxygen = rand() % 1000 / 10;
    input.flow = rand() % 1000 / 10;
    machine.rpm = rand() % 5000;

    machine.err_code = rand() % 255;
    output.oxygen = rand() % 1000 / 10;
}
/*数据自动标定*/
void liner_cal(void)
{
    float f, f1;
    if (modbus_dis[cal_stat] == 0)
    {
        sensor.k_pre = 1;
        sensor.k_valu = 1;
        sensor.b_pre = 0;
        sensor.b_valu = 0;
    }
    else
    {
        f = (float)(modbus_dis[pre_y2] - modbus_dis[pre_y1]);
        f1 = (float)(modbus_dis[pre_x2] - modbus_dis[pre_x1]);
        sensor.k_pre = f / f1;
        f = (float)modbus_dis[pre_y1];
        sensor.b_pre = f - sensor.k_pre * modbus_dis[pre_x1];

        f = (float)(modbus_dis[flow_y2] - modbus_dis[flow_y1]);
        f1 = (float)(modbus_dis[flow_x2] - modbus_dis[flow_x1]);
        sensor.k_valu = f / f1;
        f = (float)modbus_dis[flow_y1];
        sensor.b_valu = f - sensor.k_valu * modbus_dis[flow_x1];
    }
}

void get_sensor_value(void)
{
    static filter_t breath_pre = {0}, berath_value = {0};
    double f = 0;
    liner_cal();
    sensor.last_breath_pre = sensor.breath_pre;
    f = (double)get_adc_value(0);
    f = sliding_average_filter(&breath_pre, f);
    f = f * sensor.k_pre + sensor.b_pre;
    if (f > 5000)
    {
        f = 5000;
    }
    else if (f <= 0)
    {
        f = 0;
    }
    sensor.breath_pre = (uint16_t)(f + 1000 - modbus_dis[breath_offset]); // 增加偏移提高系统稳定性

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    sensor.last_berath_value = sensor.berath_value;
    f = (double)get_adc_value(1);
    f = sliding_average_filter(&berath_value, f);
    f = f * sensor.k_valu + sensor.b_valu;
    sensor.berath_value = (int16_t)(f + 1000 - modbus_dis[huxi_offset]); // 增加偏移提高系统稳定性
    // zhjw_test();
    if (abs(sensor.berath_value) < 3) // 减小系统稳定误差
    {
        sensor.breath_stat = 0;
    }
    else if (sensor.berath_value > 0)
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
    uint16_t temp = 0;
    static uint8_t flag = 0, last_flag = 0;
    static uint16_t count = 0;
    if (count++ < modbus_dis[relay_plus])
    {
        if (flag == 0)
        {
            TEST_IO1 = 0;
            TEST_IO2 = 1;
        }
        else
        {
            TEST_IO1 = 1;
            TEST_IO2 = 0;
        }
        last_flag = flag;
    }
    else
    {
        TEST_IO1 = 0;
        TEST_IO2 = 0;
        if (last_flag == 1)
        {
            flag = 0;
        }
        else
        {
            flag = 1;
        }
    }
    temp = modbus_dis[relay_cycle] + modbus_dis[relay_plus];
    if (count >= temp)
    {
        count = 0;
    }
}

/*周期获取流量积分*/
void get_breathcount(void)
{
    static int8_t last_stat = 0;
    if (last_stat != sensor.breath_stat && sensor.breath_stat != 0)
    {
        sensor.breath_count = 0;
        last_stat = sensor.breath_stat;
    }
    if (sensor.breath_stat != 0) // 根据呼吸状态获取呼吸流量累计值
        sensor.breath_count += sensor.berath_value / 1000;
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
        value = -sensor.breath_count * time / 1000 / 60; // L/min转化成ml 10是系数
        modbus_dis[huqivalue_5] = modbus_dis[huqivalue_4];
        modbus_dis[huqivalue_4] = modbus_dis[huqivalue_3];
        modbus_dis[huqivalue_3] = modbus_dis[huqivalue_2];
        modbus_dis[huqivalue_2] = modbus_dis[huqivalue_1];
        modbus_dis[huqivalue_1] = value;
    }
    else if (sensor.breath_stat == -1) // 呼气状态计算吸气积分
    {
        value = sensor.breath_count * time / 1000 / 60;
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
    compressor_set(modbus_dis[Compressor_setspeed], 0x00, 0x00, 0x00);
    display_trans();
}

void set_sensor_value(void)
{
    get_sensor_value();
    breath_Deal();
    // TIM_SetCompare1(TIM3, modbus_dis[fan_out]);     // 设定无刷风机
    // TIM_SetCompare2(TIM3, modbus_dis[p_value_out]); // 设定比例阀输出
    test_pwm();
}

// PID控制器更新函数
uint16_t PID_cal(PIDController *pid, double setpoint, double input, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    uint16_t out;
    pid->Kp = (double)Kp / 1000000;
    pid->Ki = (double)Ki / 1000000;
    pid->Kd = (double)Kd / 1000000;
    // 计算偏差
    pid->Err_P = setpoint - input;

    pid->Err_I = pid->Err_P - pid->LastError;
    pid->Err_D = pid->Err_P - 2 * pid->LastError + pid->L_LastError;
    pid->Output += pid->Kp * pid->Err_P + pid->Ki * pid->Err_I + pid->Kd * pid->Err_D;
    if (setpoint == 0 || pid->Output <= 0 || pid->Err_P < 0)
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
    static PIDController mixed = {0};
    modbus_dis[p_value_out] = PID_cal(&mixed, modbus_dis[mixed_setoxygen], output.oxygen, modbus_dis[mixed_kp], modbus_dis[mixed_ki], modbus_dis[mixed_kd]);
    if (modbus_dis[p_value_out] > 500)
    {
        modbus_dis[p_value_out] = 500;
    }
}

void pressure_closed(void)
{
    static PIDController pressure = {0};
    if (modbus_dis[breath_stat] == 1) // 恒压模式---射钉固定输出
    {
        modbus_dis[fan_out] = modbus_dis[set_pre];
        FAN_BREAK = 0;
    }
    else // 跟随模式
    {
        if (sensor.breath_stat == 1 && (sensor.berath_value - sensor.last_berath_value > 0)&&(sensor.breath_pre<=modbus_dis[set_xi_pre])) // 吸气的时候并且吸气加速度为正的时候风机工作
        {
            modbus_dis[fan_out] = PID_cal(&pressure, modbus_dis[set_xi_pre], sensor.breath_pre, modbus_dis[pressure_kp], modbus_dis[pressure_ki], modbus_dis[pressure_kd]);
            FAN_BREAK = 0;
        }
        else
        {
            modbus_dis[fan_out] = 0;
            FAN_BREAK = 1;
        }
    }

    if (modbus_dis[fan_out] > 500)
    {
        modbus_dis[fan_out] = 500;
    }
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
    modbus_dis[huxi_offset] = 1000;
    modbus_dis[relay_plus] = 3200;
    modbus_dis[relay_cycle] = 500;

    modbus_dis[Compressor_kp] = 1000;
    modbus_dis[Compressor_ki] = 0;
    modbus_dis[Compressor_kd] = 0;

    modbus_dis[mixed_kp] = 1000;
    modbus_dis[mixed_ki] = 0;
    modbus_dis[mixed_kd] = 0;

    modbus_dis[pressure_kp] = 1000;
    modbus_dis[pressure_ki] = 0;
    modbus_dis[pressure_kd] = 0;

    fmc_read(pre_x1, modbus_dis);
}

void write_flash(void)
{
    if (modbus_dev.write_flag)
    {
        fmc_write(modbus_dev.write_flag, modbus_dis);
        modbus_dev.write_flag = 0;
    }
}

void print_task(void)
{
    // printf("adc1:%d,adc2:%d\r\n", get_adc_value(0), get_adc_value(1));
    // printf("breath_pre:%d,berath_value:%d\r\n", sensor.breath_pre, sensor.berath_value);
}

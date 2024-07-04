#include "sensor.h"

display_t sensor = {0};
uint16_t modbus_dis[128];

extern oxygen_t input;
extern oxygen_t output;
extern compressor_t machine;

/*线性方程线性转化*/
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

double adc_bu[3];
void adc_Cal(void)
{
    adc_bu[0] = (double)Get_ATOD(0);
    adc_bu[1] = (double)Get_ATOD(1);
    adc_bu[2] = (double)Get_ATOD(2);
}

/*
F :L/MIN
P :CMH20
*/
void get_sensor_value(void)
{
    static filter_t breath_pre = {0}, berath_value = {0};
    double f = 0;
    liner_cal();
    // f = (double)adc_bu[0]/32767*500;
    f = (double)adc_bu[0];
    f = median_filter(&breath_pre, f);
    f = f * sensor.k_pre + sensor.b_pre;
    // f = -0.143224203722728*f*f*f+0.809567865321594*f*f+8091152612841201*f-5.13046589156455;
    // if(f<0) P=0;
    // if(f>25) P=25;
    sensor.breath_pre = (uint16_t)(f + 1000 - modbus_dis[breath_offset]); 

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // f = (double)adc_bu[1]/32767*500;
    f = (double)adc_bu[1];
    f = median_filter(&berath_value, f);
    f = f * sensor.k_valu + sensor.b_valu;
    // f = 2.74273849138829*pow(f,7)-50.1823050124469*pow(f,6)+373.525387950575*pow(f,5)-1453.24303259627*pow(f,4)+3168.88268278098*pow(f,3)-3871.08286011955*pow(f,2)+2549.63479716526*f-827.709782903377;
    // if(f>150) f = 150;
    sensor.berath_value = (int16_t)(f + 1000 - modbus_dis[huxi_offset]); 
}
/*3205线序触发机制*/
void valve_control(void)
{
    uint16_t temp = 0;
    static uint8_t flag = 0, last_flag = 0;
    static uint16_t count = 0;
    if (count++ < modbus_dis[relay_plus])
    {
        if (flag == 0)
        {
            TEST_IO1 = 1;
            TEST_IO2 = 0;
        }
        else
        {
            TEST_IO1 = 0;
            TEST_IO2 = 1;
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


/*task--1ms*/
void timing_task(void)
{
    valve_control();
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

    modbus_dis[huxi_flow] = sensor.berath_value; 
    modbus_dis[breath_pressure] = sensor.breath_pre;
    modbus_dis[chaoqi_value] = sensor.breath_chaoqi;
    modbus_slave_parse(modbus_dis);

}

void datatrans_deal(void)
{
    compressor_set(modbus_dis[Compressor_setspeed], 0x00, 0x00, 0x00);
}

void set_sensor_value(void)
{
    get_sensor_value();
    TIM_SetCompare1(TIM3, modbus_dis[fan_out]);     
    TIM_SetCompare2(TIM3, modbus_dis[p_value_out]); 
}


/**
 * PID控制算法函数 100ms调度周期
 * @param pid 指向PID控制器的指针
 * @param setpoint 设定值
 * @param input 当前输入值
 * @param Kp 比例增益
 * @param Ki 积分增益
 * @param Kd 微分增益
 * @return 控制输出值
 */
uint16_t PID_cal(PIDController *pid, double setpoint, double input, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    uint16_t out;
    // 将整数增益转换为更精确的double类型，以便进行计算
    pid->Kp = (double)Kp / 10000;
    pid->Ki = (double)Ki / 10000;
    pid->Kd = (double)Kd / 10000;
    // 计算误差：设定值 - 当前输入值
    double error = setpoint - input;
    // 假设采样时间为1，实际应用中可能需要根据系统时钟来调整
    pid->Timestamp = 0.1;
    // 计算积分项，防止积分饱和
    double integral = pid->Integral + pid->Ki * error * pid->Timestamp;
    // 计算微分项
    double derivative = (error - pid->LastError) / pid->Timestamp;
    // 计算输出增量
    double outputIncrement = pid->Kp * error + integral + pid->Kd * derivative;
    // 累加输出增量到历史输出
    pid->Output += outputIncrement;
    // 更新误差为上一次的误差
    pid->LastError = error;
    // 更新积分项
    pid->Integral = integral;
    // 输出限幅，限制在0到500之间
    if (pid->Output > 100)
    {
        pid->Output = 100;
    }
    else if (pid->Output < 0)
    {
        pid->Output = 0;
    }
    // 将输出转换为uint16_t类型
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
    if (modbus_dis[p_value_out] > 100)
    {
        modbus_dis[p_value_out] = 100;
    }
}

void pressure_closed(void)
{
    static PIDController pressure = {0};
    if (modbus_dis[breath_stat] == 1) 
    {
        modbus_dis[fan_out] = modbus_dis[set_pre];
        FAN_BREAK_OFF;
    }
    else 
    {
        modbus_dis[fan_out] = PID_cal(&pressure, Set, sensor.breath_pre, modbus_dis[pressure_kp], modbus_dis[pressure_ki], modbus_dis[pressure_kd]);  
    }

    if (modbus_dis[fan_out] > 100)
    {
        modbus_dis[fan_out] = 100;
    }
}

void closed_loop_control(void)
{
    if (modbus_dis[debug_mode])
    {
        FAN_BREAK_OFF;
        return;
    }
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

    // modbus_dis[pre_x1] = 255;
    // modbus_dis[pre_y1] = 0;
    // modbus_dis[pre_x2] = 200;
    // modbus_dis[pre_y2] = 1000;

    // modbus_dis[flow_x1] = 1834;
    // modbus_dis[flow_y1] = 0;
    // modbus_dis[flow_x2] = 1921;
    // modbus_dis[flow_y2] = 500;

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
    // printf("adc1:%.2f,adc2:%.2f,adc3:%.2f\r\n", adc_bu[0], adc_bu[1],adc_bu[2]);
}

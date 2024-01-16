#include "sensor.h"

display_t sensor = {0};
uint16_t modbus_dis[50];

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

void get_sensor_value(void)
{
    static uint16_t i = 0;
    static filter_t breath_pre = {0}, berath_value = {0};
    double f = 0;
    f = (double)get_adc_value(0);
    f = sliding_average_filter(&breath_pre, f);
    f = f / 4096 * 3.3;
    f = f * 2000 - 400;
    if (f > 5000)
    {
        f = 5000;
    }
    else if (f <= 0)
    {
        f = 0;
    }
    i++;
    // f = (5 - 5 * sin(3.14 * i / 1000)) * 100;
    sensor.breath_pre = (uint16_t)(f + 1000 - modbus_dis[breath_offset]); // ����ƫ�����ϵͳ�ȶ���

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
    // f = 50 * cos(3.14 * i / 500);
    sensor.berath_value = (int16_t)(f + 1000 - modbus_dis[huxi_offset]); // ����ƫ�����ϵͳ�ȶ���
    // zhjw_test();
    if (abs(sensor.berath_value) < 5) // ��Сϵͳ�ȶ����
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
/*��������߼�*/
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

/*���ڻ�ȡ��������*/
void get_breathcount(void)
{
    static int8_t last_stat = 0;
    if (last_stat != sensor.breath_stat && sensor.breath_stat != 0)
    {
        sensor.breath_count = 0;
        last_stat = sensor.breath_stat;
    }
    if (sensor.breath_stat != 0) // ���ݺ���״̬��ȡ���������ۼ�ֵ
        sensor.breath_count += sensor.berath_value;
}
/*��ʱ���������ۼƺʹ�������--1ms*/
void timing_task(void)
{
    valve_control();
    get_breathcount();
}

void get_breathvalue(uint32_t time)
{
    int32_t value = 0;
    if (sensor.breath_stat == 1) // ����״̬�������������
    {
        value = -sensor.breath_count * time / 1000 / 60 / 10; // L/minת����ml 10��ϵ��
        modbus_dis[huqivalue_5] = modbus_dis[huqivalue_4];
        modbus_dis[huqivalue_4] = modbus_dis[huqivalue_3];
        modbus_dis[huqivalue_3] = modbus_dis[huqivalue_2];
        modbus_dis[huqivalue_2] = modbus_dis[huqivalue_1];
        modbus_dis[huqivalue_1] = value;
    }
    else if (sensor.breath_stat == -1) // ����״̬������������
    {
        value = sensor.breath_count * time / 1000 / 60 / 10;
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
        if (count++ > 500) // 3.5sû�к�����״̬
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

    sensor.breath_frq = 60000 / (in_time + out_time); // ����Ƶ��
    sensor.breath_rat = in_time * 100 / out_time;     // ������
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

    modbus_dis[huxi_flow] = sensor.berath_value; ////Ϊ�˽�������в��ܳ��ָ���������
    modbus_dis[breath_pressure] = sensor.breath_pre;
    modbus_dis[chaoqi_value] = modbus_dis[xiqivalue_1];
    modbus_slave_parse(modbus_dis);
    // ������֮��������趨��ָ���ṹ��֮��
}

void datatrans_deal(void)
{
    compressor_set(modbus_dis[Compressor_setspeed], 0x01, 0x01, 0x00);
    display_trans();
}

void set_sensor_value(void)
{
    get_sensor_value();
    breath_Deal();
    TIM_SetCompare1(TIM3, modbus_dis[fan_out]);     // �趨��ˢ���
    TIM_SetCompare2(TIM3, modbus_dis[p_value_out]); // �趨���������
}

// PID���������º���
uint16_t PID_cal(PIDController *pid, double setpoint, double input, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    uint16_t out;
    pid->Kp = (double)Kp / 1000000;
    pid->Ki = (double)Ki / 1000000;
    pid->Kd = (double)Kd / 1000000;
    // ����ƫ��
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
    if (modbus_dis[breath_stat] == 1) // ��ѹģʽ
    {
        modbus_dis[fan_out] = PID_cal(&pressure, modbus_dis[set_pre], sensor.breath_pre, modbus_dis[pressure_kp], modbus_dis[pressure_ki], modbus_dis[pressure_kd]);
    }
    else // ����ģʽ
    {
        if (sensor.breath_stat == 1)
        {
            modbus_dis[fan_out] = PID_cal(&pressure, modbus_dis[set_xi_pre], sensor.breath_pre, modbus_dis[pressure_kp], modbus_dis[pressure_ki], modbus_dis[pressure_kd]);
        }
        else
        {
            modbus_dis[fan_out] = 0;
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
    modbus_dis[relay_plus] = 32;
    modbus_dis[relay_cycle] = 5;

    modbus_dis[Compressor_kp] = 1000;
    modbus_dis[Compressor_ki] = 0;
    modbus_dis[Compressor_kd] = 0;

    modbus_dis[mixed_kp] = 1000;
    modbus_dis[mixed_ki] = 0;
    modbus_dis[mixed_kd] = 0;

    modbus_dis[pressure_kp] = 1000;
    modbus_dis[pressure_ki] = 0;
    modbus_dis[pressure_kd] = 0;
}

void print_task(void)
{
    // printf("adc1:%d,adc2:%d\r\n", get_adc_value(0), get_adc_value(1));
    // printf("breath_pre:%d,berath_value:%d\r\n", sensor.breath_pre, sensor.berath_value);
}

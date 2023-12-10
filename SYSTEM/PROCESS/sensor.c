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
        f = f * 16 - 3.2;
        sensor.qiti_pre = (uint16_t)(f * 1000);
    }
    else
    {
        sensor.qiti_pre = 0;
    }

    f = (double)get_adc_value(1);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 2 / 5 - 0.58;
        // sensor.breath_pre = (uint16_t)(f * 1000);
    }
    else
    {
        // sensor.qiti_pre = 0;
    }    

    f = (double)get_adc_value(2);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 16 - 3.2;
        // sensor.set_flow2 = (uint16_t)(f * 1000);
    }
        else
    {
        // sensor.qiti_pre = 0;
    }

    f = (double)get_adc_value(3);
    f = f / 4096 * 3.3;
    if (f >= 0.2 && f <= 2.7)
    {
        f = f * 2 / 5 - 0.58;
        sensor.breath_pre = (uint16_t)(f * 1000);
    }
        else
    {
        sensor.breath_pre = 0;
    }

    modbus_dis[Compressor_setswitch] = machine.set_switch;
    modbus_dis[Compressor_setfun] = machine.set_fan;
    modbus_dis[Compressor_speed] = machine.speed;
    modbus_dis[Compressor_setspeed] = machine.set_speed;
    modbus_dis[Compressor_cur] = machine.current;
    modbus_dis[Compressor_vol] = machine.volatge;
    modbus_dis[Compressor_err] = machine.err_code;
    modbus_dis[Compressor_ambient] = machine.ambient;
    modbus_dis[Compressor_gas_temp] = machine.gas_temp;

    modbus_dis[concentrator_oxygen] = input.oxygen;
    modbus_dis[concentrator_flow] = input.flow;
    modbus_dis[mixed_oxygen] = output.oxygen;
    modbus_dis[mixed_flow] = output.flow;

    modbus_dis[xi_pre] = sensor.qiti_pre;
    modbus_dis[hu_pre] = sensor.breath_pre;

    modbus_dis[p_value_out] = sensor.set_p_value;
    modbus_dis[fan_out] = sensor.set_fan_out;

    modbus_slave_parse(modbus_dis);

    if (modbus_dis[debug_mode])
    {
        machine.set_switch = modbus_dis[Compressor_setswitch];
        machine.set_fan = modbus_dis[Compressor_setfun];
        machine.set_speed = modbus_dis[Compressor_setspeed];
        sensor.set_p_value = modbus_dis[p_value_out];
        sensor.set_fan_out = modbus_dis[fan_out];        
    }
}

void set_sensor_value(void)
{
    compressor_set(machine.set_switch,machine.set_speed,machine.set_fan);//压缩机设定
    TIM_SetCompare1(TIM3, sensor.set_fan_out);//设定无刷风机
    TIM_SetCompare2(TIM3, sensor.set_p_value);//设定比例阀输出
}

// PID控制器初始化函数
void PID_Init(PIDController *pid, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    pid->Kp = Kp / 1000;
    pid->Ki = Ki / 1000;
    pid->Kd = Kd / 1000;
    pid->LastError = 0.0;
    pid->L_LastError = 0.0;
}

// PID控制器更新函数
void PID_cal(PIDController *pid, double setpoint, double input)
{
    // 计算偏差
    pid->Err_P = setpoint - input;

    pid->Err_I = pid->Err_P - pid->LastError;
    pid->Err_D = pid->Err_P - 2 * pid->LastError + pid->L_LastError;
    pid->Output += pid->Kp * pid->Err_P + pid->Ki * pid->Err_I + pid->Kd * pid->Err_D;
    if (setpoint == 0)
    {
        pid->Output = 0;
    }
    pid->L_LastError = pid->LastError;
    pid->LastError = pid->Err_P;
}



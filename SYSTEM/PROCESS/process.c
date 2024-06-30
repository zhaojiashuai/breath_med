#include "process.h"

float Flow,FlowLast,FlowD,FlowDLast,FlowD2,XiSum[10],HuSum[10],XiP,HuP;

int16_t State,StateLast,XiTryTime[10],XiTime[10],HuTime[10],XiTik,HuTik,StateTime,StateTimeLast,Hz,Set,I_Trigger_Flow = 10,Trigger_flag,ItoE_Flow,ItoE_Flow_temp;

/*流量的采集队列集获取最新峰值*/
int16_t get_F_peak(int16_t data)
{
    static int16_t back_value,value,front_value,peak_value;
    back_value = value;
    value = front_value;
    front_value = data;
    if(value>back_value&&value>front_value)//获取波峰数值
    {
        peak_value = value;
    }
    return peak_value;
}

/*转化数据传递*/
void data_send(void)
{
    Hz = 100;//采集和计算频率取决于你放在多少的调度周期里面
    Flow = sensor.berath_value;//流量数值赋值
    ItoE_Flow_temp = get_F_peak(Flow)/4;//流量测试值
    // modbus_dis[set_pre] = Set;//压力设定
    XiP = modbus_dis[set_xi_pre];//吸气压力赋值输出
    HuP = modbus_dis[set_hu_pre];//呼气压力赋值输出
    
    modbus_dis[xiqivalue_1] = XiSum[0];
    modbus_dis[xiqivalue_2] = XiSum[1];
    modbus_dis[xiqivalue_3] = XiSum[2];
    modbus_dis[xiqivalue_4] = XiSum[3];
    modbus_dis[xiqivalue_5] = XiSum[4];

    modbus_dis[huqivalue_1] = HuSum[0];
    modbus_dis[huqivalue_2] = HuSum[1];
    modbus_dis[huqivalue_3] = HuSum[2];
    modbus_dis[huqivalue_4] = HuSum[3];
    modbus_dis[huqivalue_5] = HuSum[4];
}

/*10ms调度周期--100hz*/
void soft_process(void)
{
    static uint8_t n = 0;
    static float deltaFlow3[3];
    FlowD=Flow-FlowLast;
    FlowD2=FlowD-FlowDLast;

    deltaFlow3[n%3] = FlowD;
    n++;

    int Trigger_flag;
    int Hang_off_flag;

    int i;
    //判断连续3个流量导数，如果3个导数都为正，则Trigger_flag=1
    Trigger_flag = 1;
    for(i=0; i<3; i++)
    {
    if(deltaFlow3[i]<0){Trigger_flag = 0; break;}
    }

    //判断连续3个流量导数，如果3个导数都为负，则Hang_off_flag=1
    Hang_off_flag = 1;
    for(i=0;i<3; i++)
    {
    if(deltaFlow3[i]>0){Hang_off_flag = 0; break;}
    }

    //状态切换（将原来的连续if语句改成switch判断语句）
    switch (State)
    {
    case 0: 
            if(Flow>I_Trigger_Flow  && Trigger_flag==1) State=1;   //1-吸气努力状态
                break;
    case 1: 
                if(Hang_off_flag==1) State=2;       //2-保压状态
                    break;
    case 2: 
                if(StateTime>0.1*StateTimeLast) State=3;     //3-吸气末期
                    break;
    case 3: 
                if(Flow<ItoE_Flow  && Hang_off_flag==1) State=0;      //0 - 呼相
                    break;
    default: break;
    }

    //计算吸呼气时间和吸呼流量
    if(State!=StateLast){     //呼吸状态发生改变
        StateTimeLast=StateTime;
        StateTime=0;
        if(State==1){
            XiTik++;
            XiTryTime[(XiTik-1)%5]=0;
            XiTime[(XiTik-1)%5]=0;
            XiSum[(XiTik-1)%5]=0;}

        if(State==0){
            HuTik++;
            HuTime[(HuTik-1)%5]=0;
            HuSum[(HuTik-1)%5]=0;
        }
    }else{                //呼吸状态未发生改变
        StateTime++;
            if(State==1){XiTryTime[(XiTik-1)%5]++;}
            if(State>0){
                XiTime[(XiTik-1)%5]++;
                XiSum[(XiTik-1)%5]+=Flow/60/Hz*1000;}
            if(State== 3){  //在本次呼吸的吸气末期改变呼气切换流量，阈值由本次呼吸的峰值决定
            ItoE_Flow = ItoE_Flow_temp;      //改变吸呼相切换流量
            }
            if(State==0){      //在呼相时改变吸气触发流量
                HuTime[(HuTik-1)%5]++;
                HuSum[(HuTik-1)%5]+=( - Flow/60/Hz*1000);}
        }

    StateLast=State;

    //计算吸气努力时间
    int avgtime,open;
    if(XiTik<5) avgtime=8;
    else
    {for(i=0;i<5;i++) avgtime+=XiTryTime[i];
    avgtime=avgtime/5;}

    //设置吸呼压力
    if(State==1){
        Set=-(XiP-HuP)/pow(avgtime,2)*StateTime+(XiP-HuP)*2/avgtime*StateTime+HuP;
        if(Set>XiP) Set=XiP;}
    if(State==2)
    {Set=XiP;}
    if(State==3||State==0)
    {Set=HuP;}

    FlowLast=Flow; FlowDLast=FlowD;  
}

/*10ms调用一次*/
void breath_value_Cal(void)
{
    double ChaoQi,HuXiHz,XiHuBi;
    ChaoQi=XiSum[(XiTik-2)%5];
    HuXiHz=60/(XiTime[(XiTik-2)%5]/Hz+HuTime[(XiTik-2)%5]/Hz);
    XiHuBi=XiTime[(XiTik-2)%5]/HuTime[(XiTik-2)%5];

    sensor.breath_chaoqi = (uint16_t)ChaoQi;
    sensor.breath_frq = (uint16_t)HuXiHz;
    sensor.breath_rat = (uint16_t)XiHuBi;
}


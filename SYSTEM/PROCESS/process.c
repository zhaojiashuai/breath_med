#include "process.h"

float Flow,FlowLast,FlowD,FlowDLast,FlowD2,XiSum[10],HuSum[10],XiP,HuP;

int16_t State,StateLast,XiTryTime[10],XiTime[10],HuTime[10],XiTik,HuTik,StateTime,StateTimeLast,Hz,Set;

/*转化数据传递*/
void data_send(void)
{
    Hz = 100;//采集和计算频率
    Flow = sensor.berath_value;//流量数值赋值
    modbus_dis[set_pre] = Set;//压力设定
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
    FlowD=Flow-FlowLast; //流量导数
    FlowD2=FlowD-FlowDLast;//流量 二次导数


    if((State==0||State==-1)&&Flow>10) State=1; //进入吸气状态
    if(State==1&&FlowD<0) State=2;  //吸气努力结束，保压状态
    if(State==2&&StateTime>0.1*StateTimeLast) State=3; //吸气末期
    if(Flow<0) State=-1; //呼气相

    if(State!=StateLast){  //若状态切换
        StateTimeLast=StateTime; //记录上一个状态持续时间
        StateTime=0; //本状态清零
        if(State==1){
            XiTik++;  //吸气次数增加
            XiTryTime[(XiTik-1)%5]=0; //此次吸气努力时间清零
            XiTime[(XiTik-1)%5]=0;XiSum[(XiTik-1)%5]=0;}  //此次吸气时间、吸气量清零    if(State<0){ //呼气状态，同上
            if(State==0){
            HuTik++;
            HuTime[(HuTik-1)%5]=0;
            HuSum[(HuTik-1)%5]=0;}
    }else{ //状态保持
        StateTime++; //状态时间累加
            if(State==1){XiTryTime[(XiTik-1)%5]++;}  //吸气努力时间累加
            if(State>0){
                XiTime[(XiTik-1)%5]++; //吸气时间累加
                XiSum[(XiTik-1)%5]+=Flow/60/Hz*1000;} //吸气量计算
            if(State<0){ //呼气同上
                HuTime[(HuTik-1)%5]++;
                HuSum[(HuTik-1)%5]+=( - Flow/60/Hz*1000);}
        }
    StateLast=State; //状态更新


    int avgtime,i;
    if(XiTik<5) avgtime=8; //默认吸气努力时间
    else
    {for(i=0;i<5;i++) avgtime+=XiTryTime[i]; 
    avgtime=avgtime/5;}  //计算平均吸气努力时间（前五次吸气努力时间平均）

    if(State==0) Set=0; //如果0状态，设定压力为0
    if(State==1){
        Set=-(XiP-HuP)/pow(avgtime,2)*StateTime+(XiP-HuP)*2/avgtime*StateTime+HuP;
        if(Set>XiP) Set=XiP;}  //处于吸气努力状态时，设定压力呈一次函数增加
    if(State==2) 
    {Set=XiP;} //努力后附加时间，设定压力为吸气压力
    if(State==3||State==-1)
    {Set=HuP;}  //呼气压力设定

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


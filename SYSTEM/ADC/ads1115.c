#include "ADS1115.h"
#include "delay.h"
#include "stm32f10x.h"

u8 WriteIntBuf[10],WritepointBuf[10];

void delay_iic(void)  //??4?????,??4.34us  
{  
    delay_us(4);
}  

void setADS1115_sda(u8 i)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//GPIOB
	
	if(i)
	{
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	
	}else
	{
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	}
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	//GPIO_SetBits(GPIOB,GPIO_Pin_7);
}

void ADS1115_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//GPIOB
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	GPIO_SetBits(GPIOF,GPIO_Pin_1|GPIO_Pin_2);
}

void Init__ADS1115(void)
{
		ADS1115_init();
    scl_H;  
    delay_iic();  
    sda_H;  
    delay_iic();  
}

void Start_1115(void)
{
    sda_H;  
    delay_iic();   
    scl_H ;  
    delay_iic();      //sda?scl???????4.7us??    
    sda_L;          //???  
    delay_iic();
    scl_L;  
    delay_iic(); 
}

void Stop_1115(void)
{
    sda_L;
    delay_iic();  //????  
    scl_H ;  
    delay_iic();      //???????????4us??  
    sda_H;          //scl?????,sda??????  
    delay_iic();      //sda??4.7us??,4.34??????????4.7us  
                        //?:??scl?sda??1 
}

void WriteByte_1115(u8 WriteData)
{
    u8 i;  
    u8 b;
    u8 temp=WriteData;  
    for(i=0;i<8;i++)  
    {  
        b=temp>>(7-i);//temp=temp<<1;   //????????PSW????CY?? 
        b=b&0x01;  
        scl_L;           //??  
        delay_iic();            //????   
        if(b)
        {
          sda_H; 
        }else
        {
          sda_L; 
        }
        delay_iic();  
        scl_H;            //????????,ic?????????  
        delay_iic();          
    }  
  
    scl_L;              //????scl=1;sda=1??????,?????  
    delay_iic();                  
    sda_H;              //????,??????????  
    delay_iic();  
}

u8 ReadByte_1115(void)
{
    u8 i,temp;  
    scl_L;               //?????  
    delay_iic();  
    sda_H;             //????  
    delay_iic(); 
    sda_in;
    for(i=0;i<8;i++)  
    {  
        delay_iic();       
        scl_H;         //mcu?????  
        delay_iic();      //scl?????,ic?????1?????sda?  
                            //????????4.34us?,??????mcu?sda?  
        //temp=(temp<<1)|sda; //??????temp?  
        temp=(temp<<1);
        if(sda_st)
        {
          temp=temp+1;
        }
        scl_L;    
    }
    sda_out;
    return temp; 
}

void ReceiveAck_1115(void)
{
    u8 i=0;
    sda_in;
    scl_H;  
    delay_iic();
	  scl_L;           //????,??1??????   
    delay_iic(); 
   while((sda_st==1)&&(i<255))
   	  i++;
    scl_L;           //????,??1??????   
    delay_iic(); 
    sda_out;
}

void Acknowledge_1115(void)
{
sda_L;
delay_iic();
scl_L;
delay_iic();
scl_H;
delay_iic();
scl_L;
delay_iic();
sda_H; 
delay_iic();
}

 void Confige1115 (u8 port)
{
	static u8 chnel,i;
	switch (port)
		{
    case 0: chnel=0xC2; 
    break;
    case 1:chnel=0xD2;
    break;
		case 2:chnel=0xE2;
    break;
    case 3:chnel=0xF2;
    break;
		default:
    break;
		}
		
		WriteIntBuf[0] = 0x90;
    WriteIntBuf[1] = 0x01;
		WriteIntBuf[2] = chnel;
		WriteIntBuf[3] = 0xe3;//0X83,0XE3,0X82
		
		Start_1115();
		
		for(i=0;i<4;i++)
		{
		WriteByte_1115(WriteIntBuf[i]);
    ReceiveAck_1115();
		}
		Stop_1115();
}

/*******************************************************************************
* Function Name  : PointRegister
* Attention   :  ??ADS1115?????????????
*******************************************************************************/
static void PointRegister (void)
{
	u8 i;
	WritepointBuf[0] = 0x90; //90
	WritepointBuf[1] = 0x00; //00
	
  Start_1115();
  for(i=0;i<2;i++)
  {
    WriteByte_1115(WritepointBuf[i]);
    ReceiveAck_1115();
  }
		Stop_1115();
}

u8 ReadBuffer[10];

/*******************************************************************************
* ??ADS1115?16???
*******************************************************************************/
static u16 ReadData (u8 chnnal1)
{ 

		u16 data;

		Start_1115();

		WriteByte_1115(0x91);
		ReceiveAck_1115();

		ReadBuffer[0] = ReadByte_1115();
		ReceiveAck_1115();

		ReadBuffer[1] = ReadByte_1115(); ////??????
		Acknowledge_1115();

		Stop_1115();
		data = (ReadBuffer[0]<<8)|ReadBuffer[1];
		if(data>0x7fff)// fu ya 
		data = ~data+1;	
		return data;
}

/*******************************************************************************
* Function Name  : Get_ATOD
* Attention :??ADS1115??????
*******************************************************************************/
u16 Get_ATOD (u8 channel)
{
	static u8 chn;
	static u32 data_get;
   chn = channel;

Confige1115(channel); ////??ADS1115????
delay_ms(1);
PointRegister();
delay_ms(1);
data_get = ReadData(chn);
delay_ms(1);	
return data_get;
}
#include "ADS1115.h"

u8 WriteIntBuf[10],WritepointBuf[10];
void delay_iic(void)  
{  
    delay_us(4);
}  

/**
 * @brief 设置ADS1115的I2C SDA引脚状态
 * 
 * @param i 如果为0，将SDA引脚设置为输出模式；如果为非0，将SDA引脚设置为输入模式。
 */
void setADS1115_sda(u8 i)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义一个GPIO初始化结构体
    // 使能GPIOB的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
    // 设置GPIO_InitStructure的GPIO_Pin为GPIOB的第7脚，即SDA引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    // 根据参数i的值设置SDA引脚的模式
    if(i)
    {
        // 如果i非0，设置为输入模式
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
    }
    else
    {
        // 如果i为0，设置为输出模式
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    }
    // 设置为推挽输出类型
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    // 设置GPIOB的输出速度为100MHz
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    // 设置上拉
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    // 根据GPIO_InitStructure的配置初始化GPIOB
    GPIO_Init(GPIOB, &GPIO_InitStructure);   
}

/**
 * @brief 初始化ADS1115的I2C引脚
 * 
 * 这个函数用于配置STM32的GPIO引脚，以作为I2C通信的SCL和SDA线。
 */
void ADS1115_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义一个GPIO初始化结构体
    // 使能GPIOB的时钟，因为SCL和SDA引脚位于GPIOB组
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
    // 设置GPIO_InitStructure的GPIO_Pin为GPIOB的第6脚和第7脚，即SCL和SDA引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
    // 设置GPIO_InitStructure的GPIO_Mode为输出模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    // 设置GPIO_InitStructure的GPIO_OType为推挽输出类型
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    // 设置GPIOB的输出速度为100MHz
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    // 设置GPIO_InitStructure的GPIO_PuPd为上拉
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    // 根据GPIO_InitStructure的配置初始化GPIOB
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    // 设置GPIOB的第6脚和第7脚为高电平，确保I2C总线初始状态为空闲
    GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
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
    delay_iic();        
    sda_L;          
    delay_iic();
    scl_L;  
    delay_iic(); 
}

void Stop_1115(void)
{
    sda_L;
    delay_iic();  
    scl_H ;  
    delay_iic();       
    sda_H;          
    delay_iic();      
}

void WriteByte_1115(u8 WriteData)
{
    u8 i;  
    u8 b;
    u8 temp=WriteData;  
    for(i=0;i<8;i++)  
    {  
        b=temp>>(7-i);
        b=b&0x01;  
        scl_L;           
        delay_iic();             
        if(b)
        {
          sda_H; 
        }else
        {
          sda_L; 
        }
        delay_iic();  
        scl_H;             
        delay_iic();          
    }  
  
    scl_L;              
    delay_iic();                  
    sda_H;               
    delay_iic();  
}

u8 ReadByte_1115(void)
{
    u8 i,temp;  
    scl_L;                
    delay_iic();  
    sda_H;             
    delay_iic(); 
    sda_in;
    for(i=0;i<8;i++)  
    {  
        delay_iic();       
        scl_H;         
        delay_iic();      
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
	  scl_L;           
    delay_iic(); 
   while((sda_st==1)&&(i<255))
   	  i++;
    scl_L;            
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

 void Confige1115 (u8 port)/*修改量程为最大*/
{
	static u8 chnel,i;
	switch (port)
		{
    case 0: chnel=0xC0; 
    break;
    case 1:chnel=0xD0;
    break;
		case 2:chnel=0xE0;
    break;
    case 3:chnel=0xF0;
    break;
		default:
    break;
		}
		
		WriteIntBuf[0] = 0x90;
    WriteIntBuf[1] = 0x01;
		WriteIntBuf[2] = chnel;
		WriteIntBuf[3] = 0xe3;
		
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
		ReadBuffer[1] = ReadByte_1115(); 
		Acknowledge_1115();
		Stop_1115();
		data = (ReadBuffer[0]<<8)|ReadBuffer[1];
		if(data>0x7fff)
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


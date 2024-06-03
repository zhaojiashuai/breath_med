#ifndef __ADS1115_H
#define __ADS1115_H
#include "stm32f10x.h"


#define CMD_Write           0x90     //write cmd 10010000 is addr "1":read  "0":write
#define CMD_Read            0x91    //write cmd 10010000 is addr "1":read  "0":write
#define CMD_POINT_REG       0x00      // zhi xiang ji cun qi  pei zhi
#define CMD_CONF_REG        0x01   // pei zhi ji cun qi pei zhi 
#define CONF_L              0xe3    // di 8 wei 
#define ADS1115_ADDR        0x90         //addr  0x90~GND;0X92~VCC;0X94~SDA;0X96~SCL;



#define				scl_H					GPIO_SetBits(GPIOF,GPIO_Pin_1)
#define				scl_L					GPIO_ResetBits(GPIOF,GPIO_Pin_1)

#define				sda_H					GPIO_SetBits(GPIOF,GPIO_Pin_2)
#define				sda_L					GPIO_ResetBits(GPIOF,GPIO_Pin_2)

#define				sda_st				    GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_2)

#define				sda_out				    setADS1115_sda(0)
#define				sda_in				    setADS1115_sda(1)

void Init__ADS1115(void);
u16 Get_ATOD (u8 channel);


u16 AD_00(void);
u16 AD_01(void);
u16 AD_02(void);
u16 AD_03(void);
#endif

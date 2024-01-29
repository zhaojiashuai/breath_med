#include "flash.h"

//////////////////////////////////////////////////
// ��ȡָ����ַ�İ���(16λ����)
// faddr:����ַ
// ����ֵ:��Ӧ����.
static u32 STMFLASH_ReadWord(u32 faddr)
{
    return *(vu32 *)faddr;
}
// ��ȡĳ����ַ���ڵ�flash����
// addr:flash��ַ
// ����ֵ:0~11,��addr���ڵ�����
static uint16_t STMFLASH_GetFlashSector(u32 addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)
        return FLASH_Sector_0;
    else if (addr < ADDR_FLASH_SECTOR_2)
        return FLASH_Sector_1;
    else if (addr < ADDR_FLASH_SECTOR_3)
        return FLASH_Sector_2;
    else if (addr < ADDR_FLASH_SECTOR_4)
        return FLASH_Sector_3;
    else if (addr < ADDR_FLASH_SECTOR_5)
        return FLASH_Sector_4;
    else if (addr < ADDR_FLASH_SECTOR_6)
        return FLASH_Sector_5;
    else if (addr < ADDR_FLASH_SECTOR_7)
        return FLASH_Sector_6;
    else if (addr < ADDR_FLASH_SECTOR_8)
        return FLASH_Sector_7;
    else if (addr < ADDR_FLASH_SECTOR_9)
        return FLASH_Sector_8;
    else if (addr < ADDR_FLASH_SECTOR_10)
        return FLASH_Sector_9;
    else if (addr < ADDR_FLASH_SECTOR_11)
        return FLASH_Sector_10;
    return FLASH_Sector_11;
}
// ��ָ����ַ��ʼд��ָ�����ȵ�����
// �ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
//          д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//          д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//          û����Ҫ����,��������������Ȳ�����,Ȼ����������д.
// �ú�����OTP����Ҳ��Ч!��������дOTP��!
// OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F
// WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
// pBuffer:����ָ��
// NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.)
static void STMFLASH_Write(u32 WriteAddr, u32 *pBuffer, u32 NumToWrite)
{
    FLASH_Status status = FLASH_COMPLETE;
    u32 addrx = 0;
    u32 endaddr = 0;
    if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 4)
        return;                  // �Ƿ���ַ
    FLASH_Unlock();              // ����
    FLASH_DataCacheCmd(DISABLE); // FLASH�����ڼ�,�����ֹ���ݻ���

    addrx = WriteAddr;                    // д�����ʼ��ַ
    endaddr = WriteAddr + NumToWrite * 4; // д��Ľ�����ַ
    if (addrx < 0X1FFF0000)               // ֻ�����洢��,����Ҫִ�в�������!!
    {
        while (addrx < endaddr) // ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
        {
            if (STMFLASH_ReadWord(addrx) != 0XFFFFFFFF) // �з�0XFFFFFFFF�ĵط�,Ҫ�����������
            {
                status = FLASH_EraseSector(STMFLASH_GetFlashSector(addrx), VoltageRange_3); // VCC=2.7~3.6V֮��!!
                if (status != FLASH_COMPLETE)
                    break; // ����������
            }
            else
                addrx += 4;
        }
    }
    if (status == FLASH_COMPLETE)
    {
        while (WriteAddr < endaddr) // д����
        {
            if (FLASH_ProgramWord(WriteAddr, *pBuffer) != FLASH_COMPLETE) // д������
            {
                break; // д���쳣
            }
            WriteAddr += 4;
            pBuffer++;
        }
    }
    FLASH_DataCacheCmd(ENABLE); // FLASH��������,�������ݻ���
    FLASH_Lock();               // ����
}

// ��ָ����ַ��ʼ����ָ�����ȵ�����
// ReadAddr:��ʼ��ַ
// pBuffer:����ָ��
// NumToRead:��(4λ)��
static void STMFLASH_Read(u32 ReadAddr, u32 *pBuffer, u32 NumToRead)
{
    u32 i;
    for (i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr); // ��ȡ4���ֽ�.
        ReadAddr += 4;                            // ƫ��4���ֽ�.
    }
}
// STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer,SIZE);
// STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)datatemp,SIZE);

void fmc_write(uint16_t num, uint16_t *data)
{
    uint32_t addr = 0, read_data = 0, temp[255] = {0};
    if (num < pre_x1) // �趨��ַ������Ҫ����flash�����򲻻�洢
        return;
    addr = (num - pre_x1) * 4 + FLASH_SAVE_ADDR;
    STMFLASH_Read(addr, temp, 1);
    read_data = temp[0];
    if (data[num] != (uint16_t)read_data)
    {
        memcpy(temp,data+pre_x1,(all_num-pre_x1));
        STMFLASH_Write(addr, temp, (all_num-pre_x1));
    }
}

uint16_t fmc_read(uint16_t num)
{
    uint32_t addr = 0, read_data = 0, temp[1] = {0};

    addr = (num - pre_x1) * 4 + FLASH_SAVE_ADDR;
    STMFLASH_Read(addr, temp, 1);
    read_data = temp[0];
    return (uint16_t)read_data;
}

#include "flash.h"


/**
 * @brief 读取STM32 FLASH中的一个字的静态函数
 * 
 * 这个函数读取STM32的闪存中的一个32位字。它通过将闪存地址强制转换为指向32位无符号整型的指针，
 * 然后解引用该指针来读取数据。由于闪存是只读的，这个函数不需要进行任何写入操作。
 * 
 * @param faddr 闪存的地址
 * @return u32 读取到的32位字
 */
static u32 STMFLASH_ReadWord(u32 faddr)
{
    return *(vu32 *)faddr;
}


/**
 * @brief 获取STM32 FLASH中的一个扇区的地址
 * 
 * 这个函数用于获取STM32 FLASH中的一个扇区的地址。它通过输入的闪存地址来确定扇区的编号。
 * 如果输入地址小于ADDR_FLASH_SECTOR_1，则返回FLASH_Sector_0；
 * 如果输入地址小于ADDR_FLASH_SECTOR_2，则返回FLASH_Sector_1；
 * 如果输入地址小于ADDR_FLASH_SECTOR_3，则返回FLASH_Sector_2；
 * 以此类推，直到输入地址小于ADDR_FLASH_SECTOR_11，则返回FLASH_Sector_10。
 * 
 * @param addr 输入的闪存地址
 * @return uint16_t 返回对应的扇区编号
 */
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
    else
        return FLASH_Sector_11;
}


/**
 * @brief 向STM32 FLASH中写入数据的静态函数
 * 
 * 这个函数用于向STM32 FLASH中写入数据。它首先检查输入的写入地址是否有效，然后解锁FLASH并禁用数据缓存，
 * 以确保数据写入FLASH时不会被缓存中的数据影响。接着，函数会检查要写入的区域是否需要先擦除，
 * 如果需要，则调用擦除扇区函数。最后，函数将数据缓冲区中的数据逐个写入FLASH中。
 * 
 * @param WriteAddr 写入数据的起始地址
 * @param pBuffer 指向数据缓冲区的指针
 * @param NumToWrite 要写入的数据字节数
 * @return void
 */
static void STMFLASH_Write(u32 WriteAddr, u32 *pBuffer, u32 NumToWrite)
{
    FLASH_Status status = FLASH_COMPLETE;
    u32 addrx = 0;
    u32 endaddr = 0;

    // 检查写入地址是否在FLASH的范围内，并且地址对齐到4字节边界
    if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 4)
        return;                  // 检查地址是否有效
    FLASH_Unlock();              // 解锁FLASH
    FLASH_DataCacheCmd(DISABLE); // 禁用数据缓存，确保数据写入FLASH时不会被缓存中的数据影响

    // 初始化地址和结束地址变量
    addrx = WriteAddr;
    endaddr = WriteAddr + NumToWrite * 4;

    // 判断是否需要擦除FLASH扇区
    if (addrx < 0X1FFF0000) // 只处理小于0X1FFF0000的地址，即内部FLASH
    {
        // 循环检查要写入的每个扇区，如果当前扇区已经被写入，则擦除该扇区
        while (addrx < endaddr)
        {
            if (STMFLASH_ReadWord(addrx) != 0XFFFFFFFF) // 如果当前扇区没有被写入，则需要擦除
            {
                FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR); // 清除FLASH标志
                status = FLASH_EraseSector(STMFLASH_GetFlashSector(addrx), VoltageRange_3); // 擦除扇区
                if (status != FLASH_COMPLETE)
                    break; // 如果擦除失败，则跳出循环
            }
            else
                addrx += 4; // 如果当前扇区已经被写入，则跳过
        }
    }

    // 如果擦除成功，则继续写入数据
    if (status == FLASH_COMPLETE)
    {
        // 循环写入数据
        while (WriteAddr < endaddr)
        {
            if (FLASH_ProgramWord(WriteAddr, *pBuffer) != FLASH_COMPLETE) // 写入数据
            {
                break; // 如果写入失败，则跳出循环
            }
            WriteAddr += 4;
            pBuffer++;
        }
    }

    // 写入完成后，重新启用数据缓存并锁定FLASH
    FLASH_DataCacheCmd(ENABLE); // 重新启用数据缓存
    FLASH_Lock();               // 锁定FLASH
}


/**
 * @brief 从STM32 FLASH中读取数据的静态函数
 * 
 * 这个函数用于从STM32 FLASH中读取数据。它接受一个起始地址和一个数据缓冲区指针，以及要读取的数据字节数。
 * 函数通过循环调用`STMFLASH_ReadWord`函数，从FLASH中读取指定数量的字节，并存储在数据缓冲区中。
 * 
 * @param ReadAddr 读取数据的起始地址
 * @param pBuffer 指向数据缓冲区的指针
 * @param NumToRead 要读取的数据字节数（以4字节为单位）
 * @return void
 */
static void STMFLASH_Read(u32 ReadAddr, u32 *pBuffer, u32 NumToRead)
{
    u32 i;
    for (i = 0; i < NumToRead; i++)
    {
        // 从FLASH中读取4字节数据，并存储在数据缓冲区中
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr); 
        // 更新读取地址，跳过4字节
        ReadAddr += 4;
    }
}



/**
 * @brief 将数据写入FMC（Flash Memory Controller）控制的FLASH存储器
 * 
 * 这个函数用于将数据写入STM32的FLASH存储器。它首先检查输入的数字`num`是否小于`pre_x1`，
 * 如果小于，则返回，不执行写入操作。然后，它将输入的`data`数组中从`pre_x1`开始的元素复制到一个临时数组`write_temp`中。
 * 最后，它调用`STMFLASH_Write`函数，将临时数组中的数据写入FLASH存储器，从地址`FLASH_SAVE_ADDR`开始。
 * 
 * @param num 数据中的索引
 * @param data 指向要写入的数据的指针
 * @return void
 */
void fmc_write(uint16_t num, uint16_t *data)
{
    uint16_t write_temp[50] = {0}; // 声明一个临时数组，用于存储要写入的数据
    if (num < pre_x1) // 如果索引小于预设的索引，则不需要写入FLASH
        return;
    for (uint8_t i = 0; i < (all_num - pre_x1); i++) // 循环复制数据
    {
        write_temp[i] = data[pre_x1 + i]; // 复制数据到临时数组
    }
    STMFLASH_Write(FLASH_SAVE_ADDR, (uint32_t *)write_temp, (all_num - pre_x1)); // 写入FLASH
}

/**
 * @brief 从FMC控制的FLASH存储器中读取数据
 * 
 * 这个函数用于从STM32的FLASH存储器中读取数据。它首先检查输入的数字`num`是否小于`pre_x1`，
 * 如果小于，则返回，不执行读取操作。然后，它调用`STMFLASH_Read`函数，从FLASH存储器中读取数据，
 * 从地址`FLASH_SAVE_ADDR`开始，读取的数量为`all_num - pre_x1`个字节。最后，它将读取到的数据复制到输入的`data`数组中，从`pre_x1`开始。
 * 
 * @param num 数据中的索引
 * @param data 指向要写入的数据的指针
 * @return void
 */
void fmc_read(uint16_t num, uint16_t *data)
{
    uint16_t read_temp[50] = {0}; // 声明一个临时数组，用于存储读取的数据
    if (num < pre_x1) // 如果索引小于预设的索引，则不需要读取FLASH
        return;
    STMFLASH_Read(FLASH_SAVE_ADDR, (uint32_t *)read_temp, (all_num - pre_x1)); // 读取FLASH
    for (uint8_t i = 0; i < (all_num - pre_x1); i++) // 循环复制数据
    {
        data[pre_x1 + i] = read_temp[i]; // 复制数据到目标数组
    }
}


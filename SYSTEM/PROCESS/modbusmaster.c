#include "modbusmaster.h"

mdb modbus_dev;

void calculate_CRC(uint8_t *data, uint8_t length, uint8_t *crc1, uint8_t *crc2)
{
    u16 MSBInfo;
    u16 wCrcData = 0xFFFF;
    uint8_t i, j;

    for (i = 0; i < length; i++)
    {
        wCrcData = wCrcData ^ data[i];
        for (j = 0; j < 8; j++)
        {
            MSBInfo = wCrcData & 0x0001;
            wCrcData = wCrcData >> 1;
            if (MSBInfo != 0)
            {
                wCrcData = wCrcData ^ 0xA001;
            }
        }
    }
    *crc1 = wCrcData % 256;
    *crc2 = wCrcData / 256;
}

uint16_t get_crc16(uint8_t *data, uint8_t length)
{
    u16 MSBInfo, i, j;
    u16 wCrcData = 0xFFFF;

    for (i = 0; i < length; i++)
    {
        wCrcData = wCrcData ^ data[i];
        for (j = 0; j < 8; j++)
        {
            MSBInfo = wCrcData & 0x0001;
            wCrcData = wCrcData >> 1;
            if (MSBInfo != 0)
            {
                wCrcData = wCrcData ^ 0xA001;
            }
        }
    }
    return wCrcData;
}

void modbus_host_deal(uint8_t data)
{
    uint8_t i;
    uint16_t crc16;
    static uint8_t len;
    static uint8_t cnt, rxbuf[256];
    rxbuf[cnt++] = data;
    if (cnt <= 1)
        return;
    if (rxbuf[1] == 0x03)
    {
        if (cnt > 2)
        {
            len = (rxbuf[2] + 4);
            if (cnt > len)
            {
                crc16 = rxbuf[cnt - 2] | (rxbuf[cnt - 1] << 8);
                if (crc16 == get_crc16(rxbuf, cnt - 2))
                {
                    modbus_dev.rx_count = cnt;
                    for (i = 0; i < cnt; i++)
                    {
                        modbus_dev.rx_buf[i] = rxbuf[i];
                    }
                    modbus_dev.ok_flag = 1;
                }
                cnt = 0;
            }
        }
    }
    else if (rxbuf[1] == 0x06 || rxbuf[1] == 0x10)
    {
        len = 7;
        if (cnt > len)
        {
            crc16 = rxbuf[cnt - 2] | (rxbuf[cnt - 1] << 8);
            if (crc16 == get_crc16(rxbuf, cnt - 2))
            {
                modbus_dev.rx_count = cnt;
                for (i = 0; i < cnt; i++)
                {
                    modbus_dev.rx_buf[i] = rxbuf[i];
                }
                modbus_dev.ok_flag = 1;
            }
            cnt = 0;
        }
    }
    else
    {
        cnt = 0;
    }
    if (cnt > 250)
    {
        cnt = 0;
    }

    //////////////////////////////////////////////////////////////
}

void modbus_03_hook(uint16_t *buf)
{
    static uint8_t len, i;
    len = modbus_dev.rx_buf[2] / 2;
    for (i = 0; i < len; i++)
    {
        buf[(i)] = (modbus_dev.rx_buf[3 + i * 2] << 8) + modbus_dev.rx_buf[4 + i * 2];
    }
}

void modbus_06_hook(uint16_t *buf)
{
}

void modbus_10_hook(uint16_t *buf)
{
}

void modbus_parse(uint16_t *buf)
{
    if (modbus_dev.ok_flag == 1)
    {
        switch (modbus_dev.rx_buf[1])
        {
        case 0x03:
            modbus_03_hook(buf);
            break;
        case 0x06:
            modbus_06_hook(buf);
            break;
        case 0x10:
            modbus_10_hook(buf);
            break;
        }
    }
    modbus_dev.ok_flag = 0;
}

void modbus_read(uint8_t mainaddr, uint8_t cmd, uint16_t add, uint16_t data, uint8_t *M_tx)
{

    uint8_t add_low = 0, add_hig = 0, data_low = 0, data_hig = 0, i, j;

    if (cmd == 0x03)
    {
        add_hig = (uint8_t)((add & 0xff00) >> 8);
        add_low = (uint8_t)(add & 0xff);

        data_hig = (uint8_t)((data & 0xff00) >> 8);
        data_low = (uint8_t)(data & 0xff);

        M_tx[0] = mainaddr;
        M_tx[1] = cmd;
        M_tx[2] = add_hig;
        M_tx[3] = add_low;
        M_tx[4] = data_hig;
        M_tx[5] = data_low;
        calculate_CRC(&M_tx[0], 6, &i, &j);
        M_tx[6] = i;
        M_tx[7] = j;
    }
    else if (cmd == 0x06)
    {
        add_hig = (uint8_t)((add & 0xff00) >> 8);
        add_low = (uint8_t)(add & 0xff);

        data_hig = (uint8_t)((data & 0xff00) >> 8);
        data_low = (uint8_t)(data & 0xff);

        M_tx[0] = mainaddr;
        M_tx[1] = cmd;
        M_tx[2] = add_hig;
        M_tx[3] = add_low;
        M_tx[4] = data_hig;
        M_tx[5] = data_low;
        calculate_CRC(&M_tx[0], 6, &i, &j);
        M_tx[6] = i;
        M_tx[7] = j;
    }
}

void modbus_slave_deal(uint8_t data)
{
    uint8_t i;
    uint16_t crc16;
    static uint8_t len;
    static uint8_t cnt, rxbuf[256];
    rxbuf[cnt++] = data;
        if (cnt <= 1)
        return;
    if (rxbuf[1] == 0x10) // 01 10 00 00 00 01 02 00 01 69 50 主站 功能码 寄存器地址 寄存器数量 字节数 后面的写入数据 校验
    {
        if (cnt > 2)
        {
            len = (rxbuf[6] + 8);
            if (cnt > len)
            {
                crc16 = rxbuf[cnt - 2] | (rxbuf[cnt - 1] << 8);
                if (crc16 == get_crc16(rxbuf, cnt - 2))
                {
                    modbus_dev.rx_count = cnt;
                    for (i = 0; i < cnt; i++)
                    {
                        modbus_dev.rx_buf[i] = rxbuf[i];
                    }
                    modbus_dev.ok_flag = 1;
                }
                cnt = 0;
            }
        }
    }
    else if (rxbuf[1] == 0x03 || rxbuf[1] == 0x06) // 01 03 00 00 00 01 84 0A 主站 功能码 寄存器地址 寄存器数量/写入的数值 校验
    {
        len = 7;
        if (cnt > len)
        {
            crc16 = rxbuf[cnt - 2] | (rxbuf[cnt - 1] << 8);
            if (crc16 == get_crc16(rxbuf, cnt - 2))
            {
                modbus_dev.rx_count = cnt;
                for (i = 0; i < cnt; i++)
                {
                    modbus_dev.rx_buf[i] = rxbuf[i];
                }
                modbus_dev.ok_flag = 1;
            }
            cnt = 0;
        }
    }
    else
    {
        cnt = 0;
    }
    if (cnt > 250)
    {
        cnt = 0;
    }
}
void modbus_03_slave(uint16_t *buf) // 01 03 02 00 01 84 0A 主站 功能码 寄存器数量 寄存器里面的内容 校验
{
    uint8_t cmd_03[256] = {0}, len = 0, i, j;
    uint16_t num = 0;
    cmd_03[0] = modbus_dev.rx_buf[0];
    cmd_03[1] = modbus_dev.rx_buf[1];
    num = modbus_dev.rx_buf[2] << 8 | modbus_dev.rx_buf[3];
    cmd_03[2] = (modbus_dev.rx_buf[4] << 8 | modbus_dev.rx_buf[5]) * 2;
    len = cmd_03[2] / 2;
    for (i = 0; i < len; i++)
    {
        cmd_03[3 + i * 2] = buf[(i) + num] >> 8;
        cmd_03[4 + i * 2] = buf[(i) + num];
    }
    len = cmd_03[2] + 3;
    calculate_CRC(&cmd_03[0], len, &i, &j);
    cmd_03[len] = i;
    cmd_03[len + 1] = j;
    len = len + 2;
    uart1_printf(cmd_03, len);
    uart4_printf(cmd_03, len);
    // send
}

void modbus_06_slave(uint16_t *buf) // 01 06 00 00 00 01 84 0A  原样返回
{
    uint16_t num = 0, len = 0;
    len = 8;
    num = modbus_dev.rx_buf[2] << 8 | modbus_dev.rx_buf[3];
    buf[num] = modbus_dev.rx_buf[4] << 8 | modbus_dev.rx_buf[5];
    fmc_write(num,buf);
    uart1_printf(modbus_dev.rx_buf, len);
    uart4_printf(modbus_dev.rx_buf, len);
    // send
}

void modbus_10_slave(uint16_t *buf) // 01 10 00 00 00 01 69 50 主站 功能码 寄存器的地址 寄存器的数量 校验
{
    uint8_t i, j;
    uint16_t len = 6;
    calculate_CRC(&modbus_dev.rx_buf[0], len, &i, &j);
    modbus_dev.rx_buf[len] = i;
    modbus_dev.rx_buf[len + 1] = j;
    len = len + 2;
    uart1_printf(modbus_dev.rx_buf, len);
    uart4_printf(modbus_dev.rx_buf, len);
    // send
}

void modbus_slave_parse(uint16_t *buf)
{
    if (modbus_dev.ok_flag == 1)
    {
        switch (modbus_dev.rx_buf[1])
        {
        case 0x03:
            modbus_03_slave(buf);
            break;
        case 0x06:
            modbus_06_slave(buf);
            break;
        case 0x10:
            modbus_10_slave(buf);
            break;
        }
    }
    modbus_dev.ok_flag = 0;
}

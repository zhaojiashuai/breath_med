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
    u16 crc16;

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



void modbus_read(uint8_t mainaddr, uint8_t cmd, uint16_t add, uint16_t data,uint8_t *M_tx)
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




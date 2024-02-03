#ifndef __MODBUSMASTER_H
#define __MODBUSMASTER_H
#include "common.h"

typedef struct
{
	uint8_t rx_count;
	uint8_t rx_buf[256];
    uint8_t tx_buf[256];
	uint8_t ok_flag;
	uint8_t write_flag;
}mdb;

extern mdb modbus_dev;

uint16_t get_crc16(uint8_t *data, uint8_t length);
void calculate_CRC(uint8_t *data, uint8_t length, uint8_t *crc1, uint8_t *crc2);
void modbus_slave_deal(uint8_t data, uint8_t addr);
void modbus_slave_parse(uint16_t *buf);

#endif

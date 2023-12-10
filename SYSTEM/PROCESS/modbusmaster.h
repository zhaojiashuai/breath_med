#ifndef __MODBUSMASTER_H
#define __MODBUSMASTER_H
#include "common.h"

typedef struct
{
	uint8_t rx_count;
	uint8_t rx_buf[256];
    uint8_t tx_buf[256];
	uint8_t ok_flag;
}mdb;

void modbus_slave_deal(uint8_t data);
void modbus_slave_parse(uint16_t *buf);

#endif

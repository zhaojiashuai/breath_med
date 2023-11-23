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

#endif

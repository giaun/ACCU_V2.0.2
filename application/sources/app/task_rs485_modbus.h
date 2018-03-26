#ifndef __TASK_RS485_H__
#define __TASK_RS485_H__
#include <stdint.h>

#include "../driver/modbus_rtu/modbus_rtu.h"
#define BAUDRATE	9600
extern int8_t rs485_stt[10];

void rs485SttHistory();
#endif // __TASK_RS485_H__

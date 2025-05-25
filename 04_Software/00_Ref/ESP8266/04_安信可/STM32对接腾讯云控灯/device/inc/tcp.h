#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f10x.h"

#define ProductID "1DYPPSAMBC"
#define DeviceName "aithinker"
#define Devicesecret_Checksum "9SSUuHOA5rWjSJa0hBkaPg=="
#define data "hello_world"

extern volatile uint8_t TcpClosedFlag;  //连接状态标志

void Tencent_AT(void);

#endif

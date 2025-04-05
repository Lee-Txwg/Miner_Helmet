/**
 * @file         bsp_dht11.c
 * @brief        DHT11 BSP (Board Support Package) source file
 * @details      This file implements the functions to control the DHT11 temperature and humidity sensor,
 *               including initialization, communication, and data reading operations.
 * @author       chan
 * @date         2025/2/18
 * @version      v1.0
 */

#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "stdio.h"
#include "stm32f1xx_hal.h"
 
void DHT11_Send(void);
uint8_t Is_DHT11_ACK(void);
uint8_t DHT11_Data(uint8_t *humi, uint8_t *temp);
 
typedef enum DHT11_STATUS{
                          DHT11_DAT_RESET,
                          DHT11_DAT_SET
}DHT11_STATUS;

#endif // End of __DHT11_H__

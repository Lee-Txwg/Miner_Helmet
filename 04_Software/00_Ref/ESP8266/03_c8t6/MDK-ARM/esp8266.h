#ifndef ESP8266_H__
#define ESP8266_H__

#include "main.h"
#include "usart.h"
#include "UART_DMA.h"
#include "stdio.h"
#include "string.h"
#define ESP8266_Result_OK  1
#define ESP8266_Result_Err  2
uint16_t  esp_init(void);
uint16_t esp_wifi_connect(void);
void esp_mqtt_connect(void);
void esp_reset();
void send_temp();



#endif
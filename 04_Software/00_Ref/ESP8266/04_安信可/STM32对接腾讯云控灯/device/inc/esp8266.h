#ifndef __ESP8266_H
#define __ESP8266_H 			   
#include "stm32f10x.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define ESP8266_USART(fmt, ...)  USART_printf (USART2, fmt, ##__VA_ARGS__)    
#define PC_USART(fmt, ...)       printf(fmt, ##__VA_ARGS__)       //这是串口打印函数，串口1，执行printf后会自动执行fput函数，重定向了printf。
#define AP_ssid    "softAP"
#define AP_passwd  "12345678"
#define STA_ssid   "iot@xuhongv"
#define STA_passwd "xuhong12345678"

typedef enum
{
    STA,
    AP,
    STA_AP
}ENUM_Net_ModeTypeDef;

typedef enum
{
    Direct_Connection,
    SmartConfig,
    SoftAP  
}Distribution_method;

#define RX_BUF_MAX_LEN 1024       //最大字节数
extern struct STRUCT_USART_Fram   //数据帧结构体
{
    char Data_RX_BUF[RX_BUF_MAX_LEN];
    union 
    {
        __IO u16 InfAll;
        struct 
        {
            __IO u16 FramLength       :15;                               // 14:0 
            __IO u16 FramFinishFlag   :1;                                // 15 
        }InfBit;
    }; 
	
}ESP8266_Fram_Record_Struct;


//初始化和功能函数
void ESP8266_Init(u32 bound);
bool ESP8266_Send_AT_Cmd(char *cmd,u32 time);
bool ESP8266_Scan(void);
bool ESP8266_Scan1(void);
void ESP8266_restore(void);
void ESP8266_Disconnect_Ap(void);
void ESP8266_Disconnect_MQTT(void);
void ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode);
void ESP8266_JoinAP(Distribution_method method);
uint8_t ESP8266_Get_LinkStatus(void);
uint8_t ESP8266_Get_MqttStatus(void);
bool ESP8266_Set_Device_Information(void);
bool ESP8266_Configure_MQTT_Connection_Parameters(void);
bool ESP8266_Subscribe_To_MQTT_Topic_Information(void);
bool ESP8266_Post_MQTT_Topic_Information(void);
void USART_printf( USART_TypeDef * USARTx, char * Data, ... );

#endif

#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"
#include "tcp.h"
#include "led.h"
#include <stdarg.h>

struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct = { 0 };  //定义了一个数据帧结构体
void ESP8266_Init(u32 bound)
{
    uart2_Init(bound); 
}

//对ESP8266模块发送相关指令
// cmd 待发送的指令
bool ESP8266_Send_AT_Cmd(char *cmd,u32 time)
{
    ESP8266_Fram_Record_Struct .InfBit .FramLength = 0; //重新接收新的数据包
    ESP8266_USART("%s\r\n", cmd);
	  
    delay_ms(time);   //延时
    ESP8266_Fram_Record_Struct.Data_RX_BUF[ESP8266_Fram_Record_Struct.InfBit.FramLength ] = '\0';
		return true;			
}

bool ESP8266_Scan(void)
{
	if(strstr(ESP8266_Fram_Record_Struct.Data_RX_BUF,"WIFI GOT IP")||strstr(ESP8266_Fram_Record_Struct.Data_RX_BUF,"WIFI_CONNECT_SUCCESS"))
	{
		LED_OFF();
		return false;
	}
	return true;
}

bool ESP8266_Scan1(void)
{
	printf("%s",ESP8266_Fram_Record_Struct.Data_RX_BUF);
	if(strstr(ESP8266_Fram_Record_Struct.Data_RX_BUF,"{\"power_switch\":0}"))
	{
		LED1_OFF();
		return true;
	}
	else if(strstr(ESP8266_Fram_Record_Struct.Data_RX_BUF,"{\"power_switch\":1}"))
	{
		LED1_ON();
		return true;
	}
	delay_ms(200);
	return true;
}


void ESP8266_restore(void)//发送恢复出厂默认指令初始化模块，清除WIFI模块的配网信息
{ 
   ESP8266_Send_AT_Cmd("AT+RESTORE",1000);
}

void ESP8266_Disconnect_Ap(void)//断开与路由器的连接
{
	 ESP8266_Send_AT_Cmd("AT+CWQAP",1000);
}

void ESP8266_Disconnect_MQTT(void)//断开腾讯云连接
{
	 ESP8266_Send_AT_Cmd("AT+TCMQTTDISCONN",1000);
}

void ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)//设置WIFI模式
{
	 switch ( enumMode )
    {
        case STA:
             ESP8266_Send_AT_Cmd ( "AT+CWMODE=1", 2500 );
						 break;
        case AP:
             ESP8266_Send_AT_Cmd ( "AT+CWMODE=2", 2500 );
						 break;
        case STA_AP:
             ESP8266_Send_AT_Cmd ( "AT+CWMODE=3", 2500 );
						 break;
        default:
						 break;
    }       
}

void ESP8266_JoinAP(Distribution_method method)//设置WIFI配网方式
{
	   char cCmd[50];
     switch ( method )
      {
        case Direct_Connection://直连路由
					  sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", STA_ssid, STA_passwd );
            ESP8266_Send_AT_Cmd ( cCmd, 3000 ); 
            break; 
        case SmartConfig://一键配网
            ESP8266_Send_AT_Cmd ( "AT+TCSTARTSMART", 1000 ); 
						break;
        case SoftAP://AP配网
					  sprintf ( cCmd, "AT+TCSAP=\"%s\",\"%s\"", AP_ssid, AP_passwd );
            ESP8266_Send_AT_Cmd ( cCmd, 1000 );
						break;
        default:
					  break;
      }
}

uint8_t ESP8266_Get_LinkStatus(void)//获取联网状态
{
	 if(ESP8266_Send_AT_Cmd("AT+CIPSTATUS",500))
	 {
		 if(strstr(ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:2\r\n" ))
			 return 2;
		 
		 else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:3\r\n" ) )
       return 3;

     else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:4\r\n" ) )
       return 4;
		 
		 else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:5\r\n" ) )
       return 5;
		 
	 }
	 return 0;
}

uint8_t ESP8266_Get_MqttStatus(void)//获取MQTT连接状态
{
	if(ESP8266_Send_AT_Cmd("AT+TCMQTTSTATE?",500))
	 {
		 if(strstr(ESP8266_Fram_Record_Struct .Data_RX_BUF, "+TCMQTTSTATE:1\r\n" ))
			 return 1;
	 }
	 return 0;
}


bool ESP8266_Set_Device_Information(void)//设置平台设备信息
{
	 char cCmd[120];
	 sprintf ( cCmd, "AT+TCDEVINFOSET=1,\"%s\",\"%s\",\"%s\"", ProductID, DeviceName, Devicesecret_Checksum );
	 return ESP8266_Send_AT_Cmd ( cCmd, 1000 );
}

bool ESP8266_Configure_MQTT_Connection_Parameters(void)//配置MQTT连接参数
{
	 return ESP8266_Send_AT_Cmd ( "AT+TCMQTTCONN=1,5000,240,1,0", 1000 );
}


bool ESP8266_Subscribe_To_MQTT_Topic_Information(void)//订阅MQTT主题信息
{
	 char cCmd[120];
	 sprintf ( cCmd, "AT+TCMQTTSUB=\"$thing/down/property/%s/%s\",0",ProductID,DeviceName );
	 return ESP8266_Send_AT_Cmd ( cCmd, 1000 );
}

bool ESP8266_Post_MQTT_Topic_Information(void)//发布MQTT主题信息
{
	 char cCmd[120];
	 sprintf ( cCmd, "AT+TCMQTTPUB =\"$thing/up/property/%s/%s\",0,\"%s\"",ProductID,DeviceName,data );
	 return ESP8266_Send_AT_Cmd ( cCmd, 1000 );
}


static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;

    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
    const char *s;
    int d;   
    char buf[16];


    va_list ap;
    va_start(ap, Data);

    while ( * Data != 0 )     // 判断数据是否到达结束符
    {                                         
        if ( * Data == 0x5c )  //'\'
        {                                     
            switch ( *++Data )
            {
                case 'r':                                     //回车符
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;

                case 'n':                                     //换行符
                USART_SendData(USARTx, 0x0a);   
                Data ++;
                break;

                default:
                Data ++;
                break;
            }            
        }

        else if ( * Data == '%')
        {                                     
            switch ( *++Data )
            {               
                case 's':                                         //字符串
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                Data++;
                break;

                case 'd':           
                    //十进制
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                     Data++;
                     break;
                default:
                     Data++;
                     break;
            }        
        }
        else USART_SendData(USARTx, *Data++);
        while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );

    }
}

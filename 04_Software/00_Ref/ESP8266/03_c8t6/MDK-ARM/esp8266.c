#include "esp8266.h"

extern uint8_t flag,flag_mqtt,flag_reset;
extern uint8_t flag_wifi;
uint8_t ssid[]={"iphone"};
uint8_t psd[]={"11111111"};
uint8_t devicename[]={"test"};
uint8_t productid[]={"422b4zbv12"};
uint8_t deviceid[]={"version=2018-10-31&res=products%2F422b4zbv12%2Fdevices%2Ftest&et=2052911776&method=md5&sign=kSKpEHBa9KKDg%2FtfYQLKFQ%3D%3D"};
uint8_t subscribe[]={"$sys/422b4zbv12/test/thing/property/post/reply"};
uint8_t pubscribe[]={"$sys/422b4zbv12/test/thing/property/post"};

uint8_t fuc[]={"temperature"};
//uint8_t data[]={"$sys/422b4zbv12/test/thing/property/post","{\"id\":\"123\"\,\"params\":{\"temperature\":{\"value\":30\}}}"};
int flag_temp;
extern int temp;
void esp_reset()
{
	int i=0;
		while(i<=6)
	{
		USB_printf("AT+RST\r\n");
		HAL_Delay(500);
    if(flag==1)
		{
			break;
			flag=0;
		}
	}

	
}
uint16_t  esp_init(void)
{
int i=0;

	while(i<=6)
{	USB_printf("AT\r\n");
	HAL_Delay(500);
	i++;
}
if(flag==1)
{
//HAL_Delay(1000);
return ESP8266_Result_OK;
}
else 
{
	return ESP8266_Result_Err;
}
}

uint16_t esp_wifi_connect()
{
	int i=0;
	while(1)
	{
		USB_printf("AT\r\n");
		HAL_Delay(500);
    if(flag==1)
		{
			break;
			flag=0;
		}
	}
	HAL_Delay(500);
	while(1)
	{
		USB_printf("AT+CWMODE=1\r\n");
		HAL_Delay(200);
	if(flag==1)
		break;
	else 
		return ESP8266_Result_Err;
	}
		HAL_Delay(500);
	while(1)
	{
		USB_printf("AT+CWDHCP=1,1\r\n");
		HAL_Delay(200);
	if(flag==1)
		break;
	else 
		return ESP8266_Result_Err;
	}

	while(1)
	{
		flag=0;
		USB_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,psd);
		HAL_Delay(200);
    if(flag_wifi==2)
			break;
//		else
//			return ESP8266_Result_Err;
	}
	
	}

void esp_mqtt_connect()
	{
		int i=0;
		flag=0;
//		if(flag_wifi==2)
		{
			while(1)
			{
			USB_printf("AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",devicename,productid,deviceid);
			HAL_Delay(200);
				if(flag==1)
				break;
				i++;
			}
		
			flag=0;
			HAL_Delay(200);
			while(1)
			{

			USB_printf("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
			HAL_Delay(200);
			if(flag==1)
			break;
			}
			flag=0;
			HAL_Delay(200);
			while(1)
			{
				i++;
			USB_printf("AT+MQTTSUB=0,\"%s\",1\r\n",subscribe);
			HAL_Delay(200);
			if(flag==1)
			{
			break;
			}
			}
			flag_temp=1;

			HAL_Delay(500);
//			while(1)
//			{
//				i++;
//				USB_printf("AT+MQTTPUB=0,\"%s\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"%s\\\":{\\\"value\\\":%d}}}\",0,0\r\n",pubscribe,fuc,i);
//			HAL_Delay(1500);
//			}

			
		}
		
	}
	
void send_temp()
{
		
	if(flag_temp==1)
		{
//					while(1)
			{
			USB_printf("AT+MQTTPUB=0,\"%s\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"%s\\\":{\\\"value\\\":%d}}}\",0,0\r\n",pubscribe,fuc,temp);
//			HAL_Delay(1500);
			}
		}
		
	}

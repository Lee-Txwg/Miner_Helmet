#include "tcp.h"
#include "usart.h"
#include "esp8266.h"
#include "delay.h"
#include "key.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"
#include "led.h"

volatile u8 TcpClosedFlag = 0;
void Tencent_AT(void)
{
	
	uint8_t key=0;
	ESP8266_Get_LinkStatus();
	ESP8266_Get_MqttStatus();
  while(ESP8266_Scan())
	{
		   key=KEY_Scan(0);	//得到键值
	     if(key)
		   {						   
			    switch(key)
			    {				 
				     case WKUP_PRES:	//进入配网模式
					        LED_ON();
					        ESP8266_Net_Mode_Choose(STA);

				          ESP8266_JoinAP(Direct_Connection);//这里使用了直连的方式，方便做调试，如需一键配网或者AP配网请自行更改参数值
					        break;
				     case KEY0_PRES:
					        ESP8266_restore();
					        break;
				          case KEY1_PRES:
					        break;
			    }
		   }
			 else
		   delay_ms(10);
	};//配网步骤
	ESP8266_Set_Device_Information();
	ESP8266_Configure_MQTT_Connection_Parameters();
	while(ESP8266_Scan1())
	{
     ESP8266_Subscribe_To_MQTT_Topic_Information();
	};

}

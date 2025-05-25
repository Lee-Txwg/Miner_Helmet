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
		   key=KEY_Scan(0);	//�õ���ֵ
	     if(key)
		   {						   
			    switch(key)
			    {				 
				     case WKUP_PRES:	//��������ģʽ
					        LED_ON();
					        ESP8266_Net_Mode_Choose(STA);

				          ESP8266_JoinAP(Direct_Connection);//����ʹ����ֱ���ķ�ʽ�����������ԣ�����һ����������AP���������и��Ĳ���ֵ
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
	};//��������
	ESP8266_Set_Device_Information();
	ESP8266_Configure_MQTT_Connection_Parameters();
	while(ESP8266_Scan1())
	{
     ESP8266_Subscribe_To_MQTT_Topic_Information();
	};

}

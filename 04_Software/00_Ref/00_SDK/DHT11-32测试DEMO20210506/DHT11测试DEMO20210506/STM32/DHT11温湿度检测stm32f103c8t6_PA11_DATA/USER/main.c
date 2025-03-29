/***************STM32F103C8T6**********************
 * 文件名  ：main.c
 * 描述    ：dht11传感器
 * 备注    : dht11温度湿度传感器
 * 接口    ：参看dht11.c

********************LIGEN*************************/

#include "stm32f10x.h"
#include "led.h"
#include "usart1.h"
#include "delay.h"
#include "dht11.h"

u8 temp;
u8 humi;

int main(void)
{ 
	
  SystemInit();//配置系统时钟为72M	
	LED_GPIO_Config();//led初始化
	USART1_Config();//串口初始化
	Delay_init(72);
	printf("Start \n");
	while(DHT11_Init())
	{
		printf("DHT11 Error \r\n");
		Delay_ms(1000);
	}
	
  while (1)
  {
		DHT11_Read_Data(&temp,&humi);//
		printf("temp %d ,humi %d\r\n",temp,humi);
		LED_Toggle();
		Delay_ms(1000);
  }
}




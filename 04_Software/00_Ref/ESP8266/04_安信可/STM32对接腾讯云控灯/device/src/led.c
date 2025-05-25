#include "led.h"





void LED_Init(void)//以灯做开关功能,打开插座为灯亮，关闭插座为灯灭
{

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PB5
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	  GPIO_SetBits(GPIOB,GPIO_Pin_5);//灯灭
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PE5
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	  GPIO_SetBits(GPIOE,GPIO_Pin_5);//灯灭
}

void LED_ON(void)
{
	 GPIO_ResetBits(GPIOE,GPIO_Pin_5);
}

void LED1_ON(void)
{
	 GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}

void LED1_OFF(void)
{
	 GPIO_SetBits(GPIOB,GPIO_Pin_5);
}

void LED_OFF(void)
{
	 GPIO_SetBits(GPIOE,GPIO_Pin_5);
}

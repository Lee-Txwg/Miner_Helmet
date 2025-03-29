/***************STM32F103C8T6**********************
 * 文件名  ：led.c
 *          
 * 实验平台：STM32F103C8T6
 * 硬件连接：LED端口PC13,PB12，
 * 备注    ：修改led端口时要同时修改gpio和pin

********************LIGEN*************************/

#include "led.h"
#include "delay.h"

#define LED GPIO_Pin_8

 /***************  配置LED用到的I/O口 *******************/
void LED_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); // 使能端口时钟  
  GPIO_InitStructure.GPIO_Pin = LED;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化端口
  GPIO_SetBits(GPIOB, LED);	 // 关闭所有LED
}
void LED_Toggle(void)
{
	GPIO_WriteBit(GPIOB, LED, (BitAction)((1-GPIO_ReadOutputDataBit(GPIOB, LED))));//led电平翻转
}
void LED_On()
{
	GPIO_WriteBit(GPIOB, LED,(BitAction)0);
}
void LED_Off()
{
	GPIO_WriteBit(GPIOB, LED,(BitAction)1);
}

void LED_Twinkle()
{
	LED_On();
	Delay_ms(10);
	LED_Off();
}

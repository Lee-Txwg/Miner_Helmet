/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20200401
	平台:MINI-STM32F103C8T6

*******************************************************************************/
#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "gps.h"  

extern void GpsDataRead(void);//读取GPS数据并显示

//USART2管脚初始化
void GPIO_Config_Init(void)
{
 
	GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure USART1 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void RCC_Configuration(void)
{
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  /* Enable USART1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
}

void USART_Config_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	
  USART_InitStructure.USART_BaudRate = 38400;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  /* Enable USARTy Receive  interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//使能空闲中断
  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);
		 delay_ms(10);//等待10ms

}

void USART_BaudRate_Init(uint32_t Data)
{
	USART_InitTypeDef USART_InitStructure;
	
  USART_InitStructure.USART_BaudRate = Data;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  /* Enable USARTy Receive  interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//使能空闲中断
  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);
	delay_ms(10);//等待10ms
	
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

//主函数，采用外部8M晶振，72M系统主频，可以在void SetSysClock(void)函数中选择主频率设置
int main(void)
 {	
		uint8_t key=0XFF;

	delay_init();	    	//延时函数初始化,通过Systick中断实现1ms延时功能 
	RCC_Configuration();//USART2时钟使能，GPIO时钟使能
	NVIC_Configuration();//中断接收使能
	GPIO_Config_Init();	//初始化串口2,PA2->USART2_TX，PA3->USART2_RX
	USART_Config_Init();//USART2配置，38400波特率，8位数据，无校验，1位停止
	OLED_Init();//OLED初始化  
	OLED_Clear();//清屏
	OLED_ShowCHinese(18,0,0);//光
	OLED_ShowCHinese(36,0,1);//子
	OLED_ShowCHinese(54,0,2);//物
	OLED_ShowCHinese(72,0,3);//联
	OLED_ShowCHinese(90,0,4);//网
	 delay_ms(500);//等待500ms
	if(Ublox_Cfg_Rate(1000,1)!=0)	//设置定位信息更新速度为1000ms,顺便判断GPS模块是否在位. 
	{
  		OLED_ShowCHinese(18,3,10);//模
		OLED_ShowCHinese(36,3,11);//块
		OLED_ShowCHinese(54,3,12);//同
		OLED_ShowCHinese(72,3,13);//步
		OLED_ShowCHinese(90,3,14);//中
		while((Ublox_Cfg_Rate(1000,1)!=0)&& key)	//持续判断,直到可以检查到NEO-6M,且数据保存成功，如果时初次上电使用，先用9600波特率再重新配置
		{
			USART_BaudRate_Init(9600);//初始化串口2波特率为9600(EEPROM没有保存数据的时候,波特率为9600.)

	  	Ublox_Cfg_Prt(38400);			//重新设置模块的波特率为38400
			Ublox_Cfg_Tp(1000000,100000,1);	//设置PPS为1秒钟输出1次,脉冲宽度为100ms	    
			key=Ublox_Cfg_Cfg_Save();		//保存配置  
		}	  					 
		delay_ms(500);//等待500ms
	
	}
  while (1)
  {
		OLED_Clear();//清屏
		OLED_ShowCHinese(18,0,0);//光
		OLED_ShowCHinese(36,0,1);//子
		OLED_ShowCHinese(54,0,2);//物
		OLED_ShowCHinese(72,0,3);//联
		OLED_ShowCHinese(90,0,4);//网
		GpsDataRead();//读取经纬度，时间信息，并在液晶上显示
		delay_ms(800);//等待800ms
  }
 }


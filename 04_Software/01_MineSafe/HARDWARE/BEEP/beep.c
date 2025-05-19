/*
 * @Author: your name
 * @Date: 2020-04-06 02:16:49
 * @LastEditTime: 2020-04-06 02:18:40
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \undefinedc:\Users\Administrator\Desktop\sunjianping\HARDWARE\BEEP\beep.c
 */

#include "beep.h"
		    
//BEEP IO��ʼ��
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

 //buzzer 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
 GPIO_Init(GPIOA, &GPIO_InitStructure);				
 GPIO_ResetBits(GPIOA,GPIO_Pin_0);	
 
 //buzzer remote
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
 GPIO_Init(GPIOB, &GPIO_InitStructure);		
 GPIO_ResetBits(GPIOB,GPIO_Pin_9);
}
 

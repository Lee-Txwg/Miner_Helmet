#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//�ⲿ�ж� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/01  
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  


// Define PWM states as an enumeration

	 
void EXTIX_Init(void);//IO��ʼ��
void GPIO_PWM_Init(void);
void GPIO_EXTI_Init(void);
void TIM1_PWM_Init(void);
void GPIO_Init_intercom(void);
void Intercom_GPIO_EXTI_Init(void);
void GPIO_Init_intercom(void);
#endif


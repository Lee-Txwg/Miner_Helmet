#ifndef _USART_H_
#define _USART_H_


#include "stm32f10x.h"


#define USART_DEBUG		USART1		//���Դ�ӡ��ʹ�õĴ�����


void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

void DEBUG_LOG(char *fmt,...);

void GPIO_Config_Init(void);
void USART_BaudRate_Init(uint32_t Data);
void USART_Config_Init(void);
void NVIC_Configuration(void);

#endif

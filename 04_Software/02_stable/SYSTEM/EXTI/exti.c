#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "main.h"
extern u8 alarmFlag;	 // �Ƿ񱨾��ı�־
extern u8 alarm_is_free; // �������Ƿ��ֶ�������������ֶ�����������Ϊ0
extern u8 Led_Status;	 // LED status
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// Mini STM32������
// �ⲿ�ж� ��������
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// �޸�����:2010/12/01
// �汾��V1.0
// ��Ȩ���У�����ؾ���
// Copyright(C) ����ԭ�� 2009-2019
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////
// Current PWM state
typedef enum
{
	PWM_OFF = 0,
	PWM_LOW,
	PWM_MEDIUM,
	PWM_HIGH
} pwm_state_t;
pwm_state_t currentPWMState = PWM_OFF;
uint32_t pwmDutyCycle[] = {0, 333, 666, 999}; // Duty cycle percentages: 0%, 25%, 50%, 75

// GPIO and EXTI Initialization Function
void GPIO_EXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // �ⲿ�жϣ���Ҫʹ��AFIOʱ��

	// Enable GPIOB clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// Configure PB0 as input with external interrupt (falling edge)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Pull-up mode
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Enable and configure EXTI line 0 (connected to PB0)
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // Trigger on falling edge
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	// Enable EXTI0 interrupt

	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

// GPIO Configuration for PWM Output (PA11 for TIM1 Channel 4)
void GPIO_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIOA clock (for PA11)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// Configure PA11 as alternate function for TIM1_CH4 (PWM output)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	// Alternate function push-pull mode
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	// Set GPIO speed
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// External interrupt handler for button press on PB3
void EXTI0_IRQHandler(void)
{
	// Check if the interrupt is triggered by PB3 (Button Press)
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		// Clear the interrupt flag
		EXTI_ClearITPendingBit(EXTI_Line0);

		// Switch to the next PWM state
		currentPWMState++;
		if (currentPWMState > PWM_HIGH)
		{
			currentPWMState = PWM_OFF; // Cycle back to off
		}

		// Update Led_Status based on PWM state
		if (currentPWMState == PWM_OFF)
		{
			Led_Status = 0; // Set LED status to OFF when PWM is OFF
		}
		else
		{
			Led_Status = 1; // Set LED status to ON for other states
		}

		// Update the PWM duty cycle based on the new state
		TIM_SetCompare4(TIM1, pwmDutyCycle[currentPWMState]);
	}
}

void TIM1_PWM_Init(void)
{
	// Timer configuration for PWM
	TIM_TimeBaseInitTypeDef TIM_BaseInit;
	TIM_OCInitTypeDef TIM_OCInit;

	// Enable Timer 1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	// Configure Timer 1 for PWM
	TIM_BaseInit.TIM_Period = 1000 - 1;	 // Set timer period for PWM frequency (1 kHz)
	TIM_BaseInit.TIM_Prescaler = 72 - 1; // Prescaler value (adjust as needed)
	TIM_BaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseInit.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_BaseInit);

	// Configure PWM Output (Channel 4)
	TIM_OCInit.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInit.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInit.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInit.TIM_Pulse = pwmDutyCycle[currentPWMState]; // Set initial duty cycle
	TIM_OC4Init(TIM1, &TIM_OCInit);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	// Enable Timer 1 and start PWM
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/* ===================intercom==================== */
// GPIO and EXTI Initialization Function
void Intercom_GPIO_EXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // �ⲿ�жϣ���Ҫʹ��AFIOʱ��

	// Enable GPIOB clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// Configure PB3 as input with external interrupt (falling edge)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Pull-up mode
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Enable and configure EXTI line 3 (connected to PB3)
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);

	EXTI_InitStruct.EXTI_Line = EXTI_Line3;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // Trigger on falling edge
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	// Enable EXTI3 interrupt

	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

// External interrupt handler for button press on PB3
void EXTI3_IRQHandler(void)
{
	// Check if the interrupt is triggered by PB3 (Button Press)
	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		// Clear the interrupt flag
		EXTI_ClearITPendingBit(EXTI_Line3);
		// Toggle PB4 (Flip the state from HIGH to LOW or LOW to HIGH)
		GPIOB->ODR ^= GPIO_Pin_4; // Flip the state of PB4 using XOR operation
	}
}

void GPIO_Init_intercom(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO Remap ??? AFIO ?????????
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	// ʹ�� GPIOB ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// ���� PB4, PB5, PB6, PB7 Ϊ���������������Ϊ����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7; // ���� PB4, PB5, PB6, PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;					 // �������ģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;					 // ���� GPIO �ٶ�Ϊ 50MHz
	GPIO_Init(GPIOB, &GPIO_InitStruct);								 // ��ʼ�� GPIOB

	// ���� PB5 Ϊ��������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ����Ϊ��������
	GPIO_Init(GPIOB, &GPIO_InitStruct);				   // ��ʼ�� PB5 Ϊ��������

	// ����Ϊ��������Ὣ�������ŵ����״̬����Ϊ����
	GPIO_SetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7); // ���� PB4, PB5, PB6, PB7 Ϊ�ߵ�ƽ������
}

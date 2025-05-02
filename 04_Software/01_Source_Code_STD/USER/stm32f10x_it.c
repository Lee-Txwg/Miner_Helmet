/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "gps.h"
#include "string.h"
extern uint8_t USART3_RX_BUF[USART_MAX_RECV_LEN]; 	

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

uint8_t rx_index;
uint8_t rec_over;
#define RXBUFF_SIZE 200

uint8_t aRxBuffer[RXBUFF_SIZE];  // Temporary receive buffer
uint16_t RX_len;                 // Number of bytes received

/*--------------------------------------------------*/
/* USART3 Interrupt Handler: handles RXNE and IDLE */
/*--------------------------------------------------*/
void USART3_IRQHandler(void)
{
    uint8_t temp;

    /* Overrun error? clear it by reading DR */
    if (RESET != USART_GetFlagStatus(USART3, USART_FLAG_ORE))
    {
        (void)USART_ReceiveData(USART3);
    }

    /* Data received? */
    if (RESET != USART_GetITStatus(USART3, USART_IT_RXNE))
    {
        temp = USART_ReceiveData(USART3);

        if (rx_index >= RXBUFF_SIZE)  // Prevent buffer overflow
        {
            return;
        }
        aRxBuffer[rx_index++] = temp;
    }

    /* Idle line detected => frame complete */
    if (RESET != USART_GetFlagStatus(USART3, USART_FLAG_IDLE))
    {
        (void)USART_ReceiveData(USART3);     // Clear IDLE flag
        RX_len = rx_index;                  // Save length
        memcpy(USART3_RX_BUF, aRxBuffer, RX_len);  // Copy to user buffer
        rx_index = 0;                       // Reset index
    }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

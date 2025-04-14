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
#define RXBUFF_SIZE	200

uint8_t aRxBuffer[RXBUFF_SIZE];
uint16_t RX_len;

void USART3_IRQHandler(void)
{
    uint8_t temp;

    // 1. 先处理溢出（ORE），防止死循环
    if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET) {
        // 读 DR 清 ORE
        (void)USART_ReceiveData(USART3);
    }

    // 2. 接收中断
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        temp = (uint8_t)USART_ReceiveData(USART3);
        if (rx_index >= RXBUFF_SIZE) {
            // 缓冲区满，丢弃后续数据
            return;
        }
        aRxBuffer[rx_index++] = temp;
    }

    // 3. 空闲中断（IDLE）标志，表示一帧数据接收完毕
    if (USART_GetFlagStatus(USART3, USART_FLAG_IDLE) != RESET) {
        // 读 SR、DR 清 IDLE
        (void)USART_ReceiveData(USART3);

        RX_len = rx_index;
        // 把本次接收的数据拷贝到外部缓冲区，便于主程序处理
        memcpy(USART3_RX_BUF, aRxBuffer, RX_len);

        // 重置索引，准备下一帧接收
        rx_index = 0;
    }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

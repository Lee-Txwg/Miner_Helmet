#include "UART_DMA.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "NumAndStr.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "oled.h"
uint8_t RxBuffer[UART_RX_BUF_SIZE] = {0};
  uint8_t TxBuffer[UART_RX_BUF_SIZE] = {0};
  uint8_t sendCompleteSign = 1;
  uint8_t flag,flag_wifi,flag_mqtt,flag_reset;
 uint8_t TxLen = 0;
int32_t a,b,c;
double  d,e,f;
	extern uint8_t uart3_RX_Buffer[r3_length];
 extern uint8_t  UART3_temp[REC_LENGTH]; 
extern unsigned int  UART3_Rx_cnt ;

void DataProcess(void)
{
	//在这里加入数据处理的函数
	a = str2int(RxBuffer, ' ', 1);
	b = str2int(RxBuffer, ' ', 2);
	c = str2int(RxBuffer, ' ', 3);
	str2double(RxBuffer, ' ', 4, &d);
	str2double(RxBuffer, ' ', 5, &e);
	str2double(RxBuffer, ' ', 6, &f);

}

//到USARTx_IRQHandler中添加，如:
//void USART1_IRQHandler(void)
//{
//  /* USER CODE BEGIN USART1_IRQn 0 */
//  if(__HAL_UART_GET_FLAG(&USB_Huart,UART_FLAG_IDLE))
//  {
//	  HAL_UART_IdleCallback(&USB_Huart);
//  }
//
//  /* USER CODE END USART1_IRQn 0 */
//  HAL_UART_IRQHandler(&huartx);
//}
void HAL_UART_IdleCallback(UART_HandleTypeDef *huart)
{
	__HAL_UART_CLEAR_IDLEFLAG(huart);
	{
		HAL_UART_DMAStop(huart);

        ProcessData();

        StartUartRxDMA();
	}
}

void ProcessData()
{
    uint32_t len = 0;

    //得到已经接收了多少个字节 = 总共要接收的字节数 - >NDTR F1为CNDTR F4为NDTR
    #ifdef __STM32F1xx_HAL_H
    	len = UART_RX_BUF_SIZE - USB_Huart.hdmarx->Instance->CNDTR;
    	#define ProcessDataOK
    #endif

    #ifdef  __STM32F4xx_HAL_H
        len = UART_RX_BUF_SIZE - USB_Huart.hdmarx->Instance->NDTR;
    	#define ProcessDataOK
    #endif

    #ifndef ProcessDataOK
    	增加所用芯片的版本
    #endif

    if(len > 0)
    {
        if(sendCompleteSign == 1)
        {
#if UART_RXTX_Switch
					memset((void *)TxBuffer, 0, sizeof(TxBuffer));//将TxBuffer数组全部赋值为0
					memcpy(TxBuffer, RxBuffer, len);//将RxBuffer的内容全部复制到TxBuffer
            TxLen = len;
            StartUartTxDMA();	//串口回显
#endif
            {
            	//在这里面加入数据处理的函数
            	DataProcess();
            }
        }
    }
}

void USB_DMA_printf(const char *format,...)
{
	uint32_t length;
	va_list args;

	va_start(args, format);
	length = vsnprintf((char*)TxBuffer, sizeof(TxBuffer)+1, (char*)format, args);
	va_end(args);

	HAL_UART_Transmit_DMA(&USB_Huart,TxBuffer,length);
}

void USB_printf(const char *format,...)
{
	uint32_t length;
	va_list args;

	va_start(args, format);
	length = vsnprintf((char*)TxBuffer, sizeof(TxBuffer)+1, (char*)format, args);
	va_end(args);

	HAL_UART_Transmit(&USB_Huart,TxBuffer,length,0xFFFF);
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
//  UNUSED(huart);
    if(huart == &USB_Huart)
    {
        sendCompleteSign = 1;
    }
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
//  UNUSED(huart;);
    if(huart == &huart3)
		{
			uart3_RX_Buffer[UART3_Rx_cnt]=UART3_temp[0];
			UART3_Rx_cnt++;
//      USB_printf("%s",uart3_RX_Buffer);
			HAL_UART_Receive_IT(&huart3,(uint8_t *)UART3_temp,REC_LENGTH);

		}
    if(huart == &USB_Huart)
    {
        ProcessData();
      StartUartRxDMA();
			
			
       
    }
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}

uint8_t UartTxData(UART_HandleTypeDef *huart, uint8_t *buf, const uint32_t len)
{
	HAL_StatusTypeDef status;
	uint8_t ret = 1;

	if(sendCompleteSign == 0 || len == 0)
	{
		return 0;
	}

    sendCompleteSign = 0;

	status = HAL_UART_Transmit_DMA(huart, (uint8_t*)buf, len);

	if(HAL_OK != status)
	{
		ret = 0;
	}

	return ret;
}

//启动DMA发送
uint8_t StartUartTxDMA()
{
    return UartTxData(&USB_Huart, TxBuffer, TxLen);
}

uint8_t UartRxData(UART_HandleTypeDef *huart, uint8_t *buf, const uint32_t len)
{
	
	HAL_StatusTypeDef status;
	uint8_t ret = 1;

	status = HAL_UART_Receive_DMA(huart, (uint8_t*)buf, len);
	{	
if(strstr(buf,"OK")!=NULL)
{
	flag=1;
}
if(strstr(buf,"WIFI GOT IP")!=NULL)
   flag_wifi=2;
if(strstr(buf,"MQTTCONNECTED")!=NULL)
   flag_mqtt=1;
if(strstr(buf,"MQTTDISCONNECTED")!=NULL)
   flag_reset=1;

// if(strstr(buf,"CLOUD CONNECTED")!=NULL)
//	flag=3;
//if(strstr(buf,"led1off")!=NULL)
// LED1_OFF();
//if(strstr(buf,"led1on")!=NULL)
// LED1_ON();
//		OLED_ShowStr(0+8,5,buf,1);
		uart3_printf("%s",buf);
//		LCD_ShowString(10,10,500,12,12,buf);
		
//		
//	
//	if(buf[0]==0x31)
//	{
//		flag=1;
//	}
//else	if(buf[0]==0x32)
//	{
//		flag=2;
//	}
//else	if(buf[0]==0x33)
//	{
//		flag=3;
//	}
//else	if(buf[0]==0x34)
//	{
//		flag=4;
//	}
//	i++;
}
//pulseWide=buf[0]*10+buf[1];
	if(HAL_OK != status)
	{
		ret = 0;
	}
	else
	{
		/* 开启空闲接收中断 */
	    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	}

	return ret;
}

//启动DMA接收
uint8_t StartUartRxDMA()
{
    return UartRxData(&USB_Huart, RxBuffer, UART_RX_BUF_SIZE);
}


			


/*开始本次的代码解释 首先是USART1的全局中断进行改写，在ISR里添加了代码，对于串口中断的信号位进行判断
判断之后，就开始中断，，其实这个函数集其实也是层层利用的，在TX（由存储器到外设，发送）的中断回调函数里
设置串口中断标志位，然后是RX（外设到存储器，接收）函数的中断回调函数里接受DMA初始化
进而开启DMA接收，在DMA接收函数里返回接收到的数据，其中还有一个数据处理函数，里面将串口数据回显*/
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
	#include "stdio.h"
	#include "dht11.h"
	#include "oled.h"
	#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
struct student
{
	uint8_t longitude[9];//经度
	uint8_t latitude[9];//纬度
	uint8_t time[6];//时间
	bool connect;//连接
};


struct student demo;//经度纬度等信息

uint16_t temperature;//温度数据
uint16_t humidity;//湿度数据
	
uint8_t message[200] = {0}; //接收字符串缓冲区
uint8_t offset; //接收字符串缓冲区的下标及大小
uint8_t mesg; //用于中断时，接收单个字符
uint8_t RX_Flag; //发生中断的标志
uint8_t data[500];
uint8_t dem[10];



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint32_t Distance_Calculate(uint32_t count)//传入时间单位100us
{
    uint32_t Distance = 0;
    Distance = (uint32_t)(((float)count *17)/10);//距离单位cm,声速340M/S，时间*速度/2=距离
    return Distance;
}

uint32_t Distance;//距离单位cm

uint32_t Distance1;//距离单位cm

uint32_t Timcounter;
uint32_t HalTime1,HalTime2;
extern volatile uint32_t TimeCounter;	

/*超声波1,采集数据函数*/
void SR_04_1(){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_RESET);//预先拉低Trig引脚
		HAL_Delay(5);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_SET);//拉高Trig引脚
		Delay_us(20);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_RESET);//拉低Trig引脚
	 Delay_us(20);
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0);//读取PA1脉冲值
	 HalTime1= TimeCounter;
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 1);
		if(TimeCounter>HalTime1)
		{
		HalTime2 = TimeCounter-HalTime1;
			if(HalTime2<0x300)
			{
		Distance = Distance_Calculate(HalTime2);//计算距离值，单位cm
			}
		}
}

/*GPS解析采集*/
void gather_GPS(){
		HAL_UART_Receive(&huart2,data,500,1000);
		for(int i=0;i<500;i++){
			if(data[i]=='$'&&data[i+1]=='G'&&data[i+2]=='N'&&data[i+3]=='R'&&data[i+4]=='M'&&data[i+5]=='C'){
					if(i+42>=500){
							break;
					}
					/*北纬转化*/
					OLED_ShowChar(0,0,data[i+20]);
					OLED_ShowChar(0+8,0,data[i+21]);
					OLED_ShowChar(0+8+8,0,data[i+22]);
					OLED_ShowChar(0+8+8+8,0,data[i+23]);
					OLED_ShowChar(0+8+8+8+8,0,data[i+24]);
					OLED_ShowChar(0+8+8+8+8+8,0,data[i+25]);
					OLED_ShowChar(0+8+8+8+8+8+8,0,data[i+26]);
					OLED_ShowChar(0+8+8+8+8+8+8+8,0,data[i+27]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8,0,data[i+28]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8+8,0,data[i+29]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8+8+8,0,data[i+19]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8+8+8+8,0,data[i+18]);
					demo.latitude[0]=data[i+20];
					demo.latitude[0]=data[i+21];
					demo.latitude[0]=data[i+22];
					demo.latitude[0]=data[i+23];
					demo.latitude[0]=data[i+24];
					demo.latitude[0]=data[i+25];
					demo.latitude[0]=data[i+26];
					demo.latitude[0]=data[i+27];
					demo.latitude[0]=data[i+28];
					demo.latitude[0]=data[i+29];
					/*东经转化*/
//					OLED_ShowChar(0,2,data[i+30]);
					OLED_ShowChar(0,2,data[i+31]);
					OLED_ShowChar(0+8,2,data[i+32]);
					OLED_ShowChar(0+8+8,2,data[i+33]);
					OLED_ShowChar(0+8+8+8,2,data[i+34]);
					OLED_ShowChar(0+8+8+8+8,2,data[i+35]);
					OLED_ShowChar(0+8+8+8+8+8,2,data[i+36]);
					OLED_ShowChar(0+8+8+8+8+8+8,2,data[i+37]);
					OLED_ShowChar(0+8+8+8+8+8+8+8,2,data[i+38]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8,2,data[i+39]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8+8,2,data[i+40]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8+8+8,2,data[i+41]);
					OLED_ShowChar(0+8+8+8+8+8+8+8+8+8+8+8,2,data[i+42]);
					demo.longitude[0]=data[i+33];
					demo.longitude[0]=data[i+34];
					demo.longitude[0]=data[i+35];
					demo.longitude[0]=data[i+36];
					demo.longitude[0]=data[i+37];
					demo.longitude[0]=data[i+38];
					demo.longitude[0]=data[i+39];
					demo.longitude[0]=data[i+40];
					demo.longitude[0]=data[i+41];
					demo.longitude[0]=data[i+42];
					/*时间转化*/
					OLED_ShowChar(0,4,data[i+7]);
					OLED_ShowChar(0+8,4,data[i+8]);
					OLED_ShowChar(0+8+8,4,data[i+9]);
					OLED_ShowChar(0+8+8+8,4,data[i+10]);
					OLED_ShowChar(0+8+8+8+8,4,data[i+11]);
					OLED_ShowChar(0+8+8+8+8+8,4,data[i+12]);
					demo.time[0]=data[i+7];
					demo.time[0]=data[i+8];
					demo.time[0]=data[i+9];
					demo.time[0]=data[i+10];
					demo.time[0]=data[i+11];
					demo.time[0]=data[i+12];
					HAL_Delay(10);
					break;
			}
			
			if(data[i]=='$'&&data[i+1]=='G'&&data[i+2]=='P'&&data[i+3]=='T'&&data[i+4]=='X'&&data[i+5]=='T'&&data[i+24]=='O'&&data[i+25]=='K'){
				if(i>25){break;}
				demo.connect=true;
				break;
			}
			else{
				demo.connect=false;
				break;
			}
		}
}
/*温湿度DHT11采集*/
void gather_DHT11(){
	DHT11_Read_Data(&temperature,&humidity);
	printf("DHT11 Temperature = %d.%d degree\r\n",temperature>>8,temperature&0xff);
	printf("DHT11 Humidity = %d.%d%%\r\n",humidity>>8,humidity&0xff);
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	lcd_showuint4(0,0,100);
  while(DHT11_Init()){
//	printf("DHT11 Checked failed!!!\r\n");
	HAL_Delay(500);
  }
//  printf("DHT11 Checked Sucess!!!\r\n");
	
	if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
 {
	 while(1);
 }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		SR_04_1();
		
		gather_DHT11();
		
		gather_GPS();

		HAL_Delay(1000);
		
		printf("温度：");
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *) &ch,1,HAL_MAX_DELAY);
	return ch;	
}
int fgetc(FILE *f)
{
	uint8_t ch;
	HAL_UART_Receive(&huart1,(uint8_t *)&ch,1,HAL_MAX_DELAY);
	return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

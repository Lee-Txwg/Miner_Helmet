#include "timer.h"
#include "stdio.h"
#include "gps.h"
#include "exti.h"

//////////////////////////////////////////////////////////////////////////////////	 
// This code is for learning purposes only. Without the author's permission, it cannot be used for any other purposes.
// Mini STM32 Development Board
// General Timer Driver Code			   
// Author: ALIENTEK
// Technical Forum: www.openedv.com
// Modification Date: 2010/12/03
// Version: V1.0
// Copyright (C) ALIENTEK 2009-2019
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////	  

char oledBuf[20];

extern nmea_msg gpsx;
// Global GPS display variables
float g_latitude;
float g_longitude;
char  g_ns_hemi;
char  g_ew_hemi;


// External variable declarations
extern u8 humidityH;    // Humidity integer part
extern u8 humidityL;    // Humidity decimal part
extern u8 temperatureH; // Temperature integer part
extern u8 temperatureL; // Temperature decimal part
extern float Light;     // Light intensity
extern float smoke_ppm; // smoke concentration
extern u8 alarmFlag;    // Alarm flag

// TIM3 Interrupt Initialization
// The clock is chosen to be twice that of APB1, and APB1 is 36MHz
// arr: auto-reload value
// psc: clock prescaler
void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // Enable TIM3 clock

    TIM_TimeBaseStructure.TIM_Period = arr; // Set the value to be loaded into the auto-reload register at the next update event (e.g., 5000 for 500ms)
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // Set the prescaler value used as the TIMx clock frequency divisor (e.g., 10kHz counting frequency)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // Set the clock division: TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM counts up
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // Initialize the TIMx time base unit with the specified parameters

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // Enable the specified TIM interrupt: TIM update interrupt

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  // TIM3 interrupt
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // Preemption priority level 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  // Subpriority level 3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // Enable IRQ channel
    NVIC_Init(&NVIC_InitStructure);  // Initialize the peripheral NVIC register with the specified parameters

    TIM_Cmd(TIM3, ENABLE);  // Enable TIMx peripheral
}

void TIM3_IRQHandler(void)   // TIM3 interrupt handler
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // Check if the specified TIM interrupt occurred: TIM update interrupt
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  // Clear TIMx interrupt pending bit: TIM update interrupt
        
        if (alarmFlag) // If alarmFlag is set, toggle LED and BEEP
        {
//            LED1 = !LED1;
//            BEEP = !BEEP;
             LED1 = 0;
             BEEP = 1;
        }
        else
        {
            LED1 = 1;
            BEEP = 0;
        }
    }
}

// TIM2 Interrupt Initialization
// The clock is chosen to be twice that of APB1, and APB1 is 36MHz
// arr: auto-reload value
// psc: clock prescaler
void TIM2_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Enable TIM2 clock

    TIM_TimeBaseStructure.TIM_Period = arr; // Set the value to be loaded into the auto-reload register at the next update event (e.g., 5000 for 500ms)
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // Set the prescaler value used as the TIMx clock frequency divisor (e.g., 10kHz counting frequency)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // Set the clock division: TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM counts up
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); // Initialize the TIMx time base unit with the specified parameters

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // Enable the specified TIM interrupt: TIM update interrupt

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // TIM2 interrupt
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // Preemption priority level 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  // Subpriority level 3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // Enable IRQ channel
    NVIC_Init(&NVIC_InitStructure);  // Initialize the peripheral NVIC register with the specified parameters

    TIM_Cmd(TIM2, ENABLE);  // Enable TIMx peripheral
}

// void TIM2_IRQHandler(void)   // TIM2 interrupt handler
// {
//     if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) // Check if the specified TIM interrupt occurred: TIM update interrupt
//     {
//         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // Clear TIMx interrupt pending bit: TIM update interrupt

//         // sprintf(oledBuf, "Welcome");
//         sprintf(oledBuf, "MineSafe");
//         OLED_ShowString(32, 0, (u8*)oledBuf, 16, 1); // Display "Welcome" at (32,0) with font size 16
//         sprintf(oledBuf, "Hum:%d.%d %%", humidityH, humidityL);
//         OLED_ShowString(0, 32, (u8*)oledBuf, 16, 1); // Display humidity at (0,16)
//         sprintf(oledBuf, "Temp:%d.%d C", temperatureH, temperatureL);
//         OLED_ShowString(0, 16, (u8*)oledBuf, 16, 1); // Display temperature at (0,32)
//         sprintf(oledBuf, "Smoke:%.1f PPM", smoke_ppm);
//         OLED_ShowString(0, 48, (u8*)oledBuf, 16, 1); // Display smoke concentration at (0,48)
//         OLED_Refresh();
//     }
// }

void TIM2_IRQHandler(void)   // TIM2 interrupt handler
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        // Update globals
        g_longitude = gpsx.longitude / 100000.0f;
        g_latitude  = gpsx.latitude  / 100000.0f;
        g_ns_hemi   = gpsx.nshemi;   // 'N' or 'S'
        g_ew_hemi   = gpsx.ewhemi;   // 'E' or 'W'

        // Display first line
        sprintf(oledBuf, "Latitude:%.5f %c", g_latitude, g_ns_hemi);
        OLED_ShowString(0, 0, (u8*)oledBuf, 8, 1);

        sprintf(oledBuf, "Longitude:%.5f %c", g_longitude, g_ew_hemi);
        OLED_ShowString(0, 8, (u8*)oledBuf, 8, 1);

        // Rest of display...
        sprintf(oledBuf, "Hum:%d.%d %%", humidityH, humidityL);
        OLED_ShowString(0, 16, (u8*)oledBuf, 16, 1);
        sprintf(oledBuf, "Temp:%d.%d C", temperatureH, temperatureL);
        OLED_ShowString(0, 32, (u8*)oledBuf, 16, 1);
        sprintf(oledBuf, "Smoke:%.1f PPM", smoke_ppm);
        OLED_ShowString(0, 48, (u8*)oledBuf, 16, 1);

        OLED_Refresh();
    }
}



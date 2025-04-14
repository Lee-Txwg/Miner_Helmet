#include "main.h"
#include "stm32f10x.h"
#include "delay.h"
#include "stm32f10x_adc.h"

#include "mq2.h"

void ADCx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;   // Define GPIO configuration structure
    ADC_InitTypeDef ADC_InitStructure;      // Define ADC configuration structure
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // Enable GPIOA clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   // Enable ADC1 clock
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // Analog input mode
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;     // Set the pin PA1
    GPIO_Init(GPIOA, &GPIO_InitStructure);         // Initialize GPIOA pin 1
    
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // Independent mode
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // Single channel, not scanning
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // Disable continuous conversion
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // No external trigger
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // Right-aligned data
    ADC_InitStructure.ADC_NbrOfChannel = 1; // Single channel conversion
    ADC_Init(ADC1, &ADC_InitStructure);  // Initialize ADC1

    ADC_Cmd(ADC1, ENABLE);  // Enable ADC1
    ADC_ResetCalibration(ADC1); // Reset the ADC calibration registers
    while(ADC_GetResetCalibrationStatus(ADC1));  // Wait for the reset to finish
    ADC_StartCalibration(ADC1);  // Start calibration
    while(ADC_GetCalibrationStatus(ADC1));  // Wait for calibration to complete
}

u16 Get_ADC_Value(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;

    // Configure ADC channel, sequence, and sampling time
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5); // Sample time of 55.5 cycles for better accuracy
    
    for(t = 0; t < times; t++)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // Start ADC conversion via software trigger
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));  // Wait for conversion to complete
        temp_val += ADC_GetConversionValue(ADC1);  // Accumulate conversion result
        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);  // Clear end of conversion flag
        delay_ms(5);  // Small delay to ensure stability
    }
    
    return temp_val / times;  // Return the average value
}

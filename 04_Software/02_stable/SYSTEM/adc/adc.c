#include "adc.h"

void Adc_Init()//初始化函数
{ 
 GPIO_InitTypeDef GPIO_Initstructre;     
 ADC_InitTypeDef ADC_InitStruct;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOARCC_APB2Periph_ADC1,ENABLE)；//使能时钟
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能io口
 GPIO_Initstructre.GPIO_Mode=GPIO_Mode_AIN; 
 GPIO_Initstructre.GPIO_Pin=GPIO_Pin_1;
 GPIO_Initstructre.GPIO_Speed=GPIO_Speed_50MHz;
 GPIO_Init(GPIOA,&GPIO_Initstructre);
 GPIO_SetBits(GPIOA,GPIO_Pin_1);
 
 RCC_ADCCLKConfig(RCC_PCLK2_Div6);//保证不超过14M
 ADC_DeInit(ADC1);//复位ADC1
 ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//不使能连续扫描
 ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//数据右对齐
 ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//软件触发
 ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;//独立模式
 ADC_InitStruct.ADC_NbrOfChannel = 1;
 ADC_InitStruct.ADC_ScanConvMode = DISABLE;//不使用扫描模式
 ADC_Init(ADC1,&ADC_InitStruct);
 ADC_Cmd(ADC1,ENABLE);//使能指定的ADC1
 ADC_ResetCalibration(ADC1);//使能复位校准  
  
 while(ADC_GetResetCalibrationStatus(ADC1));//等待复位校准结束
 
 ADC_StartCalibration(ADC1);//开启AD校准
 
 while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}


u16 Get_Adc(u8 ch)
{
 ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_239Cycles5);//ADC1，通道1，配置采集周期

 ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件复位

 while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//状态发生改变

 return ADC_GetConversionValue(ADC1);//返回值	

}


u16 Get_Adc_Average(u8 ch,u8 times)//两个入口参数，通道，取平均值的次数
{
 u32 temp_val=0;
 u8 t;
 for(t=0;t<times;t++)
  {
   temp_val+=Get_Adc(ch);
   delay_ms(5);
  }
 return temp_val/times;

}


void MQ2_cumlate(float RS)
{
		R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
}

float MQ2_GetPPM(void)
{   
	  u16 adcx;
	  adcx=Get_Adc_Average(ADC_Channel_1,30);//ADC1,取30次的平均值
      float Vrl = 3.3f * adcx / 4096.f;//3.3v的参考电压，4096份
	  Vrl = ( (float)( (int)( (Vrl+0.005)*100 ) ) )/100;
      float RS = (3.3f - Vrl) / Vrl * RL;
	  
      if(times<6) // 获取系统执行时间，3s前进行校准，用到了定时器
       {
		  R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);//校准R0
       } 
	  float ppm = 613.9f * pow(RS/R0, -2.074f);

      return  ppm;
}


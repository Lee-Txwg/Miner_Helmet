#include "event.h"
#include "pid.h"
#include "oled.h"
#include "esp8266.h"
extern uint8_t flag,c,flag_reset;
extern uint8_t flag_wifi;
extern int flag_temp;
extern int temp,temp_set;
int cnt1,cnt2,cnt3,cnt4,cnt5;
extern float m_water;
int power=1000;
float pid[3] 	 = {1,0.0,0.0};//下传送带3508pid
float p_heat,q_set,q_current;
char ok[]={"ok"};
char wifi_ok[]={"wifi is ok"};
char mqtt_ok[]={"mqtt ok"};
GPIO_PinState pin1,pin2;
int def;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	    PID_struct_init(&pid_spd[0], POSITION_PID, 20000, 2000,
    pid[0],pid[1],pid[2]);  //4 motos angular rate closeloop.
		if(htim->Instance == htim1.Instance)
		{
			cnt4++;
			pin1=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
			pin2=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);

					while(pin1==GPIO_PIN_SET)
			{
				cnt3++;
			if(cnt3==4000)
			{
				if(pin1==GPIO_PIN_SET)
			{
				def++;
				temp_set++;
				break;
			}
			}
		}
					while(pin2==GPIO_PIN_SET)
			{
				cnt3++;
			if(cnt3==4000)
			{
				if(pin2==GPIO_PIN_SET)
			{
				def++;
				temp_set--;
				break;
			}
			}
		}


		}
		if(htim->Instance == htim2.Instance)
		{
				cnt1++;
				temp=Ds18b20_Get_Temp();
				if(temp<0)
					{
					temp=-temp;
					}						 
		send_temp();
//					pin=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
//					while(pin==GPIO_PIN_SET)
//			{
//				cnt3++;
//			if(cnt3==4000)
//			{
//				if(	pin==GPIO_PIN_SET)
//			{
//				def++;
////				temp_set++;
//				break;
//			}
//			}
//		}
	
		q_current=m_water*shc_of_water*temp;
		q_set=m_water*shc_of_water*temp_set;
		pid_calc(&pid_spd[0],q_current,q_set,0);		//
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,((pid_spd[0].pout/1.6/6.5*20000)));		//duoji_angle
		
		}
		
		if(htim->Instance == htim3.Instance)
		{
							cnt2++;

			OLED_ShowStr(0,3,"temp_set:",1);
			OLED_ShowFNum(55,3,temp_set,2,0);
			OLED_ShowStr(0,4,"temp:",1);
			OLED_ShowFNum(51,4,temp,2,0);
			
			if(flag==1)
			{
				OLED_ShowStr(0,0,ok,1);
			}
			if(flag_wifi==2)
			{
				OLED_ShowStr(0,1,wifi_ok,1);
			}
			if(flag_temp==1)
			{
				OLED_ShowStr(0,2,mqtt_ok,1);
			}
			if(flag_reset==1)
			{
				esp_reset();
			}			
//			while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
//			{
//			if()

//			}				


			
		}//Q=mc(t)
	
	
}
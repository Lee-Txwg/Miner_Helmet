#include "main_program.h"
#include <stdio.h>
#include "time_handle.h"
#include "led.h"
#include "oled.h"
#include <stdio.h>

/*
*************************************
宏定义
*************************************
*/

/*
*************************************
变量声明
*************************************
*/
extern uint16_t index_led;
extern uint16_t index_oled;
/*
*************************************
函数声明
*************************************
*/

/*
*************************************
变量定义
*************************************
*/
uint8_t led_status =0;
/*
*************************************
函数定义
*************************************
*/

/**
  * @brief          初始化函数,相当于Arduino的setup()函数,只在初始的时候执行一次
  * @param[in]      none
  * @retval         none
  */
void user_init_program(void)
{
  OLED_init(); 
  OLED_printf (0,0,"  Alibaba Cloud IOT ");
  time_slot_start();
}
/**
  * @brief          初始化函数,相当于Arduino的loop()函数,一直执行该函数
  * @param[in]      none
  * @retval         none
  */
void user_main_program(void)
{
  static int count =0;

  if(index_led==1)
  {
    led_status = !led_status;
    set_led(led_status);
    index_led=0;
  }
  if(index_oled==1)
  {
    OLED_printf (2,0,"count:%d",count);
    index_oled=0;
    count++;
  }
}


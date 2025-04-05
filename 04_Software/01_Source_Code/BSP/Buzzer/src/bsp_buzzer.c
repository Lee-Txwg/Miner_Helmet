/**
 * @file         bsp_led.c
 * @brief        LED BSP (Board Support Package) source file
 * @details      This file implements the functions to control the LED, including
 *               initialization, turning on, turning off, and toggling the LED state.
 * @author       lqz
 * @date         2025/2/18
 * @version      v1.0
 * @par Copyright(c): lqz
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_buzzer.h"

/* function prototypes -----------------------------------------------*/ 
//static led_op_result_t led_init   (void); 
//static led_op_result_t led_on     (void); 
//static led_op_result_t led_off    (void); 
//static led_op_result_t led_toggle (void); 
 
/* Defines ------------------------------------------------------------------*/ 
//bsp_led_interface_t bsp_led_interface = { 
//   .pf_led_init   =   led_init, 
//   .pf_led_on     =     led_on, 
//   .pf_led_off    =    led_off, 
//   .pf_led_toggle = led_toggle, 
//}; 
// 
///** 
// * @brief  Initialize the LED 
// * @param  void
// * @retval LED_OP_SUCCESS: Initialization successful 
// */ 
//static led_op_result_t led_init(void) 
//{ 
//    // Set LED to OFF (high level) 
//    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); 
// 
//    return LED_OP_SUCCESS; 
//} 
// 
///** 
// * @brief  Turn on the LED 
// * @param  void
// * @retval LED_OP_SUCCESS: Operation successful 
// */ 
//static led_op_result_t led_on(void) 
//{ 
//    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // Set low level to turn on LED 
// 
//    return LED_OP_SUCCESS; 
//} 
// 
///** 
// * @brief  Turn off the LED 
// * @param  void
// * @retval LED_OP_SUCCESS: Operation successful 
// */ 
//static led_op_result_t led_off(void) 
//{ 
//    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // Set high level to turn off LED 
// 
//    return LED_OP_SUCCESS; 
//} 
// 
///** 
// * @brief  Toggle the LED state 
// * @param  void
// * @retval LED_OP_SUCCESS: Operation successful 
// */ 
//static led_op_result_t led_toggle(void) 
//{ 
//    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // Toggle the LED 
// 
//    return LED_OP_SUCCESS; 
//}

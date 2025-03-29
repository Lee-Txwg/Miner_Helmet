/**
* @file         bsp_led.h
* @brief		led bsp header file
* @details	    This file provides the interface of led.
* @author		lqz
* @date		    2025/2/18
* @version	    v1.0
* @par Copyright(c): 	lqz
*/

#ifndef __BSP_LED_H__ // Avoid repeated including same files later
#define __BSP_LED_H__

//******************************** Includes *********************************//
#include "main.h"
//******************************** Includes *********************************//


//******************************** Defines ********************************//
// Enum for LED operation result return values
typedef enum
{
	LED_OP_SUCCESS = 0,		  // Operation succeeded
	LED_OP_FAILURE,			  // Operation failed
	LED_OP_TIMEOUT,			  // Operation timed out
	LED_OP_INVALID_PARAMETER, // Invalid parameter provided
	LED_OP_BUSY,			  // LED is currently busy and cannot process the operation
	LED_OP_NOT_SUPPORTED,	  // Operation not supported on this hardware or by the firmware
	LED_OP_UNKNOWN_ERROR	  // Undefined error occurred
} led_op_result_t;
//******************************** Defines ********************************//


//******************************** Declaring ********************************//
typedef struct
{
	led_op_result_t  (*pf_led_init)    (void);  // Init the LED
	led_op_result_t  (*pf_led_on)      (void);  // Turn on the LED
	led_op_result_t  (*pf_led_off)     (void);  // Turn off the LED
	led_op_result_t  (*pf_led_toggle)  (void);  // Toggle the LED
} bsp_led_interface_t;

extern bsp_led_interface_t bsp_led_interface;
//******************************** Declaring ********************************//

#endif // End of __BSP_LED_H__

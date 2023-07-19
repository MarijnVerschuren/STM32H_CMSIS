//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_USB_H
#define STM32H_CMSIS_USB_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	USB_CLK_SRC_DISABLED =	0b00,	//R
	USB_CLK_SRC_PLL1_Q =	0b01,
	USB_CLK_SRC_PLL3_Q =	0b10,
	USB_CLK_SRC_HSI48 =		0b11
} USB_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t USB_kernel_frequency;


/*!<
 * init
 * */
void config_USB_kernel_clock(USB_CLK_SRC_t src);


#endif //STM32H_CMSIS_USB_H

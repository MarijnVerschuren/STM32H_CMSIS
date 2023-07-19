//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_CAN_H
#define STM32H_CMSIS_CAN_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	FDCAN_CLK_SRC_HSE =		0b00,	//R
	FDCAN_CLK_SRC_PLL1_Q =	0b01,
	FDCAN_CLK_SRC_PLL2_Q =	0b10
} FDCAN_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t FDCAN_kernel_frequency;


/*!<
 * init
 * */
void config_FDCAN_kernel_clock(FDCAN_CLK_SRC_t src);


#endif //STM32H_CMSIS_CAN_H

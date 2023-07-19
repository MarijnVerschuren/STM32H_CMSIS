//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_RNG_H
#define STM32H_CMSIS_RNG_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	RNG_CLK_SRC_HSI48 =		0b00,	//R
	RNG_CLK_SRC_PLL1_Q =	0b01,
	RNG_CLK_SRC_LSE =		0b10,
	RNG_CLK_SRC_LSI =		0b11
} RNG_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t RNG_kernel_frequency;


/*!<
 * init
 * */
void config_RNG_kernel_clock(RNG_CLK_SRC_t src);


#endif //STM32H_CMSIS_RNG_H

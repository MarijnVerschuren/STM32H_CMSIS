//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_FMC_H
#define STM32H_CMSIS_FMC_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	FMC_CLK_SRC_AHB =		0b00,	//R
	FMC_CLK_SRC_PLL1_Q =	0b01,
	FMC_CLK_SRC_PLL2_R =	0b10,
	FMC_CLK_SRC_PER =		0b11
} FMC_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t FMC_kernel_frequency;


/*!<
 * init
 * */
void config_FMC_kernel_clock(FMC_CLK_SRC_t src);


#endif //STM32H_CMSIS_FMC_H

//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_ADC_H
#define STM32H_CMSIS_ADC_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	ADC_CLK_SRC_PLL2_P =	0b00,	//R
	ADC_CLK_SRC_PLL3_R =	0b01,
	ADC_CLK_SRC_PER =		0b10
} ADC_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t ADC_kernel_frequency;


/*!<
 * init
 * */
void config_ADC_kernel_clock(ADC_CLK_SRC_t src);


#endif //STM32H_CMSIS_ADC_H

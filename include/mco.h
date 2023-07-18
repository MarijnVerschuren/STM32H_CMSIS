//
// Created by marijn on 7/6/23.
//

#ifndef STM32H_CMSIS_MCO_H
#define STM32H_CMSIS_MCO_H
#include "main.h"
#include "gpio.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	MCO1_A8 = 0x08000000,
	MCO2_C9 = 0x09200001
} MCO_GPIO_t;

typedef enum {
	MCO1_SRC_HSI =		0b000,	//R
	MCO2_SRC_SYS =		0b000,	//R
	MCO1_SRC_LSE =		0b001,
	MCO2_SRC_PLL2_P =	0b001,
	MCO1_SRC_HSE =		0b010,
	MCO2_SRC_HSE =		0b010,
	MCO1_SRC_PLL1_Q =	0b011,
	MCO2_SRC_PLL1_P =	0b011,
	MCO1_SRC_HSI48 =	0b100,
	MCO2_SRC_CSI =		0b100,
	MCO2_SRC_LSI =		0b101,
} MCO_SRC_t;


/*!<
 * variables
 * */
extern uint32_t MCO1_kernel_frequency;
extern uint32_t MCO2_kernel_frequency;


/*!<
 * init
 * */
void config_MCO(MCO_GPIO_t pin, MCO_SRC_t src, uint8_t prescaler);


#endif //STM32H_CMSIS_MCO_H

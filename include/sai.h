//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_SAI_H
#define STM32H_CMSIS_SAI_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	SAI_CLK_SRC_PLL1_Q =		0b000,	//R
	SAI_CLK_SRC_PLL2_P =		0b001,
	SAI_CLK_SRC_PLL3_P =		0b010,
	SAI_CLK_SRC_I2S_CKIN =		0b011,
	SAI_CLK_SRC_PER =			0b100
} SAI_CLK_SRC_t;

typedef enum {
	SPDIFRX_CLK_SRC_PLL1_Q =	0b00,	//R
	SPDIFRX_CLK_SRC_PLL2_R =	0b01,
	SPDIFRX_CLK_SRC_PLL3_R =	0b10,
	SPDIFRX_CLK_SRC_HSI =		0b11
} SPDIFRX_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t SAI1_kernel_frequency;
extern uint32_t SAI23_kernel_frequency;
extern uint32_t SAI4A_kernel_frequency;
extern uint32_t SAI4B_kernel_frequency;
extern uint32_t SPDIFRX_kernel_frequency;


/*!<
 * init
 * */
void config_SAI_kernel_clocks(
		SAI_CLK_SRC_t sai1_src, SAI_CLK_SRC_t sai23_src,
		SAI_CLK_SRC_t sai4a_src, SAI_CLK_SRC_t sai4b_src,
		SPDIFRX_CLK_SRC_t spdifrx_src
);


#endif //STM32H_CMSIS_SAI_H

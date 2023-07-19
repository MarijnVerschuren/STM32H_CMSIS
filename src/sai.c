//
// Created by marijn on 7/19/23.
//

#include "sai.h"


/*!<
 * variables
 * */
uint32_t SAI1_kernel_frequency		= 0;
uint32_t SAI23_kernel_frequency		= 0;
uint32_t SAI4A_kernel_frequency		= 0;
uint32_t SAI4B_kernel_frequency		= 0;
uint32_t SPDIFRX_kernel_frequency	= 0;


/*!<
 * init
 * */
void config_SAI_kernel_clocks(
		SAI_CLK_SRC_t sai1_src, SAI_CLK_SRC_t sai23_src,
		SAI_CLK_SRC_t sai4a_src, SAI_CLK_SRC_t sai4b_src,
		SPDIFRX_CLK_SRC_t spdifrx_src
) {
	RCC->D2CCIP1R &= ~(
			RCC_D2CCIP1R_SAI1SEL	|
			RCC_D2CCIP1R_SAI23SEL	|
			RCC_D2CCIP1R_SPDIFSEL_Pos
	);
	RCC->D2CCIP1R |= (
			(sai1_src << RCC_D2CCIP1R_SAI1SEL_Pos)		|
			(sai23_src << RCC_D2CCIP1R_SAI23SEL_Pos)	|
			(spdifrx_src << RCC_D2CCIP1R_SPDIFSEL_Pos)
	);
	RCC->D3CCIPR &= ~(
			RCC_D3CCIPR_SAI4ASEL |
			RCC_D3CCIPR_SAI4BSEL
	);
	RCC->D3CCIPR |= (
			(sai4a_src << RCC_D3CCIPR_SAI4ASEL_Pos) |
			(sai4b_src << RCC_D3CCIPR_SAI4BSEL_Pos)
	);
	switch (sai1_src) {
		case SAI_CLK_SRC_PLL1_Q:		SAI1_kernel_frequency = PLL1_Q_clock_frequency; break;
		case SAI_CLK_SRC_PLL2_P:		SAI1_kernel_frequency = PLL2_P_clock_frequency; break;
		case SAI_CLK_SRC_PLL3_P:		SAI1_kernel_frequency = PLL3_P_clock_frequency; break;
		case SAI_CLK_SRC_I2S_CKIN:		SAI1_kernel_frequency = 0; break;  // external clock
		case SAI_CLK_SRC_PER:			SAI1_kernel_frequency = PER_clock_frequency; break;
	}	switch (sai23_src) {
		case SAI_CLK_SRC_PLL1_Q:		SAI23_kernel_frequency = PLL1_Q_clock_frequency; break;
		case SAI_CLK_SRC_PLL2_P:		SAI23_kernel_frequency = PLL2_P_clock_frequency; break;
		case SAI_CLK_SRC_PLL3_P:		SAI23_kernel_frequency = PLL3_P_clock_frequency; break;
		case SAI_CLK_SRC_I2S_CKIN:		SAI23_kernel_frequency = 0; break;  // external clock
		case SAI_CLK_SRC_PER:			SAI23_kernel_frequency = PER_clock_frequency; break;
	}	switch (sai4a_src) {
		case SAI_CLK_SRC_PLL1_Q:		SAI4A_kernel_frequency = PLL1_Q_clock_frequency; break;
		case SAI_CLK_SRC_PLL2_P:		SAI4A_kernel_frequency = PLL2_P_clock_frequency; break;
		case SAI_CLK_SRC_PLL3_P:		SAI4A_kernel_frequency = PLL3_P_clock_frequency; break;
		case SAI_CLK_SRC_I2S_CKIN:		SAI4A_kernel_frequency = 0; break;  // external clock
		case SAI_CLK_SRC_PER:			SAI4A_kernel_frequency = PER_clock_frequency; break;
	}	switch (sai4b_src) {
		case SAI_CLK_SRC_PLL1_Q:		SAI4B_kernel_frequency = PLL1_Q_clock_frequency; break;
		case SAI_CLK_SRC_PLL2_P:		SAI4B_kernel_frequency = PLL2_P_clock_frequency; break;
		case SAI_CLK_SRC_PLL3_P:		SAI4B_kernel_frequency = PLL3_P_clock_frequency; break;
		case SAI_CLK_SRC_I2S_CKIN:		SAI4B_kernel_frequency = 0; break;  // external clock
		case SAI_CLK_SRC_PER:			SAI4B_kernel_frequency = PER_clock_frequency; break;
	}	switch (spdifrx_src) {
		case SPDIFRX_CLK_SRC_PLL1_Q:	SPDIFRX_kernel_frequency = PLL1_Q_clock_frequency; return;
		case SPDIFRX_CLK_SRC_PLL2_R:	SPDIFRX_kernel_frequency = PLL2_R_clock_frequency; return;
		case SPDIFRX_CLK_SRC_PLL3_R:	SPDIFRX_kernel_frequency = PLL3_R_clock_frequency; return;
		case SPDIFRX_CLK_SRC_HSI:		SPDIFRX_kernel_frequency = HSI_clock_frequency; return;
	}
}
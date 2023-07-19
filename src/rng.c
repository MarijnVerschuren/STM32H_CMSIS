//
// Created by marijn on 7/19/23.
//

#include "rng.h"


/*!<
 * variables
 * */
uint32_t RNG_kernel_frequency = 0;


/*!<
 * init
 * */
void config_RNG_kernel_clock(RNG_CLK_SRC_t src) {
	RCC->D2CCIP2R &= ~RCC_D2CCIP2R_RNGSEL;
	RCC->D2CCIP2R |= src << RCC_D2CCIP2R_RNGSEL_Pos;
	switch (src) {
		case RNG_CLK_SRC_HSI48:		RNG_kernel_frequency = HSI48_clock_frequency; return;
		case RNG_CLK_SRC_PLL1_Q:	RNG_kernel_frequency = PLL1_Q_clock_frequency; return;
		case RNG_CLK_SRC_LSE:		RNG_kernel_frequency = LSE_clock_frequency; return;
		case RNG_CLK_SRC_LSI:		RNG_kernel_frequency = LSI_clock_frequency; return;
	}
}
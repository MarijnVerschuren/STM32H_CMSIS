//
// Created by marijn on 7/19/23.
//

#include "fmc.h"


/*!<
 * variables
 * */
uint32_t FMC_kernel_frequency = 0;


/*!<
 * init
 * */
void config_FMC_kernel_clock(FMC_CLK_SRC_t src) {
	RCC->D1CCIPR &= ~RCC_D1CCIPR_FMCSEL;
	RCC->D1CCIPR |= src << RCC_D1CCIPR_FMCSEL_Pos;
	switch (src) {
		case FMC_CLK_SRC_AHB:		FMC_kernel_frequency = AHB_clock_frequency; return;
		case FMC_CLK_SRC_PLL1_Q:	FMC_kernel_frequency = PLL1_Q_clock_frequency; return;
		case FMC_CLK_SRC_PLL2_R:	FMC_kernel_frequency = PLL2_R_clock_frequency; return;
		case FMC_CLK_SRC_PER:		FMC_kernel_frequency = PER_clock_frequency; return;
	}
}
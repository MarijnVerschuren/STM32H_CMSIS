//
// Created by marijn on 7/19/23.
//

#include "can.h"


/*!<
 * variables
 * */
uint32_t FDCAN_kernel_frequency = 0;


/*!<
 * init
 * */
void config_FDCAN_kernel_clock(FDCAN_CLK_SRC_t src) {
	RCC->D2CCIP1R &= ~RCC_D2CCIP1R_FDCANSEL;
	RCC->D2CCIP1R |= src << RCC_D2CCIP1R_FDCANSEL_Pos;
	switch (src) {
		case FDCAN_CLK_SRC_HSE:		FDCAN_kernel_frequency = HSE_clock_frequency; return;
		case FDCAN_CLK_SRC_PLL1_Q:	FDCAN_kernel_frequency = PLL1_Q_clock_frequency; return;
		case FDCAN_CLK_SRC_PLL2_Q:	FDCAN_kernel_frequency = PLL2_Q_clock_frequency; return;
	}
}
//
// Created by marijn on 7/19/23.
//

#include "hdmi.h"


/*!<
 * variables
 * */
uint32_t CEC_kernel_frequency = 0;


/*!<
 * init
 * */
void config_CEC_kernel_clock(CEC_CLK_SRC_t src) {
	RCC->D2CCIP2R &= ~RCC_D2CCIP2R_CECSEL;
	RCC->D2CCIP2R |= src << RCC_D2CCIP2R_CECSEL_Pos;
	switch (src) {
		case CEC_CLK_SRC_LSE: CEC_kernel_frequency = LSE_clock_frequency; return;
		case CEC_CLK_SRC_LSI: CEC_kernel_frequency = LSI_clock_frequency; return;
		case CEC_CLK_SRC_CSI: CEC_kernel_frequency = CSI_clock_frequency; return;
	}
}
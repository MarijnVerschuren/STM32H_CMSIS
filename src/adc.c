//
// Created by marijn on 7/19/23.
//

#include "adc.h"


/*!<
 * variables
 * */
uint32_t ADC_kernel_frequency = 0;


/*!<
 * init
 * */
void config_ADC_kernel_clock(ADC_CLK_SRC_t src) {
	RCC->D3CCIPR &= ~RCC_D3CCIPR_ADCSEL;
	RCC->D3CCIPR |= src << RCC_D3CCIPR_ADCSEL_Pos;
	switch (src) {
		case ADC_CLK_SRC_PLL2_P:	ADC_kernel_frequency = PLL2_P_clock_frequency; return;
		case ADC_CLK_SRC_PLL3_R:	ADC_kernel_frequency = PLL3_R_clock_frequency; return;
		case ADC_CLK_SRC_PER:		ADC_kernel_frequency = PER_clock_frequency; return;
	}
}
//
// Created by marijn on 7/19/23.
//

#include "sd_mmc.h"


/*!<
 * variables
 * */
uint32_t SDMMC_kernel_frequency = 0;


/*!<
 * init
 * */
void config_SDMMC_kernel_clocks(SDMMC_CLK_SRC_t src) {
	RCC->D1CCIPR &= ~RCC_D1CCIPR_SDMMCSEL;
	RCC->D1CCIPR |= src << RCC_D1CCIPR_SDMMCSEL_Pos;
	switch (src) {
		case SDMMC_CLK_SRC_PLL1_Q: SDMMC_kernel_frequency = PLL1_Q_clock_frequency; return;
		case SDMMC_CLK_SRC_PLL2_R: SDMMC_kernel_frequency = PLL2_R_clock_frequency; return;
	}
}
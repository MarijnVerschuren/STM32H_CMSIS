//
// Created by marijn on 7/19/23.
//

#include "spi.h"


/*!<
 * variables
 * */
uint32_t SPI123_kernel_frequency =	0;
uint32_t SPI45_kernel_frequency =	0;
uint32_t SPI6_kernel_frequency =	0;
uint32_t QSPI_kernel_frequency =	0;


/*!<
 * init
 * */
void config_SPI_kernel_clocks(
		SPI123_CLK_SRC_t spi123_src, SPI456_CLK_SRC_t spi45_src,
		SPI456_CLK_SRC_t spi6_src, QSPI_CLK_SRC_t qspi_src
) {
	RCC->D2CCIP1R &= ~(
			RCC_D2CCIP1R_SPI123SEL |
			RCC_D2CCIP1R_SPI45SEL
	);
	RCC->D2CCIP1R |= (
			(spi123_src << RCC_D2CCIP1R_SPI123SEL_Pos) |
			(spi45_src<< RCC_D2CCIP1R_SPI45SEL_Pos)
	);
	RCC->D3CCIPR &= RCC_D3CCIPR_SPI6SEL;
	RCC->D3CCIPR |= spi6_src << RCC_D3CCIPR_SPI6SEL_Pos;
	RCC->D1CCIPR &= ~RCC_D1CCIPR_QSPISEL;
	RCC->D1CCIPR |= qspi_src << RCC_D1CCIPR_QSPISEL_Pos;
	switch (spi123_src) {
		case SPI123_CLK_SRC_PLL1_Q:		SPI123_kernel_frequency = PLL1_Q_clock_frequency; break;
		case SPI123_CLK_SRC_PLL2_P:		SPI123_kernel_frequency = PLL2_P_clock_frequency; break;
		case SPI123_CLK_SRC_PLL3_P:		SPI123_kernel_frequency = PLL3_P_clock_frequency; break;
		case SPI123_CLK_SRC_I2S_CKIN:	SPI123_kernel_frequency = 0; break;  // external
		case SPI123_CLK_SRC_PER:		SPI123_kernel_frequency = PER_clock_frequency; break;
	}	switch (spi45_src) {
		case SPI456_CLK_SRC_APBx:		SPI45_kernel_frequency = APB2_clock_frequency; break;	// APB2
		case SPI456_CLK_SRC_PLL2_Q:		SPI45_kernel_frequency = PLL2_Q_clock_frequency; break;
		case SPI456_CLK_SRC_PLL3_Q:		SPI45_kernel_frequency = PLL3_Q_clock_frequency; break;
		case SPI456_CLK_SRC_HSI:		SPI45_kernel_frequency = HSI_clock_frequency; break;
		case SPI456_CLK_SRC_CSI:		SPI45_kernel_frequency = CSI_clock_frequency; break;
		case SPI456_CLK_SRC_HSE:		SPI45_kernel_frequency = HSE_clock_frequency; break;
	}	switch (spi6_src) {
		case SPI456_CLK_SRC_APBx:		SPI6_kernel_frequency = APB4_clock_frequency; break;	// APB4
		case SPI456_CLK_SRC_PLL2_Q:		SPI6_kernel_frequency = PLL2_Q_clock_frequency; break;
		case SPI456_CLK_SRC_PLL3_Q:		SPI6_kernel_frequency = PLL3_Q_clock_frequency; break;
		case SPI456_CLK_SRC_HSI:		SPI6_kernel_frequency = HSI_clock_frequency; break;
		case SPI456_CLK_SRC_CSI:		SPI6_kernel_frequency = CSI_clock_frequency; break;
		case SPI456_CLK_SRC_HSE:		SPI6_kernel_frequency = HSE_clock_frequency; break;
	}	switch (qspi_src) {
		case QSPI_CLK_SRC_AHB:			QSPI_kernel_frequency = AHB_clock_frequency; return;	// AHB3
		case QSPI_CLK_SRC_PLL1_Q:		QSPI_kernel_frequency = PLL1_Q_clock_frequency; return;
		case QSPI_CLK_SRC_PLL2_R:		QSPI_kernel_frequency = PLL2_R_clock_frequency; return;
		case QSPI_CLK_SRC_PER:			QSPI_kernel_frequency = PER_clock_frequency; return;
	}
}
//
// Created by marijn on 6/26/23.
//

#include "sys.h"


/*!< static variables */
static sys_tick_t tick_func = NULL;


/*!< constants */
const uint32_t LSI_clock_frequency = 32000;  // TODO: 32.xxx KHz
const uint32_t LSE_clock_frequency = 32000;  // TODO: 32.xxx KHz
const uint32_t CSI_clock_frequency = 4000000;


/*!< variables */  // values are updated when calling sys_clock_init
uint32_t HSI_clock_frequency = 64000000;	// HSI is 64 MHz by default
uint32_t HSE_clock_frequency = 0;			// HSE is within [4, 48] MHz

uint32_t PLL1_P_clock_frequency = 0;
uint32_t PLL1_Q_clock_frequency = 0;
uint32_t PLL1_R_clock_frequency = 0;
uint32_t PLL2_P_clock_frequency = 0;
uint32_t PLL2_Q_clock_frequency = 0;
uint32_t PLL2_R_clock_frequency = 0;
uint32_t PLL3_P_clock_frequency = 0;
uint32_t PLL3_Q_clock_frequency = 0;
uint32_t PLL3_R_clock_frequency = 0;

// TODO: <default values>
uint32_t APB1_clock_frequency = 0;
uint32_t AHB1_clock_frequency = 0;
uint32_t APB2_clock_frequency = 0;
uint32_t AHB2_clock_frequency = 0;
uint32_t APB3_clock_frequency = 0;
uint32_t AHB3_clock_frequency = 0;
uint32_t APB4_clock_frequency = 0;
uint32_t AHB4_clock_frequency = 0;

uint32_t RTC_clock_frequency = 0;

uint32_t SYS_clock_frequency = 0;
// TODO: </default values>

volatile uint64_t tick = 0;  // updated with sys_tick


/*!< interrupts */
void SysTick_Handler(void) { tick++; if (tick_func) { tick_func(); } }


/*!< init */
void sys_clock_init(SYS_CLK_Config_t* config) {
	/*!< update base clock frequency variables */
	HSE_clock_frequency = config->HSE_freq;
	HSI_clock_frequency /= (1 << config->HSI_div);
	/*!< configure PLL clocks */
	RCC->PLLCKSELR = (  // PLL clock source and prescalers (PLL input settings)
			(config->PLL3_config.M_factor << RCC_PLLCKSELR_DIVM3_Pos)		|
			(config->PLL2_config.M_factor << RCC_PLLCKSELR_DIVM2_Pos)		|
			(config->PLL1_config.M_factor << RCC_PLLCKSELR_DIVM1_Pos)		|
			(config->PLL_src << RCC_PLLCKSELR_PLLSRC_Pos)
	);
	RCC->PLLCFGR = (  // PLL enable settings
			// PLL1
			(config->PLL1_config.R_enable << RCC_PLLCFGR_DIVR1EN_Pos)		|
			(config->PLL1_config.Q_enable << RCC_PLLCFGR_DIVQ1EN_Pos)		|
			(config->PLL1_config.P_enable << RCC_PLLCFGR_DIVP1EN_Pos)		|
			(config->PLL1_config.input_range << RCC_PLLCFGR_PLL1RGE_Pos)	|
			(config->PLL1_config.VCO_range << RCC_PLLCFGR_PLL1VCOSEL_Pos)	|
			(config->PLL1_config.frac_enable << RCC_PLLCFGR_PLL1FRACEN_Pos)	|
			// PLL2
			(config->PLL2_config.R_enable << RCC_PLLCFGR_DIVR2EN_Pos)		|
			(config->PLL2_config.Q_enable << RCC_PLLCFGR_DIVQ2EN_Pos)		|
			(config->PLL2_config.P_enable << RCC_PLLCFGR_DIVP2EN_Pos)		|
			(config->PLL2_config.input_range << RCC_PLLCFGR_PLL2RGE_Pos)	|
			(config->PLL2_config.VCO_range << RCC_PLLCFGR_PLL2VCOSEL_Pos)	|
			(config->PLL2_config.frac_enable << RCC_PLLCFGR_PLL2FRACEN_Pos)	|
			// PLL3
			(config->PLL3_config.R_enable << RCC_PLLCFGR_DIVR3EN_Pos)		|
			(config->PLL3_config.Q_enable << RCC_PLLCFGR_DIVQ3EN_Pos)		|
			(config->PLL3_config.P_enable << RCC_PLLCFGR_DIVP3EN_Pos)		|
			(config->PLL3_config.input_range << RCC_PLLCFGR_PLL3RGE_Pos)	|
			(config->PLL3_config.VCO_range << RCC_PLLCFGR_PLL3VCOSEL_Pos)	|
			(config->PLL3_config.frac_enable << RCC_PLLCFGR_PLL3FRACEN_Pos)
	);
	/*!< PLL settings */
	RCC->PLL1DIVR = (
			(config->PLL1_config.R_factor << RCC_PLL1DIVR_R1_Pos)			|
			(config->PLL1_config.Q_factor << RCC_PLL1DIVR_Q1_Pos)			|
			(config->PLL1_config.P_factor << RCC_PLL1DIVR_P1_Pos)			|
			(config->PLL1_config.N_factor << RCC_PLL1DIVR_N1_Pos)
	);
	RCC->PLL1FRACR = (config->PLL1_config.N_fraction << RCC_PLL1FRACR_FRACN1_Pos);
	RCC->PLL2DIVR = (
			(config->PLL2_config.R_factor << RCC_PLL2DIVR_R2_Pos)			|
			(config->PLL2_config.Q_factor << RCC_PLL2DIVR_Q2_Pos)			|
			(config->PLL2_config.P_factor << RCC_PLL2DIVR_P2_Pos)			|
			(config->PLL2_config.N_factor << RCC_PLL2DIVR_N2_Pos)
	);
	RCC->PLL2FRACR = (config->PLL2_config.N_fraction << RCC_PLL2FRACR_FRACN2_Pos);
	RCC->PLL3DIVR = (
			(config->PLL3_config.R_factor << RCC_PLL3DIVR_R3_Pos)			|
			(config->PLL3_config.Q_factor << RCC_PLL3DIVR_Q3_Pos)			|
			(config->PLL3_config.P_factor << RCC_PLL3DIVR_P3_Pos)			|
			(config->PLL3_config.N_factor << RCC_PLL3DIVR_N3_Pos)
	);
	RCC->PLL3FRACR = (config->PLL3_config.N_fraction << RCC_PLL3FRACR_FRACN3_Pos);
	/*!< update frequency variables */
	uint32_t PLL_src_freq = 0;
	uint32_t PLL1_freq, PLL2_freq, PLL3_freq;
	switch (config->PLL_src) {
		case PLL_SRC_HSI: PLL_src_freq = HSI_clock_frequency; break;
		case PLL_SRC_CSI: PLL_src_freq = CSI_clock_frequency; break;
		case PLL_SRC_HSE: PLL_src_freq = HSE_clock_frequency; break;
	}
	PLL1_freq = PLL_src_freq * config->PLL1_config.N_factor;
	PLL2_freq = PLL_src_freq * config->PLL2_config.N_factor;
	PLL3_freq = PLL_src_freq * config->PLL3_config.N_factor;
	if (config->PLL1_config.P_enable) { PLL1_P_clock_frequency = PLL1_freq / config->PLL1_config.P_factor; }
	if (config->PLL1_config.Q_enable) { PLL1_Q_clock_frequency = PLL1_freq / config->PLL1_config.Q_factor; }
	if (config->PLL1_config.R_enable) { PLL1_R_clock_frequency = PLL1_freq / config->PLL1_config.R_factor; }
	if (config->PLL2_config.P_enable) { PLL2_P_clock_frequency = PLL2_freq / config->PLL2_config.P_factor; }
	if (config->PLL2_config.Q_enable) { PLL2_Q_clock_frequency = PLL2_freq / config->PLL2_config.Q_factor; }
	if (config->PLL2_config.R_enable) { PLL2_R_clock_frequency = PLL2_freq / config->PLL2_config.R_factor; }
	if (config->PLL3_config.P_enable) { PLL3_P_clock_frequency = PLL3_freq / config->PLL3_config.P_factor; }
	if (config->PLL3_config.Q_enable) { PLL3_Q_clock_frequency = PLL3_freq / config->PLL3_config.Q_factor; }
	if (config->PLL3_config.R_enable) { PLL3_R_clock_frequency = PLL3_freq / config->PLL3_config.R_factor; }
	switch (config->SYS_CLK_src) {
		case SYS_CLK_SRC_HSI: SYS_clock_frequency = HSI_clock_frequency; break;
		case SYS_CLK_SRC_CSI: SYS_clock_frequency = CSI_clock_frequency; break;
		case SYS_CLK_SRC_HSE: SYS_clock_frequency = HSE_clock_frequency; break;
		case SYS_CLK_SRC_PLL: SYS_clock_frequency = PLL1_P_clock_frequency; break;
	}
	/*!< configure flash */
	FLASH->ACR = (
			(config->FLASH_program_delay << FLASH_ACR_WRHIGHFREQ_Pos)		|
			(config->FLASH_latency << FLASH_ACR_LATENCY_Pos)
	);
	/*!< configure domain pre-scalars */
	RCC->D1CFGR = (
			(config->SYS_CLK_prescaler << RCC_D1CFGR_D1CPRE_Pos)			|
			(config->APB3_prescaler << RCC_D1CFGR_D1PPRE_Pos)				|
			(config->AHB_prescaler << RCC_D1CFGR_HPRE_Pos)
	);
	RCC->D2CFGR = (
			(config->APB2_prescaler << RCC_D2CFGR_D2PPRE2_Pos)				|
			(config->APB1_prescaler << RCC_D2CFGR_D2PPRE1_Pos)
	);
	RCC->D3CFGR = (config->APB4_prescaler << RCC_D3CFGR_D3PPRE_Pos);
	/*!< configure peripheral kernel clocks */  // TODO: !!!!!!
	RCC->D1CCIPR = (
			(config->PER_src << RCC_D1CCIPR_CKPERSEL_Pos)					|
			(config->SDMMC_CLK_src << RCC_D1CCIPR_SDMMCSEL_Pos)				|
			(config->QSPI_CLK_src << RCC_D1CCIPR_QSPISEL_Pos)				|
			(config->FMC_CLK_src << RCC_D1CCIPR_FMCSEL_Pos)
	);
	RCC->D2CCIP1R = (
			(config->SWPMI_CLK_src << RCC_D2CCIP1R_SWPSEL_Pos)				|
			(config->FDCAN_CLK_src << RCC_D2CCIP1R_FDCANSEL_Pos)			|
			(config->DFSDM1_CLK_src << RCC_D2CCIP1R_DFSDM1SEL_Pos)			|
			(config->SPDIFRX_CLK_src << RCC_D2CCIP1R_SPDIFSEL_Pos)			|
			(config->SPI45_CLK_src << RCC_D2CCIP1R_SPI45SEL_Pos)			|
			(config->SPI123_CLK_src << RCC_D2CCIP1R_SPI123SEL_Pos)			|
			(config->SAI23_CLK_src << RCC_D2CCIP1R_SAI23SEL_Pos)			|
			(config->SAI1_CLK_src << RCC_D2CCIP1R_SAI1SEL_Pos)
	);
	RCC->D2CCIP2R = (
			(config->LPTIM1_CLK_src << RCC_D2CCIP2R_LPTIM1SEL_Pos)			|
			(config->HDMI_CEC_CLK_src << RCC_D2CCIP2R_CECSEL_Pos)			|
			(config->USB_OTG12_CLK_src << RCC_D2CCIP2R_USBSEL_Pos)			|
			(config->I2C123_CLK_src << RCC_D2CCIP2R_I2C123SEL_Pos)			|
			(config->RNG_CLK_src << RCC_D2CCIP2R_RNGSEL_Pos)				|
			(config->USART16_CLK_src << RCC_D2CCIP2R_USART16SEL_Pos)		|
			(config->USART234578_CLK_src << RCC_D2CCIP2R_USART28SEL_Pos)
	);
	RCC->D3CCIPR = (
			(config->SPI6_CLK_src << RCC_D3CCIPR_SPI6SEL_Pos)				|
			(config->SAI4B_CLK_src << RCC_D3CCIPR_SAI4BSEL_Pos)				|
			(config->SAI4A_CLK_src << RCC_D3CCIPR_SAI4ASEL_Pos)				|
			(config->SAR_ADC_CLK_src << RCC_D3CCIPR_ADCSEL_Pos)				|
			(config->LPTIM345_CLK_src << RCC_D3CCIPR_LPTIM345SEL_Pos)		|
			(config->LPTIM2_CLK_src << RCC_D3CCIPR_LPTIM2SEL_Pos)			|
			(config->I2C4_CLK_src << RCC_D3CCIPR_I2C4SEL_Pos)				|
			(config->LPUART1_CLK_src << RCC_D3CCIPR_LPUART1SEL_Pos)
	);
	/*!< enable clocks */
	RCC->CSR = (config->LSI_enable << RCC_CSR_LSION_Pos);
	while ((RCC->CSR & RCC_CSR_LSIRDY) != (config->LSI_enable << RCC_CSR_LSIRDY_Pos));
	RCC->BDCR = (
			(config->RTC_enable << RCC_BDCR_RTCEN_Pos)						|
			(config->RTC_src << RCC_BDCR_RTCSEL_Pos)						|
			(config->LSE_CSS_enable << RCC_BDCR_LSECSSON_Pos)				|
			(config->LSE_enable << RCC_BDCR_LSEON_Pos)
	);
	while ((RCC->BDCR & RCC_BDCR_LSERDY) != (config->LSE_enable << RCC_BDCR_LSERDY_Pos));
	RCC->CR = (
			// enable clocks
			(config->PLL3_config.enable << RCC_CR_PLL3ON_Pos)				|
			(config->PLL2_config.enable << RCC_CR_PLL2ON_Pos)				|
			(config->PLL1_config.enable << RCC_CR_PLL1ON_Pos)				|
			(config->HSI_enable << RCC_CR_HSION_Pos)						|
			(config->HSE_enable << RCC_CR_HSEON_Pos)						|
			(config->CSI_enable << RCC_CR_CSION_Pos)						|
			(config->HSI48_enable << RCC_CR_HSI48ON_Pos)					|
			// other settings
			(config->HSE_CSS_enable << RCC_CR_CSSHSEON_Pos)					|
			(config->HSI_enable_stop_mode << RCC_CR_HSIKERON_Pos)			|
			(config->CSI_enable_stop_mode << RCC_CR_CSIKERON_Pos)
	);
	uint32_t clock_ready_mask = (
			(config->PLL3_config.enable << RCC_CR_PLL3RDY_Pos)				|
			(config->PLL2_config.enable << RCC_CR_PLL2RDY_Pos)				|
			(config->PLL1_config.enable << RCC_CR_PLL1RDY_Pos)				|
			(config->HSI_enable << RCC_CR_HSIRDY_Pos)						|
			(config->HSE_enable << RCC_CR_HSERDY_Pos)						|
			(config->CSI_enable << RCC_CR_CSIRDY_Pos)						|
			(config->HSI48_enable << RCC_CR_HSI48RDY_Pos)
	);
	while ((RCC->CR & clock_ready_mask) != clock_ready_mask);	// wait until all enabled clocks are ready

	/*!< switch sys-clock */
	RCC->CFGR = (
			(config->MCO2_CLK_src << RCC_CFGR_MCO2_Pos)						|
			(config->MCO2_CLK_prescaler << RCC_CFGR_MCO2PRE_Pos)			|
			(config->MCO1_CLK_src << RCC_CFGR_MCO1_Pos)						|
			(config->MCO1_CLK_prescaler << RCC_CFGR_MCO1PRE_Pos)			|
			(config->TIM_prescaler << RCC_CFGR_TIMPRE_Pos)					|
			(config->HRTIM_src << RCC_CFGR_HRTIMSEL_Pos)					|
			(config->RTC_HSE_prescaler << RCC_CFGR_RTCPRE_Pos)				|
			(config->SYS_CLK_src << RCC_CFGR_SW_Pos)  // switch sys clock
	);
	while ((RCC->CFGR & RCC_CFGR_SWS) != (config->SYS_CLK_src << RCC_CFGR_SWS_Pos));	// wait until the sys clock is switched

	/*!< enable sys-tick */
	SysTick->LOAD = ((SYS_clock_frequency / (1 + (7 * config->SYSTICK_CLK_src))) / 1000) - 1;
	SysTick->VAL  = 0;
	SysTick->CTRL = (
			(config->SYSTICK_enable << SysTick_CTRL_ENABLE_Pos)				|
			(config->SYSTICK_IRQ_enable << SysTick_CTRL_TICKINT_Pos)		|
			(config->SYSTICK_CLK_src << SysTick_CTRL_CLKSOURCE_Pos)
	);
	if (config->SYSTICK_IRQ_enable) { SCB->SHPR[(SysTick_IRQn & 0xFUL) - 4UL] = 0xF0UL; }	// set SysTick irq priority
}
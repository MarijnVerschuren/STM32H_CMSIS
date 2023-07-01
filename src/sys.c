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
	/*!< configure domain pre-scalars */  // TODO!!!!
	RCC->D1CFGR = (
			0
	);
	RCC->D2CFGR = (
			0
	);
	RCC->D3CFGR = (
			0
	);
	/*!< configure peripheral kernel clocks */
	RCC->D1CCIPR = (
			0
	);
	RCC->D2CCIP1R = (
			0
	);
	RCC->D2CCIP2R = (
			0
	);
	RCC->D3CCIPR = (
			0
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

	RCC->CFGR = (
			(config->TIM_prescaler << RCC_CFGR_TIMPRE_Pos)					|
			(config->HRTIM_src << RCC_CFGR_HRTIMSEL_Pos)					|
			(config->RTC_HSE_prescaler << RCC_CFGR_RTCPRE_Pos)				|
			(config->SYS_CLK_src << RCC_CFGR_SW_Pos)  // switch sys clock
	);
	while ((RCC->CFGR & RCC_CFGR_SWS) != (config->SYS_CLK_src << RCC_CFGR_SWS_Pos));	// wait until the sys clock is switched

	// TODO: peripheral clocks (D1CKRDY, D2CKRDY)
}
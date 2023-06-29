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
	HSE_clock_frequency = config->HSE_freq;

	// TODO: config HSI before PLL because it can be used as its source

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
	// PLL settings
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
	// update PLL variables
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

	// TODO: SYS, RTC clocks (all things seen in the RCC diagram first)
	// TODO: peripheral clocks
}
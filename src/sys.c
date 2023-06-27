//
// Created by marijn on 6/26/23.
//

#include "sys.h"


/*!< init */
void sys_clock_init(SYS_CLK_Config_t* config) {
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
			(config->PLL1_config.R_enable << RCC_PLLCFGR_DIVP1EN_Pos)		|
			(config->PLL1_config.input_range << RCC_PLLCFGR_PLL1RGE_Pos)	|
			(config->PLL1_config.VCO_range << RCC_PLLCFGR_PLL1VCOSEL_Pos)	|
			(config->PLL1_config.frac_enable << RCC_PLLCFGR_PLL1FRACEN_Pos)	|
			// PLL2
			(config->PLL2_config.R_enable << RCC_PLLCFGR_DIVR2EN_Pos)		|
			(config->PLL2_config.Q_enable << RCC_PLLCFGR_DIVQ2EN_Pos)		|
			(config->PLL2_config.R_enable << RCC_PLLCFGR_DIVP2EN_Pos)		|
			(config->PLL2_config.input_range << RCC_PLLCFGR_PLL2RGE_Pos)	|
			(config->PLL2_config.VCO_range << RCC_PLLCFGR_PLL2VCOSEL_Pos)	|
			(config->PLL2_config.frac_enable << RCC_PLLCFGR_PLL2FRACEN_Pos)	|
			// PLL3
			(config->PLL3_config.R_enable << RCC_PLLCFGR_DIVR3EN_Pos)		|
			(config->PLL3_config.Q_enable << RCC_PLLCFGR_DIVQ3EN_Pos)		|
			(config->PLL3_config.R_enable << RCC_PLLCFGR_DIVP3EN_Pos)		|
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


}
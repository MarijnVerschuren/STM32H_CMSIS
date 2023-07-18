//
// Created by marijn on 6/26/23.
//

#include "sys.h"


/*!<
 * static variables
 * */
static IRQ_callback_t	sys_tick_func =		NULL;
static IRQ_callback_t	clock_fault_func =	NULL;


/*!<
 * constants
 * */
const uint32_t LSI_clock_frequency = 32768;
const uint32_t LSE_clock_frequency = 32768;
const uint32_t CSI_clock_frequency = 4000000;


/*!<
 * variables
 * */
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
uint32_t AHB_clock_frequency = 64000000;
uint32_t APB1_clock_frequency = 64000000;
uint32_t APB2_clock_frequency = 64000000;
uint32_t APB3_clock_frequency = 64000000;
uint32_t APB4_clock_frequency = 64000000;
uint32_t RTC_clock_frequency = 0;
uint32_t SYS_clock_frequency = 64000000;

volatile uint64_t tick = 0;  // updated with sys_tick


/*!<
 * interrupts
 * */
extern void SysTick_Handler(void) { tick++; if (sys_tick_func) { sys_tick_func(); } }
extern void RCC_IRQHandler(void) {
	if (!(RCC->CIFR & (RCC_CIFR_HSECSSF | RCC_CIFR_LSECSSF))) { RCC->CICR = 0x1FFUL; return; }
	if (clock_fault_func) { clock_fault_func(); }
	for(;;);  // halt cpu when clock failure is detected on HSE or LSE
}


/*!<
 * config functions
 * */
SYS_CLK_Config_t* new_SYS_CLK_config(void) {
	SYS_CLK_Config_t* config = malloc(sizeof(SYS_CLK_Config_t));
	for (uint8_t i = 0; i < sizeof(SYS_CLK_Config_t); i++) { ((uint8_t*)config)[i] = 0x00UL; }
	// set default settings
	config->HSI_enable = 1;
	config->PLL3_config.M_factor = config->PLL2_config.M_factor = config->PLL1_config.M_factor = 0x20UL;
	config->PLL3_config.N_factor = config->PLL2_config.N_factor = config->PLL1_config.N_factor = 0x080UL;
	config->PLL3_config.R_enable = config->PLL3_config.Q_enable = config->PLL3_config.P_enable = 0b1UL;
	config->PLL3_config.R_factor = config->PLL3_config.Q_factor = config->PLL3_config.P_factor = 0x01UL;
	config->PLL2_config.R_enable = config->PLL2_config.Q_enable = config->PLL2_config.P_enable = 0b1UL;
	config->PLL2_config.R_factor = config->PLL2_config.Q_factor = config->PLL2_config.P_factor = 0x01UL;
	config->PLL1_config.R_enable = config->PLL1_config.Q_enable = config->PLL1_config.P_enable = 0b1UL;
	config->PLL1_config.R_factor = config->PLL1_config.Q_factor = config->PLL1_config.P_factor = 0x01UL;
	config->FLASH_latency = FLASH_LATENCY1; config->CORE_VOS_level = CORE_VOS_3; return config;
}

void set_PLL_config(
	PLL_CLK_Config_t* config,				uint8_t enable,							uint8_t P_enable,
	uint8_t Q_enable,						uint8_t R_enable,						uint8_t frac_enable,
	PLL_IN_t input_range,					PLL_VCO_t VCO_range,					uint8_t M_factor,
	uint8_t P_factor,						uint8_t Q_factor,						uint8_t R_factor,
	uint16_t N_factor,						uint16_t N_fraction
) {
	config->enable =		enable;
	config->P_enable =		P_enable;
	config->Q_enable =		Q_enable;
	config->R_enable =		R_enable;
	config->frac_enable =	frac_enable;
	config->input_range =	input_range;
	config->VCO_range =		VCO_range;
	config->M_factor =		M_factor;
	config->P_factor =		P_factor ? P_factor - 1 : 0b0UL;
	config->Q_factor =		Q_factor ? Q_factor - 1 : 0b0UL;
	config->R_factor =		R_factor ? R_factor - 1 : 0b0UL;
	config->N_factor =		N_factor ? N_factor - 1 : 0b0UL;
	config->N_fraction =	N_fraction;
}

void set_RTC_config(
	SYS_CLK_Config_t* config,				uint8_t RTC_enable,
	RTC_SRC_t RTC_src,						uint8_t RTC_HSE_prescaler
) {
	config->RTC_enable =		RTC_enable;
	config->RTC_src =			RTC_src;
	config->RTC_HSE_prescaler =	RTC_HSE_prescaler;
}

void set_clock_config(
	SYS_CLK_Config_t* config,				uint8_t HSI_enable,						uint8_t HSE_enable,
	uint8_t LSI_enable,						uint8_t LSE_enable,						uint8_t CSI_enable,
	uint8_t HSI48_enable,					uint8_t HSI_enable_stop_mode,			uint8_t CSI_enable_stop_mode,
	uint8_t HSE_CSS_enable,					uint8_t LSE_CSS_enable,					HSI_DIV_t HSI_div,
	uint32_t HSE_freq,						PLL_SRC_t PLL_src
) {
	config->HSI_enable =			HSI_enable;
	config->HSE_enable =			HSE_enable;
	config->LSI_enable =			LSI_enable;
	config->LSE_enable =			LSE_enable;
	config->CSI_enable =			CSI_enable;
	config->HSI48_enable =			HSI48_enable;
	config->HSI_enable_stop_mode =	HSI_enable_stop_mode;
	config->CSI_enable_stop_mode =	CSI_enable_stop_mode;
	config->HSE_CSS_enable =		HSE_CSS_enable;
	config->LSE_CSS_enable =		LSE_CSS_enable;
	config->HSI_div =				HSI_div;
	config->HSE_freq =				HSE_freq;
	config->PLL_src =				PLL_src;
}

void set_SYS_config(
	SYS_CLK_Config_t* config,				SYS_CLK_SRC_t SYS_CLK_src,				SYS_CLK_PRE_t SYS_CLK_prescaler,
	CORE_VOS_t CORE_VOS_level,				FLASH_LATENCY_t FLASH_latency
) {
	config->SYS_CLK_src =			SYS_CLK_src;
	config->SYS_CLK_prescaler =		SYS_CLK_prescaler;
	config->CORE_VOS_level =		CORE_VOS_level;
	config->FLASH_latency =			FLASH_latency;
}

void set_domain_config(
	SYS_CLK_Config_t* config,				AHB_CLK_PRE_t AHB_prescaler,			APB_CLK_PRE_t APB1_prescaler,
	APB_CLK_PRE_t APB2_prescaler,			APB_CLK_PRE_t APB3_prescaler,			APB_CLK_PRE_t APB4_prescaler
) {
	config->AHB_prescaler =		AHB_prescaler;
	config->APB1_prescaler =	APB1_prescaler;
	config->APB2_prescaler =	APB2_prescaler;
	config->APB3_prescaler =	APB3_prescaler;
	config->APB4_prescaler =	APB4_prescaler;
}

void set_systick_config(
	SYS_CLK_Config_t* config,				uint8_t SYSTICK_enable,
	uint8_t SYSTICK_IRQ_enable,				SYSTICK_CLK_SRC_t SYSTICK_CLK_src
) {
	config->SYSTICK_enable =		SYSTICK_enable;
	config->SYSTICK_IRQ_enable =	SYSTICK_IRQ_enable;
	config->SYSTICK_CLK_src =		SYSTICK_CLK_src;
}


/*!<
 * init
 * */
void IRQ_callback_init(IRQ_callback_t sys_tick_callback, IRQ_callback_t clock_fault_callback) {
	sys_tick_func =		sys_tick_callback;
	clock_fault_func =	clock_fault_callback;
}

void peripheral_kernel_clock_init(SYS_CLK_Config_t* config) {
	//!< configure peripheral kernel clocks
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
}

void sys_clock_init(SYS_CLK_Config_t* config) {
	uint32_t PLL_src_freq = 0;
	uint32_t PLL1_freq, PLL2_freq, PLL3_freq;
	uint32_t clock_ready_mask;
	uint32_t tmp_reg;
	//!< update base clock frequency variables
	HSE_clock_frequency = config->HSE_freq;
	HSI_clock_frequency /= (0b1UL << config->HSI_div);
	//!< check if current VOS level is stable
	while (!(PWR->D3CR & PWR_D3CR_VOSRDY));
	//!< configure CORE voltage scaling
	PWR->CR3 |= PWR_CR3_BYPASS;
	PWR->D3CR |= (config->CORE_VOS_level << PWR_D3CR_VOS_Pos);
	while ((PWR->CSR1 & PWR_CSR1_ACTVOS) != (config->CORE_VOS_level << PWR_CSR1_ACTVOS_Pos));  // wait until the power scaling level is applied
	//!< enable base clocks
	RCC->CR = (  // HSI is left on
			// enable clocks
			(config->HSE_enable << RCC_CR_HSEON_Pos)						|
			(config->CSI_enable << RCC_CR_CSION_Pos)						|
			(config->HSI48_enable << RCC_CR_HSI48ON_Pos)					|
			RCC_CR_HSION  /* keep HSI on until clock switch */				|
			// other settings
			(config->HSE_CSS_enable << RCC_CR_CSSHSEON_Pos)					|
			(config->HSI_enable_stop_mode << RCC_CR_HSIKERON_Pos)			|
			(config->CSI_enable_stop_mode << RCC_CR_CSIKERON_Pos)
	);
	clock_ready_mask = (
			(config->HSI_enable << RCC_CR_HSIRDY_Pos)						|
			(config->HSE_enable << RCC_CR_HSERDY_Pos)						|
			(config->CSI_enable << RCC_CR_CSIRDY_Pos)						|
			(config->HSI48_enable << RCC_CR_HSI48RDY_Pos)					|
			(config->HSE_CSS_enable << RCC_CR_CSSHSEON_Pos)
	);
	while ((RCC->CR & clock_ready_mask) != clock_ready_mask);	// wait until all enabled basic clocks are ready
	//!< disable PLL clocks before configuring
	if (RCC->CR & RCC_CR_PLL3ON) { RCC->CR &= ~RCC_CR_PLL3ON; while (RCC->CR & RCC_CR_PLL3RDY); }
	if (RCC->CR & RCC_CR_PLL2ON) { RCC->CR &= ~RCC_CR_PLL2ON; while (RCC->CR & RCC_CR_PLL2RDY); }
	if (RCC->CR & RCC_CR_PLL1ON) { RCC->CR &= ~RCC_CR_PLL1ON; while (RCC->CR & RCC_CR_PLL1RDY); }
	//!< configure PLL clocks
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
	//!< update PLL frequency variables
	switch (config->PLL_src) {
		case PLL_SRC_HSI: PLL_src_freq = HSI_clock_frequency; break;
		case PLL_SRC_CSI: PLL_src_freq = CSI_clock_frequency; break;
		case PLL_SRC_HSE: PLL_src_freq = HSE_clock_frequency; break;
	}
	PLL1_freq = PLL_src_freq * (config->PLL1_config.N_factor + 1);
	PLL2_freq = PLL_src_freq * (config->PLL2_config.N_factor + 1);
	PLL3_freq = PLL_src_freq * (config->PLL3_config.N_factor + 1);
	if (config->PLL1_config.P_enable) { PLL1_P_clock_frequency = PLL1_freq / (config->PLL1_config.P_factor + 1); }
	if (config->PLL1_config.Q_enable) { PLL1_Q_clock_frequency = PLL1_freq / (config->PLL1_config.Q_factor + 1); }
	if (config->PLL1_config.R_enable) { PLL1_R_clock_frequency = PLL1_freq / (config->PLL1_config.R_factor + 1); }
	if (config->PLL2_config.P_enable) { PLL2_P_clock_frequency = PLL2_freq / (config->PLL2_config.P_factor + 1); }
	if (config->PLL2_config.Q_enable) { PLL2_Q_clock_frequency = PLL2_freq / (config->PLL2_config.Q_factor + 1); }
	if (config->PLL2_config.R_enable) { PLL2_R_clock_frequency = PLL2_freq / (config->PLL2_config.R_factor + 1); }
	if (config->PLL3_config.P_enable) { PLL3_P_clock_frequency = PLL3_freq / (config->PLL3_config.P_factor + 1); }
	if (config->PLL3_config.Q_enable) { PLL3_Q_clock_frequency = PLL3_freq / (config->PLL3_config.Q_factor + 1); }
	if (config->PLL3_config.R_enable) { PLL3_R_clock_frequency = PLL3_freq / (config->PLL3_config.R_factor + 1); }
	switch (config->SYS_CLK_src) {
		case SYS_CLK_SRC_HSI: SYS_clock_frequency = HSI_clock_frequency; break;
		case SYS_CLK_SRC_CSI: SYS_clock_frequency = CSI_clock_frequency; break;
		case SYS_CLK_SRC_HSE: SYS_clock_frequency = HSE_clock_frequency; break;
		case SYS_CLK_SRC_PLL1_P: SYS_clock_frequency = PLL1_P_clock_frequency; break;
	}
	//!< enable PLL clocks
	RCC->CR |= (
			(config->PLL3_config.enable << RCC_CR_PLL3ON_Pos)				|
			(config->PLL2_config.enable << RCC_CR_PLL2ON_Pos)				|
			(config->PLL1_config.enable << RCC_CR_PLL1ON_Pos)
	);
	clock_ready_mask = (
			(config->PLL3_config.enable << RCC_CR_PLL3RDY_Pos)				|
			(config->PLL2_config.enable << RCC_CR_PLL2RDY_Pos)				|
			(config->PLL1_config.enable << RCC_CR_PLL1RDY_Pos)
	);
	while ((RCC->CR & clock_ready_mask) != clock_ready_mask);	// wait until all enabled PLL clocks are ready
	//!< configure domain pre-scalars
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
	//!< update frequency variables
	if (config->SYS_CLK_prescaler & 0x8UL) {
		if (config->SYS_CLK_prescaler & 0x4UL)	{ SYS_clock_frequency /= 0x4UL << (config->SYS_CLK_prescaler & 0x7UL); }
		else									{ SYS_clock_frequency /= 0x2UL << (config->SYS_CLK_prescaler & 0x7UL); }
	} AHB_clock_frequency = SYS_clock_frequency;
	if (config->AHB_prescaler & 0x8UL) {
		if (config->AHB_prescaler & 0x4UL)		{ AHB_clock_frequency = SYS_clock_frequency / (0x4UL << (config->AHB_prescaler & 0x7UL)); }
		else									{ AHB_clock_frequency = SYS_clock_frequency / (0x2UL << (config->AHB_prescaler & 0x7UL)); }
	} APB3_clock_frequency = APB2_clock_frequency = APB1_clock_frequency = APB4_clock_frequency = AHB_clock_frequency;
	if (config->APB3_prescaler & 0x4UL)			{ APB3_clock_frequency = AHB_clock_frequency / (0x2UL << (config->APB3_prescaler & 0x3UL)); }
	if (config->APB2_prescaler & 0x4UL)			{ APB2_clock_frequency = AHB_clock_frequency / (0x2UL << (config->APB2_prescaler & 0x3UL)); }
	if (config->APB1_prescaler & 0x4UL)			{ APB1_clock_frequency = AHB_clock_frequency / (0x2UL << (config->APB1_prescaler & 0x3UL)); }
	if (config->APB4_prescaler & 0x4UL)			{ APB4_clock_frequency = AHB_clock_frequency / (0x2UL << (config->APB4_prescaler & 0x3UL)); }
	//!< configure flash
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= (config->FLASH_latency << FLASH_ACR_LATENCY_Pos);
	//!< switch sys-clock
	RCC->CFGR = (
			(config->TIM_prescaler << RCC_CFGR_TIMPRE_Pos)					|
			(config->HRTIM_src << RCC_CFGR_HRTIMSEL_Pos)					|
			(config->RTC_HSE_prescaler << RCC_CFGR_RTCPRE_Pos)
	);
	RCC->CFGR |= (config->SYS_CLK_src << RCC_CFGR_SW_Pos);  // switch sys clock
	while ((RCC->CFGR & RCC_CFGR_SWS) != (config->SYS_CLK_src << RCC_CFGR_SWS_Pos));	// wait until the sys clock is switched
	tmp_reg = RCC->CR; tmp_reg &= !RCC_CR_HSION;
	tmp_reg |= config->HSI_enable << RCC_CR_HSION_Pos;
	RCC->CR = tmp_reg;
	//!< configure RTC
	RCC->CSR = (config->LSI_enable << RCC_CSR_LSION_Pos);
	while ((RCC->CSR & RCC_CSR_LSIRDY) != (config->LSI_enable << RCC_CSR_LSIRDY_Pos));
	RCC->BDCR = (
			(config->RTC_enable << RCC_BDCR_RTCEN_Pos)						|
			(config->RTC_src << RCC_BDCR_RTCSEL_Pos)						|
			(config->LSE_CSS_enable << RCC_BDCR_LSECSSON_Pos)				|
			(config->LSE_enable << RCC_BDCR_LSEON_Pos)
	);
	while ((RCC->BDCR & RCC_BDCR_LSERDY) != (config->LSE_enable << RCC_BDCR_LSERDY_Pos));
	RCC->CIER = (config->LSE_CSS_enable << RCC_CIER_LSECSSIE_Pos);
	//!< update RTC frequency variables
	switch (config->RTC_src) {
		case RTC_SRC_LSE:	RTC_clock_frequency = LSI_clock_frequency; break;
		case RTC_SRC_LSI:	RTC_clock_frequency = LSE_clock_frequency; break;
		case RTC_SRC_HSE:
			if (config->RTC_HSE_prescaler < 2) { break; }
			RTC_clock_frequency = HSE_clock_frequency / config->RTC_HSE_prescaler; break;
		default:			RTC_clock_frequency = 0; break;
	}
	//!< configure sys-tick
	SysTick->LOAD = ((SYS_clock_frequency / (8 - (7 * config->SYSTICK_CLK_src))) / 2000) + 1;
	SysTick->VAL  = 0;
	SysTick->CTRL = (
			(config->SYSTICK_enable << SysTick_CTRL_ENABLE_Pos)				|
			(config->SYSTICK_IRQ_enable << SysTick_CTRL_TICKINT_Pos)		|
			(config->SYSTICK_CLK_src << SysTick_CTRL_CLKSOURCE_Pos)
	);
	if (config->SYSTICK_IRQ_enable) { SCB->SHPR[(SysTick_IRQn & 0xFUL) - 4UL] = 0xF0UL; }	// set SysTick irq priority

	peripheral_kernel_clock_init(config);  // set peripheral kernel clock config
}


/*!<
 * misc
 * */
void delay_ms(uint64_t ms) {
	uint64_t start = tick;
	while ((tick - start) < ms);
}
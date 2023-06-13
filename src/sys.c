//
// Created by marijn on 3/1/23.
//
#include "sys.h"


/*!< shared variables */
uint32_t PLL_clock_frequency = 0;
uint32_t AHB_clock_frequency = 16000000;
uint32_t APB1_clock_frequency = 16000000;
uint32_t APB2_clock_frequency = 16000000;
uint32_t RTC_clock_frequency = 0;

uint32_t SYS_clock_frequency = 16000000;

volatile uint64_t tick = 0;


/*!< static variables */
static sys_tick_t tick_func = NULL;


/*!< interrupts */
void SysTick_Handler(void) { tick++; if (tick_func) { tick_func(); } }


/*!< init / enable / disable */
SYS_CLK_Config_t* new_SYS_CLK_config(void) {
	SYS_CLK_Config_t* config = malloc(sizeof(SYS_CLK_Config_t));
	// reset PLL config
	config->PLL_M = 1;
	config->PLL_N = 2;
	config->PLL_P = PLL_P_DIV2;
	config->PLL_Q = 0;					// disable
	config->PLL_source = PLL_SRC_HSI;	// 16 MHz
	// reset flash config
	config->FLASH_latency = FLASH_LATENCY1;
	config->FLASH_prefetch =			0;
	config->FLASH_instruction_cache =	0;
	config->FLASH_data_cache =			0;
	// reset sys clock config
	config->SYS_CLK_source = SYS_CLK_SRC_HSI;
	config->AHB_prescaler =				0;	// no div
	config->APB1_prescaler =			0;	// no div
	config->APB2_prescaler =			0;	// no div
	config->RTC_prescaler =				0;	// disable
	config->MCO1_source =				0;	// HSI
	config->MCO1_prescaler =			0;	// no div
	config->MCO2_prescaler =			0;	// no div
	config->MCO2_source =				0;	// SYS_CLK
	// disable sys tick
	config->SYS_tick_enable =			0;
	config->SYS_tick_interrupt_enable =	0;
	// setting additional (external info) to their default
	config->SYS_power = SYS_power_nominal;  // it is assumed that the power is nominal (this is used to determine flash delay)
	return config;
}
void set_SYS_PLL_config(SYS_CLK_Config_t* config, uint8_t M, uint16_t N, PLL_P_t P, uint8_t Q, PLL_Source_t PLL_src) {
	// PLL_freq = src / m * n / p
	config->PLL_M =			M & 0x3F;
	config->PLL_N =			N & 0x1FF;
	config->PLL_P =			P;
	config->PLL_Q =			Q;
	config->PLL_source =	PLL_src;
}
void set_SYS_FLASH_config(SYS_CLK_Config_t* config, FLASH_LATENCY_t latency, uint8_t prefetch, uint8_t enable_icache, uint8_t enable_dcache) {
	config->FLASH_latency =				latency;
	config->FLASH_prefetch =			prefetch != 0;
	config->FLASH_instruction_cache =	enable_icache != 0;
	config->FLASH_data_cache =			enable_dcache != 0;
}
void set_SYS_CLOCK_config(SYS_CLK_Config_t* config, SYS_CLK_Source_t SYS_src, AHB_CLK_Prescaler_t AHB_prescaler, APBx_CLK_Prescaler_t APB1_prescaler, APBx_CLK_Prescaler_t APB2_prescaler, uint8_t RTC_prescaler) {
	config->SYS_CLK_source =	SYS_src;
	config->AHB_prescaler =		AHB_prescaler;
	config->APB1_prescaler =	APB1_prescaler;
	config->APB2_prescaler =	APB2_prescaler;
	config->RTC_prescaler =		RTC_prescaler;
}
void set_SYS_MCO_config(SYS_CLK_Config_t* config, MCO1_CLK_Source_t MCO1_src, MCOx_CLK_Prescaler_t MCO1_prescaler, MCO2_CLK_Source_t MCO2_src, MCOx_CLK_Prescaler_t MCO2_prescaler) {
	config->MCO1_source =		MCO1_src;
	config->MCO1_prescaler =	MCO1_prescaler;
	config->MCO2_source =		MCO2_src;
	config->MCO2_prescaler =	MCO2_prescaler;
}
void set_SYS_tick_config(SYS_CLK_Config_t* config, uint8_t enable, uint8_t enable_irq, sys_tick_t tick_handler) {
	config->SYS_tick_enable =			enable;
	config->SYS_tick_interrupt_enable =	enable_irq;
	tick_func =							tick_handler;
}
void set_SYS_power_config(SYS_CLK_Config_t* config, SYS_Power_t power) {
	config->SYS_power = power;
}

void sys_clock_init(SYS_CLK_Config_t* config) {
	PLL_clock_frequency = ((16000000 + (9000000 * config->PLL_source)) / config->PLL_M) * (config->PLL_N / (2 * (config->PLL_P + 1)));
	// round PLL clock frequency to 1000
	if (PLL_clock_frequency % 1000) { PLL_clock_frequency += 1000; }
	PLL_clock_frequency -= PLL_clock_frequency % 1000;
	RCC->PLLCFGR = (																									/*
				PLL_M: division factor for the main PLL and audio PLL (PLLI2S) input clock. Info:
					the software has to set these bits correctly to ensure that the VCO
					input frequency ranges from 1 to 2 MHz. It is recommended to select
					a frequency of 2 MHz to limit PLL jitter.															*/
			((config->PLL_M << RCC_PLLCFGR_PLLM_Pos) & RCC_PLLCFGR_PLLM_Msk)	|										/*
				PLL_N: main PLL multiplication factor for VCO. Info:
					the software has to set these bits correctly to ensure that
					the VCO output frequency is between 192 and 432 MHz.												*/
			((config->PLL_N << RCC_PLLCFGR_PLLN_Pos) & RCC_PLLCFGR_PLLN_Msk)	|										/*
				PLL_P: main PLL division factor for main system clock
				PLL output clock frequency = VCO frequency / PLL_P:
					00: PLL_P = 2
					01: PLL_P = 4
					10: PLL_P = 6
					11: PLL_P = 8																						*/
			((config->PLL_P << RCC_PLLCFGR_PLLP_Pos) & RCC_PLLCFGR_PLLP_Msk)	|										/*
				PLLSRC: PLL and PLLI2S clock source:
 					0: HSI
					1: HSE																								*/
			(config->PLL_source * RCC_PLLCFGR_PLLSRC_HSE)						|										/*
 				PLL_Q: main PLL division factor for USB OTG FS, SDIO and RNG clocks:
 					0000: ERROR
 					0001: ERROR
 					0010: divide PLL by 2
 					...
 					1111: divide PLL by 15																				*/
			((config->PLL_Q << RCC_PLLCFGR_PLLQ_Pos) & RCC_PLLCFGR_PLLQ_Msk)
	);

	// turn on HSE and PLL
	RCC->CR = (
			RCC_CR_HSEON |	/* switch HSE ON */
			RCC_CR_PLLON	/* switch PLL ON */
	);

	// TODO: RTC
	//PWR->CR = PWR_CR_VOS_1 | PWR_CR_DBP; /*  Enable Backup Domain Access (leave VOS default)       */
	//RCC->BDCR = (
	//		RCC_BDCR_LSEON                   | /*  Switch HSE ON                                     */
	//		RCC_BDCR_RTCSEL_0                | /*  LSE oscillator clock used as RTC clock            */
	//		RCC_BDCR_RTCEN                     /*  RTC clock enable                                  */
	//);

	/* Relation between CPU clock frequency and flash memory read time.
	To correctly read data from flash memory, the number of wait states (LATENCY) must be
	correctly programmed in the flash access control register (FLASH_ACR) according to the
	frequency of the CPU clock (HCLK) and the supply voltage of the device.
	The prefetch buffer must be disabled when the supply voltage is below 2.1 V. The
	correspondence between wait states and CPU clock frequency is given in Table 6.
	- when VOS[1:0] = 0x01, the maximum frequency of HCLK = 60 MHz.
	- when VOS[1:0] = 0x10, the maximum frequency of HCLK = 84 MHz.
	*/

	// clock speeds are calculated here so that the flash delay calculation can use them
	switch (config->SYS_CLK_source) {
		case SYS_CLK_SRC_HSI:	SYS_clock_frequency = 16000000; break;
		case SYS_CLK_SRC_HSE:	SYS_clock_frequency = 25000000; break;
		case SYS_CLK_SRC_PLL:	SYS_clock_frequency = PLL_clock_frequency; break;
		default:				SYS_clock_frequency = 0; break;
	}
	if (config->AHB_prescaler & 0x8) {
		if (config->AHB_prescaler & 0x4)	{ AHB_clock_frequency = SYS_clock_frequency / (64 << (config->AHB_prescaler & 0x3)); }
		else								{ AHB_clock_frequency = SYS_clock_frequency / (2 << (config->AHB_prescaler & 0x3)); }
	} else									{ AHB_clock_frequency = SYS_clock_frequency; }
	if (config->APB1_prescaler & 0x4)		{ APB1_clock_frequency = SYS_clock_frequency / (2 << (config->APB1_prescaler & 0x3)); }
	else									{ APB1_clock_frequency = SYS_clock_frequency; }
	if (config->APB2_prescaler & 0x4)		{ APB2_clock_frequency = SYS_clock_frequency / (2 << (config->APB2_prescaler & 0x3)); }
	else									{ APB2_clock_frequency = SYS_clock_frequency; }
	RTC_clock_frequency = 0;
	if (config->RTC_prescaler > 1)			{ RTC_clock_frequency = SYS_clock_frequency / config->RTC_prescaler; }

	// round clock frequencies to 1000
	if (AHB_clock_frequency % 1000) { AHB_clock_frequency += 1000; }
	if (APB1_clock_frequency % 1000) { APB1_clock_frequency += 1000; }
	if (APB2_clock_frequency % 1000) { APB2_clock_frequency += 1000; }
	if (RTC_clock_frequency % 1000) { RTC_clock_frequency += 1000; }
	AHB_clock_frequency -= AHB_clock_frequency % 1000;
	APB1_clock_frequency -= APB1_clock_frequency % 1000;
	APB2_clock_frequency -= APB2_clock_frequency % 1000;
	RTC_clock_frequency -= RTC_clock_frequency % 1000;

	uint8_t flash_latency;
	switch (config->SYS_power) {
		case SYS_power_1v7:		flash_latency = SYS_clock_frequency / 16000000; break;
		case SYS_power_2v1:		flash_latency = SYS_clock_frequency / 18000000; break;
		case SYS_power_2v4:		flash_latency = SYS_clock_frequency / 24000000; break;
		case SYS_power_nominal:	flash_latency = SYS_clock_frequency / 30000000; break;
	}
	flash_latency = flash_latency > FLASH_LATENCY8 ? FLASH_LATENCY8: flash_latency;							// make sure that the latency is not larger than the max
	config->FLASH_latency = config->FLASH_latency > flash_latency ? config->FLASH_latency: flash_latency;	// use the max latency

	FLASH->ACR = (																										/*
 				LATENCY: flash read latency (see the table at the definition for ACR_LATENCY_TypeDef for more info)		*/
			(config->FLASH_latency << FLASH_ACR_LATENCY_Pos)							|
			(config->FLASH_prefetch * FLASH_ACR_PRFTEN)									|	/* enable prefetch			*/
			(config->FLASH_instruction_cache * FLASH_ACR_ICEN)							|	/* enable instruction cache	*/
			(config->FLASH_data_cache * FLASH_ACR_DCEN)										/* enable data cache		*/
	);

	while(!(RCC->CR & RCC_CR_HSERDY)) { /* wait till HSE is ready */ }
	while(!(RCC->CR & RCC_CR_PLLRDY)) { /* wait till PLL is ready */ }

	RCC->CFGR = (																										/*
 				SW: system clock switch:
 					00: HSI
 					01: HSE
 					10: PLL
 					11: ERROR																							*/
			((config->SYS_CLK_source << RCC_CFGR_SW_Pos) & RCC_CFGR_SW_Msk)				|								/*
 				HPRE: AHB prescaler. Info:
					the clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after HPRE write.
					0xxx: clock not divided
					1000: clock divided by 2
					1001: clock divided by 4
					1010: clock divided by 8
					1011: clock divided by 16
					1100: clock divided by 64
					1101: clock divided by 128
					1110: clock divided by 256
					1111: clock divided by 512																			*/
			((config->AHB_prescaler << RCC_CFGR_HPRE_Pos) & RCC_CFGR_HPRE_Msk)			|								/*
				PPRE1: APB1 prescaler:
					0xx: clock not divided
					1xx: clock divided by (2 << xx)																		*/
			((config->APB1_prescaler << RCC_CFGR_PPRE1_Pos) & RCC_CFGR_PPRE1_Msk)		|								/*
				PPRE2: APB2 prescaler:
					0xx: clock not divided
					1xx: clock divided by (2 << xx)																		*/
			((config->APB2_prescaler << RCC_CFGR_PPRE2_Pos) & RCC_CFGR_PPRE2_Msk)		|								/*
 				RTCPRE: HSE prescaler for RTC:
 					00000: no clock
 					00001: no clock
 					00010: HSE / 2
 					...
 					11111: HSE / 31																						*/
			((config->RTC_prescaler << RCC_CFGR_RTCPRE_Pos) & RCC_CFGR_RTCPRE_Msk)		|								/*
 				MCO1: micro controller clock output 1:
 					00: HSI
 					01: LSE
 					10: HSE
 					11: PLL																								*/
			((config->MCO1_source << RCC_CFGR_MCO1_Pos) & RCC_CFGR_MCO1_Msk)			|								/*
 				MCO1PRE: MCO1 prescaler:
 					0xx: no division
 					1xx: clock divided by (2 + xx)																		*/
			((config->MCO1_prescaler << RCC_CFGR_MCO1PRE_Pos) & RCC_CFGR_MCO1PRE_Msk)	|								/*
 				MCO2PRE: MCO2 prescaler:
 					0xx: no division
 					1xx: clock divided by (2 + xx)																		*/
			((config->MCO2_prescaler << RCC_CFGR_MCO2PRE_Pos) & RCC_CFGR_MCO2PRE_Msk)	|								/*
 				MCO2: micro controller clock output 2:
 					00: SYSCLK
 					01: PLLI2S
 					10: HSE
 					11: PLL																								*/
			((config->MCO2_source << RCC_CFGR_MCO2_Pos) & RCC_CFGR_MCO2_Msk)
	);

	while((RCC->CFGR & RCC_CFGR_SWS) != ((config->SYS_CLK_source << RCC_CFGR_SWS_Pos) & RCC_CFGR_SWS_Msk)) { /* wait untill the selected clock is enabled*/ }

	/* configure SysTick timer. By default the clock source of SysTick is AHB/8 */
	SysTick->LOAD = (AHB_clock_frequency / 8000) - 1;						/* set reload register */
	SysTick->VAL  = 0;														/* load counter value  */
	SysTick->CTRL = (														/* start SysTick timer */
			(SysTick_CTRL_ENABLE_Msk * config->SYS_tick_enable)				|
			(SysTick_CTRL_TICKINT_Msk * config->SYS_tick_interrupt_enable)
	);
	// set IRQ priority
	SCB->SHP[(SysTick_IRQn & 0xFUL) - 4UL] = ((((1UL << __NVIC_PRIO_BITS) - 1UL) << (8U - __NVIC_PRIO_BITS)) & 0xFFUL);
}


/*!< misc */
void delay_ms(uint64_t ms) {
	uint64_t start = tick;
	while ((tick - start) < ms) {}
}
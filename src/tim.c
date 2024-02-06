//
// Created by marijn on 7/6/23.
//

#include "tim.h"


/*!<
 * variables
 * */
uint32_t TIM_APB1_kernel_frequency =	0;
uint32_t TIM_APB2_kernel_frequency =	0;
uint32_t HRTIM_kernel_frequency =		0;
uint32_t LPTIM1_kernel_frequency =		0;
uint32_t LPTIM2_kernel_frequency =		0;
uint32_t LPTIM345_kernel_frequency =	0;


/*!<
 * misc
 * */
uint8_t TIM_to_IRQn(TIM_TypeDef* tim) {
	if (tim == TIM1)	{ return TIM1_UP_IRQn; }
	if (tim == TIM5)	{ return TIM5_IRQn; }
	if (tim == TIM8)	{ return TIM8_UP_TIM13_IRQn; }
	uint8_t num = dev_to_int(tim);
	if (num < 3)		{ return 28 + num; }	// TIM2_IRQn, TIM3_IRQn, TIM4_IRQn
	if (num < 6)		{ return 50 + num; }	// TIM6_DAC_IRQn, TIM7_IRQn
	if (num < 9)		{ return 37 + num; }	// TIM8_BRK_TIM12_IRQn, TIM8_UP_TIM13_IRQn, TIM8_TRG_COM_TIM14_IRQn
	if (num < 19)		{ return 100 + num; }	// TIM15_IRQn, TIM16_IRQn, TIM17_IRQn
	return -1;  // error
}


/*!<
 * init
 * */
void config_TIM_kernel_clocks(
	TIM_PRE_t prescaler, HRTIM_SRC_t hrtim_src, LPTIM_CLK_SRC_t lptim1_src,
	LPTIM_CLK_SRC_t lptim2_src, LPTIM_CLK_SRC_t lptim345_src
) {
	RCC->CFGR &= ~(
		RCC_CFGR_TIMPRE	|
		RCC_CFGR_HRTIMSEL
	);
	RCC->CFGR |= (
		(prescaler << RCC_CFGR_TIMPRE_Pos) |
		(hrtim_src << RCC_CFGR_HRTIMSEL_Pos)
	);
	RCC->D2CCIP2R &= ~RCC_D2CCIP2R_LPTIM1SEL;
	RCC->D2CCIP2R |= lptim1_src << RCC_D2CCIP2R_LPTIM1SEL_Pos;
	RCC->D3CCIPR &= ~(
		RCC_D3CCIPR_LPTIM2SEL |
		RCC_D3CCIPR_LPTIM345SEL
	);
	RCC->D3CCIPR |= (
		(lptim2_src << RCC_D3CCIPR_LPTIM2SEL_Pos)	|
		(lptim345_src << RCC_D3CCIPR_LPTIM345SEL_Pos)
	);
	if (APB1_clock_frequency >= (AHB_clock_frequency / 2))	{ TIM_APB1_kernel_frequency = AHB_clock_frequency; }
	else													{ TIM_APB1_kernel_frequency = APB1_clock_frequency * (2 << prescaler); }
	if (APB2_clock_frequency >= (AHB_clock_frequency / 2))	{ TIM_APB2_kernel_frequency = AHB_clock_frequency; }
	else													{ TIM_APB2_kernel_frequency = APB2_clock_frequency * (2 << prescaler); }
	switch (hrtim_src) {
		case HRTIM_SRC_APB2:			HRTIM_kernel_frequency = APB2_clock_frequency; break;
		case HRTIM_SRC_CPU:				HRTIM_kernel_frequency = SYS_clock_frequency; break;
	}
	switch (lptim1_src) {
		case LPTIM_CLK_SRC_APBx:	LPTIM1_kernel_frequency = APB1_clock_frequency; break;		// APB1
		case LPTIM_CLK_SRC_PLL2_P:	LPTIM1_kernel_frequency = PLL2_P_clock_frequency; break;
		case LPTIM_CLK_SRC_PLL3_R:	LPTIM1_kernel_frequency = PLL3_R_clock_frequency; break;
		case LPTIM_CLK_SRC_LSE:		LPTIM1_kernel_frequency = LSE_clock_frequency; break;
		case LPTIM_CLK_SRC_LSI:		LPTIM1_kernel_frequency = LSI_clock_frequency; break;
		case LPTIM_CLK_SRC_PER:		LPTIM1_kernel_frequency = PER_clock_frequency; break;
	}
	switch (lptim2_src) {
		case LPTIM_CLK_SRC_APBx:	LPTIM2_kernel_frequency = APB4_clock_frequency; break;		// APB4
		case LPTIM_CLK_SRC_PLL2_P:	LPTIM2_kernel_frequency = PLL2_P_clock_frequency; break;
		case LPTIM_CLK_SRC_PLL3_R:	LPTIM2_kernel_frequency = PLL3_R_clock_frequency; break;
		case LPTIM_CLK_SRC_LSE:		LPTIM2_kernel_frequency = LSE_clock_frequency; break;
		case LPTIM_CLK_SRC_LSI:		LPTIM2_kernel_frequency = LSI_clock_frequency; break;
		case LPTIM_CLK_SRC_PER:		LPTIM2_kernel_frequency = PER_clock_frequency; break;
	}
	switch (lptim345_src) {
		case LPTIM_CLK_SRC_APBx:	LPTIM345_kernel_frequency = APB4_clock_frequency; return;	// APB4
		case LPTIM_CLK_SRC_PLL2_P:	LPTIM345_kernel_frequency = PLL2_P_clock_frequency; return;
		case LPTIM_CLK_SRC_PLL3_R:	LPTIM345_kernel_frequency = PLL3_R_clock_frequency; return;
		case LPTIM_CLK_SRC_LSE:		LPTIM345_kernel_frequency = LSE_clock_frequency; return;
		case LPTIM_CLK_SRC_LSI:		LPTIM345_kernel_frequency = LSI_clock_frequency; return;
		case LPTIM_CLK_SRC_PER:		LPTIM345_kernel_frequency = PER_clock_frequency; return;
	}
}

void fconfig_TIM(
	TIM_TypeDef* tim,				uint16_t prescaler,		uint32_t limit,
	uint8_t auto_reload_preload,	uint8_t down_count,		uint8_t one_pulse
){
	uint32_t mask = TIM2 >= tim && tim <= TIM5 ? 0xffffffff : 0xffff;  // 32 | 16 bit timers
	enable_dev(tim);
	tim->PSC = prescaler;
	tim->ARR = limit & mask;
	tim->CR1 &= ~TIM_CR1_ARPE;
	tim->CR1 &= ~TIM_CR1_DIR;
	tim->CR1 &= ~TIM_CR1_OPM;
	if (auto_reload_preload)	{ tim->CR1 |= TIM_CR1_ARPE; }
	if (down_count)				{ tim->CR1 |= TIM_CR1_DIR; }
	if (one_pulse)				{ tim->CR1 |= TIM_CR1_OPM; }
}
void config_TIM(TIM_TypeDef* tim, uint16_t prescaler, uint32_t limit) {
	fconfig_TIM(tim, prescaler, limit, 0, 0, 0);
}


/*!<
 * actions
 * */
void start_TIM(TIM_TypeDef* tim)	{ tim->CR1 |= TIM_CR1_CEN; }
void stop_TIM(TIM_TypeDef* tim)		{ tim->CR1 &= ~TIM_CR1_CEN; }



/*!<
 * irq
 * */
void start_TIM_update_irq(TIM_TypeDef* tim) {
	uint8_t irqn = TIM_to_IRQn(tim);
	NVIC->ISER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_EnableIRQ
	tim->DIER |= TIM_DIER_UIE;
}

void stop_TIM_update_irq(TIM_TypeDef* tim) {
	uint8_t irqn = TIM_to_IRQn(tim);
	NVIC->ICER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_DisableIRQ
	__DSB(); __ISB();  // flush processor pipeline before fetching
	tim->DIER &= ~TIM_DIER_UIE;
}
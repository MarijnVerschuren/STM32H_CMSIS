//
// Created by marijn on 2/13/23.
//
#include "tim.h"


/*!< static */
static inline uint32_t TIM_to_update_IRQn(TIM_TypeDef* tim) {
	if (tim == TIM1)											{ return TIM1_UP_TIM10_IRQn; }
	if ((((uint32_t)tim) - APB1PERIPH_BASE) >= 0x00010000UL)	{ return (((uint32_t)(tim - APB2PERIPH_BASE) >> 10u) & 0xfu) + TIM1_BRK_TIM9_IRQn; }
	uint32_t irqn = (((uint32_t)(tim - AHB1PERIPH_BASE) >> 10u) & 0xfu) + TIM2_IRQn;
	if (irqn > TIM4_IRQn)										{ return TIM5_IRQn; }
	return irqn;
}


/*!< variables */
// volatile uint64_t ticks = 0;


/*!< irq */
//extern void SysTick_IRQHandler(void) { ticks++; }


/*!< init / disable */
void config_TIM(TIM_TypeDef* tim, uint32_t prescaler, uint32_t limit) {
	if ((((uint32_t)tim) - APB1PERIPH_BASE) >= 0x00010000UL)	{ RCC->APB2ENR |= (0b1u << (((uint32_t)tim - APB2PERIPH_BASE) >> 10u)); }
	else														{ RCC->APB1ENR |= (0b1u << (((uint32_t)tim - APB1PERIPH_BASE) >> 10u)); }
	//RCC_TypeDef* ptr = RCC;
	tim->PSC = prescaler;
	tim->ARR = limit;
}
void disable_TIM(TIM_TypeDef* tim) {
	if ((((uint32_t)tim) - APB1PERIPH_BASE) >= 0x00010000UL)	{ RCC->APB2ENR &= ~(0b1u << (((uint32_t)tim - APB2PERIPH_BASE) >> 10u)); }
	else														{ RCC->APB1ENR &= ~(0b1u << (((uint32_t)tim - APB1PERIPH_BASE) >> 10u)); }
}

/*!< actions */
void start_TIM(TIM_TypeDef* tim)	{ tim->CR1 |= TIM_CR1_CEN; }
void stop_TIM(TIM_TypeDef* tim)		{ tim->CR1 &= ~TIM_CR1_CEN; }

/*!< irq */
void start_TIM_update_irq(TIM_TypeDef* tim) {
	uint32_t irqn = TIM_to_update_IRQn(tim);
	NVIC->ISER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_EnableIRQ
	tim->DIER |= TIM_DIER_UIE;
}
void stop_TIM_update_irq(TIM_TypeDef* tim) {
	uint32_t irqn = TIM_to_update_IRQn(tim);
	NVIC->ICER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_DisableIRQ
	__DSB(); __ISB();  // flush processor pipeline before fetching
	tim->DIER &= ~TIM_DIER_UIE;
}
// these are only for TIM1 but this can change on other generations
void start_TIM_capture_compare_irq(TIM_TypeDef* tim) {
	NVIC->ISER[((TIM1_CC_IRQn) >> 5UL)] = (uint32_t)(1UL << ((TIM1_CC_IRQn) & 0x1FUL));  // NVIC_EnableIRQ
}
void stop_TIM_capture_compare_irq(TIM_TypeDef* tim) {
	NVIC->ICER[((TIM1_CC_IRQn) >> 5UL)] = (uint32_t)(1UL << ((TIM1_CC_IRQn) & 0x1FUL));  // NVIC_DisableIRQ
	__DSB(); __ISB();  // flush processor pipeline before fetching
}
void start_TIM_break_irq(TIM_TypeDef* tim) {
	NVIC->ISER[((TIM1_BRK_TIM9_IRQn) >> 5UL)] = (uint32_t)(1UL << ((TIM1_BRK_TIM9_IRQn) & 0x1FUL));  // NVIC_EnableIRQ
}
void stop_TIM_break_irq(TIM_TypeDef* tim) {
	NVIC->ICER[((TIM1_BRK_TIM9_IRQn) >> 5UL)] = (uint32_t)(1UL << ((TIM1_BRK_TIM9_IRQn) & 0x1FUL));  // NVIC_DisableIRQ
	__DSB(); __ISB();  // flush processor pipeline before fetching
}
void start_TIM_trigger_commutation_irq(TIM_TypeDef* tim) {
	NVIC->ISER[((TIM1_TRG_COM_TIM11_IRQn) >> 5UL)] = (uint32_t)(1UL << ((TIM1_TRG_COM_TIM11_IRQn) & 0x1FUL));  // NVIC_EnableIRQ
}
void stop_TIM_trigger_commutation_irq(TIM_TypeDef* tim) {
	NVIC->ICER[((TIM1_TRG_COM_TIM11_IRQn) >> 5UL)] = (uint32_t)(1UL << ((TIM1_TRG_COM_TIM11_IRQn) & 0x1FUL));  // NVIC_DisableIRQ
	__DSB(); __ISB();  // flush processor pipeline before fetching
}
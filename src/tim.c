//
// Created by marijn on 7/6/23.
//

#include "tim.h"


/*!<
 * init
 * */
static inline void enable_TIM(const TIM_TypeDef* const tim) {
	uint32_t mask;
	if ((((uint32_t)tim) - APB1PERIPH_BASE) >= 0x00010000UL) {
		mask = (0b1u << (((uint32_t)tim - APB2PERIPH_BASE) >> 10u));
		do { RCC->APB2ENR |= mask; } while (!(RCC->APB2ENR & mask));
	} else {
		mask = (0b1u << (((uint32_t)tim - APB1PERIPH_BASE) >> 10u));
		do { RCC->APB1LENR |= mask; } while (!(RCC->APB1LENR & mask));
	}
}


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
void fconfig_TIM(TIM_TypeDef* tim, uint16_t prescaler, uint32_t limit, uint8_t auto_reload_preload, uint8_t down_count, uint8_t one_pulse) {
	uint32_t mask = TIM2 >= tim && tim <= TIM5 ? 0xffffffff : 0xffff;  // 32 | 16 bit timers
	enable_TIM(tim);
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
void disable_TIM(TIM_TypeDef* tim) {
	if ((((uint32_t)tim) - APB1PERIPH_BASE) >= 0x00010000UL)	{ RCC->APB2RSTR |= (0b1u << (((uint32_t)tim - APB2PERIPH_BASE) >> 10u)); }
	else														{ RCC->APB1LRSTR |= (0b1u << (((uint32_t)tim - APB1PERIPH_BASE) >> 10u)); }
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
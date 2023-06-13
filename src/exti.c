//
// Created by marijn on 2/13/23.
//
#include "exti.h"


/*!< static */
static inline uint32_t EXTI_line_to_IRQn(uint8_t EXTI_line) {
	uint32_t irqn = (EXTI_line & 0xf) + EXTI0_IRQn;
	if (irqn > 10) {
		if (irqn < 16)	{ irqn = EXTI9_5_IRQn; }
		else			{ irqn = EXTI15_10_IRQn; }
	} return irqn;
}


/*!< init */
void disable_EXTI(void) {
	RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGEN;
}


/*!< enable / disable */
void config_EXTI(uint8_t EXTI_line, GPIO_TypeDef* EXTI_port, uint8_t falling_edge, uint8_t rising_edge) {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  // enable EXTI
	EXTI_line &= 0xfu;  // only allow values upto 15
	uint8_t pos = (EXTI_line & 0x3u);  // index in the register [0:3]
	SYSCFG->EXTICR[EXTI_line >> 2u] &= ~(0xfu << (pos << 2));  // clear EXTI port in the register
	SYSCFG->EXTICR[EXTI_line >> 2u] |= GPIO_to_int(EXTI_port) << (pos << 2);  // set EXTI port in the register
	// set triggers
	EXTI->FTSR |= ((falling_edge & 0b1u) << EXTI_line);
	EXTI->RTSR |= ((rising_edge & 0b1u) << EXTI_line);
	EXTI->IMR |= (0b1u << EXTI_line);  // unmask interrupt
}

void start_EXTI(uint8_t EXTI_line) {
	uint32_t irqn = EXTI_line_to_IRQn(EXTI_line);
	NVIC->ISER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_EnableIRQ
}

void stop_EXTI(uint8_t EXTI_line) {
	uint32_t irqn = EXTI_line_to_IRQn(EXTI_line);
	NVIC->ICER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_DisableIRQ
	__DSB(); __ISB();  // flush processor pipeline before fetching
}

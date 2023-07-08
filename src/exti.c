//
// Created by marijn on 7/8/23.
//

#include "exti.h"


/*!<
 * static
 * */
static inline uint8_t EXTI_line_to_IRQn(uint8_t EXTI_line) {
	if (EXTI_line < 5)	{ return 6 + EXTI_line; }  // EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn
	if (EXTI_line < 10) { return EXTI9_5_IRQn; }
	return EXTI15_10_IRQn;
}


/*!<
 * init
 * */
void disable_EXTI(void) {
	RCC->APB4RSTR |= RCC_APB4ENR_SYSCFGEN;
}

void config_EXTI(uint8_t EXTI_line, GPIO_TypeDef* EXTI_port, uint8_t falling_edge, uint8_t rising_edge) {
	RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN; EXTI_line &= 0xfu;
	uint8_t pos =			(EXTI_line & 0x3UL);  // index in the register [0:3]
	uint8_t index =			(EXTI_line >> 2);
	uint32_t mask =			0xFUL << (pos << 2);
	uint32_t port_mask =	(dev_to_int(EXTI_port) & 0xFUL) << (pos << 2);
	do {  // write has to be looped because peripheral startup may not have been finished
		SYSCFG->EXTICR[index] &= ~mask;		// clear EXTI selection
		SYSCFG->EXTICR[index] |= port_mask;	// set EXTI selection
	} while ((SYSCFG->EXTICR[index] & mask) != port_mask);
	EXTI->FTSR1 |= ((falling_edge & 0b1UL) << EXTI_line);
	EXTI->RTSR1 |= ((rising_edge & 0b1UL) << EXTI_line);
	EXTI->IMR1 |= (0b1u << EXTI_line);
}

void start_EXTI(uint8_t EXTI_line) {
	uint8_t irqn = EXTI_line_to_IRQn(EXTI_line);
	NVIC->ISER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_EnableIRQ
}

void stop_EXTI(uint8_t EXTI_line) {
	uint8_t irqn = EXTI_line_to_IRQn(EXTI_line);
	NVIC->ICER[((irqn) >> 5UL)] = (uint32_t)(1UL << ((irqn) & 0x1FUL));  // NVIC_DisableIRQ
	__DSB(); __ISB();  // flush processor pipeline before fetching
}

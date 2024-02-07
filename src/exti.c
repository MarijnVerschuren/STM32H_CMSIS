//
// Created by marijn on 7/8/23.
//

#include "exti.h"


/*!<
 * static
 * */
static inline IRQn_Type EXTI_line_to_IRQn(uint8_t EXTI_line) {
	if (EXTI_line < 5)	{ return 6 + EXTI_line; }  // EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn
	if (EXTI_line < 10) { return EXTI9_5_IRQn; }
	return EXTI15_10_IRQn;
}


/*!<
 * init
 * */
void config_EXTI(uint8_t EXTI_line, GPIO_TypeDef* EXTI_port, uint8_t falling_edge, uint8_t rising_edge) {
	do { RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN; } while (!(RCC->APB4ENR & RCC_APB4ENR_SYSCFGEN));
	EXTI_line &= 0xfu;
	uint8_t pos =			(EXTI_line & 0x3UL);								// register index [0:3]
	uint8_t index =			(EXTI_line >> 2);
	SYSCFG->EXTICR[index] &= ~(0xFUL << (pos << 2));							// clear EXTI selection
	SYSCFG->EXTICR[index] |= ((GPIO_to_int(EXTI_port) & 0xFUL) << (pos << 2));	// set EXTI selection
	EXTI->FTSR1 |= ((falling_edge & 0b1UL) << EXTI_line);
	EXTI->RTSR1 |= ((rising_edge & 0b1UL) << EXTI_line);
	EXTI->IMR1 |= (0b1u << EXTI_line);
}

void start_EXTI(uint8_t EXTI_line) {
	enable_IRQ(EXTI_line_to_IRQn(EXTI_line));
}

void stop_EXTI(uint8_t EXTI_line) {
	disable_IRQ(EXTI_line_to_IRQn(EXTI_line));
}

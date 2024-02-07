//
// Created by marijn on 2/6/24.
//

#include "nvic.h"


/*!<
 * init
 * */
void enable_IRQ(IRQn_Type irqn) {
	NVIC->ISER[(((uint32_t)irqn) >> 0x5UL)] = (
		0b1UL << (((uint32_t)irqn) & 0x1FUL)
	);
}
void disable_IRQ(IRQn_Type irqn) {
	NVIC->ICER[(((uint32_t)irqn) >> 0x5UL)] = (
		0b1UL << (((uint32_t)irqn) & 0x1FUL)
	);
	// the next steps ensure that after this function is complete the interrupt is definitely off
	__DSB();  // wait for the data bus to clear
	__ISB();  // flush cpu pipeline
}
void set_IRQ_priority(IRQn_Type irqn, uint32_t priority) {
	volatile uint8_t* reg = &NVIC->IP[(uint32_t)irqn];
	if (irqn < 0)	{ reg = &SCB->SHPR[(((uint32_t)irqn) & 0xFUL) - 0x4UL]; }
    *reg = (uint8_t)((priority << 0x4UL) & 0xFFUL);
}
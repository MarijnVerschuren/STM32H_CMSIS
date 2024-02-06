//
// Created by marijn on 2/6/24.
//

#include "nvic.h"


/*!<
 * init
 * */
void enable_IRQ(uint8_t irqn) {
	NVIC_EnableIRQ(irqn);
}
void disable_IRQ(uint8_t irqn) {
	NVIC_DisableIRQ(irqn);
}
void set_IRQ_priority(uint8_t irqn) {
	NVIC_SetPriority(irqn, 0);
}
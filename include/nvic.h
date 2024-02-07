//
// Created by marijn on 2/6/24.
//

#ifndef NVIC_H
#define NVIC_H
#include "main.h"

/*!<
 * init
 * */
void enable_IRQ(IRQn_Type irqn);
void disable_IRQ(IRQn_Type irqn);
void set_IRQ_priority(IRQn_Type irqn, uint32_t priority);

#endif //NVIC_H

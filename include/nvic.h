//
// Created by marijn on 2/6/24.
//

#ifndef NVIC_H
#define NVIC_H
#include "main.h"

/*!<
 * init
 * */
void enable_IRQ(uint8_t irqn);
void disable_IRQ(uint8_t irqn);
void set_IRQ_priority(uint8_t irqn);

#endif //NVIC_H

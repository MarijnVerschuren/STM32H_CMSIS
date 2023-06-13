//
// Created by marijn on 4/15/23.
//

#ifndef STM32F_CMSIS_RNG_H
#define STM32F_CMSIS_RNG_H

#include "main.h"
#include "sys.h"


/*!< init / enable / disable */
void start_RNG(void);
void set_RNG(uint32_t seed);
/*!< usage */
uint32_t RNG_generate(uint32_t max);



#endif //STM32F_CMSIS_RNG_H

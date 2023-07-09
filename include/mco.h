//
// Created by marijn on 7/6/23.
//

#ifndef STM32H_CMSIS_MCO_H
#define STM32H_CMSIS_MCO_H
#include "main.h"
#include "gpio.h"


/*!<
 * types
 * */
typedef enum {
	MCO1_A8 = 0x08000000,
	MCO2_C9 = 0x09200000
} MCO_GPIO_t;


/*!<
 * init
 * */
void config_MCO(MCO_GPIO_t pin);
// TODO: move config here from sys


#endif //STM32H_CMSIS_MCO_H

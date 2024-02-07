//
// Created by marijn on 7/8/23.
//

#ifndef STM32H_CMSIS_EXTI_H
#define STM32H_CMSIS_EXTI_H
#include "main.h"
#include "gpio.h"
#include "nvic.h"


/*!<
 * init
 * */
void config_EXTI(uint8_t EXTI_line, GPIO_TypeDef* EXTI_port, uint8_t falling_edge, uint8_t rising_edge);
void start_EXTI(uint8_t EXTI_line);
void stop_EXTI(uint8_t EXTI_line);


#endif //STM32H_CMSIS_EXTI_H
